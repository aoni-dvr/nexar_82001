/**
 *  @file AmbaDSP_Priv.h
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
 *  @details Definitions & constants for SSP (internal)
 *
 */

#ifndef AMBA_DSP_PRIV_H
#define AMBA_DSP_PRIV_H

#include "AmbaDSP.h"

/*YY-MM-DD-VV*/
#define DSP_DRIVER_VERSION  21060400UL

//#define SUPPORT_IWONG_UCODE
#define SUPPORT_MAX_UCODE
//#define SUPPORT_VIN_ATTACH_PYMD_UPDATE //WIP, for testing PymdUpdate
#define SUPPORT_RESAMPLING_CHECK
//#define DEBUG_SYNC_CMD
//#define DEBUG_SYNC_ISO_CMD
#define SUPPORT_VPROC_RT_RESTART
#define SUPPORT_VPROC_RT_RESTART_MTH_1 //method#1
//#define SUPPORT_VPROC_RT_RESTART_MTH_2 //method#2
//#define SUPPORT_DSP_SUSPEND

#define SUPPORT_DSP_EXT_PIN_BUF // from Rev_287643
#define SUPPORT_VOUT_BG_ONE_SYNC_RESET
#define SUPPORT_DSP_MAIN_ME_DISABLE
//#define SUPPORT_IK_UV_DMA
//#define SUPPORT_IK_FULL_TILE  // cv5 use ik-set-stitch-info, so c2y/warp must be same tile-num

/* In FS Camera mode, HL need at least one vout instance for boot flow */
/* In PlayBackMode, HL NEED DUAL VOUT to be actived */
/* To avoid Boot2Lv without dualvout then switch to Playback
 *   we enable daul-vout bit(DspCfgCmd.VoutBitMask) all the time,
 *   but with minimal vout resource (DspCfgCmd.Vout(A/B)_max_(pic/osd/mixer_n(col/row))) for less cmem/smem usage
 *   From ChenHan's suggest(20191105) we can set they as 16x1 if such vout is in-active
 */
#define SUPPORT_DUMMY_VOUT_THREAD
#define DUMMY_VOUT_MAX_COL                  (16U)
#define DUMMY_VOUT_MAX_ROW                  (1U)

//fs
#define VOUT_DISP_DEF_MAX_ROW               (4U)
#define VOUT_OSD_DEF_MAX_ROW                (4U) // based on latest ucode internal setting 2021/1/14
#define VOUT_MIXER_DEF_MAX_ROW              (4U) // based on latest ucode internal setting 2021/1/14

// for DJI's project which has small LCD(240Width) and broken 16x DownScale limitation from 3840Main
// Here use PreviewRIO to cut 2 line at right side, which lead to 2/16 = 0.125 pixel lost but could be ignored visually
// Currently we only support it in VOUT + NonEffect YuvStrm
#define SUPPORT_VOUT_16XDS_TRUNCATE_TRICK
#define VOUT_16XDS_TRUNCATE_LINE_NUM        (2U)

//#define SUPPORT_VPROC_RT_RESTART_2 //method#2

#define SUPPORT_VPROC_GROUPING
//#define SUPPORT_VPROC_DISABLE_BIT
#define SUPPORT_VPROC_INDEPENDENT_WITHIN_GROUPING //after Rev_288209

//#define UCODE_SUPPORT_DUPLICATED_OUTPUT //means uCode support one pin output to multiple dest
//#define UCODE_SUPPORT_MAIN_2_VOUT //means uCode support Main to Vout
//#define UCODE_SUPPORT_MAIN_DISABLE //means uCode support Main output to NULL. Otherwise, MainPin must be output to Dram
//#define UCODE_SUPPORT_PREVC_2_VOUT //means uCode support PrevC to Vout
#define UCODE_SUPPORT_PREVFILTER_REARRANGE //means uCode support preview filter to any destination
//#define UCODE_SUPPORT_EFFECT_PREV_REARRANGE //means uCode support preview filter to any destination in EffectChan
//6/15/2021, CV5 support Effect Fix rearrange
#define UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE //means uCode support preview B/C filter to any vout destination in EffectChan
//#define UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE //means uCode support preview A/B/C filter to any vout destination in EffectChan
#define UCODE_SUPPORT_EFFECT_SHARE_NONEFFECT_BUF_LOGIC //means uCode share same buffer definition(NonEffct) in Effct/NonEffct
//#define ENABLE_EFCT_BUF_IMG_SZ_CMD

//#define EFFECT_SYSTEM_PARAM_USE_LEGACY // if liveview with effect channel uses previously tuning parameters
//#define PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING // if ppstrm could be runtime changed without tmp buf restriction
//#define DUPLEX_SYSTEM_PARAM_USE_LEGACY // duplex uses previously tuning parameters
//#define PPSTRM_SWITCH_CHANGE_INPUTS_NUM // if num_of_inputs could be runtime changed
#define PPSTRM_LAST_CHAN_MUST_BLEND // if ppstrm must have a blending job in its last channel
//#define PPSTRM_INDPT_INT_BLD_BUFFER //use independent internal blend buffer
//#define PPSTRM_REBASE_CHK_OVERLAP //consider overlap when rebase
#define UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC // 2021/6/16, means uCode use async cmd to handle virt-vin relative cmd

//#define SUPPORT_IDSP_AFFINITY
//#define SUPPORT_VDSP_ENC_AFFINITY
//#define SUPPORT_VDSP_DEC_AFFINITY

//#define SUPPORT_DSP_PIN_OUT
//#define ENABLE_CMD_MSG_CRC

/* CV2FS not support following compare to cv2x */
//#define SUPPORT_DSP_MV_DUMP
//#define SUPPORT_DSP_CMPR_STRO
//#define SUPPORT_DSP_SET_RAW_CAP_CNT
//#define SUPPORT_DSP_SET_IDSP_DBG_CMD
//#define SUPPORT_DSP_DEC_BATCH
//#define SUPPORT_DSP_EXT_MEM_INIT_MODE
//#define SUPPORT_DSP_LDY_SLICE
//#defien SUPPORT_DSP_VIN_YUV_ENC_FBP_INIT
#define SUPPORT_DSP_VIN_DEFT_RAW //ucode 0x420ee411
#define SUPPORT_DSP_VIN_FRM_CTRL //ucode 0x420ee411
//#defien SUPPORT_DSP_RAW_AAA_ONLY
//#define SUPPORT_IK_AAA_HEADER_ISOTAG
#define SUPPORT_RESC_CHECK
#define SUPPORT_DRAM_PAGE
//#define SUPPORT_DRAM_SMEMGRP
//#define SUPPORT_ENC_VINATTACH_1FRM_DLY
#define SUPPORT_VPROC_DLY
//#define TEST_FOR_ISOCFG_VIA_IK_CFG_CMD
#define SUPPORT_DSP_PINOUT_DECI
#define SUPPORT_MCTF_DRAM_DISABLE
#define SUPPORT_IS_VPROC_SKIP_FLAG //ucode 0x420ee411
#define SUPPORT_ENC_Q_LD //ucode 0x89921c72
#define SUPPORT_AAA_STAT_SEQ // ucode 0x308df0f8
#define SUPPORT_MULTI_VIN_HDR //ucode 0x886a3205
#define SUPPORT_DEC_CHAN_RES //ucode 0x886a3205
#define SUPPORT_AAA_MUX_REPORT // ucode 0x7b12970b
#define SUPPORT_BIND_VIA_ASYNC // ucode 0x8bf697db
#define SUPPORT_ENCSTATUS_RESET // ucode 0x04fed832
#define SUPPORT_ENC_QP_ROI // ucode 0x58f83ad2
#define SUPPORT_DEC_I_STRIPE // ucode 0x4e5eeb22
#define SUPPORT_IDSP_DBLCMD // ucode 0x4724f83a
#define SUPPORT_DSP_VIN_SHRTFRM_DROP //ucode
#define SUPPORT_EFCT_PRESEND
#define SUPPORT_EFCT_EMBD_MARKER
#define USE_TIME_MANAGER
#define SUPPORT_VPROC_MAIN_OUT_DISABLE  // ucode 0x5d7c8905, 2022/9/7
#define USE_FLEXIBLE_BATCH_CMD_POOL
#define SUPPORT_EFCT_UNION_GRP

/* Under multi-core,
 * we shall make tile-num as core-align for better performance balance.
 * only applied when original tile-num > 1
 */
#define SUPPORT_TILE_NUM_ALIGN
//#define SUPPORT_DSP_TILE_SMEM_EXPLICIT //ucode not support it on cv5x

/*
 * CV2FS HL use PrevB master mode
 * which means PrevB must be enabled and can Output2Dummy for saving Dram
 * TO make things simple, we keep orignal binding rule and enable PrevB after binding decided
 */
#define SUPPORT_DSP_PREVB_MASTER_MODE

//#define TEST_FHD_TILE_MODE
//#define TEST_MAIN_ONLY
//#define TEST_MAIN_B_ONLY
//#define TEST_MAIN_B_A_ONLY

/*
 * all decode instance share same virtual-vin.
 * due to ucode forbid feed data multiple times in one sync
 * so we can't enable this trick for saving virtual-vin usage.
 * Legacy method will occupied virtual-vin per decode instance.
 */
//#define DUPLEX_DEC_SHARE_VIRT_VIN

//#define SUPPORT_INTERNAL_CHECK
#define SUPPORT_DSP_SUSPEND_API
#define SUPPORT_DSP_SHUTDOWN

#ifndef DSP_BUF_ADDR_ALIGN
#define DSP_BUF_ADDR_ALIGN          (128U)
#define DSP_MSG_CTLR_BUF_SIZE_ALIGN (128U) // In CV5, we need 128B align to have better dma performance
#endif

#define WAIT_FLAG_TIMEOUT                   (3000U) //3sec
#define MODE_SWITCH_TIMEOUT                 (3000U) //3sec
#define WAIT_INTERRUPT_TIMEOUT              (100U) //100ms

/* ExtMem for raw encode relative */
#define MAX_EXT_MEM_BUF_NUM                 (1U)
#define MAX_EXT_MEM_BUF_TYPE_NUM            (2U)
#define MAX_STL_EXT_MEM_STAGE_NUM           (3U) /* [0]R2Y [1]Y2Y [2]R2R */
#define MAX_STL_BATCH_CMD_POOL_NUM          (2U) // OneCmd + OneEOFCmd

typedef struct {
    ULONG  Base;
    UINT32 Size;
    UINT32 IsCached;
} DSP_MEM_BLK_t;

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DSP_CFG (1)
\*-----------------------------------------------------------------------------------------------*/
/* Dsp Config Parameter Index */
#ifdef EFFECT_SYSTEM_PARAM_USE_LEGACY
#define DSP_CONFIG_PARAM_SET_2VPROC         (0U)
#define DSP_CONFIG_PARAM_SET_4VPROC         (1U)
#define DSP_CONFIG_PARAM_SET_8VPROC         (2U)
#define DSP_CONFIG_PARAM_SET_9VPROC         (3U)
#define DSP_CONFIG_PARAM_SET_2VPROC_DEC     (4U)
#define DSP_CONFIG_PARAM_SET_NUM            (5U)
#else
#define DSP_CONFIG_PARAM_SET_2VPROC_DEC     (0U)
#define DSP_CONFIG_PARAM_SET_NUM            (1U)
#endif

#define DSP_CONFIG_PARAM_SET_DEC            (0U)
#define DSP_CONFIG_PARAM_SET_DEC_NUM        (1U)

/* Dsp Config Parameter */
#define DSP_CONFIG_PARAM_DRAM_PAR           (0U)
#define DSP_CONFIG_PARAM_SMEM_PAR           (1U)
#define DSP_CONFIG_PARAM_SUB_PAR            (2U)
#define DSP_CONFIG_PARAM_SUP_PAR            (3U)
#define DSP_CONFIG_PARAM_FBP                (4U)
#define DSP_CONFIG_PARAM_FB                 (5U)
#define DSP_CONFIG_PARAM_DBP                (6U)
#define DSP_CONFIG_PARAM_CBUF               (7U)
#define DSP_CONFIG_PARAM_BDT                (8U)
#define DSP_CONFIG_PARAM_BD                 (9U)
#define DSP_CONFIG_PARAM_IMGINF             (10U)
#define DSP_CONFIG_PARAM_EXT_FB             (11U)
#define DSP_CONFIG_PARAM_MCBL               (12U)
#define DSP_CONFIG_PARAM_MCB                (13U)
#define DSP_CONFIG_PARAM_MBUF_PAR           (14U)
#define DSP_CONFIG_PARAM_MBUF               (15U)
#define DSP_CONFIG_PARAM_AIK_PAR            (16U)
#define DSP_CONFIG_PARAM_MFBP               (17U)
#define DSP_CONFIG_PARAM_MFB                (18U)
#define DSP_CONFIG_PARAM_MEXT_FB            (19U)
#define DSP_CONFIG_PARAM_MIMGINF            (20U)
#define DSP_CONFIG_PARAM_PG_SZ_LOG2         (21U)
#define DSP_CONFIG_PARAM_IDSP_SBLK          (22U)
#define DSP_CONFIG_PARAM_FBP_CACHE          (23U)
#define DSP_CONFIG_PARAM_FB_CACHE           (24U)
#define DSP_CONFIG_PARAM_IMGINF_CACHE       (25U)
#define DSP_CONFIG_PARAM_MFBP_CACHE         (26U)
#define DSP_CONFIG_PARAM_MFB_CACHE          (27U)
#define DSP_CONFIG_PARAM_MIMGINF_CACHE      (28U)
#define DSP_CONFIG_PARAM_ORCCODE_MSG_Q      (29U)
#define DSP_CONFIG_PARAM_ORCALL_MSG_Q       (30U)
#define DSP_CONFIG_IDSP_TIMEOUT             (31U)
#define DSP_CONFIG_WARP_DRAM_OUT            (32U)
#define DSP_CONFIG_RSVD                     (33U)
#define DSP_CONFIG_C2Y_DRAM_OUT             (34U)
#define DSP_CONFIG_C2Y_BURST_TILE           (35U)
#define DSP_CONFIG_PARAM_IDSP_SBLK0         (36U)
#define DSP_CONFIG_AAA_ENABLE               (37U)
#define DSP_CONFIG_LI_ENABLE                (38U)
#define DSP_CONFIG_RAW_OVERLAP              (39U)
#define DSP_CONFIG_VIN_REPEAT               (40U)
#define DSP_CONFIG_C2Y_FB                   (41U)
#define DSP_CONFIG_TILE_DCROP               (42U)
#define DSP_CONFIG_VIN_REP_PORTION          (43U)
#define DSP_CONFIG_FORBID_ISO_UPT           (44U)
#define DSP_CONFIG_DRAM_PRI                 (45U)
#define DSP_CONFIG_PRI_XFER                 (46U)
#define DSP_CONFIG_ENC_512B_XFER            (47U)
#define DSP_CONFIG_VPROC_IN                 (48U)
#define DSP_CONFIG_VOUT_DBL_CMD             (49U)
#define DSP_CONFIG_MONO_VDO_ENC             (50U)
#define DSP_CONFIG_IDSP_DBL_BANK            (51U)
#define DSP_CONFIG_IDSP_WIN_STRM_NUM        (52U)
#define DSP_CONFIG_VPROC_ROTATE             (53U)
#define DSP_CONFIG_VIN_AUTO_DROP            (54U)

// leave some blank
#define DSP_CONFIG_MAX_IDSP_AFF             (64U)
#define DSP_CONFIG_IDSP_AFF                 (65U)
#define DSP_CONFIG_MAX_VDSP_ENC_AFF         (66U)
#define DSP_CONFIG_MAX_VDSP_ENC_AFF_OPT     (67U)
#define DSP_CONFIG_VDSP_ENC_AFF             (68U)
#define DSP_CONFIG_VDSP_ENC_AFF_OPT         (69U)
#define DSP_CONFIG_MAX_VDSP_DEC_AFF         (70U)
#define DSP_CONFIG_MAX_VDSP_DEC_AFF_OPT     (71U)
#define DSP_CONFIG_VDSP_DEC_AFF             (72U)
#define DSP_CONFIG_VDSP_DEC_AFF_OPT         (73U)
#define DSP_CONFIG_MAX_VDSP_DEC_ENG_AFF     (74U)
#define DSP_CONFIG_VDSP_DEC_ENG_AFF         (75U)
#define DSP_CONFIG_PARAM_NUM                (76U)

/* SSP defined Vin status */
#define DSP_VIN_STATUS_INVALID              (0U)
#define DSP_VIN_STATUS_TIMER                (1U)
#define DSP_VIN_STATUS_ACTIVE               (2U)
#define DSP_VIN_STATUS_IDLE2RUN             (3U)
#define DSP_VIN_STATUS_TIMEOUT              (4U)
#define DSP_VIN_STATUS_CTX_RESET            (5U)

/* HL FlowProcessor binding scene */
#define BIND_VIDEO_YUVCONV                  (0U)
#define BIND_VIDEO_ENCODE                   (1U)
#define BIND_STILL_ENCODE                   (2U)
#define BIND_VIDEO_INTERVAL_ENCODE          (3U)
#define BIND_VIDEO_ENCODE_YUV               (4U)
//#define BIND_VIDEO_DECODE                   (5U)
#define BIND_STILL_PROC                     (6U)
#define BIND_VIDEO_DECODE_TO_VPROC          (7U)
#define BIND_VIDEO_RECON_TO_VPROC           (8U)
//#define BIND_STILL_DECODE                   (9U)
//#define BIND_STILL_DECODE_Y2DISP            (10U)
#define BIND_VIDEO_ENCODE_VENC              (11U)
#define BIND_VIDEO_DECODE_TEST              (12U)

/* FlowProcessor binding Operation */
#define FP_BIND_TYPE_BIND                   (1U)
#define FP_BIND_TYPE_UNBIND                 (2U)

/* FlowProcessor binding Type */
#define FP_BIND_DATA                        (0U) /** bind data pin of flow processor */
#define FP_BIND_MSG                         (1U) /** bind message pin of flow processor */

/* FlowProcessor Type */
#define FP_TYPE_VIN                         (1U)
#define FP_TYPE_VPROC_DV                    (2U)
#define FP_TYPE_SPROC                       (3U)
#define FP_TYPE_VENC                        (4U)
#define FP_TYPE_ENG0                        (5U)
#define FP_TYPE_DEC                         (6U)
#define FP_TYPE_ENG1                        (7U)

/* DSP debug level */
#define DBG_LVL_SILENT                      (0U)
#define DBG_LVL_MIN                         (1U)
#define DBG_LVL_NORMAL                      (2U)
#define DBG_LVL_VERBOSE                     (3U)
#define DBG_LVL_DBG                         (4U)

#define DBG_MODULE_MEM                      (1U)
#define DBG_MODULE_API                      (2U)
#define DBG_MODULE_MSG                      (4U)
#define DBG_MODULE_BOOT                     (8U)
#define DBG_MODULE_IDSP                     (16U)
#define DBG_MODULE_GME                      (32U)
#define DBG_MODULE_VPROC                    (64U)
#define DBG_MODULE_SPROC                    (128U)
#define DBG_MODULE_VIN_HL                   (256U)
#define DBG_MODULE_PREV                     (512U)
#define DBG_MODULE_ME_LL                    (4096U)
#define DBG_MODULE_MD_LL                    (8192U)
#define DBG_MODULE_ENC_HL                   (16384U)
#define DBG_MODULE_GME_HL                   (32768U)
#define DBG_MODULE_ENG0                     (65536U)
#define DBG_MODULE_PROXY                    (131072U)
#define DBG_MODULE_VOUT                     (262144U)
#define DBG_MODULE_DEC_HL                   (1048576U)
#define DBG_MODULE_DEC_LL                   (2097152U)
#define DBG_MODULE_DEC_CODE                 (8388608U)

/* vproc_input_format_t */
#define DSP_VPROC_INPUT_RAW_RGB             (0U)
#define DSP_VPROC_INPUT_YUV_422             (1U)
#define DSP_VPROC_INPUT_YUV_420             (2U)
#define DSP_VPROC_INPUT_MIPI_YUYV           (3U)


/*-----------------------------------------------------------------------------------------------*\
 * CAT_VPROC (2)
\*-----------------------------------------------------------------------------------------------*/
/* VCAP_PREV_ID */
#define DSP_VPROC_PREV_A                    (0U)
#define DSP_VPROC_PREV_B                    (1U)
#define DSP_VPROC_PREV_C                    (2U)
#define DSP_VPROC_PREV_D                    (3U)
#define DSP_VPROC_PREV_NUM                  (4U)

/* HIER_RESAMP_OCTAVE_MODE */
#define DSP_HIER_HALF_OCTAVE_MODE           (0U)
#define DSP_HIER_OCTAVE_MODE_MODE           (1U)
#define DSP_HIER_OCTAVE_MODE_NUM            (2U)

/* prev_mode in CMD_VPROC_CONFIG:
 *  0: DRAM preview where input is from DRAM for all preview filters.
 *  1: Warp SMEM preview where SMEM is connected between WARP filter (or MCTF) and preview filters.
 *     After that, preview filters generate YUV420 or YUV422 frames to DRAM for vout to display.
 *  2: Vout_th preview where vout_th takes the main output in YUV420 and use one of the preview
 *     filter to resize to preview size and then display. This mode is most bandwidth efficient.
 *  Note: prev_src is prev_src_t
*/
#define VPROC_PREV_SRC_DRAM                 (0U)
#define VPROC_PREV_SRC_SMEM                 (1U)
#define VPROC_PREV_SRC_VOUT_TH              (2U)
#define VPROC_PREV_SRC_STOP                 (0xFU)

/* Per Max on 2014/10/18, in any DV case, preview A is so far only connected 2nd stream as 2nd encoding stream.
 * therefore, we don't need to support other output sources for DV. That is: preview A->2nd stream, preview B->HDMI, preview C-> LCD.
 * That is very likely, there is no need to re-assign the preview filter output destination.
 * Later on under multi-chan application, we may need to support dst  = 4.
*/
#define AMBA_DSP_PREV_DEST_NULL             (0U)
#define AMBA_DSP_PREV_DEST_VOUT0            (0x1U)
#define AMBA_DSP_PREV_DEST_VOUT1            (0x2U)
#define AMBA_DSP_PREV_DEST_PIN              (0x4U)
#define AMBA_DSP_PREV_DEST_DUMMY            (0x8U) /* No Dram out */

#define VPROC_PREV_FMT_PROG                 (0U) // 0:Progressive
#define VPROC_PREV_FMT_INTL                 (1U) // 1:Interlaced
#define VPROC_PREV_FMT_INTL_TOP             (2U) // 2:Interlaced Top field
#define VPROC_PREV_FMT_INTL_BOT             (3U) // 3:Interlaced Bottom field

#define VPROC_PREV_FR_29_97                 (0U)
#define VPROC_PREV_FR_59_94                 (1U)
#define VPROC_PREV_FR_15                    (15U)
#define VPROC_PREV_FR_24                    (24U)
#define VPROC_PREV_FR_30                    (30U)
#define VPROC_PREV_FR_60                    (60U)

/* reset_option in CMD_VPROC_STOP:
 *  0: reset on the frame boundary.
 *  1: reset immediately.
*/
#define AMBA_DSP_VPROC_RESET_FRM            (0U)
#define AMBA_DSP_VPROC_RESET_DIRECT         (1U)

#define DSP_VPROC_MAIN_W_ALIGN              (32U)
#define DSP_VPROC_MAIN_H_ALIGN              (16U)

#define VPROC_MCTF_MODE_FILTER              (0U)
#define VPROC_MCTF_MODE_BLEND               (2U)
#define NUM_VPROC_MCTF_MODE_TYPE            (3U)


#define NUM_VPROC_MAX_GROUP                 (VPROC_MAX_GRP_NUM)     /* 4 */
#define NUM_VPROC_MAX_CHAN                  (VPROC_MAX_CHAN_NUM)    /* 20 */

#define DSP_VPROC_LNDEC_NUM_DISABLE         (0XFFU)
#define DSP_VPROC_HIER_NUM_DISABLE          (0XFFU)
#define DSP_VPROC_FB_NUM_DISABLE            (0XFFU)
#define DSP_VPROC_EXT_FB_DEFAULT_NUM        (0X2U)

#define DSP_MAX_OSD_BLEND_AREA_NUMBER       (3U) //uCode logic limit is 32

#define DSP_VPROC_C2Y_TESTFRAME_TYPE        (0U)
#define DSP_VPROC_Y2Y_TESTFRAME_TYPE        (1U)

/* External memory description Relative */
#define MAX_VPROC_EXT_MEM_TYPE_NUM          (NUM_VPROC_EXT_MEM_TYPE)
#define DEFAULT_EXT_MEM_DADDR_NUM           (16U)
#define DEFAULT_EXT_MEM_POOL_VIN_BUF_NUM    (4U)
#define DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM  (3U)

#define VPROC_MODE_VIDEO_LISO               (0U)
#define VPROC_MODE_STILL_LISO               (1U)
#define VPROC_MODE_STILL_HISO               (2U)

#define DSP_VIDEO_ME1_DATA_SHIFT            (2U) // 1/4
#define DSP_VIDEO_ME0_DATA_SHIFT            (3U) // 1/8

#define DSP_MAX_3A_SLICE_NUM                (32U)
#define DSP_3A_BUFFER                       (DSP_MAX_3A_SLICE_NUM)

/* IK id definition */
typedef struct {
    UINT32 BufIdx:16;   // IsoCfg ring buffer
    UINT32 Rsvd:2;
    UINT32 Ability:6;
    UINT32 Pipe:2;
    UINT32 CtxId:6;
} IK_ID_s;

/* is_rotate_1st(2nd)_blending_inp */
#define DSP_EFCT_ROT_NONE           (0U)
#define DSP_EFCT_ROT_90_DEGREE      (1U) // rotate=1
#define DSP_EFCT_ROT_270_DEGREE     (2U) // rotate=1, h+v flip
#define DSP_EFCT_ROT_180_DEGREE     (3U) // h+v flip
#define DSP_EFCT_ROTATE_NONE_HFLIP  (4U) // hflip=1
#define DSP_EFCT_ROTATE_NONE_VFLIP  (5U) // vflip=1
#define DSP_EFCT_ROTATE_90_HFLIP    (6U) // rotate=1, hflip=1
#define DSP_EFCT_ROTATE_90_VFLIP    (7U) // rotate=1, vflip=1

#define DSP_EFCT_INVALID_IDX        (0xFFFFU)
/* HL Effect Copy Job description */
typedef struct {
    UINT16 JobId;
    UINT16 SrcBufIdx;
    UINT16 DestBufIdx;
    UINT16 Rsvd;
    AMBA_DSP_WINDOW_s SrcWin;
    AMBA_DSP_WINDOW_s DstWin;
} DSP_EFFECT_COPY_JOB_s;

/* HL Effect Blend Job description */
typedef struct {
    UINT16 JobId;
    UINT16 Src0BufIdx;
    UINT16 Src1BufIdx;
    UINT16 DestBufIdx;
    UINT16 ReBaseOutBufIdx;
    UINT16 OverlapChan; //bit-wise indicates which chan overlap to this Blend area
#define BLD_JOB_Y2Y_BUF_NEW                 (1U) // Create new Y2Y buffer
#define BLD_JOB_Y2Y_BUF_NEW_INT             (2U) // Create new Y2Y buffer for internal usage, ie. Y2Y for Rotate/Flip with others
#define BLD_JOB_Y2Y_BUF_NEW_INT_DUMMY       (3U) // Create new Y2Y buffer for internal usage, ie. Y2Y for Rotate/Flip with itself
#define BLD_JOB_Y2Y_BUF_REUSE               (4U) // Using Existed Y2Y buffer
#define BLD_JOB_Y2Y_BUF_REUSE_INT           (5U) // Using Existed Y2Y buffer for internal usage, ie. Y2Y for Rotate/Flip with others
#define BLD_JOB_Y2Y_BUF_REUSE_INT_DUMMY     (6U) // Using Existed Y2Y buffer for internal usage, ie. Y2Y for Rotate/Flip with itself
#define BLD_JOB_Y2Y_BUF_NEW_OUT             (7U) // Create new Y2Y buffer, and output on it
#define BLD_JOB_Y2Y_BUF_NEW_OUT_INT         (8U) // Create new Y2Y buffer for internal usage, ie. Y2Y for Rotate/Flip with others
#define BLD_JOB_Y2Y_BUF_NEW_OUT_INT_DUMMY   (9U) // Create new Y2Y buffer for internal usage, ie. Y2Y for Rotate/Flip with itself
    UINT16 NeedY2YBuf;
    ULONG  AlphaAddr;   //all Src0 when 255, all src1 when 0
    UINT16 AlphaPitch;
    AMBA_DSP_WINDOW_s Src0Win;
    AMBA_DSP_WINDOW_s Src1Win;
    AMBA_DSP_WINDOW_s DestWin;
} DSP_EFFECT_BLEND_JOB_s;

/* HL Effect buffer Id description */
typedef struct {
    UINT16 OutputBufId;
    UINT16 CpyBufId;
    UINT16 Y2YBufId;
    UINT16 FirstPsThIdx;    //0xFFFF means no such channel
    UINT16 FirstCpyIdx;     //0xFFFF means no such channel
    UINT16 FirstBldIdx;     //0xFFFF means no such channel
    UINT16 FirstY2YBldIdx;  //0xFFFF means no such channel
    UINT16 LastPsThIdx;     //0xFFFF means no such channel
    UINT16 LastCpyIdx;      //0xFFFF means no such channel
    UINT16 LastBldIdx;      //0xFFFF means no such channel
#ifdef PPSTRM_INDPT_INT_BLD_BUFFER
    UINT32 ChannelOutputMask;
#endif
} DSP_EFFECT_BUF_DESC_s;

/* StlProc PIN default setting */
#define DSP_STLPROC_Y2Y_DEFAULT_PIN     DSP_VPROC_PIN_PREVA //using PrevB for Y2Y
#define DSP_STLPROC_R2Y_DEFAULT_PIN     DSP_VPROC_PIN_PREVA //using PrevB for R2Y

/* Mctf ouput buffer need to have extra padding for following AVC/HEVC encode */
/* since AVC need 16 align, so here we set padding number as 16 */
#define DSP_POSTP_MAIN_PADDING_FOR_ENC  (16U)

/* VCAP_INPUT_FORMAT */
#define DSP_VIN_INPUT_RGB_RAW           (0U)
#define DSP_VIN_INPUT_YUV_422_INTLC     (1U)
#define DSP_VIN_INPUT_YUV_422_PROG      (2U)
#define DSP_VIN_INPUT_DRAM_INTLC        (3U) //Yuv420
#define DSP_VIN_INPUT_DRAM_PROG         (4U) //Yuv420
#define DSP_VIN_INPUT_NUM               (5U)

/* HL_VIN_START_CONFIG_OPTION */
#define VIN_START_CFG_ISOCFG            (0x1U)
#define VIN_START_CFG_CMD               (0x2U)
#define VIN_START_CFG_ALL               (VIN_START_CFG_ISOCFG|VIN_START_CFG_CMD)

/* HL_VIN_START_PURPOSE_e */
#define VIN_START_LIVEVIEW_ONLY                 (0U)
#define VIN_START_STILL_RAW_ONLY                (1U)
#define VIN_START_LIVEVIEW_STILL_RAW_CONCURR    (2U)
#define VIN_START_LIVEVIEW_RAW_STITCH           (3U)
#define VIN_START_VIRT_VIN                      (4U)
#define VIN_START_VIRT_VIN_LIVEVIEW             (5U)

/* Raw compression */
#define RAW_COMPRESS_OFF                (0U) //IK_RAW_COMPRESS_OFF
#define RAW_COMPRESS_6P75               (1U) //IK_RAW_COMPRESS_6P75
#define RAW_COMPRESS_7p5                (2U)
#define RAW_COMPRESS_7p75               (3U)
#define RAW_COMPRESS_8p5                (4U)
#define RAW_COMPRESS_8p75               (5U) // IK_RAW_COMPRESS_8p5
#define RAW_COMPRESS_9p5                (6U)
#define RAW_COMPRESS_9p75               (7U)
#define RAW_COMPRESS_10p5               (8U)
#define RAW_COMPRESS_10p75              (9U)
#define RAW_COMPRESS_11P5               (10U)
#define RAW_COMPRESS_11P75              (11U)
#define RAW_COMPRESS_12P5               (12U)
#define RAW_COMPRESS_4P75               (13U)
#define RAW_COMPRESS_5P5                (14U)
#define RAW_COMPRESS_5P75               (15U)
#define RAW_COMPRESS_6P5                (16U)
#define RAW_COMPRESS_NUM                (17U)

/* Raw compact */
#define RAW_COMPACT_OFF                 (0U)
#define RAW_COMPACT_8BIT                (1U) //IK_RAW_COMPACT_8B
#define RAW_COMPACT_10BIT               (2U) //IK_RAW_COMPACT_10B
#define RAW_COMPACT_12BIT               (3U) //IK_RAW_COMPACT_12B
#define RAW_COMPACT_14BIT               (4U) //IK_RAW_COMPACT_14B
#define RAW_COMPACT_NUM                 (5U)

/* 0: Discard Captured raw
 * 1: Captured raw won't pass on to any R2Y processing
 * 2: Captured raw will pass on to any R2Y processing
 */
#define VIN_RAW_DISCARD                 (0U)
#define VIN_RAW_DEST_EXTERNAL           (1U)
#define VIN_RAW_DEST_INTERNAL           (2U)

/*
 * 0: sensor in
 * 1: ext data (raw/yuv422/yuv420)
 * 2: vin yuv422 input
*/
#define VIN_RAW_SRC_TYPE_SENSOR         (0U)
#define VIN_RAW_SRC_TYPE_RAW            (1U)
#define VIN_RAW_SRC_TYPE_YUV422         (2U)

/* vin_ext_mem_type_t */
#define DSP_VIN_EXT_MEM_TYPE_RAW        (0U)
#define DSP_VIN_EXT_MEM_TYPE_YUV422     (1U)
#define DSP_VIN_EXT_MEM_TYPE_HDS        (2U)
#define DSP_VIN_EXT_MEM_TYPE_ME         (3U)
#define DSP_VIN_EXT_MEM_TYPE_AUX        (4U)
#define DSP_VIN_EXT_MEM_TYPE_NUM        (5U)

/* vin_send_in_data_type_t */
#define DSP_VIN_SEND_IN_DATA_RAW    (0U)
#define DSP_VIN_SEND_IN_DATA_YUV422 (1U)
#define DSP_VIN_SEND_IN_DATA_YUV420 (2U)
#define DSP_VIN_SEND_IN_DATA_NUM    (3U)

/* vin_send_in_data_type_t */
#define DSP_VIN_SEND_IN_DATA_DEST_VPROC     (0U)
#define DSP_VIN_SEND_IN_DATA_DEST_ENC       (1U)
#define DSP_VIN_SEND_IN_DATA_DEST_NUM       (2U)

/*  0: System gives distinct buffer addresses for each frame up to the num of frame buffers specified in num_frm_buf.
 *  1: System gives a start address of buffer segment and uCode will divide it up to num_frm_buf.
*/
#define VIN_EXT_MEM_ALLOC_TYPE_DISTINCT     (0U)
#define VIN_EXT_MEM_ALLOC_TYPE_CONTIGUOUS   (1U)
#define VIN_EXT_MEM_ALLOC_TYPE_NUM          (2U)

/*  0: Brand new allocation.
 *  1: Append new set of buffers to the existing pool.
*/
#define VIN_EXT_MEM_MODE_NEW                (0U)
#define VIN_EXT_MEM_MODE_APPEND             (1U)
#define VIN_EXT_MEM_MODE_NUM                (2U)

/*  0: Rotate back to 1st buffer when the last buffer is reached.
 *  1: Stall when the last buffer is reached, pending system for adding more buffers.
*/
#define VIN_EXT_MEM_CTRL_CIRCULAR           (0U)
#define VIN_EXT_MEM_CTRL_WAIT_APPEND        (1U)
#define VIN_EXT_MEM_CTRL_NUM                (2U)

/* 20190102, from ChenHan, uCode use 7bit for ExtMemNum */
#define EXT_MEM_MAX_NUM                     (127U)
#define EXT_MEM_VIN_MAX_NUM                 (64U)  //currently ucode support this number when BrandNewAllocType
#define EXT_MEM_VPROC_MAX_NUM               (16U)  //currently ucode support this number when BrandNewAllocType

/* event_mask_bit_pos_t, refer to event_mask_t */
#define DSP_BIT_POS_EXT_MAIN                (0U)
#define DSP_BIT_POS_EXT_PREV_A              (1U)
#define DSP_BIT_POS_EXT_PREV_B              (2U)
#define DSP_BIT_POS_EXT_PREV_C              (3U)
#define DSP_BIT_POS_EXT_LN_DEC              (4U)
#define DSP_BIT_POS_EXT_HIER_0              (5U)
#define DSP_BIT_POS_EXT_COMP_RAW            (6U)
#define DSP_BIT_POS_EXT_MAIN_ME             (7U)
#define DSP_BIT_POS_EXT_PREV_A_ME           (8U)
#define DSP_BIT_POS_EXT_PREV_B_ME           (9U)
#define DSP_BIT_POS_EXT_PREV_C_ME           (10U)
#define DSP_BIT_POS_EXT_MCTS                (11U)
#define DSP_BIT_POS_EXT_MCTF                (12U)
#define DSP_BIT_POS_EXT_HIER_Y12            (13U)
#define DSP_BIT_POS_EXT_C2Y_Y12             (14U)
#define DSP_BIT_POS_EXT_PREV_D_ME           (15U)
#define DSP_BIT_POS_VPROC_RPT               (16U)
#define DSP_BIT_POS_VPROC_SKIP              (17U)
#define DSP_BIT_POS_ENC_START               (18U)
#define DSP_BIT_POS_ENC_STOP                (19U)
#define DSP_BIT_POS_EXT_IR                  (20U)
#define DSP_BIT_POS_EXT_NUM                 (21U)

/*
 * Raw compression type in CMD_VIN_INITIATE_RAW_CAP_TO_EXT_BUF
 * 0: No compression
 * 1: Compression with 6.75 bit/pix ratio
 */
#define DSP_VIN_CMPR_NONE                   (0U)
#define DSP_VIN_CMPR_6_75                   (1U)

/*
 * Instruction after raw capture done
 *  0: Back IDLE mode. DSP won't touch VIN HW, and only use timer to drive the cmd/msg exchange timing.
 *  1: Back to internal raw buffer. Those raw frames will pass to Vproc.
 */
#define DSP_VIN_CAP_DONE_2_IDLE             (0U)
#define DSP_VIN_CAP_DONE_2_INT_BUF          (1U)
#define NUM_VIN_CAP_DONE                    (2U)

/*
 * Capture buffer control
 * 0: Raw capture only. Those raw frames won't pass to Vproc
 * 1: Raw frame will pass to Vproc.
 */
#define DSP_VIN_CAP_ONLY                    (0U)
#define DSP_VIN_CAP_TO_YUV                  (1U)
#define NUM_VIN_CAP_BUF_CTRL                (2U)

/* raw_cap_sync_event in CMD_VIN_INITIATE_RAW_CAP_TO_EXT_BUF:
 *  0: None.
 *  1: Sync first raw capture with encode start, so the first raw capture is the first frame in encoded system.
*/
#define VIN_RAW_CAP_SYNC_NONE                   (0U)
#define VIN_RAW_CAP_SYNC_TO_ENC                 (1U)
#define NUM_VIN_RAW_CAP_SYNC                    (2U)

#define DSP_MAX_VIN_CAP_SLICE_NUM               (10U)

#define DSP_VIN_CAP_SLICE_PRESEND_MARKER        (0xFU)

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VOUT (5)
\*-----------------------------------------------------------------------------------------------*/
/* VOUT_SRC */
#define DSP_VOUT_SRC_DFTIMG     (0U)
#define DSP_VOUT_SRC_BG         (1U)
#define DSP_VOUT_SRC_VCAP       (2U)
#define DSP_VOUT_SRC_POSTP      (3U) // obsolete in cv5x
#define DSP_VOUT_SRC_DEC        (4U)

/* VOUT CSC param depth */
#define DSP_VOUT_CSC_MATRIX_DEPTH   (9U)

/* HL_ENC_VOUT_SOURCE_e */
#define HL_VOUT_SOURCE_VIDEO_CAP    (0U)
#define HL_VOUT_SOURCE_NONE         (1U)

/* AMBA_DSP_VOUT_FLIP_e */
#define DSP_NO_FLIP             (0U)
#define DSP_V_H_FLIP            (1U)
#define DSP_H_FLIP              (2U)
#define DSP_V_FLIP              (3U)
#define DSP_FLIP_NUM            (4U)

/* AMBA_DSP_VOUT_ROTATE_e */
#define DSP_NO_ROTATE           (0U)
#define DSP_ROTATE_90_DEGREE    (1U)
#define DSP_ROTATE_NUM          (2U)

/* Frame rate vout : AMBA_DSP_VIN_ENC_FRAME_RATE_e */
#define VOUT_FRAME_RATE_29_97   (0U)    /* non-integral frame rate */
#define VOUT_FRAME_RATE_59_94   (1U)
#define VOUT_FRAME_RATE_25      (25U)
#define VOUT_FRAME_RATE_50      (50U)

/* Frame rate type : AMBA_DSP_FRAME_RATE_TYPE_e */
#define VOUT_FRAME_RATE         (0U)
#define VIN_ENC_FRAME_RATE      (1U)

/* DSP_VOUT_RESET_STATE_e */
#define DSP_VOUT_RESET_PROGRAMED        (0x0U)
#define DSP_VOUT_RESET_SET              (0x1U)
#define DSP_VOUT_RESET_DISPLAY_SETUP    (0x2U)
#define DSP_VOUT_RESET_MIXER_SETUP      (0x4U)

/* DSP OSD MODE */
#define DSP_OSD_SRC_MAPPED_IN           (0U)
#define DSP_OSD_SRC_DIRECT_IN_16        (1U)
#define DSP_OSD_SRC_DIRECT_IN_32        (2U)

/* DSP OSD DIR MODE : OSD_DIR_MODE */
#define DSP_OSD_MODE_VYU565             (0U)
#define DSP_OSD_MODE_UYV565             (1U)
#define DSP_OSD_MODE_AYUV4444           (2U)
#define DSP_OSD_MODE_RGBA4444           (3U)
#define DSP_OSD_MODE_BGRA4444           (4U)
#define DSP_OSD_MODE_ABGR4444           (5U)
#define DSP_OSD_MODE_ARGB4444           (6U)
#define DSP_OSD_MODE_AYUV1555           (7U)
#define DSP_OSD_MODE_YUV555             (8U)
#define DSP_OSD_MODE_RGBA5551           (9U)
#define DSP_OSD_MODE_BGRA5551           (10U)
#define DSP_OSD_MODE_ABGR1555           (11U)
#define DSP_OSD_MODE_ARGB1555           (12U)
#define DSP_OSD_MODE_AYUV8888           (27U)
#define DSP_OSD_MODE_RGBA8888           (28U)
#define DSP_OSD_MODE_BGRA8888           (29U)
#define DSP_OSD_MODE_ABGR8888           (30U)
#define DSP_OSD_MODE_ARGB8888           (31U)
#define DSP_OSD_MODE_NUM                (32U)

/* DSP TV system */
#define DSP_TVE_CFG_SIZE                (4U*128U) //U32*128

/*-----------------------------------------------------------------------------------------------*\
 * CAT_ENC (6)
\*-----------------------------------------------------------------------------------------------*/
/* ENC_MAX_TYPES_NUM */
#define DSP_ENC_MAX_TYPES_ALL   (0U)
#define DSP_ENC_MAX_TYPES_H264  (1U)
#define DSP_ENC_MAX_TYPES_NUM   (2U)

/* enc_smem_encode_t */
#define DSP_SMEM_ENC_OFF        (0U)
#define DSP_SMEM_ENC_TAR        (1U)
#define DSP_SMEM_ENC_TAR_REF    (2U)

/* Enc Scan format, enc_paff_mode_t */
#define DSP_ENC_PAFF_ALL_FRM    (1U)
#define DSP_ENC_PAFF_ALL_FLD    (2U)

/* enc_start_method_t */
#define DSP_ENC_START_FROM_CMD      (0U)
#define DSP_ENC_START_FROM_PICINFO  (1U)

/* enc_stop_method_t */
#define DSP_ENC_STOP_FROM_CMD       (0U)
#define DSP_ENC_STOP_FROM_PICINFO   (1U)

/* enc_max_resol_t */
#define DSP_ENC_MAX_RESOL_4096          (0U)
#define DSP_ENC_MAX_RESOL_3840          (1U)
#define DSP_ENC_MAX_RESOL_1920          (2U)
#define DSP_ENC_MAX_RESOL_1280          (3U)
#define DSP_ENC_MAX_RESOL_JPEG          (4U)
#define DSP_ENC_MAX_RESOL_4000x3008     (5U)
#define DSP_ENC_MAX_RESOL_USER_DEFINED  (6U)
#define DSP_ENC_MAX_RESOL_NUM           (7U)

#define VIN_ENC_FRAME_RATE_29_97_I      (0U)    /* non-integral frame rate */
#define VIN_ENC_FRAME_RATE_29_97_P      (1U)
#define VIN_ENC_FRAME_RATE_59_94_I      (2U)
#define VIN_ENC_FRAME_RATE_59_94_P      (3U)

/* Intra-Refresh */
#define ENC_GOP_INFINTE_N               (0xFFFFU)
#define ENC_MAX_IR_CYCLE                (0xFEU)

/* hevc_perf_mode_t */
#define ENC_PERF_DEFAULT                    (0U)
#define ENC_PERF_4KP30_FASTEST_P1           (1U)
#define ENC_PERF_FAST_800                   (10U)  // 800 cycles/CTB

#define DSP_ENC_DUMMY_MARK                  (0xDEADBEEFU)  /* Dummy encode frame */
//#define DSP_ENC_BITS_INFO_STEPSIZE      (64U)
//#define DSP_ENC_END_MARK                (0xFFFFFFU)

/* rc_mode_t */
#define DSP_ENC_RC_OFF                  (0U)
#define DSP_ENC_RC_CBR                  (1U)
#define DSP_ENC_RC_CBR_TEST             (2U) /* reserved for internal use*/
#define DSP_ENC_RC_VBR                  (3U)
#define DSP_ENC_RC_SIMPLE               (4U) /* simple CBR */
#define DSP_ENC_RC_SIMPLE_VBR           (5U) /* simple VBR */
#define DSP_ENC_RC_CMPR_STRO            (6U) /* compress stereo */

/* scalelist_opt in CMD_ENCODER_SETUP: (H264 field only)
 *  0: Disable scaling list.
 *  1: Enable default scaling list.
 *  others: X>=2: Enable customized scaling list No.(X-1).
*/
#define ENC_SCALING_DISABLE             (0U) /* non-integral frame rate */
#define ENC_SCALING_DEFAULT             (1U)

/* cout_dwidth_mul in CMD_DSP_ENC_FLOW_MAX_CFG:
 * bs_buf_width = 2048 * cout_dwidth_mul
 * bs_buf_height = bs_size / bs_buf_width
 * MAX(bs_size) = MAX(bs_buf_width) * MAX(bs_buf_height) = 32768 * 8191 = 268402688 = 255.9MB
 * MAX(cout_dwidth_mul) = MAX(bs_buf_width)/2048 = 16
*/
#define DSP_ENC_MAX_BS_BUF_WIDTH        (32768U)
#define DSP_ENC_MAX_BS_BUF_HEIGHT       (8191U)
#define DSP_ENC_MAX_BS_DWIDTH_MULTI     (16U)
#define DSP_ENC_MAX_BS_BUF_SIZE         (268402688U)

#define DSP_DEC_MAX_BS_BUF_SIZE         (67108864U) //uCode decode bits buffer size limitation (64MB)

/* Compression Stereo Config */
#define DSP_CMPR_STRO_NUM       (2U) //MSB/LSB
#define DSP_CMPR_STRO_RES_NUM   (3U) // three resolution

typedef struct {
  UINT32 ComplexEstimation[DSP_CMPR_STRO_NUM];  // ie. complex_estimate
  UINT32 OverHead[DSP_CMPR_STRO_NUM];           // ie. oh
  UINT32 PixelNumber;                           // ie. pix_per_res

  UINT32 Rsvd[8U]; //padding
} DSP_ENC_GRP_CMPR_STRO_RES_CFG_s;

typedef struct {
  UINT32 HeaderSize;    // sizeof(DSP_ENC_GRP_CMPR_STRO_HEADER_s), ie. cs_comm_cfg_dsize
  UINT32 ResCfgSize;    // sizeof(DSP_ENC_GRP_CMPR_STRO_RES_CFG_s), ie. cs_res_cfg_dsize_each
  UINT32 FrmBits;       // ie. alloc_bits_per_frame
  UINT32 ComplexIIR;    // ie. complexity_iir
  UINT32 FeedBack;      // ie. feedback
  UINT8  QpMsb;         // ie. msb_qp
  UINT8  QpMinLsb;      // ie. lsb_qp_min
  UINT8  QpMaxLsb;      // ie. lsb_qp_max
  UINT8  NumRefCfg;     // ie. res_num

  UINT32 Rsvd[8U]; //padding
} DSP_ENC_GRP_CMPR_STRO_HEADER_s;

typedef struct {
  DSP_ENC_GRP_CMPR_STRO_HEADER_s    Header;
  DSP_ENC_GRP_CMPR_STRO_RES_CFG_s   ResCfg[DSP_CMPR_STRO_RES_NUM];
} DSP_ENC_GRP_CMPR_STRO_s;

#define DSP_MV_BUF_CTB_IDX          (0U)
#define DSP_MV_BUF_COLOC_MV_IDX     (1U)
#define DSP_MV_BUF_NUM              (2U)

#define DEFAULT_RECON_NUM_IP        (2U)
#define DEFAULT_RECON_NUM_IPB       (3U)
#define DEFAULT_REF_NUM             (1U)
#define DEFAULT_REF_NUM_B           (2U)

/* Affinity */
#define DSP_ENC_MAX_CORE_NUM        (2U)

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DEC (7)
\*-----------------------------------------------------------------------------------------------*/
/* dec_codec_type_t */
#define DSP_DEC_TYPE_H264           (0U)
#define DSP_DEC_TYPE_H265           (1U)
#define DSP_DEC_TYPE_JPEG           (2U)
#define DSP_DEC_TYPE_NUM            (3U)

// FIXME: merge CAT_STILLDEC and CAT_H264DEC to CAT_DEC, need to check define items
/* use_cust_dec_buf in CMD_STILLDEC_SETUP:
 *  0: Default buffer allocated by DSP.
 *  1: Customized buffer allocated by APP.
*/
#define    STILLDEC_BUF_TYPE_DEF  0U     /* non-integral frame rate */
#define    STILLDEC_BUF_TYPE_CUS  1U

#define    IMG_NO_FLIP              0U
#define    IMG_FLIP_HORIZONTALLY    1U
#define    IMG_FLIP_VERTICALLY      2U

#define    IMG_NO_ROTATE           0U
#define    IMG_ROTATE_90_DEGREE    1U
#define    IMG_ROTATE_180_DEGREE   2U
#define    IMG_ROTATE_270_DEGREE   3U

/* decoder_type in decoder hdr:
 *  0:      Full compliant type.
 *  Bit0:   No FMO type
 *  Bit1:   I only type
 *  Bit2:   Ambarella type
 *  Bit3:   AU bounded type
 *  Bit4:   Low delay type
 *  Bit7:   Coding only type.
*/
#define    HDEC_FULLCOMP_TYPE    0U
#define    HDEC_NOFMO_TYPE       1U
#define    HDEC_IONLY_TYPE       2U
#define    HDEC_AMBA_TYPE        4U
#define    HDEC_AU_BOUND_TYPE    8U
#define    HDEC_LOWDELAY_TYPE    16U
#define    HDEC_IPONLY_TYPE      64U
#define    HDEC_CODING_ONLY_TYPE 128U

/* conceal_mode in CMD_H264DEC_SETUP:
 *  0:  Disabled
 *  1:  Zero-motion concealment
*/
#define    H264DEC_CONCEAL_MODE_DISABLE     0U
#define    H264DEC_CONCEAL_MODE_ZERO_MOTION 1U
#define    NUM_H264DEC_CONCEAL_MODE         2U

/* To identify the output of SPROC message */
#define    DSP_STILL_MAIN_YUV   0x00U     /* Piv MainYuv */
#define    DSP_STILL_NAIL_YUV      1U     /* Y2Y */
#define    DSP_STILL_RAW_AAA       2U     /* Raw2Raw AAA */
#define    DSP_STILL_RAW_RESCALE   3U     /* Raw2Raw Resample */
#define    DSP_STILL_JPG_DATA      4U     /* Jpeg data */
#define    DSP_STILL_ME1_YUV       5U     /* Y2Y for Me1 */
#define    DSP_STILL_PIV_SCRN_YUV  6U     /* Piv Screennail Yuv, internal */
#define    DSP_STILL_PIV_THMB_YUV  7U     /* Piv Thumbnail Yuv, internal */
#define    DSP_STILL_PIV_PREV_YUV  8U     /* Piv Preview Yuv, internal */
#define    DSP_STILL_ALPHA_BLEND   9U     /* Alpha blending */
#define    NUM_STILL_JOB_ID       10U

/* To identify the output of VPROC message */
#define    DSP_VIDEO_EXT_MAIN_YUV    0U     /* Main Yuv */
#define    DSP_VIDEO_EXT_PIP_YUV     1U     /* Pip Yuv */
#define    DSP_VIDEO_EXT_PREVA_YUV   2U     /* Preview A Yuv */
#define    DSP_VIDEO_EXT_PREVB_YUV   3U     /* Preview B Yuv */
#define    DSP_VIDEO_EXT_MAIN_ME_YUV 4U     /* Main ME Yuv */
#define    DSP_VIDEO_EXT_PIP_ME_YUV  5U     /* Pip ME Yuv */
#define    NUM_VIDEO_JOB_ID          6U

/* PostProc information */
#define PP_VOUT_ACTIVE_TIMEOUT  (5000)

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Main.c
\*-----------------------------------------------------------------------------------------------*/
//INT32 AmbaDSP_FrameRateMap(UINT8 Type, AMBA_DSP_FRAME_RATE_s *pFrameRate);
//UINT32 AmbaDSP_GetDspSysState(void);
//AMBA_DSP_VERSION_INFO_s *AmbaGetDspUCodeVerInfo(void);
//extern INT32 DSP_SwitchProfile(UINT8 ProfState, UINT32 TimeOutMs);
extern void HL_SwitchDspSysState(const UINT8 CurOpModeStatus);
#define SWITCH_PROF_PREPROC     (0U)
#define SWITCH_PROF_PROC        (1U)
#define SWITCH_PROF_POSTPROC    (2U)
extern UINT32 HL_SwitchProfile(UINT8 TargetProfile, UINT8 Timing);
extern void HL_GetUCodeVerInfo(AMBA_DSP_VERSION_INFO_s *pInfo);
#define AVC_MB_SIZE         (16U) //16 pixel
#define AVC_MV_SIZE         (4U)
extern UINT32 AmbaDSP_CalcEncAvcMvBuf(UINT16 Width, UINT16 Height, UINT16 *pBufPitch, UINT16 *pBufWidth, UINT16 *pBufHeight);

extern void HL_GetProtectBufInfo(ULONG *Addr, UINT32 *Size);

/* Event extern function */
extern UINT32 HL_EventInit(void);


#if 0
/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Vin.c
\*----------------------------------------------------------------------------------------------*/
INT32 AmbaHL_VinConfigPostOpCheck(AMBA_DSP_VIN_OP_POST_e Type, UINT32 VinID);
INT32 AmbaHL_VinConfigPostOp(AMBA_DSP_VIN_OP_POST_e Type, UINT32 VinID);
#endif

/* vout extern functions */
extern UINT32 HL_VoutResetSetup(const UINT8 VoutIdx, const UINT8 WriteMode);
extern UINT32 HL_VoutDisplaySetup(const UINT8 VoutIdx, const UINT8 WriteMode);
extern UINT32 HL_VoutMixerSetup(const UINT8 VoutIdx, const UINT8 WriteMode);
extern UINT32 HL_VoutOsdSetup(const UINT8 VoutIdx, const UINT8 WriteMode);
extern UINT32 HL_VoutVideoSetup(const UINT8 VoutIdx, const UINT8 WriteMode);
extern UINT32 HL_VoutVideoSetupSourceSelect(const UINT8 VoutIdx, const UINT8 Src, const UINT8 Content);
extern UINT32 HL_VoutDveSetup(const UINT8 VoutIdx, const UINT8 WriteMode);
extern UINT32 HL_VoutGammaSetup(const UINT8 VoutIdx, const UINT8 WriteMode);

/* Liveview extern functions */
#define VOUT_CFG_TIME_PRE_VIN               (0U)    //when booting
#define VOUT_CFG_TIME_POST_VIN              (1U)    //when booting
#define VOUT_CFG_TIME_PRE_VIN_PROF_SWITCH   (2U)
#define VOUT_CFG_TIME_POST_VIN_PROF_SWITCH  (3U)
extern void HL_LiveviewVoutSetupImpl(const UINT8 WriteMode, const UINT8 Timing);
extern UINT32 HL_LiveviewCmdPrepare(const UINT8 WriteMode, const UINT8 IsPartialEnable);
extern UINT32 HL_LvBootPostProcVirtVinSetupImpl(void);

/* StillCapture extern functions */
extern UINT32 HL_StillCaptureYuvImpl(const UINT16 NumCapInstance, const UINT16 *pCapInstance);
#define DSP_DATACAP_CALTYPE_LUMA    (0U)
#define DSP_DATACAP_CALTYPE_ME0     (1U)
#define DSP_DATACAP_CALTYPE_ME1     (2U)
#define DSP_DATACAP_CALTYPE_PROCRAW (3U)
#define DSP_DATACAP_CALTYPE_NUM     (4U)
extern UINT32 HL_StillCaptureCalcExtBufImpl(const UINT16 StreamIdx, const UINT16 CalType, AMBA_DSP_BUF_s *pExtBuf);
#define DSP_DATACAP_INFINITE_CAP_NUM    (0xFFFFFFFFU)

/* Context extern functions */
extern void HL_CreateCtxInitMtx(void);

#endif  /* AMBA_DSP_PRIV_H */

