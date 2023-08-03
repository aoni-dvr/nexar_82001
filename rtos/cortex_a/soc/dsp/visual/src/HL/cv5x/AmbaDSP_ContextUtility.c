/**
*  @file AmbaDSP_ContextUtility.c
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
*  @details Implement of HL context utility APIs
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Int.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"

/* Buffer use for DSP */
#ifdef SUPPORT_IWONG_UCODE
static DSP_SRCBUF_CFG CtxDspSrcBufCfg[DSP_SRC_BUF_NUM] GNU_ALIGNED_CACHESAFE;
#endif
#ifdef SUPPORT_MAX_UCODE
static CTX_BATCHQ_INFO_s CtxDspBatchQInfo[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_BATCH_CMD_POOL_NUM] GNU_SECTION_NOZEROINIT;
#endif

/* Mutex Table */
static osal_mutex_t CtxInitMtx GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxResMtx GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxDspInstMtx GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxSensorMtx[AMBA_DSP_MAX_VIN_NUM * AMBA_DSP_MAX_VIN_SENSOR_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxVinMtx[DSP_VIN_MAX_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxViewZoneMtx[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxYuvStrmMtx[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxYuvStrmEfctSyncMtx[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxVprocMtx[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxBatchMapMtx[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxBatchQBufMtx[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxStrmMtx[AMBA_DSP_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxVoutMtx[NUM_VOUT_IDX] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxEncoderMtx GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxDecoderMtx GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxVidDecMtx[AMBA_DSP_MAX_DEC_STREAM_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxStlMtx GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxDataCapMtx[AMBA_DSP_MAX_DATACAP_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxTimeLapseMtx[AMBA_DSP_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;
static osal_mutex_t CtxEncGrpMtx[AMBA_DSP_MAX_ENC_GRP_NUM] GNU_SECTION_NOZEROINIT;

/* Ctx Table */
static UINT8 CreateCtxInitMtx = 0U;
static UINT8 CtxInitFlag = 0U;
static CTX_RESOURCE_INFO_s CtxResInfo GNU_SECTION_NOZEROINIT;
static CTX_SENSOR_INFO_s CtxSensorInfo[AMBA_DSP_MAX_VIN_NUM/* only care physical vin */ * AMBA_DSP_MAX_VIN_SENSOR_NUM] GNU_SECTION_NOZEROINIT;
static CTX_VIN_INFO_s CtxVinInfo[DSP_VIN_MAX_NUM] GNU_SECTION_NOZEROINIT;
static CTX_VIEWZONE_INFO_s CtxViewZoneInfo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static CTX_YUV_STRM_INFO_s CtxYuvStrmInfo[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;
static CTX_YUVSTRM_EFCT_SYNC_CTRL_s CtxYuvStrmEfctSyncCtrl[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;
static CTX_EFCT_SYNC_JOB_SET_s CtxYuvStrmEfctSyncJobSet[AMBA_DSP_MAX_YUVSTRM_NUM][MAX_GROUP_CMD_POOL_NUM] GNU_SECTION_NOZEROINIT;
static CTX_YUV_STRM_LAYOUT_s CtxYuvStrmEfctSyncJobSetLayout[AMBA_DSP_MAX_YUVSTRM_NUM][MAX_GROUP_CMD_POOL_NUM] GNU_SECTION_NOZEROINIT;
static CTX_VPROC_INFO_s CtxVprocInfo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static CTX_STREAM_INFO_s CtxStrmInfo[AMBA_DSP_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;
static CTX_VOUT_INFO_s CtxVoutInfo[NUM_VOUT_IDX] GNU_SECTION_NOZEROINIT;
static CTX_ENCODER_INFO_s CtxEncoderInfo GNU_SECTION_NOZEROINIT;
static CTX_DECODER_INFO_s CtxDecoderInfo GNU_SECTION_NOZEROINIT;
static CTX_VID_DEC_INFO_s CtxVidDecInfo[AMBA_DSP_MAX_DEC_STREAM_NUM] GNU_SECTION_NOZEROINIT;
static CTX_STILL_INFO_s CtxStlInfo GNU_SECTION_NOZEROINIT;
static CTX_DSP_INST_INFO_s CtxDspInstanceInfo GNU_SECTION_NOZEROINIT;
static CTX_DATACAP_INFO_s CtxDataCapInfo[AMBA_DSP_MAX_DATACAP_NUM] GNU_SECTION_NOZEROINIT;
static CTX_TIMELAPSE_INFO_s CtxTimeLapseInfo[AMBA_DSP_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;
static CTX_ENC_GRP_INFO_s CtxEncGrpInfo[AMBA_DSP_MAX_ENC_GRP_NUM] GNU_SECTION_NOZEROINIT;

/* Default Ctx Ptr */
static CTX_RESOURCE_INFO_s HL_DefCtxResInfoPtr GNU_SECTION_NOZEROINIT;
static CTX_VIEWZONE_INFO_s HL_DefCtxViewZoneInfoPtr GNU_SECTION_NOZEROINIT;
static CTX_YUV_STRM_INFO_s HL_DefCtxYuvStrmInfoPtr GNU_SECTION_NOZEROINIT;
static CTX_YUVSTRM_EFCT_SYNC_CTRL_s HL_DefCtxYuvStrmEfctSyncCtrlPtr GNU_SECTION_NOZEROINIT;
static CTX_EFCT_SYNC_JOB_SET_s HL_DefCtxYuvStrmEfctSyncJobSetPtr GNU_SECTION_NOZEROINIT;
static CTX_YUV_STRM_LAYOUT_s HL_DefCtxYuvStrmEfctLayoutPtr GNU_SECTION_NOZEROINIT;
CTX_RESOURCE_INFO_s *HL_CtxResInfoPtr = &HL_DefCtxResInfoPtr;
CTX_VIEWZONE_INFO_s *HL_CtxViewZoneInfoPtr = &HL_DefCtxViewZoneInfoPtr;
CTX_YUV_STRM_INFO_s *HL_CtxYuvStrmInfoPtr = &HL_DefCtxYuvStrmInfoPtr;
CTX_YUVSTRM_EFCT_SYNC_CTRL_s *HL_CtxYuvStrmEfctSyncCtrlPtr = &HL_DefCtxYuvStrmEfctSyncCtrlPtr;
CTX_EFCT_SYNC_JOB_SET_s *HL_CtxYuvStrmEfctSyncJobSetPtr = &HL_DefCtxYuvStrmEfctSyncJobSetPtr;
CTX_YUV_STRM_LAYOUT_s *HL_CtxYuvStrmEfctLayoutPtr = &HL_DefCtxYuvStrmEfctLayoutPtr;

/* Cmd buffer pool */
#define CTX_CMD_BUF_POOL_NUM            (8U)
typedef struct {
    ULONG BaseAddr;
    UINT8 PoolUsage[CTX_CMD_BUF_POOL_NUM];
    osal_mutex_t PoolMtx;
} CTX_CMD_BUF_INFO_s;

static CTX_CMD_BUF_INFO_s CtxCmdBufInfo GNU_SECTION_NOZEROINIT;

cmd_dsp_config_t *HL_DefCtxCmdBufPtrSys;
cmd_dsp_hal_inf_t *HL_DefCtxCmdBufPtrHal;
cmd_dsp_suspend_profile_t *HL_DefCtxCmdBufPtrSus;
cmd_set_debug_level_t *HL_DefCtxCmdBufPtrDbgLvl;
cmd_print_th_disable_mask_t *HL_DefCtxCmdBufPtrDbgthd;
cmd_binding_cfg_t *HL_DefCtxCmdBufPtrBind;
cmd_dsp_activate_profile_t *HL_DefCtxCmdBufPtrActPrf;
cmd_dsp_vproc_flow_max_cfg_t *HL_DefCtxCmdBufPtrVpcMax;
cmd_dsp_vin_flow_max_cfg_t *HL_DefCtxCmdBufPtrVinMax;
cmd_dsp_set_profile_t *HL_DefCtxCmdBufPtrSetPrf;
cmd_dsp_enc_flow_max_cfg_t *HL_DefCtxCmdBufPtrEncMax;
cmd_dsp_dec_flow_max_cfg_t *HL_DefCtxCmdBufPtrDecMax;
cmd_dsp_vout_flow_max_cfg_t *HL_DefCtxCmdBufPtrVoutMax;
cmd_vproc_cfg_t *HL_DefCtxCmdBufPtrVpc;
cmd_vproc_setup_t *HL_DefCtxCmdBufPtrVpcSetup;
cmd_vproc_ik_config_t *HL_DefCtxCmdBufPtrVpcIkCfg;
cmd_vproc_img_pyramid_setup_t *HL_DefCtxCmdBufPtrVpcPymd;
cmd_vproc_prev_setup_t *HL_DefCtxCmdBufPtrVpcPrev;
cmd_vproc_lane_det_setup_t *HL_DefCtxCmdBufPtrVpcLndt;
cmd_vproc_set_ext_mem_t *HL_DefCtxCmdBufPtrVpcExtMem;
cmd_vproc_stop_t *HL_DefCtxCmdBufPtrVpcStop;
cmd_vproc_osd_blend_t *HL_DefCtxCmdBufPtrVpcOsd;
cmd_vproc_pin_out_deci_t *HL_DefCtxCmdBufPtrVpcDeci;
cmd_vproc_fov_grp_cmd_t *HL_DefCtxCmdBufPtrVpcFovGrp;
cmd_vproc_echo_t *HL_DefCtxCmdBufPtrVpcEcho;
cmd_vproc_multi_stream_pp_t *HL_DefCtxCmdBufPtrVpcPp;
cmd_vproc_set_effect_buf_img_sz_t *HL_DefCtxCmdBufPtrVpcEff;
cmd_vproc_multi_chan_proc_order_t *HL_DefCtxCmdBufPtrVpcChOrd;
cmd_vproc_set_vproc_grping *HL_DefCtxCmdBufPtrVpcGrp;
cmd_vproc_warp_group_update_t *HL_DefCtxCmdBufPtrVpcWrpGrp;
cmd_vproc_set_testframe_t *HL_DefCtxCmdBufPtrVpcTestFrm;
cmd_vin_start_t *HL_DefCtxCmdBufPtrVinStart;
cmd_vin_idle_t *HL_DefCtxCmdBufPtrVinIdle;
cmd_vin_set_ext_mem_t *HL_DefCtxCmdBufPtrVinExtMem;
cmd_vin_send_input_data_t *HL_DefCtxCmdBufPtrVinSndData;
cmd_vin_initiate_raw_cap_to_ext_buf_t *HL_DefCtxCmdBufPtrVinInitCap;
cmd_vin_attach_proc_cfg_to_cap_frm_t *HL_DefCtxCmdBufPtrVinAttfrm;
cmd_vin_attach_event_to_raw_t *HL_DefCtxCmdBufPtrVinAttRaw;
cmd_vin_cmd_msg_dec_rate_t *HL_DefCtxCmdBufPtrVinMsgRate;
cmd_vin_ce_setup_t *HL_DefCtxCmdBufPtrVinCe;
cmd_vin_hdr_setup_t *HL_DefCtxCmdBufPtrVinHdr;
cmd_vin_set_frm_lvl_flip_rot_control_t *HL_DefCtxCmdBufPtrVinFlip;
cmd_vin_set_fov_layout_t *HL_DefCtxCmdBufPtrVinLayout;
cmd_vin_set_frm_vproc_delay_t *HL_DefCtxCmdBufPtrVinDly;
cmd_vin_attach_sideband_info_to_cap_frm_t *HL_DefCtxCmdBufPtrVinSidBend;
cmd_vin_vout_lock_setup_t *HL_DefCtxCmdBufPtrVinVoutLock;
cmd_vin_attach_metadata_t *HL_DefCtxCmdBufPtrVinAttDta;
cmd_vin_temporal_demux_setup_t *HL_DefCtxCmdBufPtrVinTd;
cmd_vout_mixer_setup_t *HL_DefCtxCmdBufPtrVoutMx;
cmd_vout_video_setup_t *HL_DefCtxCmdBufPtrVoutVdo;
cmd_vout_default_img_setup_t *HL_DefCtxCmdBufPtrVoutDef;
cmd_vout_osd_setup_t *HL_DefCtxCmdBufPtrVoutOsd;
cmd_vout_osd_buf_setup_t *HL_DefCtxCmdBufPtrVoutOsdBuf;
cmd_vout_osd_clut_setup_t *HL_DefCtxCmdBufPtrVoutOsdClut;
cmd_vout_display_setup_t *HL_DefCtxCmdBufPtrVoutDisp;
cmd_vout_dve_setup_t *HL_DefCtxCmdBufPtrVoutDve;
cmd_vout_reset_t *HL_DefCtxCmdBufPtrVoutReset;
cmd_vout_display_csc_setup_t *HL_DefCtxCmdBufPtrVoutCac;
cmd_vout_digital_output_mode_setup_t *HL_DefCtxCmdBufPtrVoutDigMode;
cmd_vout_gamma_setup_t *HL_DefCtxCmdBufPtrVoutGamma;
cmd_encoder_setup_t *HL_DefCtxCmdBufPtrEnc;
cmd_encoder_start_t *HL_DefCtxCmdBufPtrEncStart;
cmd_encoder_stop_t *HL_DefCtxCmdBufPtrEncStop;
cmd_encoder_jpeg_setup_t *HL_DefCtxCmdBufPtrEncJpg;
encoder_realtime_setup_t *HL_DefCtxCmdBufPtrEncRt;
cmd_decoder_setup_t *HL_DefCtxCmdBufPtrDec;
cmd_decoder_start_t *HL_DefCtxCmdBufPtrDecStart;
cmd_decoder_stop_t *HL_DefCtxCmdBufPtrDecStop;
cmd_decoder_bitsfifo_update_t *HL_DefCtxCmdBufPtrDecBsUpt;
cmd_decoder_speed_t *HL_DefCtxCmdBufPtrDecSpeed;
cmd_decoder_trickplay_t *HL_DefCtxCmdBufPtrDecTrick;
cmd_decoder_stilldec_t *HL_DefCtxCmdBufPtrDecStlDec;
set_vin_config_t *HL_DefCtxCmdBufPtrVinCfg;
lossy_compression_t *HL_DefCtxCmdBufPtrCmpr;

void HL_CreateCtxInitMtx(void)
{
    static char InitCtxMtx[] = "DspInitCtxMtx";

    (void)dsp_osal_mutex_init(&CtxInitMtx, InitCtxMtx);
    CreateCtxInitMtx = 1U;
}

static void HL_GetCtxInitFlag(const UINT8 MtxOpt, UINT8 *pInitFlag)
{
    if (CreateCtxInitMtx == 1U) {
        (void)HL_MtxLock(&CtxInitMtx, MtxOpt);
        (void)dsp_osal_memcpy(pInitFlag, &CtxInitFlag, sizeof(UINT8));
        (void)HL_MtxUnLock(&CtxInitMtx, MtxOpt);
    } else {
        (void)dsp_osal_memcpy(pInitFlag, &CtxInitFlag, sizeof(UINT8));
    }
}

static void HL_SetCtxInitFlag(const UINT8 MtxOpt, const UINT8 *pInitFlag)
{
    if (CreateCtxInitMtx == 1U) {
        (void)HL_MtxLock(&CtxInitMtx, MtxOpt);
        (void)dsp_osal_memcpy(&CtxInitFlag, pInitFlag, sizeof(UINT8));
        (void)HL_MtxUnLock(&CtxInitMtx, MtxOpt);
    } else {
        (void)dsp_osal_memcpy(&CtxInitFlag, pInitFlag, sizeof(UINT8));
    }
}

static inline UINT32 HL_CtxInitMtx(void)
{
    UINT32 Rval = OK;
    UINT8 i;

    static char ResCtxMtx[] = "DspResCtxMtx";
    static char InstCtxMtx[] = "DspInstCtxMtx";
    static char SensorCtxMtx[] = "DspSensorCtxMtx";
    static char VinCtxMtx[] = "DspVinCtxMutex";
    static char VZoneCtxMtx[] = "DspVZoneCtxMtx";
    static char YuvStrmCtxMtx[] = "DspYuvStrmCtxMtx";
    static char YuvStrmEfctSyncCtxMtx[] = "DspYuvStrmEfctSyncMtx";
    static char VprocCtxMtx[] = "DspVprocCtxMtx";
    static char BatchMapIdMtx[] = "DspBatchIdMapMtx";
    static char BatchQBufMtx[] = "DspBatchQbufMtx";
    static char StrmCtxMtx[] = "DspStrmCtxMtx";
    static char VoutCtxMtx[] = "DspVoutCtxMtx";
    static char EncCtxMtx[] = "DspEncCtxMtx";
    static char DecCtxMtx[] = "DspDecCtxMtx";
    static char VdecCtxMtx[] = "DspVdecCtxMutex";
    static char StlCtxMtx[] = "DspStlCtxMutex";
    static char DataCapCtxMtx[] = "DspDataCapCtxMutex";
    static char TimeLapseCtxMtx[] = "DspTimeLapseCtxMutex";
    static char EncGrpCtxMtx[] = "DspEncGrpCtxMutex";

    Rval |= dsp_osal_mutex_init(&CtxResMtx, ResCtxMtx);
    Rval |= dsp_osal_mutex_init(&CtxDspInstMtx, InstCtxMtx);
    for (i = 0U; i < (AMBA_DSP_MAX_VIN_NUM*AMBA_DSP_MAX_VIN_SENSOR_NUM); i++) {
        Rval |= dsp_osal_mutex_init(&CtxSensorMtx[i], SensorCtxMtx);
    }
    for (i = 0U; i < DSP_VIN_MAX_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxVinMtx[i], VinCtxMtx);
    }
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxViewZoneMtx[i], VZoneCtxMtx);
    }
    for (i = 0U; i < AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxYuvStrmMtx[i], YuvStrmCtxMtx);
    }
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxYuvStrmEfctSyncMtx[i], YuvStrmEfctSyncCtxMtx);
    }
    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxVprocMtx[i], VprocCtxMtx);
    }
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxBatchMapMtx[i], BatchMapIdMtx);
    }
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxBatchQBufMtx[i], BatchQBufMtx);
    }
    for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxStrmMtx[i], StrmCtxMtx);
    }
    for (i = 0U; i < NUM_VOUT_IDX; i++) {
        Rval |= dsp_osal_mutex_init(&CtxVoutMtx[i], VoutCtxMtx);
    }
    Rval |= dsp_osal_mutex_init(&CtxEncoderMtx, EncCtxMtx);
    Rval |= dsp_osal_mutex_init(&CtxDecoderMtx, DecCtxMtx);
    for (i = 0U; i < AMBA_DSP_MAX_DEC_STREAM_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxVidDecMtx[i], VdecCtxMtx);
    }
    Rval |= dsp_osal_mutex_init(&CtxStlMtx, StlCtxMtx);
    for (i = 0U; i < AMBA_DSP_MAX_DATACAP_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxDataCapMtx[i], DataCapCtxMtx);
    }
    for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxTimeLapseMtx[i], TimeLapseCtxMtx);
    }
    for (i = 0U; i < AMBA_DSP_MAX_ENC_GRP_NUM; i++) {
        Rval |= dsp_osal_mutex_init(&CtxEncGrpMtx[i], EncGrpCtxMtx);
    }
    return Rval;
}

static inline void HL_CtxInitCtx(void)
{
    UINT8 i = 0U;
    UINT16 j = 0U;

#ifdef SUPPORT_DSP_EXT_PIN_BUF
    ext_mem_desc_t *pExtMemDesc = NULL;

    DSP_GetDspVprocExtMemDescBuf(0U, &pExtMemDesc);
    (void)dsp_osal_memset(pExtMemDesc, 0, ((sizeof(ext_mem_desc_t)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_VPROC_EXT_MEM_TYPE_NUM));
#endif

    // VinCtrl
    for (i = 0U; i<DSP_VIN_MAX_NUM; i++) {
        CtxVinInfo[i].VinCtrl.RawSeqWp = 0xFFU;
        // Cmd be updated by default
        (void)dsp_osal_memset(&(CtxVinInfo[i].CmdUpdate), 0xFF, sizeof(CTX_CMD_NEED_UPDATE_s));

        for (j = 0U; j<AMBA_DSP_MAX_VIRT_CHAN_NUM; j++) {
            (void)dsp_osal_memset(&CtxVinInfo[i].TimeDivisionVinId[j][0U], 0xFFFF, sizeof(UINT16)*AMBA_DSP_MAX_VIN_TD_NUM);
        }
    }

    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        // BatchQ
        CtxViewZoneInfo[i].SourceTdIdx = VIEWZONE_SRC_TD_IDX_NULL;

        CtxViewZoneInfo[i].BatchQPoolDesc.StartAddr = 0U;
        CtxViewZoneInfo[i].BatchQPoolDesc.Size = MAX_BATCH_CMD_POOL_NUM;
        CtxViewZoneInfo[i].BatchQPoolDesc.Wp = MAX_BATCH_CMD_POOL_NUM - 1U;
        CtxViewZoneInfo[i].BatchQPoolDesc.Rp = MAX_BATCH_CMD_POOL_NUM - 1U;

        CtxViewZoneInfo[i].YuvInput.RawSeqWp = 0xFFU;
        CtxViewZoneInfo[i].PymdInput.RawSeqWp = 0xFFU;
        CtxViewZoneInfo[i].MainY12Input.RawSeqWp = 0xFFU;
        CtxViewZoneInfo[i].MainIrInput.RawSeqWp = 0xFFU;
        // Cmd be updated by default
        (void)dsp_osal_memset(&(CtxViewZoneInfo[i].CmdUpdate), 0xFF, sizeof(CTX_CMD_NEED_UPDATE_s));

#ifdef SUPPORT_DSP_EXT_PIN_BUF
        // VprocExtMem
        DSP_GetDspVprocExtMemDescBuf(i, &pExtMemDesc);
        for (j = 0U; j<MAX_VPROC_EXT_MEM_TYPE_NUM ;j++) {
            pExtMemDesc[j].memory_type = (UINT8)j;
            pExtMemDesc[j].max_daddr_slot = (UINT8)DEFAULT_EXT_MEM_DADDR_NUM;
            pExtMemDesc[j].pool_buf_num = (UINT8)DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;
        }
#endif
    }

    //YuvStrm
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        CtxYuvStrmInfo[i].PpStrmPoolDesc.StartAddr = 0U;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Size = MAX_GROUP_CMD_POOL_NUM;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Wp = MAX_GROUP_CMD_POOL_NUM - 1U;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Rp = MAX_GROUP_CMD_POOL_NUM - 1U;

        /* EffectBufDesc */
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.EffectBufDesc, 0xFFFF, sizeof(CtxYuvStrmInfo[i].Layout.EffectBufDesc));

        // EffectSyncCtrl
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.StartAddr = 0U;
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.Size = MAX_GROUP_CMD_POOL_NUM;
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.Wp = MAX_GROUP_CMD_POOL_NUM - 1U;
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.Rp = MAX_GROUP_CMD_POOL_NUM - 1U;
    }

    // Vout
    for (i = 0U; i < NUM_VOUT_IDX; i++) {
        CtxVoutInfo[i].PtsWp = 0xFFU;
    }

    // Still
    CtxStlInfo.RawInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
    CtxStlInfo.YuvInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
    CtxStlInfo.YuvEncVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
    CtxStlInfo.RawInVprocId = DSP_VPROC_IDX_INVALID;
    CtxStlInfo.YuvInVprocId = DSP_VPROC_IDX_INVALID;
    CtxStlInfo.EncStmId = DSP_VPROC_IDX_INVALID;

    CtxStlInfo.RawInVprocStatus = STL_VPROC_STATUS_IDLE;
    CtxStlInfo.YuvInVprocStatus = STL_VPROC_STATUS_IDLE;
    CtxStlInfo.YuvInVprocPin = DSP_VPROC_PIN_NUM;
    CtxStlInfo.RawInVprocPin = DSP_VPROC_PIN_NUM;
    CtxStlInfo.EncStatus = STL_ENC_STATUS_IDLE;

#ifndef SUPPORT_VPROC_RT_RESTART
    CtxStlInfo.Yuv422InVprocStatus = STL_VPROC_STATUS_IDLE;
    CtxStlInfo.Yuv422InVprocId = DSP_VPROC_IDX_INVALID;
#endif

    //DataCap

    // TimeLapse
    for (i = 0U; i<AMBA_DSP_MAX_STREAM_NUM; i++) {
        CtxTimeLapseInfo[i].VirtVinId = DSP_VIRT_VIN_IDX_INVALID;
        CtxTimeLapseInfo[i].Status = ENC_TIMELAPSE_STATUS_IDLE;
        CtxStrmInfo[i].TimeLapseId = DSP_TIMELAPSE_IDX_INVALID;

        /* QP */
        (void)dsp_osal_memset(&CtxStrmInfo[i].QpCfg, 255, sizeof(AMBA_DSP_VIDEO_QP_CONFIG_s));
    }

    // EncGrp
    for (i = 0U; i<AMBA_DSP_MAX_ENC_GRP_NUM; i++) {
        for (j = 0U; j<AMBA_DSP_MAX_ENC_GRP_STRM_NUM; j++) {
            CtxEncGrpInfo[i].StrmIdx[j] = DSP_ENC_GRP_STRM_IDX_INVALID;
        }
    }

    // Decode
    for (i = 0U; i < AMBA_DSP_MAX_DEC_STREAM_NUM; i++) {
        CtxVidDecInfo[i].DspState = DEC_OPM_INVALID;
        CtxVidDecInfo[i].YuvInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
        CtxVidDecInfo[i].ViewZoneId = DSP_VPROC_IDX_INVALID;
        for (j = 0U; j < AMBA_DSP_DEC_BITS_FORMAT_NUM; j++) {
            if (j == AMBA_DSP_DEC_BITS_FORMAT_JPEG) {
                CtxVidDecInfo[i].DecTypeId[j] = 0U;
            } else {
                CtxVidDecInfo[i].DecTypeId[j] = DSP_DEC_TYPE_IDX_INVALID;
            }
        }
    }
}

UINT32 HL_CtxInit(void)
{
    static dsp_cmd_t HL_DefCtxCmdBuf GNU_SECTION_NOZEROINIT;
    static dsp_cmd_t CtxCmdBuf[CTX_CMD_BUF_POOL_NUM] GNU_SECTION_NOZEROINIT;

    UINT32 Rval = OK;
    UINT16 i, j;
    UINT8 CtxInit = 0U;

    HL_GetCtxInitFlag(HL_MTX_OPT_GET, &CtxInit);
    if (CtxInit == 0U) {
        UINT32 *pArray = NULL;
        UINT8 *pU8Array = NULL;
        enc_cfg_t *pEncCfg = NULL;
        fov_layout_t *pFovLayout = NULL;
        vin_fov_cfg_t *pFovStartCfg = NULL;
        sideband_info_t *pSideBandInfo = NULL;
        vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
        vproc_post_r2y_copy_cfg_t *pPpStrmCopyCtrl = NULL;
        vproc_y2y_blending_cfg_t *pPpStrmBldCtrl = NULL;
        vin_fov_batch_cmd_set_t *pCmdSet = NULL;
        AMBA_DSP_VIN_MAIN_CONFIG_DATA_s *pVinIfo = NULL;
        AMBA_DSP_VOUT_DISP_CFG_DATA_s *pVoutDispIfo = NULL;
        AMBA_DSP_VOUT_MIXER_CSC_DATA_s *pVoutMixerCscInfo = NULL;
        AMBA_DSP_VOUT_TVE_DATA_s *pVoutTveInfo = NULL;
#ifdef SUPPORT_DSP_LDY_SLICE
        DSP_SLICE_LAYOUT_s *pSliceLayout = NULL;
#endif

        /* API Resource checking */
#ifdef SUPPORT_RESC_CHECK
        ResourceApiReg(0U/*Disable*/, 0U, 1U);
#endif

        /* API Timing */
        TimingApiReg(0U/*Disable*/, 0U, 1U);

        /* API check */
        CheckApiReg(0U/*Disable*/, 0U, 1U);

        /* Ctx Reset */
#ifdef SUPPORT_IWONG_UCODE
        (void)dsp_osal_memset(&CtxDspSrcBufCfg[0U], 0, sizeof(DSP_SRCBUF_CFG)*DSP_SRC_BUF_NUM);
#endif

        DSP_GetDspVinPinNum(0U, &pU8Array);
        (void)dsp_osal_memset(pU8Array, 0x0, sizeof(UINT8)*AMBA_DSP_MAX_VIN_NUM);

        DSP_GetDspVirtVinPinNum(0U, &pU8Array);
        (void)dsp_osal_memset(pU8Array, 0x0, sizeof(UINT8)*AMBA_DSP_MAX_VIRT_VIN_NUM);

        DSP_GetDspEncStrmCfg(0U, &pEncCfg);
        (void)dsp_osal_memset(pEncCfg, 0x0, sizeof(enc_cfg_t)*AMBA_DSP_MAX_STREAM_NUM);

        for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
            DSP_GetDspVinCfg(i, &pVinIfo);
            (void)dsp_osal_memset(pVinIfo, 0, sizeof(AMBA_DSP_VIN_MAIN_CONFIG_DATA_s));
#ifdef SUPPORT_MAX_UCODE
            DSP_GetDspExtVinRawBufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);

            DSP_GetDspExtVinAuxBufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);
#endif
        }

        for (i = 0U; i < AMBA_DSP_MAX_DATACAP_NUM; i++) {
            for (j = 0U; j < MAX_EXT_BUF_TBL_MEMBER_NUM; j++) {
                DSP_GetDspDataCapBufArray(i, j, &pArray);
                (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);

                DSP_GetDspDataCapAuxBufArray(i, j, &pArray);
                (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);
            }
        }

        for (i=0U; i < NUM_VOUT_IDX; i++) {
            DSP_GetDspVoutDispCfg(i, &pVoutDispIfo);
            (void)dsp_osal_memset(pVoutDispIfo, 0, sizeof(AMBA_DSP_VOUT_DISP_CFG_DATA_s));

            DSP_GetDspVoutMixerCscCfg(i, &pVoutMixerCscInfo);
            (void)dsp_osal_memset(pVoutMixerCscInfo, 0, sizeof(AMBA_DSP_VOUT_MIXER_CSC_DATA_s));

            DSP_GetDspVoutTveCfg(i, &pVoutTveInfo);
            (void)dsp_osal_memset(pVoutTveInfo, 0, sizeof(AMBA_DSP_VOUT_TVE_DATA_s));
        }

#ifdef SUPPORT_MAX_UCODE
        DSP_GetDspVinFovLayoutCfg(0U, 0U, &pFovLayout);
        (void)dsp_osal_memset(pFovLayout, 0, (sizeof(fov_layout_t)*DSP_VIN_MAX_NUM)*AMBA_DSP_MAX_VIEWZONE_NUM);

        DSP_GetDspVinStartFovLayoutCfg(0U, 0U, &pFovStartCfg);
        (void)dsp_osal_memset(pFovStartCfg, 0, (sizeof(vin_fov_cfg_t)*DSP_VIN_MAX_NUM)*AMBA_DSP_MAX_VIEWZONE_NUM);

        DSP_GetFovSideBandInfoCfg(0U, 0U, &pSideBandInfo);
        (void)dsp_osal_memset(pSideBandInfo, 0, ((sizeof(sideband_info_t)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_SIDEBAND_BUF_NUM)*MAX_SIDEBAND_BUF_ARRAY_NUM);

        DSP_GetDspPpStrmCtrlCfg(0U, 0U, 0U, &pPpStrmCtrl);
        (void)dsp_osal_memset(pPpStrmCtrl, 0, ((sizeof(vproc_pp_stream_cntl_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);

        DSP_GetDspPpStrmCopyCfg(0U, 0U, 0U, &pPpStrmCopyCtrl);
        (void)dsp_osal_memset(pPpStrmCopyCtrl, 0, (((sizeof(vproc_post_r2y_copy_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);

        DSP_GetDspPpStrmBldCfg(0U, 0U, 0U, &pPpStrmBldCtrl);
        (void)dsp_osal_memset(pPpStrmBldCtrl, 0, (((sizeof(vproc_y2y_blending_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);

        DSP_GetDspPpDummyBldTbl(&pU8Array);
        (void)dsp_osal_memset(pU8Array, 0, (sizeof(UINT8)*DSP_DMY_BLD_SIZE));

        DSP_GetDspBatchCmdSet(0U, 0U, &pCmdSet);
        (void)dsp_osal_memset(pCmdSet, 0, ((sizeof(vin_fov_batch_cmd_set_t)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_BATCH_CMD_SET_DEPTH));

        DSP_GetDspBatchCmdQ(0U, 0U, &pArray);
#ifdef USE_FLEXIBLE_BATCH_CMD_POOL
        (void)dsp_osal_memset(pArray, 0, ((sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_BATCH_CMD_POOL_NUM)*CMD_SIZE_IN_WORD);
#else
        (void)dsp_osal_memset(pArray, 0, (((sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_BATCH_CMD_POOL_NUM)*MAX_BATCH_CMD_NUM)*CMD_SIZE_IN_WORD);
#endif
        (void)dsp_osal_memset(&CtxDspBatchQInfo[0U][0U], 0x0, (sizeof(CTX_BATCHQ_INFO_s)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_BATCH_CMD_POOL_NUM);

        DSP_GetDspGroupCmdQ(0U, 0U, 0U, &pArray);
        (void)dsp_osal_memset(pArray, 0, ((((sizeof(UINT32)*MAX_GROUP_STRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*MAX_GROUP_CMD_SUBJOB_NUM)*MAX_GROUP_CMD_NUM)*CMD_SIZE_IN_WORD);

        DSP_GetDspExtRawBufArray(0U, 0U, &pArray);
        (void)dsp_osal_memset(pArray, 0, ((sizeof(UINT32)*DSP_VIN_MAX_NUM)*MAX_EXT_MEM_BUF_TYPE_NUM)*MAX_EXT_MEM_BUF_NUM);

        DSP_GetDspVprocGrpingNumBuf(0U, &pU8Array);
        (void)dsp_osal_memset(pU8Array, 0, (sizeof(UINT8)*NUM_VPROC_MAX_GROUP));

        DSP_GetDspVprocGrpingOrdBuf(0U, 0U, &pU8Array);
        (void)dsp_osal_memset(pU8Array, 0, ((sizeof(UINT8)*NUM_VPROC_MAX_GROUP)*NUM_VPROC_MAX_CHAN));

        DSP_GetDspVprocOrderBuf(0U, 0U, &pU8Array);
        (void)dsp_osal_memset(pU8Array, 0, (sizeof(UINT8)*NUM_VPROC_MAX_CHAN));

#ifdef SUPPORT_DSP_LDY_SLICE
        DSP_GetDspVprocSliceLayoutBuf(0U, &pSliceLayout);
        (void)dsp_osal_memset(pSliceLayout, 0, ((sizeof(DSP_SLICE_LAYOUT_s)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_TOTAL_SLICE_NUM));
#endif

        for (i = 0U; i<MAX_STL_EXT_MEM_STAGE_NUM; i++) {
            DSP_GetDspExtStlBufArray((UINT8)i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32));

            DSP_GetDspStlBatchCmdQ((UINT8)i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(dsp_cmd_t)*MAX_STL_BATCH_CMD_POOL_NUM);
        }

        for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
            DSP_GetDspExtPymdBufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);

            DSP_GetDspExtLndtBufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);

            DSP_GetDspExtMainY12BufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);

            DSP_GetDspExtMainIrBufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);
        }

        for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
            DSP_GetDspExtYuvStrmBufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);

            DSP_GetDspExtYuvStrmAuxBufArray(i, &pArray);
            (void)dsp_osal_memset(pArray, 0, sizeof(UINT32)*MAX_EXT_DISTINCT_DEPTH);
        }
#endif
        (void)dsp_osal_memset(&CtxResInfo, 0, sizeof(CTX_RESOURCE_INFO_s));
        (void)dsp_osal_memset(&CtxDspInstanceInfo, 0, sizeof(CTX_DSP_INST_INFO_s));
        (void)dsp_osal_memset(&CtxSensorInfo[0], 0, sizeof(CTX_SENSOR_INFO_s)*AMBA_DSP_MAX_VIN_NUM*AMBA_DSP_MAX_VIN_SENSOR_NUM);
        (void)dsp_osal_memset(&CtxVinInfo[0], 0, sizeof(CTX_VIN_INFO_s)*DSP_VIN_MAX_NUM);
        (void)dsp_osal_memset(&CtxViewZoneInfo[0], 0, sizeof(CTX_VIEWZONE_INFO_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
        (void)dsp_osal_memset(&CtxYuvStrmInfo[0], 0, sizeof(CTX_YUV_STRM_INFO_s)*AMBA_DSP_MAX_YUVSTRM_NUM);
        (void)dsp_osal_memset(&CtxYuvStrmEfctSyncCtrl[0], 0, sizeof(CTX_YUVSTRM_EFCT_SYNC_CTRL_s)*AMBA_DSP_MAX_YUVSTRM_NUM);
        (void)dsp_osal_memset(&CtxYuvStrmEfctSyncJobSet[0][0], 0, sizeof(CTX_EFCT_SYNC_JOB_SET_s)*AMBA_DSP_MAX_YUVSTRM_NUM*MAX_GROUP_CMD_POOL_NUM);
        (void)dsp_osal_memset(&CtxYuvStrmEfctSyncJobSetLayout[0][0], 0, sizeof(CTX_YUV_STRM_LAYOUT_s)*AMBA_DSP_MAX_YUVSTRM_NUM*MAX_GROUP_CMD_POOL_NUM);
        (void)dsp_osal_memset(&CtxVprocInfo[0], 0, sizeof(CTX_VPROC_INFO_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
        (void)dsp_osal_memset(&CtxStrmInfo[0], 0, sizeof(CTX_STREAM_INFO_s)*AMBA_DSP_MAX_STREAM_NUM);
        (void)dsp_osal_memset(&CtxVoutInfo[0], 0, sizeof(CTX_VOUT_INFO_s)*NUM_VOUT_IDX);
        (void)dsp_osal_memset(&CtxEncoderInfo, 0, sizeof(CTX_ENCODER_INFO_s));
        (void)dsp_osal_memset(&CtxDecoderInfo, 0, sizeof(CTX_DECODER_INFO_s));
        (void)dsp_osal_memset(&CtxVidDecInfo[0], 0, sizeof(CTX_VID_DEC_INFO_s)*AMBA_DSP_MAX_DEC_STREAM_NUM);
        (void)dsp_osal_memset(&CtxStlInfo, 0, sizeof(CTX_STILL_INFO_s));
        (void)dsp_osal_memset(&CtxDataCapInfo[0], 0, sizeof(CTX_DATACAP_INFO_s)*AMBA_DSP_MAX_DATACAP_NUM);
        (void)dsp_osal_memset(&CtxTimeLapseInfo[0], 0, sizeof(CTX_TIMELAPSE_INFO_s)*AMBA_DSP_MAX_STREAM_NUM);
        (void)dsp_osal_memset(&CtxEncGrpInfo[0], 0, sizeof(CTX_ENC_GRP_INFO_s)*AMBA_DSP_MAX_ENC_GRP_NUM);

        /* Mtx create */
        Rval = HL_CtxInitMtx();

        /* Specific initial value */
        // Resource
        CtxResInfo.DspSysState = AMBA_DSP_SYS_STATE_UNKNOWN;
        CtxResInfo.DspOpStatus = DSP_OP_STATUS_UNKNOWN;
        CtxResInfo.VoutBit = RESC_DEFT_VOUT_BIT;

        // ResDsp
        CtxDspInstanceInfo.MaxVirtVinNum = (UINT8)AMBA_DSP_MAX_VIRT_VIN_NUM;
        CtxDspInstanceInfo.MaxVpocNum = (UINT8)AMBA_DSP_MAX_VIEWZONE_NUM;
        CtxDspInstanceInfo.DecSharedVirtVinId = DSP_VIRT_VIN_IDX_INVALID;

        HL_CtxInitCtx();

        /* DspDriver revision */
        CtxResInfo.DspVer.SSPVer = (UINT32)DSP_DRIVER_VERSION;

        // LL interrupt init
        (void)LL_InterruptInit();

        // LL buffer init
        (void)ArmCommInit();

#if defined (CONFIG_THREADX)
        {
            AMBA_VerInfo_s VerInfo;

            (void)dsp_osal_memset(&VerInfo, 0, sizeof(AMBA_VerInfo_s));
            AmbaVer_SetDspDspKernelHL(&VerInfo);
            if (VerInfo.DateNum == 0xDEADBEEFU) {
                AmbaLL_LogUInt5("[Err @ HL_CtxInit][%d] AmbaVer_SetDspDspKernelHL error", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
#endif

        // GetDspMonCfg
#if defined (CONFIG_ENABLE_DSP_MONITOR)
        {
            AMBA_DSP_MONITOR_CONFIG_s MonCfg = {0};

            Rval = dsp_mon_get_cfg((SSP_ERR_BASE >> 16U), &MonCfg);
            CtxResInfo.MonitorMask = MonCfg.BitMask;

            if ((CtxResInfo.MonitorMask & DSP_MON_BITMASK_CRC) > 0U) {
                extern UINT32 HL_SimilarCtxInit(void);
                Rval = HL_SimilarCtxInit();
                AmbaLL_LogUInt5("SimInit Enable [0x%X]", Rval, 0U, 0U, 0U, 0U);
            }
        }
#endif

        /* Init Cmd buffer pool */
        {
            static char CmdBufMtxStr[] = "DspCtxCmdBufMtx";
            const dsp_cmd_t *pCtxCmdBuf = CtxCmdBuf;

            (void)dsp_osal_mutex_init(&CtxCmdBufInfo.PoolMtx, CmdBufMtxStr);
            (void)dsp_osal_memset(CtxCmdBuf, 0, sizeof(CtxCmdBuf));
            dsp_osal_typecast(&CtxCmdBufInfo.BaseAddr, &pCtxCmdBuf);
            for (i = 0U; i<CTX_CMD_BUF_POOL_NUM; i++) {
                CtxCmdBufInfo.PoolUsage[i] = 0U;
            }

            pCtxCmdBuf = &HL_DefCtxCmdBuf;
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrSys, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrHal, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrSus, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDbgLvl, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDbgthd, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrBind, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrActPrf, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcMax, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinMax, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrSetPrf, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrEncMax, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDecMax, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutMax, &pCtxCmdBuf);

            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpc, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcSetup, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcIkCfg, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcPymd, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcPrev, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcLndt, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcExtMem, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcStop, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcOsd, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcDeci, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcFovGrp, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcEcho, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcPp, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcEff, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcChOrd, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcGrp, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcWrpGrp, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVpcTestFrm, &pCtxCmdBuf);

            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinStart, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinIdle, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinExtMem, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinSndData, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinInitCap, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinAttfrm, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinAttRaw, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinMsgRate, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinCe, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinHdr, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinFlip, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinLayout, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinDly, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinSidBend, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinVoutLock, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinAttDta, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinTd, &pCtxCmdBuf);

            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutMx, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutVdo, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutDef, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutOsd, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutOsdBuf, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutOsdClut, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutDisp, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutDve, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutReset, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutCac, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutDigMode, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVoutGamma, &pCtxCmdBuf);

            dsp_osal_typecast(&HL_DefCtxCmdBufPtrEnc, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrEncStart, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrEncStop, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrEncJpg, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrEncRt, &pCtxCmdBuf);

            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDec, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDecStart, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDecStop, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDecBsUpt, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDecSpeed, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDecTrick, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrDecStlDec, &pCtxCmdBuf);

            dsp_osal_typecast(&HL_DefCtxCmdBufPtrVinCfg, &pCtxCmdBuf);
            dsp_osal_typecast(&HL_DefCtxCmdBufPtrCmpr, &pCtxCmdBuf);
        }

        CtxInit = 1U;
    }
    HL_SetCtxInitFlag(HL_MTX_OPT_SET, &CtxInit);

    return Rval;
}

/* Reset context for certain ViewZoneOI */
void HL_CtxLvDataPathReset(UINT32 ViewZoneOI)
{
    UINT16 i;

    AmbaLL_LogUInt5("HL_CtxLvDataPathReset", 0U, 0U, 0U, 0U, 0U);
//FIXME, PINOI
    for (i = 0U; i<DSP_VPROC_PIN_NUM; i++) {
        CtxResInfo.PostPinLinkMask[i] = 0U;
    }

    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (1U == DSP_GetBit(ViewZoneOI, i, 1U)) {
            (void)dsp_osal_memset(&CtxVprocInfo[i].PinUsage[0U], 0, sizeof(UINT32)*DSP_VPROC_PIN_NUM);
            (void)dsp_osal_memset(&CtxVprocInfo[i].PinUsageOnStrm[0U][0U], 0, (sizeof(UINT32)*DSP_VPROC_PIN_NUM)*AMBA_DSP_MAX_YUVSTRM_NUM);
            (void)dsp_osal_memset(&CtxVprocInfo[i].PinUsageNumOnStrm[0U][0U], 0, (sizeof(UINT8)*DSP_VPROC_PIN_NUM)*AMBA_DSP_MAX_YUVSTRM_NUM);
        }
    }

    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (1U == DSP_GetBit(ViewZoneOI, i, 1U)) {
            (void)dsp_osal_memset(&CtxViewZoneInfo[i].PinWindow[DSP_VPROC_PIN_PREVA], 0, sizeof(AMBA_DSP_WINDOW_s));
            (void)dsp_osal_memset(&CtxViewZoneInfo[i].PinWindow[DSP_VPROC_PIN_PREVB], 0, sizeof(AMBA_DSP_WINDOW_s));
            (void)dsp_osal_memset(&CtxViewZoneInfo[i].PinWindow[DSP_VPROC_PIN_PREVC], 0, sizeof(AMBA_DSP_WINDOW_s));
        }
    }

//FIXME, YuvStrmOI
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.CopyJobNum[0U],
                              0,
                              sizeof(UINT16)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.CopyJobDesc[0U][0U],
                              0,
                              (sizeof(DSP_EFFECT_COPY_JOB_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.BlendJobDesc[0U][0U],
                              0,
                              (sizeof(DSP_EFFECT_BLEND_JOB_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.EffectBufDesc,
                              0,
                              sizeof(DSP_EFFECT_BUF_DESC_s));
        CtxYuvStrmInfo[i].Layout.ChanLinked = 0U;
    }

//FIXME, VirtVinOI
//    {
//        CTX_VIN_INFO_s VinInfo = {0};
//
//        CtxDspInstanceInfo.VirtVinBitMask = 0x0U;
//        //clear virtual vin state
//        for (i = DSP_VIRTUAL_VIN_START_IDX; i<DSP_VIN_MAX_NUM; i++) {
//            HL_GetVinInfo(HL_MTX_OPT_GET, i, &VinInfo);
//            if ((VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_TIMER) ||
//                (VinInfo.VinCtrl.VinState == DSP_VIN_STATUS_INVALID)) {
//                VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_IDLE2RUN;
//            }
//            HL_SetVinInfo(HL_MTX_OPT_SET, i, &VinInfo);
//        }
//    }

    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if (1U == DSP_GetBit(ViewZoneOI, i, 1U)) {
            DSP_ClearBit(&CtxDspInstanceInfo.VprocBitMask, i);
            CtxDspInstanceInfo.VprocPurpose[i] = 0U;
        }
    }
}

/* Reset all context */
void HL_CtxLvReset(void)
{
    UINT16 i, j;

    AmbaLL_LogUInt5("HL_CtxLvReset", 0U, 0U, 0U, 0U, 0U);
    /* YuvStrmInfo */
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        CtxViewZoneInfo[i].DecVprocCfgState = (UINT8)0U;
        CtxViewZoneInfo[i].DecVprocTileMode = (UINT8)0U;
        CtxViewZoneInfo[i].DecVprocColNum = (UINT8)0U;
        CtxViewZoneInfo[i].DecVprocRowNum = (UINT8)0U;
    }

    /* YuvStrmInfo */
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.CopyJobNum[0U], 0, sizeof(UINT16)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
        /* Reset everytime at HL_CalcEffectPostPJob */
//        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.CopyJobDesc[0U], 0, (sizeof(DSP_EFFECT_COPY_JOB_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);
//        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.BlendJobDesc[0U], 0, (sizeof(DSP_EFFECT_BLEND_JOB_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.EffectBufDesc, 0xFFFF, sizeof(CtxYuvStrmInfo[i].Layout.EffectBufDesc));
        CtxYuvStrmInfo[i].Layout.ChanLinked = 0U;
    }

    /* YuvStrmInfo */
//    (void)dsp_osal_memset(&CtxYuvStrmInfo[0], 0, sizeof(CTX_YUV_STRM_INFO_s)*AMBA_DSP_MAX_YUVSTRM_NUM);
    //YuvStrm
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        CtxYuvStrmInfo[i].PpStrmPoolDesc.StartAddr = 0U;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Size = MAX_GROUP_CMD_POOL_NUM;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Wp = MAX_GROUP_CMD_POOL_NUM - 1U;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Rp = MAX_GROUP_CMD_POOL_NUM - 1U;

        /* EffectBufDesc */
//        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.EffectBufDesc, 0xFFFF, sizeof(CtxYuvStrmInfo[i].Layout.EffectBufDesc));
    }

    {
        vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
        vproc_post_r2y_copy_cfg_t *pPpStrmCopyCtrl = NULL;
        vproc_y2y_blending_cfg_t *pPpStrmBldCtrl = NULL;

        DSP_GetDspPpStrmCtrlCfg(0U, 0U, 0U, &pPpStrmCtrl);
        (void)dsp_osal_memset(pPpStrmCtrl, 0x0, ((sizeof(vproc_pp_stream_cntl_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);

        DSP_GetDspPpStrmCopyCfg(0U, 0U, 0U,&pPpStrmCopyCtrl);
        (void)dsp_osal_memset(pPpStrmCopyCtrl, 0x0, (((sizeof(vproc_post_r2y_copy_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);

        DSP_GetDspPpStrmBldCfg(0U, 0U, 0U,&pPpStrmBldCtrl);
        (void)dsp_osal_memset(pPpStrmBldCtrl, 0x0, (((sizeof(vproc_y2y_blending_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_BLEND_NUMBER_CTX);
    }


    /* virtual vin */
    {
        CTX_VIN_INFO_s VinInfo = {0};

        CtxDspInstanceInfo.VirtVinBitMask = 0x0U;
        CtxDspInstanceInfo.DecSharedVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
        /* clear virtual vin state */
        for (i = DSP_VIRTUAL_VIN_START_IDX; i<DSP_VIN_MAX_NUM; i++) {
            HL_GetVinInfo(HL_MTX_OPT_GET, i, &VinInfo);
            VinInfo.VinCtrl.VinState = DSP_VIN_STATUS_INVALID;
            HL_SetVinInfo(HL_MTX_OPT_SET, i, &VinInfo);
        }
    }

    /* Stl vproc here? */
    {
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetResourcePtr(&Resource);
        if ((Resource->MaxStlMainWidth > 0U) && (Resource->MaxStlMainHeight > 0U)) {
            UINT8 VprocId;
            CTX_STILL_INFO_s StlInfo = {0};

            HL_GetStlInfo(HL_MTX_OPT_ALL, &StlInfo);
            VprocId = (UINT8)StlInfo.YuvInVprocId;
            if (VprocId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                DSP_ClearBit(&CtxDspInstanceInfo.VprocBitMask, VprocId);
                CtxDspInstanceInfo.VprocPurpose[VprocId] = 0U;
            }
        }

        // Still
        CtxStlInfo.RawInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
        CtxStlInfo.YuvInVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
        CtxStlInfo.YuvEncVirtVinId = DSP_VIRT_VIN_IDX_INVALID;
        CtxStlInfo.RawInVprocId = DSP_VPROC_IDX_INVALID;
        CtxStlInfo.YuvInVprocId = DSP_VPROC_IDX_INVALID;
        CtxStlInfo.EncStmId = DSP_VPROC_IDX_INVALID;

        CtxStlInfo.RawInVprocStatus = STL_VPROC_STATUS_IDLE;
        CtxStlInfo.YuvInVprocStatus = STL_VPROC_STATUS_IDLE;
        CtxStlInfo.YuvInVprocPin = DSP_VPROC_PIN_NUM;
        CtxStlInfo.RawInVprocPin = DSP_VPROC_PIN_NUM;
        CtxStlInfo.EncStatus = STL_ENC_STATUS_IDLE;

#ifndef SUPPORT_VPROC_RT_RESTART
        CtxStlInfo.Yuv422InVprocStatus = STL_VPROC_STATUS_IDLE;
        CtxStlInfo.Yuv422InVprocId = DSP_VPROC_IDX_INVALID;
#endif
    }

    /* DataCap */
    (void)dsp_osal_memset(&CtxDataCapInfo[0], 0, sizeof(CTX_DATACAP_INFO_s)*AMBA_DSP_MAX_DATACAP_NUM);

    /* TimeLapse */
    for (i = 0U; i<AMBA_DSP_MAX_STREAM_NUM; i++) {
        CtxTimeLapseInfo[i].VirtVinId = DSP_VIRT_VIN_IDX_INVALID;
        CtxTimeLapseInfo[i].Status = ENC_TIMELAPSE_STATUS_IDLE;
        CtxStrmInfo[i].TimeLapseId = DSP_TIMELAPSE_IDX_INVALID;
    }

    /* EncGrp */
    for (i = 0U; i<AMBA_DSP_MAX_ENC_GRP_NUM; i++) {
        for (j = 0U; j<AMBA_DSP_MAX_ENC_GRP_STRM_NUM; j++) {
            CtxEncGrpInfo[i].StrmIdx[j] = DSP_ENC_GRP_STRM_IDX_INVALID;
        }
    }

    /* Encoder */
    CtxEncoderInfo.ActiveBit = 0U;

    /* Resource */
    CtxResInfo.MaxVprocTileWidth = 0U;
    CtxResInfo.MinVprocTileNum = 0U;
    CtxResInfo.MaxVprocTileNum = 0U;
    CtxResInfo.MaxVprocTileOverlap = 0U;
    (void)dsp_osal_memset(CtxResInfo.IdspLoad, 0, sizeof(UINT32)*DSP_IDSP_CORE_NUM);
    (void)dsp_osal_memset(CtxResInfo.VdspLoad, 0, sizeof(UINT32)*DSP_VDSP_CORE_NUM);
}

#ifdef SUPPORT_IWONG_UCODE
void HL_GetPointerToDspSrcBufCfg(UINT16 Index, DSP_SRCBUF_CFG **pInfo)
{
    if (Index < DSP_SRC_BUF_NUM) {
        *pInfo = &CtxDspSrcBufCfg[Index];
    } else {
        AmbaLL_LogUInt5("Wrong SrcBufIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}
#endif

void HL_GetPointerDspVinPinCfg(UINT16 Index, UINT8 **pInfo)
{
    if (Index < AMBA_DSP_MAX_VIN_NUM) {
        DSP_GetDspVinPinNum(Index, pInfo);
    } else if (Index < DSP_VIN_MAX_NUM) {
        DSP_GetDspVirtVinPinNum(Index, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong VinIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetPointerDspEncStrmCfg(UINT16 Index, enc_cfg_t **pInfo)
{
    if (Index < AMBA_DSP_MAX_STREAM_NUM) {
        DSP_GetDspEncStrmCfg(Index, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong EncStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetPointerDspDecStrmCfg(UINT16 Index, dec_cfg_t **pInfo)
{
    if (Index < AMBA_DSP_MAX_DEC_CFG_NUM) {
        DSP_GetDspDecStrmCfg(Index, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong EncStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

#ifdef SUPPORT_MAX_UCODE
void HL_GetPointerToDspVinStartFovCfg(UINT16 Index, UINT16 LocalIndex, vin_fov_cfg_t **pInfo)
{
    if ((Index < DSP_VIN_MAX_NUM) && (LocalIndex < AMBA_DSP_MAX_VIEWZONE_NUM)) {
        DSP_GetDspVinStartFovLayoutCfg(Index, LocalIndex, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong Idx(%d %d) for VinStartFovCfg", Index, LocalIndex, 0U, 0U, 0U);
    }
}

void HL_GetPointerToDspVinFovLayoutCfg(UINT16 Index, UINT16 LocalIndex, fov_layout_t **pInfo)
{
    if ((Index < DSP_VIN_MAX_NUM) && (LocalIndex < AMBA_DSP_MAX_VIEWZONE_NUM)) {
        DSP_GetDspVinFovLayoutCfg(Index, LocalIndex, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong Idx(%d %d) for VinFovCfg", Index, LocalIndex, 0U, 0U, 0U);
    }
}

void HL_GetPointerToDspFovSideBandCfg(UINT16 ViewZoneId, UINT16 TokenIndex, sideband_info_t **pInfo)
{
    if ((ViewZoneId < AMBA_DSP_MAX_VIEWZONE_NUM) && (TokenIndex < MAX_SIDEBAND_BUF_NUM)) {
        DSP_GetFovSideBandInfoCfg(ViewZoneId, TokenIndex, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong Idx(%d %d) for FovSideBandCfg", ViewZoneId, TokenIndex, 0U, 0U, 0U);
    }
}

void HL_GetPointerToDspPpStrmCtrlCfg(UINT16 YuvStrmIdx, UINT16 BufIdx, UINT16 ChanIdx, vproc_pp_stream_cntl_t **pInfo)
{
    if ((YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) &&
        (BufIdx < MAX_GROUP_CMD_POOL_NUM) &&
        (ChanIdx < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)) {
        DSP_GetDspPpStrmCtrlCfg(YuvStrmIdx, BufIdx, ChanIdx, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong Idx(%d %d %d) for PpStrmCtrl", YuvStrmIdx, BufIdx, ChanIdx, 0U, 0U);
    }
}

void HL_GetPointerToDspPpStrmCopyCfg(UINT16 YuvStrmIdx, UINT16 BufIdx, UINT16 ChanIdx, vproc_post_r2y_copy_cfg_t **pInfo)
{
    if ((YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) &&
        (BufIdx < MAX_GROUP_CMD_POOL_NUM) &&
        (ChanIdx < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)) {
        DSP_GetDspPpStrmCopyCfg(YuvStrmIdx, ChanIdx, BufIdx, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong Idx(%d %d %d) for PpStrmCopy", YuvStrmIdx, BufIdx, ChanIdx, 0U, 0U);
    }
}

void HL_GetPointerToDspPpStrmBldCfg(UINT16 YuvStrmIdx, UINT16 BufIdx, UINT16 ChanIdx, vproc_y2y_blending_cfg_t **pInfo)
{
    if ((YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) &&
        (BufIdx < MAX_GROUP_CMD_POOL_NUM) &&
        (ChanIdx < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)) {
        DSP_GetDspPpStrmBldCfg(YuvStrmIdx, BufIdx, ChanIdx, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong Idx(%d %d %d) for PpStrmBld", YuvStrmIdx, BufIdx, ChanIdx, 0U, 0U);
    }
}

void HL_GetPointerToDspPpDummyBldTbl(UINT8 **pInfo)
{
    DSP_GetDspPpDummyBldTbl(pInfo);
}

void HL_GetPointerToDspBatchCmdSet(UINT16 ViewZoneId, UINT16 Idx, vin_fov_batch_cmd_set_t **pInfo)
{
    DSP_GetDspBatchCmdSet(ViewZoneId, Idx, pInfo);
}

void HL_GetPointerToDspBatchQ(UINT16 ViewZoneId, UINT16 BatchIdx, UINT32 **pInfo, UINT32 *pBatchCmdId)
{
    UINT32 BatchCmdId = 0U;

    DSP_GetDspBatchCmdQ(ViewZoneId, BatchIdx, pInfo);

    HL_GetBatchCmdId(ViewZoneId, BatchIdx, &BatchCmdId);
    *pBatchCmdId = BatchCmdId;
}

void HL_GetBatchCmdId(UINT16 ViewZoneId, UINT16 BatchIdx, UINT32 *pBatchCmdId)
{
    DSP_BATCH_CMD_ID_s DspBatchCmdId = {0};
    UINT32 BatchCmdId = 0U;

    DspBatchCmdId.PoolId = (UINT16)(BatchIdx + 1U);
    DspBatchCmdId.ViewZoneId = ViewZoneId;
    (void)dsp_osal_memcpy(&BatchCmdId, &DspBatchCmdId, sizeof(DSP_BATCH_CMD_ID_s));

    *pBatchCmdId = BatchCmdId;
}

#ifdef USE_FLEXIBLE_BATCH_CMD_POOL
void HL_ResetDspBatchQ(const UINT32 *pBatchQ, UINT8 Size, UINT8 SkipIsoSlot)
{
    DSP_CMD_s *pDspCmd;
    UINT8 i, StartIdx;

    dsp_osal_typecast(&pDspCmd, &pBatchQ);
    if (SkipIsoSlot == 0U) {
        /* Set First Cmd as NOP and fixed for IsoCfgUpdateCmd only */
        pDspCmd[BATCH_ISOCFGCMD_SLOT_IDX].CmdCode = 0x100000DU; //CMD_DSP_NOP
        StartIdx = (BATCH_ISOCFGCMD_SLOT_IDX + 1U);
    } else {
        StartIdx = 0U;
    }
    for (i=StartIdx; i<Size; i++) {
        pDspCmd[i].CmdCode = MAX_BATCH_CMD_END;
    }
}
#else
void HL_ResetDspBatchQ(const UINT32 *pBatchQ)
{
    /* Set First Cmd as NOP and fixed for IsoCfgUpdateCmd only */
    DSP_CMD_s *pDspCmd;
    UINT16 i;

    dsp_osal_typecast(&pDspCmd, &pBatchQ);
    pDspCmd[BATCH_ISOCFGCMD_SLOT_IDX].CmdCode = 0x100000DU; //CMD_DSP_NOP
    for (i=1U; i<MAX_BATCH_CMD_NUM; i++) {
        pDspCmd[i].CmdCode = MAX_BATCH_CMD_END;
    }
}
#endif

void HL_GetPointerToDspGroupCmdQ(UINT16 YuvStrmId, UINT16 SeqIdx, UINT16 SubJobIdx, UINT32 **pInfo)
{
    if ((YuvStrmId < MAX_GROUP_STRM_NUM) &&
        (SeqIdx < MAX_GROUP_CMD_POOL_NUM) &&
        (SubJobIdx < MAX_GROUP_CMD_SUBJOB_NUM)) {
        DSP_GetDspGroupCmdQ(YuvStrmId, SeqIdx, SubJobIdx, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong Idx(%d %d %d) for GrpCmdQ", YuvStrmId, SeqIdx, SubJobIdx, 0U, 0U);
    }
}

void HL_GetPointerToDspExtRawBufArray(UINT16 VinIdx, UINT16 Type, UINT32 **pInfo)
{
    DSP_GetDspExtRawBufArray(VinIdx, Type, pInfo);
}

void HL_GetPointerToDspVprocGrpNum(UINT16 GroupId, UINT8 **pInfo)
{
    if (GroupId < NUM_VPROC_MAX_GROUP) {
        DSP_GetDspVprocGrpingNumBuf(GroupId, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong GroupIdx(%d) for VprocGrpNum", GroupId, 0U, 0U, 0U, 0U);
    }
}

//for CMD_VPROC_MULTI_CHAN_PROC_ORDER (0x02000066)
void HL_GetPointerToDspVprocGrpOrd(const UINT16 GroupId, const UINT16 OrderId, UINT8 **pInfo)
{
    if ((GroupId < NUM_VPROC_MAX_GROUP) &&
        (OrderId < NUM_VPROC_MAX_CHAN)) {
        DSP_GetDspVprocGrpingOrdBuf(GroupId, OrderId, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong GroupIdx(%d) OrderId(%d) for VprocGrpNum", GroupId, OrderId, 0U, 0U, 0U);
    }
}

//for CMD_VPROC_SET_VPROC_GRPING (0x02000067)
void HL_GetPointerToDspVprocOrder(const UINT16 GroupId, const UINT16 OrderId, UINT8 **pInfo)
{
    if ((GroupId < NUM_VPROC_MAX_GROUP) &&
        (OrderId < NUM_VPROC_MAX_CHAN)) {
        DSP_GetDspVprocOrderBuf(GroupId, OrderId, pInfo);
    } else {
        AmbaLL_LogUInt5("Wrong GroupIdx(%d) OrderId(%d) for VprocOrder", GroupId, OrderId, 0U, 0U, 0U);
    }
}

void HL_GetPointerToDspExtStlBufArray(UINT8 StageId, UINT32 **pInfo)
{
    DSP_GetDspExtStlBufArray(StageId, pInfo);
}

void HL_GetPointerToDspStlBatchQ(UINT8 StageId, UINT32 **pInfo)
{
    DSP_GetDspStlBatchCmdQ(StageId, pInfo);
}

void HL_GetPointerToDspExtPymdBufArray(const UINT16 ViewZoneId, UINT32 **pInfo)
{
    DSP_GetDspExtPymdBufArray(ViewZoneId, pInfo);
}

void HL_GetPointerToDspExtLndtBufArray(const UINT16 ViewZoneId, UINT32 **pInfo)
{
    DSP_GetDspExtLndtBufArray(ViewZoneId, pInfo);
}

void HL_GetPointerToDspExtMainY12BufArray(const UINT16 ViewZoneId, UINT32 **pInfo)
{
    DSP_GetDspExtMainY12BufArray(ViewZoneId, pInfo);
}

void HL_GetPointerToDspExtMainIrBufArray(const UINT16 ViewZoneId, UINT32 **pInfo)
{
    DSP_GetDspExtMainIrBufArray(ViewZoneId, pInfo);
}

void HL_GetPointerToDspExtYuvStrmBufArray(const UINT16 YuvStrmId, UINT32 **pInfo)
{
    DSP_GetDspExtYuvStrmBufArray(YuvStrmId, pInfo);
}

void HL_GetPointerToDspExtYuvStrmAuxBufArray(const UINT16 YuvStrmId, UINT32 **pInfo)
{
    DSP_GetDspExtYuvStrmAuxBufArray(YuvStrmId, pInfo);
}

void HL_GetPointerToDspExtVinRawBufArray(const UINT16 VinId, UINT32 **pInfo)
{
    DSP_GetDspExtVinRawBufArray(VinId, pInfo);
}

void HL_GetPointerToDspExtVinAuxBufArray(const UINT16 VinId, UINT32 **pInfo)
{
    DSP_GetDspExtVinAuxBufArray(VinId, pInfo);
}

void HL_GetPointerToDspDataCapBufArray(const UINT16 Id, const UINT16 AuxId, UINT32 **pInfo)
{
    DSP_GetDspDataCapBufArray(Id, AuxId, pInfo);
}

void HL_GetPointerToDspDataCapAuxBufArray(const UINT16 Id, const UINT16 AuxId, UINT32 **pInfo)
{
    DSP_GetDspDataCapAuxBufArray(Id, AuxId, pInfo);
}

#endif

#if 0
void HL_GetResource(const UINT8 MtxOpt, CTX_RESOURCE_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxResMtx, MtxOpt);
    (void)dsp_osal_memcpy(pInfo, &CtxResInfo, sizeof(CTX_RESOURCE_INFO_s));
    (void)HL_MtxUnLock(&CtxResMtx, MtxOpt);
}

void HL_SetResource(const UINT8 MtxOpt, const CTX_RESOURCE_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxResMtx, MtxOpt);
    (void)dsp_osal_memcpy(&CtxResInfo, pInfo, sizeof(CTX_RESOURCE_INFO_s));
    (void)HL_MtxUnLock(&CtxResMtx, MtxOpt);
}
#endif

void HL_GetResourcePtr(CTX_RESOURCE_INFO_s **pInfo)
{
    *pInfo = &CtxResInfo;
}

void HL_GetResourceLock(CTX_RESOURCE_INFO_s **pInfo)
{
    (void)HL_MtxLock(&CtxResMtx, HL_MTX_OPT_GET);
    *pInfo = &CtxResInfo;
}

void HL_GetResourceUnLock(void)
{
    (void)HL_MtxUnLock(&CtxResMtx, HL_MTX_OPT_SET);
}

void HL_GetDspInstance(const UINT8 MtxOpt, CTX_DSP_INST_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxDspInstMtx, MtxOpt);
    (void)dsp_osal_memcpy(pInfo, &CtxDspInstanceInfo, sizeof(CTX_DSP_INST_INFO_s));
    (void)HL_MtxUnLock(&CtxDspInstMtx, MtxOpt);
}

void HL_SetDspInstance(const UINT8 MtxOpt, const CTX_DSP_INST_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxDspInstMtx, MtxOpt);
    (void)dsp_osal_memcpy(&CtxDspInstanceInfo, pInfo, sizeof(CTX_DSP_INST_INFO_s));
    (void)HL_MtxUnLock(&CtxDspInstMtx, MtxOpt);
}

void HL_GetSensorInfo(const UINT8 MtxOpt, UINT16 Index, CTX_SENSOR_INFO_s *pInfo)
{
    if (Index < (AMBA_DSP_MAX_VIN_NUM*AMBA_DSP_MAX_VIN_SENSOR_NUM)) {
        (void)HL_MtxLock(&CtxSensorMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxSensorInfo[Index], sizeof(CTX_SENSOR_INFO_s));
        (void)HL_MtxUnLock(&CtxSensorMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong SensorIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetSensorInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_SENSOR_INFO_s *pInfo)
{
    if (Index < (AMBA_DSP_MAX_VIN_NUM*AMBA_DSP_MAX_VIN_SENSOR_NUM)) {
        (void)HL_MtxLock(&CtxSensorMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxSensorInfo[Index], pInfo, sizeof(CTX_SENSOR_INFO_s));
        (void)HL_MtxUnLock(&CtxSensorMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong SensorIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetVinInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIN_INFO_s *pInfo)
{
    if (Index < DSP_VIN_MAX_NUM) {
        (void)HL_MtxLock(&CtxVinMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxVinInfo[Index], sizeof(CTX_VIN_INFO_s));
        (void)HL_MtxUnLock(&CtxVinMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong VinIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetVinInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIN_INFO_s *pInfo)
{
    if (Index < DSP_VIN_MAX_NUM) {
        (void)HL_MtxLock(&CtxVinMtx[Index], MtxOpt);
        HL_CheckVinInfoUpdate(pInfo, &CtxVinInfo[Index]);
        (void)dsp_osal_memcpy(&CtxVinInfo[Index], pInfo, sizeof(CTX_VIN_INFO_s));
        (void)HL_MtxUnLock(&CtxVinMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong VinIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

#if 0
void HL_GetViewZoneInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIEWZONE_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_VIEWZONE_NUM) {
        (void)HL_MtxLock(&CtxViewZoneMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxViewZoneInfo[Index], sizeof(CTX_VIEWZONE_INFO_s));
        (void)HL_MtxUnLock(&CtxViewZoneMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong ViewZoneIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetViewZoneInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VIEWZONE_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_VIEWZONE_NUM) {
        (void)HL_MtxLock(&CtxViewZoneMtx[Index], MtxOpt);
        HL_CheckViewZoneInfoUpdate(&CtxViewZoneInfo[Index]);
        (void)dsp_osal_memcpy(&CtxViewZoneInfo[Index], pInfo, sizeof(CTX_VIEWZONE_INFO_s));
        (void)HL_MtxUnLock(&CtxViewZoneMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong ViewZoneIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}
#endif

void HL_GetViewZoneInfoLock(UINT16 Index, CTX_VIEWZONE_INFO_s **pInfo)
{
    if (Index < AMBA_DSP_MAX_VIEWZONE_NUM) {
        (void)HL_MtxLock(&CtxViewZoneMtx[Index], HL_MTX_OPT_GET);
        *pInfo = &CtxViewZoneInfo[Index];
        HL_CheckViewZoneInfoUpdatePush(&CtxViewZoneInfo[Index]);
    } else {
        AmbaLL_LogUInt5("Wrong ViewZoneIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetViewZoneInfoUnLock(UINT16 Index)
{
    if (Index < AMBA_DSP_MAX_VIEWZONE_NUM) {
        HL_CheckViewZoneInfoUpdate(&CtxViewZoneInfo[Index]);
        (void)HL_MtxUnLock(&CtxViewZoneMtx[Index], HL_MTX_OPT_SET);
    } else {
        AmbaLL_LogUInt5("Wrong ViewZoneIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetViewZoneInfoPtr(UINT16 Index, CTX_VIEWZONE_INFO_s **pInfo)
{
    if (Index < AMBA_DSP_MAX_VIEWZONE_NUM) {
        *pInfo = &CtxViewZoneInfo[Index];
    } else {
        AmbaLL_LogUInt5("Wrong ViewZoneIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

/**
 * Check resource setting includes decode?
 * For liveview mode checking it is duplex or not.
 */
UINT8 HL_HasDecResource(void)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 HasDecRes;

    HL_GetResourcePtr(&Resource);
    if (Resource->DecMaxStreamNum > 0U) {
        HasDecRes = 1;
    } else {
        HasDecRes = 0;
    }
    return HasDecRes;
}

/**
 * Check is currently in dec2vproc?
 * Under decode mode
 */
UINT8 HL_IsDec2Vproc(void)
{
    UINT8 ProfState = DSP_GetProfState();
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 IsDec2Vproc;

    HL_GetResourcePtr(&Resource);
    if ((ProfState == DSP_PROF_STATUS_CAMERA) &&
        (Resource->MaxViewZoneNum > 0U) &&
        (Resource->DecMaxStreamNum > 0U)) {
        IsDec2Vproc = 1;
    } else {
        IsDec2Vproc = 0;
    }
    return IsDec2Vproc;
}

/**
 * Check resource setting is going to entering dec2vproc?
 * Before dsp boot, after resource settled.
 */
UINT8 HL_HasDec2Vproc(void)
{
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    UINT8 HasDec2Vproc;

    HL_GetResourcePtr(&Resource);
    if ((Resource->MaxViewZoneNum > 0U) &&
        (Resource->DecMaxStreamNum > 0U)) {
        HasDec2Vproc = 1;
    } else {
        HasDec2Vproc = 0;
    }
    return HasDec2Vproc;
}

#if 0
void HL_GetYuvStrmInfo(const UINT8 MtxOpt, UINT16 Index, CTX_YUV_STRM_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxLock(&CtxYuvStrmMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxYuvStrmInfo[Index], sizeof(CTX_YUV_STRM_INFO_s));
        (void)HL_MtxUnLock(&CtxYuvStrmMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetYuvStrmInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_YUV_STRM_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxLock(&CtxYuvStrmMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxYuvStrmInfo[Index], pInfo, sizeof(CTX_YUV_STRM_INFO_s));
        (void)HL_MtxUnLock(&CtxYuvStrmMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}
#endif

void HL_GetYuvStrmInfoPtr(const UINT16 Index, CTX_YUV_STRM_INFO_s **pInfo)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        *pInfo = &CtxYuvStrmInfo[Index];
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetYuvStrmInfoLock(UINT16 Index, CTX_YUV_STRM_INFO_s **pInfo)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxLock(&CtxYuvStrmMtx[Index], HL_MTX_OPT_GET);
        *pInfo = &CtxYuvStrmInfo[Index];
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetYuvStrmInfoUnLock(UINT16 Index)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxUnLock(&CtxYuvStrmMtx[Index], HL_MTX_OPT_SET);
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetYuvStrmEfctSyncCtrl(const UINT8 MtxOpt, UINT16 Index, CTX_YUVSTRM_EFCT_SYNC_CTRL_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxLock(&CtxYuvStrmEfctSyncMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxYuvStrmEfctSyncCtrl[Index], sizeof(CTX_YUVSTRM_EFCT_SYNC_CTRL_s));
        (void)HL_MtxUnLock(&CtxYuvStrmEfctSyncMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

#if 0
void HL_SetYuvStrmEfctSyncCtrl(const UINT8 MtxOpt, UINT16 Index, const CTX_YUVSTRM_EFCT_SYNC_CTRL_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxLock(&CtxYuvStrmEfctSyncMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxYuvStrmEfctSyncCtrl[Index], pInfo, sizeof(CTX_YUVSTRM_EFCT_SYNC_CTRL_s));
        (void)HL_MtxUnLock(&CtxYuvStrmEfctSyncMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}
#endif

void HL_GetYuvStrmEfctSyncCtrlLock(const UINT16 Index, CTX_YUVSTRM_EFCT_SYNC_CTRL_s **pInfo)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxLock(&CtxYuvStrmEfctSyncMtx[Index], HL_MTX_OPT_GET);
        *pInfo = &CtxYuvStrmEfctSyncCtrl[Index];
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetYuvStrmEfctSyncCtrlUnLock(const UINT16 Index)
{
    if (Index < AMBA_DSP_MAX_YUVSTRM_NUM) {
        (void)HL_MtxUnLock(&CtxYuvStrmEfctSyncMtx[Index], HL_MTX_OPT_SET);
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

#if 0
void HL_GetYuvStrmEfctSyncJobSet(const UINT8 MtxOpt, UINT16 YuvStrmIdx, UINT16 JobIdx, CTX_EFCT_SYNC_JOB_SET_s *pInfo)
{
    if (YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) {
        if (JobIdx < MAX_GROUP_CMD_POOL_NUM) {
            // share the same mutex with EfctSyncCtrl
            (void)MtxOpt;
            (void)dsp_osal_memcpy(pInfo, &CtxYuvStrmEfctSyncJobSet[YuvStrmIdx][JobIdx], sizeof(CTX_EFCT_SYNC_JOB_SET_s));
        } else {
            AmbaLL_LogUInt5("Wrong JobIdx(%d)", JobIdx, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", YuvStrmIdx, 0U, 0U, 0U, 0U);
    }
}

void HL_SetYuvStrmEfctSyncJobSet(const UINT8 MtxOpt, UINT16 YuvStrmIdx, UINT16 JobIdx, const CTX_EFCT_SYNC_JOB_SET_s *pInfo)
{
    if (YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) {
        if (JobIdx < MAX_GROUP_CMD_POOL_NUM) {
            // share the same mutex with EfctSyncCtrl
            (void)MtxOpt;
            (void)dsp_osal_memcpy(&CtxYuvStrmEfctSyncJobSet[YuvStrmIdx][JobIdx], pInfo, sizeof(CTX_EFCT_SYNC_JOB_SET_s));
        } else {
            AmbaLL_LogUInt5("Wrong JobIdx(%d)", JobIdx, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", YuvStrmIdx, 0U, 0U, 0U, 0U);
    }
}
#endif

void HL_GetYuvStrmEfctSyncJobSetLock(const UINT16 YuvStrmIdx, const UINT16 JobIdx, CTX_EFCT_SYNC_JOB_SET_s **pInfo)
{
    if (YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) {
        if (JobIdx < MAX_GROUP_CMD_POOL_NUM) {
            // share the same mutex with EfctSyncCtrl
            *pInfo = &CtxYuvStrmEfctSyncJobSet[YuvStrmIdx][JobIdx];
        } else {
            AmbaLL_LogUInt5("Wrong JobIdx(%d)", JobIdx, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", YuvStrmIdx, 0U, 0U, 0U, 0U);
    }
}

void HL_GetYuvStrmEfctSyncJobSetLayoutLock(const UINT16 YuvStrmIdx, const UINT16 JobIdx, CTX_YUV_STRM_LAYOUT_s **pInfo)
{
    if (YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) {
        if (JobIdx < MAX_GROUP_CMD_POOL_NUM) {
            // share the same mutex with EfctSyncCtrl
            *pInfo = &CtxYuvStrmEfctSyncJobSetLayout[YuvStrmIdx][JobIdx];
        } else {
            AmbaLL_LogUInt5("HL_GetYuvStrmEfctSyncJobSetLayoutLock Wrong JobIdx(%d)", JobIdx, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("HL_GetYuvStrmEfctSyncJobSetLayoutLock Wrong YuvStrmIdx(%d)", YuvStrmIdx, 0U, 0U, 0U, 0U);
    }
}

void HL_GetYuvStrmEfctSyncJobSetUnLock(const UINT16 YuvStrmIdx, const UINT16 JobIdx)
{
    if (YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) {
        if (JobIdx < MAX_GROUP_CMD_POOL_NUM) {
            // share the same mutex with EfctSyncCtrl
        } else {
            AmbaLL_LogUInt5("Wrong JobIdx(%d)", JobIdx, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("Wrong YuvStrmIdx(%d)", YuvStrmIdx, 0U, 0U, 0U, 0U);
    }
}

UINT32 HL_GetYuvStrmEfctSyncJobSetChanCfg(const UINT16 YuvStrmIdx,
                                          const UINT16 JobIdx,
                                          const UINT16 ViewZoneId,
                                          AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s **ChanCfg)
{
    UINT32 Rval = DSP_ERR_0000;

    if (YuvStrmIdx < AMBA_DSP_MAX_YUVSTRM_NUM) {
        if (JobIdx < MAX_GROUP_CMD_POOL_NUM) {
            UINT16 ViewzoneIdx;
            CTX_YUV_STRM_LAYOUT_s *pLayout = &CtxYuvStrmEfctSyncJobSetLayout[YuvStrmIdx][JobIdx];

            // share the same mutex with EfctSyncCtrl
            for (ViewzoneIdx = 0U; ViewzoneIdx<AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; ViewzoneIdx++) {
                if (pLayout->ChanCfg[ViewzoneIdx].ViewZoneId == ViewZoneId) {
                    *ChanCfg = &pLayout->ChanCfg[ViewzoneIdx];
                    Rval = DSP_ERR_NONE;
                    break;
                }
            }
        } else {
            AmbaLL_LogUInt5("HL_GetYuvStrmEfctSyncJobSetChanCfg Wrong JobIdx(%d)", JobIdx, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("HL_GetYuvStrmEfctSyncJobSetChanCfg Wrong YuvStrmIdx(%d)", YuvStrmIdx, 0U, 0U, 0U, 0U);
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("HL_GetYuvStrmEfctSyncJobSetChanCfg cannot find (%d)", ViewZoneId, 0U, 0U, 0U, 0U);
    }

    return Rval;
}


void HL_GetVprocInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VPROC_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_VIEWZONE_NUM) {
        (void)HL_MtxLock(&CtxVprocMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxVprocInfo[Index], sizeof(CTX_VPROC_INFO_s));
        (void)HL_MtxUnLock(&CtxVprocMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong VprocIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetVprocInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_VPROC_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_VIEWZONE_NUM) {
        (void)HL_MtxLock(&CtxVprocMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxVprocInfo[Index], pInfo, sizeof(CTX_VPROC_INFO_s));
        (void)HL_MtxUnLock(&CtxVprocMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong VprocIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetStrmInfo(const UINT8 MtxOpt, UINT16 Index, CTX_STREAM_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_STREAM_NUM) {
        (void)HL_MtxLock(&CtxStrmMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxStrmInfo[Index], sizeof(CTX_STREAM_INFO_s));
        (void)HL_MtxUnLock(&CtxStrmMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong StrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetStrmInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_STREAM_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_STREAM_NUM) {
        (void)HL_MtxLock(&CtxStrmMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxStrmInfo[Index], pInfo, sizeof(CTX_STREAM_INFO_s));
        (void)HL_MtxUnLock(&CtxStrmMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong StrmIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetVoutInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VOUT_INFO_s *pInfo)
{
    if (Index < NUM_VOUT_IDX) {
        (void)HL_MtxLock(&CtxVoutMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxVoutInfo[Index], sizeof(CTX_VOUT_INFO_s));
        (void)HL_MtxUnLock(&CtxVoutMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong VoutIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetVoutInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_VOUT_INFO_s *pInfo)
{
    if (Index < NUM_VOUT_IDX) {
        (void)HL_MtxLock(&CtxVoutMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxVoutInfo[Index], pInfo, sizeof(CTX_VOUT_INFO_s));
        (void)HL_MtxUnLock(&CtxVoutMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong VoutIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetEncoderInfo(const UINT8 MtxOpt, CTX_ENCODER_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxEncoderMtx, MtxOpt);
    (void)dsp_osal_memcpy(pInfo, &CtxEncoderInfo, sizeof(CTX_ENCODER_INFO_s));
    (void)HL_MtxUnLock(&CtxEncoderMtx, MtxOpt);
}

void HL_SetEncoderInfo(const UINT8 MtxOpt, const CTX_ENCODER_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxEncoderMtx, MtxOpt);
    (void)dsp_osal_memcpy(&CtxEncoderInfo, pInfo, sizeof(CTX_ENCODER_INFO_s));
    (void)HL_MtxUnLock(&CtxEncoderMtx, MtxOpt);
}

void HL_GetDecoderInfo(const UINT8 MtxOpt, CTX_DECODER_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxDecoderMtx, MtxOpt);
    (void)dsp_osal_memcpy(pInfo, &CtxDecoderInfo, sizeof(CTX_DECODER_INFO_s));
    (void)HL_MtxUnLock(&CtxDecoderMtx, MtxOpt);
}

void HL_SetDecoderInfo(const UINT8 MtxOpt, const CTX_DECODER_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxDecoderMtx, MtxOpt);
    (void)dsp_osal_memcpy(&CtxDecoderInfo, pInfo, sizeof(CTX_DECODER_INFO_s));
    (void)HL_MtxUnLock(&CtxDecoderMtx, MtxOpt);
}

void HL_GetVidDecInfo(const UINT8 MtxOpt, UINT16 Index, CTX_VID_DEC_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_DEC_STREAM_NUM) {
        (void)HL_MtxLock(&CtxVidDecMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxVidDecInfo[Index], sizeof(CTX_VID_DEC_INFO_s));
        (void)HL_MtxUnLock(&CtxVidDecMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong DecodeIdx(%d)", Index, 0U, 0U, 0U, 0U);
        dsp_osal_print_flush();
    }
}

void HL_SetVidDecInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_VID_DEC_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_DEC_STREAM_NUM) {
        (void)HL_MtxLock(&CtxVidDecMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxVidDecInfo[Index], pInfo, sizeof(CTX_VID_DEC_INFO_s));
        (void)HL_MtxUnLock(&CtxVidDecMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong DecodeIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetStlInfo(const UINT8 MtxOpt, CTX_STILL_INFO_s *pInfo)
{
    (void)HL_MtxLock(&CtxStlMtx, MtxOpt);
    (void)dsp_osal_memcpy(pInfo, &CtxStlInfo, sizeof(CTX_STILL_INFO_s));
    (void)HL_MtxUnLock(&CtxStlMtx, MtxOpt);
}

void HL_SetStlInfo(const UINT8 MtxOpt, const CTX_STILL_INFO_s *pInfo)
{
     (void)HL_MtxLock(&CtxStlMtx, MtxOpt);
     (void)dsp_osal_memcpy(&CtxStlInfo, pInfo, sizeof(CTX_STILL_INFO_s));
     (void)HL_MtxUnLock(&CtxStlMtx, MtxOpt);
}

void HL_GetDataCapInfo(const UINT8 MtxOpt, UINT16 Index, CTX_DATACAP_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_DATACAP_NUM) {
        (void)HL_MtxLock(&CtxDataCapMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxDataCapInfo[Index], sizeof(CTX_DATACAP_INFO_s));
        (void)HL_MtxUnLock(&CtxDataCapMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong DataCapIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetDataCapInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_DATACAP_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_DATACAP_NUM) {
        (void)HL_MtxLock(&CtxDataCapMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxDataCapInfo[Index], pInfo, sizeof(CTX_DATACAP_INFO_s));
        (void)HL_MtxUnLock(&CtxDataCapMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong DataCapIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetTimeLapseInfo(const UINT8 MtxOpt, UINT16 Index, CTX_TIMELAPSE_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_STREAM_NUM) {
        (void)HL_MtxLock(&CtxTimeLapseMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxTimeLapseInfo[Index], sizeof(CTX_TIMELAPSE_INFO_s));
        (void)HL_MtxUnLock(&CtxTimeLapseMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong TimeLapseIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetTimeLapseInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_TIMELAPSE_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_STREAM_NUM) {
        (void)HL_MtxLock(&CtxTimeLapseMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxTimeLapseInfo[Index], pInfo, sizeof(CTX_TIMELAPSE_INFO_s));
        (void)HL_MtxUnLock(&CtxTimeLapseMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong TimeLapseIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_GetEncGrpInfo(const UINT8 MtxOpt, UINT16 Index, CTX_ENC_GRP_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_ENC_GRP_NUM) {
        (void)HL_MtxLock(&CtxEncGrpMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(pInfo, &CtxEncGrpInfo[Index], sizeof(CTX_ENC_GRP_INFO_s));
        (void)HL_MtxUnLock(&CtxEncGrpMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong EncGrpIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_SetEncGrpInfo(const UINT8 MtxOpt, UINT16 Index, const CTX_ENC_GRP_INFO_s *pInfo)
{
    if (Index < AMBA_DSP_MAX_ENC_GRP_NUM) {
        (void)HL_MtxLock(&CtxEncGrpMtx[Index], MtxOpt);
        (void)dsp_osal_memcpy(&CtxEncGrpInfo[Index], pInfo, sizeof(CTX_ENC_GRP_INFO_s));
        (void)HL_MtxUnLock(&CtxEncGrpMtx[Index], MtxOpt);
    } else {
        AmbaLL_LogUInt5("Wrong EncGrpIdx(%d)", Index, 0U, 0U, 0U, 0U);
    }
}

void HL_UpdateCtxViewZoneBatchQRPtr(const UINT32 BatchCmdId)
{
    DSP_BATCH_CMD_ID_s DspBatchCmdId = {0};
    UINT16 ViewZoneIdx, BatchCmdQPool;

    if (BatchCmdId > 0U) {
        (void)dsp_osal_memcpy(&DspBatchCmdId, &BatchCmdId, sizeof(UINT32));
        ViewZoneIdx = (UINT16)DspBatchCmdId.ViewZoneId;
        BatchCmdQPool = (UINT16)(DspBatchCmdId.PoolId & 0x3FFU) - 1U;

        if (ViewZoneIdx < AMBA_DSP_MAX_VIEWZONE_NUM) {
            CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

            HL_GetViewZoneInfoLock(ViewZoneIdx, &ViewZoneInfo);
            if (ViewZoneInfo->BatchQPoolDesc.Wp == ViewZoneInfo->BatchQPoolDesc.Rp) {
                // Nothing to be read
            } else{
                UINT8 ExitLoop = 0U;
                UINT32 TempPtr = ViewZoneInfo->BatchQPoolDesc.Rp;

                if (TempPtr == BatchCmdQPool) {
                    // Already update
                } else {
                    // Find batch pool index from mapping table
                    while ((TempPtr != ViewZoneInfo->BatchQPoolDesc.Wp) && (ExitLoop == 0U)) {
                        TempPtr++;
                        if (TempPtr >= ViewZoneInfo->BatchQPoolDesc.Size) {
                            TempPtr = 0U;
                        }
                        if (TempPtr == BatchCmdQPool) {
                            ExitLoop = 1U;
                        }
                    }
                    if (ExitLoop == 1U) {
                        ViewZoneInfo->BatchQPoolDesc.Rp = TempPtr;
                        AmbaLL_Log(AMBALLLOG_TYPE_BATCH,"Rel Bch Id[%d] W/R[%d %d]", ViewZoneIdx, ViewZoneInfo->BatchQPoolDesc.Wp, ViewZoneInfo->BatchQPoolDesc.Rp);
                    } else {
//                        AmbaLL_LogUInt5("Update BatchRP: ViewZone[%d] BatchCmdId[0x%X] WP[%d] RP[%d]",
//                                         ViewZoneIdx, BatchCmdId, ViewZoneInfo->BatchQPoolDesc.Wp, ViewZoneInfo->BatchQPoolDesc.Rp, 0U);
                    }
                }
            }
            HL_GetViewZoneInfoUnLock(ViewZoneIdx);
        } else {
            AmbaLL_LogUInt5("Wrong ViewZoneIdx[%d] from BatchCmdId[0x%X]", ViewZoneIdx, BatchCmdId, 0U, 0U, 0U);
        }
    }
}

void HL_SetDspBatchQInfo(UINT32 BatchCmdId, UINT8 AutoReset, UINT8 InfoIdx, UINT32 InfoData)
{
    DSP_BATCH_CMD_ID_s DspBatchCmdId = {0};
    UINT16 ViewZoneId, BatchCmdQPool;

    if (BatchCmdId > 0U) {
        (void)dsp_osal_memcpy(&DspBatchCmdId, &BatchCmdId, sizeof(UINT32));
        ViewZoneId = (UINT16)(DspBatchCmdId.ViewZoneId);
        BatchCmdQPool = (UINT16)(DspBatchCmdId.PoolId & 0x3FFU) - 1U;

        if (ViewZoneId < AMBA_DSP_MAX_VIEWZONE_NUM) {
            if (BatchCmdQPool < MAX_BATCH_CMD_POOL_NUM) {
                (void)HL_MtxLock(&CtxBatchMapMtx[ViewZoneId], HL_MTX_OPT_ALL);
                if (AutoReset == 1U) {
                    (void)dsp_osal_memset(&CtxDspBatchQInfo[ViewZoneId][BatchCmdQPool], 0, sizeof(CTX_BATCHQ_INFO_s));
                }
                switch (InfoIdx) {
                    case BATCHQ_INFO_ISO_CFG :
                        CtxDspBatchQInfo[ViewZoneId][BatchCmdQPool].IsoCfgId = InfoData;
                        break;
                    case BATCHQ_INFO_IMG_PRMD:
                        CtxDspBatchQInfo[ViewZoneId][BatchCmdQPool].ImgPrmdId = InfoData;
                        break;
                    case BATCHQ_INFO_YUVSTRM_GRP:
                        CtxDspBatchQInfo[ViewZoneId][BatchCmdQPool].YuvStrmGrpId = InfoData;
                        break;
                    case BATCHQ_INFO_STL_PROC:
                        CtxDspBatchQInfo[ViewZoneId][BatchCmdQPool].StlProcId = InfoData;
                        break;
                    default:
                        // DO NOTHING
                        break;
                }
                (void)HL_MtxUnLock(&CtxBatchMapMtx[ViewZoneId], HL_MTX_OPT_ALL);
            } else {
                AmbaLL_LogUInt5("Wrong BatchCmdQPoolId(%d) from BatchId(0x%x)", BatchCmdQPool, BatchCmdId, 0U, 0U, 0U);
            }
        } else {
            AmbaLL_LogUInt5("Wrong ViewZoneId(%d) from BatchId(0x%x)", ViewZoneId, BatchCmdId, 0U, 0U, 0U);
        }
    }
}

void HL_GetDspBatchQInfo(UINT32 BatchCmdId, CTX_BATCHQ_INFO_s *BatchQInfo)
{
    DSP_BATCH_CMD_ID_s DspBatchCmdId = {0};
    UINT16 ViewZoneId, BatchCmdQPool;

    if (BatchCmdId > 0U) {
        (void)dsp_osal_memcpy(&DspBatchCmdId, &BatchCmdId, sizeof(UINT32));
        ViewZoneId = (UINT16)DspBatchCmdId.ViewZoneId;
        BatchCmdQPool = (UINT16)(DspBatchCmdId.PoolId & 0x3FFU) - 1U;

        if (ViewZoneId < AMBA_DSP_MAX_VIEWZONE_NUM) {
            if (BatchCmdQPool < MAX_BATCH_CMD_POOL_NUM) {
                (void)dsp_osal_memcpy(BatchQInfo, &CtxDspBatchQInfo[ViewZoneId][BatchCmdQPool], sizeof(CTX_BATCHQ_INFO_s));
            }
        }
    }
}

void HL_GetDspBatchCmdId(ULONG BatchCmdQAddr, UINT16 ViewZoneId, UINT32 *pBatchCmdId)
{
#ifdef USE_FLEXIBLE_BATCH_CMD_POOL
    ULONG ULAddr = 0U;
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 Rval;
    UINT32 BatchIdx;

    if (pBatchCmdId == NULL) {
        //TBD
    } else {
        Rval = DSP_GetProtBuf(DSP_PROT_BUF_BATCH_CMD_Q, 0U, &MemBlk);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err] DSP_GetProtBuf[%d][%d] return [0x%X] @%d", DSP_PROT_BUF_BATCH_CMD_Q, 0U, Rval, __LINE__, 0U);
        } else {
            ULAddr = MemBlk.Base;
            ULAddr += (sizeof(dsp_cmd_t)*ViewZoneId)*MAX_BATCH_CMD_POOL_NUM;
            BatchIdx = ((UINT32)(BatchCmdQAddr - ULAddr)) / (UINT32)sizeof(dsp_cmd_t);
            if (BatchIdx < MAX_BATCH_CMD_POOL_NUM) {
                HL_GetBatchCmdId(ViewZoneId, (UINT16)BatchIdx, pBatchCmdId);
            }
        }
    }
#else
    UINT16 i;
    UINT8 ExitLoop;
    UINT32 *pBatchQAddr;
    ULONG ULAddr = 0U;

    if (pBatchCmdId == NULL) {
        //TBD
    } else {
        ExitLoop = 0U;
        for (i=0U; i<MAX_BATCH_CMD_POOL_NUM; i++) {
            HL_GetPointerToDspBatchQ(ViewZoneId, i, &pBatchQAddr, pBatchCmdId);
            dsp_osal_typecast(&ULAddr, &pBatchQAddr);

            if (BatchCmdQAddr == ULAddr) {
                ExitLoop = 1U;
                break;
            }
        }

        if (ExitLoop == 0U) {
            *pBatchCmdId = 0U;
        }
    }
#endif
}

void HL_CtrlBatchQBufMtx(const UINT8 MtxOpt, UINT16 ViewZoneId)
{
    osal_mutex_t *Mtx;

    if (ViewZoneId < AMBA_DSP_MAX_VIEWZONE_NUM) {
        Mtx = &CtxBatchQBufMtx[ViewZoneId];
        if (MtxOpt == HL_MTX_OPT_GET) {
            (void)HL_MtxLock(Mtx, MtxOpt);
        } else if (MtxOpt == HL_MTX_OPT_SET) {
            (void)HL_MtxUnLock(Mtx, MtxOpt);
        } else {
            //No Such Option
#if defined(CONFIG_THREADX)
            AmbaLL_LogStr5("Mtx[%s] Opt?", Mtx->tx_mutex_name, NULL, NULL, NULL, NULL);
#else
            AmbaLL_LogUInt5("CtxBatchQBufMtx[%d] Opt[%d]?", ViewZoneId, MtxOpt, 0U, 0U, 0U);
#endif
        }
    }
}

UINT8 HL_GetRescState(void)
{
    UINT8 RescState;
    (void)HL_MtxLock(&CtxResMtx, HL_MTX_OPT_ALL);
    RescState = CtxResInfo.RescState;
    (void)HL_MtxUnLock(&CtxResMtx, HL_MTX_OPT_ALL);
    return RescState;
}

void HL_SetRescState(UINT8 RescState)
{
    if (RescState <= HL_RESC_SETTLED) {
        (void)HL_MtxLock(&CtxResMtx, HL_MTX_OPT_ALL);
        CtxResInfo.RescState = RescState;
        (void)HL_MtxUnLock(&CtxResMtx, HL_MTX_OPT_ALL);
    } else {
        AmbaLL_LogUInt5("HL_SetRescState input %u incorrect", (UINT32)RescState, 0U, 0U, 0U, 0U);
    }
}

void HL_ResetViewZoneInfo(void)
{
    UINT16 i;
    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        (void)HL_MtxLock(&CtxViewZoneMtx[i], HL_MTX_OPT_ALL);

        (void)dsp_osal_memset(&CtxViewZoneInfo[i], 0, sizeof(CTX_VIEWZONE_INFO_s));
        CtxViewZoneInfo[i].SourceTdIdx = VIEWZONE_SRC_TD_IDX_NULL;
        CtxViewZoneInfo[i].BatchQPoolDesc.StartAddr = 0U;
        CtxViewZoneInfo[i].BatchQPoolDesc.Size = MAX_BATCH_CMD_POOL_NUM;
        CtxViewZoneInfo[i].BatchQPoolDesc.Wp = MAX_BATCH_CMD_POOL_NUM - 1U;
        CtxViewZoneInfo[i].BatchQPoolDesc.Rp = MAX_BATCH_CMD_POOL_NUM - 1U;
        CtxViewZoneInfo[i].YuvInput.RawSeqWp = 0xFFU;
        CtxViewZoneInfo[i].PymdInput.RawSeqWp = 0xFFU;
        CtxViewZoneInfo[i].MainY12Input.RawSeqWp = 0xFFU;
        CtxViewZoneInfo[i].MainIrInput.RawSeqWp = 0xFFU;
        // Cmd be updated by default
        (void)dsp_osal_memset(&(CtxViewZoneInfo[i].CmdUpdate), 0xFF, sizeof(CTX_CMD_NEED_UPDATE_s));

        (void)HL_MtxUnLock(&CtxViewZoneMtx[i], HL_MTX_OPT_ALL);
    }
}

void HL_ResetYuvStreamInfo(void)
{
    UINT16 i;
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        (void)HL_MtxLock(&CtxYuvStrmMtx[i], HL_MTX_OPT_ALL);

        (void)dsp_osal_memset(&CtxYuvStrmInfo[i], 0, sizeof(CTX_YUV_STRM_INFO_s));
        CtxYuvStrmInfo[i].PpStrmPoolDesc.StartAddr = 0U;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Size = MAX_GROUP_CMD_POOL_NUM;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Wp = MAX_GROUP_CMD_POOL_NUM - 1U;
        CtxYuvStrmInfo[i].PpStrmPoolDesc.Rp = MAX_GROUP_CMD_POOL_NUM - 1U;
        /* EffectBufDesc */
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.EffectBufDesc, 0xFFFF, sizeof(CtxYuvStrmInfo[i].Layout.EffectBufDesc));

        (void)HL_MtxUnLock(&CtxYuvStrmMtx[i], HL_MTX_OPT_ALL);
    }
}

void HL_ResetYuvStreamInfoPP(void)
{
    UINT16 i;

    /* YuvStrmInfo */
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.CopyJobNum[0U], 0, sizeof(UINT16)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
        /* Reset everytime at HL_CalcEffectPostPJob */
//        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.CopyJobDesc[0U], 0, (sizeof(DSP_EFFECT_COPY_JOB_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);
//        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.BlendJobDesc[0U], 0, (sizeof(DSP_EFFECT_BLEND_JOB_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);
        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.EffectBufDesc, 0xFFFF, sizeof(CtxYuvStrmInfo[i].Layout.EffectBufDesc));
//        CtxYuvStrmInfo[i].Layout.ChanLinked = 0U;
    }

    /* YuvStrmInfo */
//    (void)dsp_osal_memset(&CtxYuvStrmInfo[0], 0, sizeof(CTX_YUV_STRM_INFO_s)*AMBA_DSP_MAX_YUVSTRM_NUM);
    //YuvStrm
//    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
//        CtxYuvStrmInfo[i].PpStrmPoolDesc.StartAddr = 0U;
//        CtxYuvStrmInfo[i].PpStrmPoolDesc.Size = MAX_GROUP_CMD_POOL_NUM;
//        CtxYuvStrmInfo[i].PpStrmPoolDesc.Wp = MAX_GROUP_CMD_POOL_NUM - 1U;
//        CtxYuvStrmInfo[i].PpStrmPoolDesc.Rp = MAX_GROUP_CMD_POOL_NUM - 1U;

        /* EffectBufDesc */
//        (void)dsp_osal_memset(&CtxYuvStrmInfo[i].Layout.EffectBufDesc, 0xFFFF, sizeof(CtxYuvStrmInfo[i].Layout.EffectBufDesc));
//    }

//    {
//        vproc_pp_stream_cntl_t *pPpStrmCtrl = NULL;
//        vproc_post_r2y_copy_cfg_t *pPpStrmCopyCtrl = NULL;
//        vproc_y2y_blending_cfg_t *pPpStrmBldCtrl = NULL;
//
//        DSP_GetDspPpStrmCtrlCfg(0U, 0U, 0U, &pPpStrmCtrl);
//        (void)dsp_osal_memset(pPpStrmCtrl, 0x0, ((sizeof(vproc_pp_stream_cntl_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
//
//        DSP_GetDspPpStrmCopyCfg(0U, 0U, 0U,&pPpStrmCopyCtrl);
//        (void)dsp_osal_memset(pPpStrmCopyCtrl, 0x0, (((sizeof(vproc_post_r2y_copy_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_COPY_NUMBER);
//
//        DSP_GetDspPpStrmBldCfg(0U, 0U, 0U,&pPpStrmBldCtrl);
//        (void)dsp_osal_memset(pPpStrmBldCtrl, 0x0, (((sizeof(vproc_y2y_blending_cfg_t)*AMBA_DSP_MAX_YUVSTRM_NUM)*MAX_GROUP_CMD_POOL_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*DSP_MAX_PP_STRM_BLEND_NUMBER);
//    }
}

void HL_ResetYuvStrmEfctSyncCtrl(void)
{
    UINT16 i;
    for (i = 0U; i<AMBA_DSP_MAX_YUVSTRM_NUM; i++) {
        (void)HL_MtxLock(&CtxYuvStrmEfctSyncMtx[i], HL_MTX_OPT_ALL);

        (void)dsp_osal_memset(&CtxYuvStrmEfctSyncCtrl[i], 0, sizeof(CTX_YUVSTRM_EFCT_SYNC_CTRL_s));
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.StartAddr = 0U;
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.Size = MAX_GROUP_CMD_POOL_NUM;
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.Wp = MAX_GROUP_CMD_POOL_NUM - 1U;
        CtxYuvStrmEfctSyncCtrl[i].PoolDesc.Rp = MAX_GROUP_CMD_POOL_NUM - 1U;

        (void)HL_MtxUnLock(&CtxYuvStrmEfctSyncMtx[i], HL_MTX_OPT_ALL);
    }
}

void HL_ResetYuvStrmEfctSyncJobSet(void)
{
    (void)dsp_osal_memset(&CtxYuvStrmEfctSyncJobSet[0][0], 0, sizeof(CTX_EFCT_SYNC_JOB_SET_s)*AMBA_DSP_MAX_YUVSTRM_NUM*MAX_GROUP_CMD_POOL_NUM);
    (void)dsp_osal_memset(&CtxYuvStrmEfctSyncJobSetLayout[0][0], 0, sizeof(CTX_YUV_STRM_LAYOUT_s)*AMBA_DSP_MAX_YUVSTRM_NUM*MAX_GROUP_CMD_POOL_NUM);
}

void HL_ResetVprocInfo(void)
{
    UINT16 i;
    for (i = 0U; i<AMBA_DSP_MAX_VIEWZONE_NUM; i++) {

        (void)HL_MtxLock(&CtxVprocMtx[i], HL_MTX_OPT_ALL);

        (void)dsp_osal_memset(&CtxVprocInfo[i], 0, sizeof(CTX_VPROC_INFO_s));

        (void)HL_MtxUnLock(&CtxVprocMtx[i], HL_MTX_OPT_ALL);
    }
}

UINT32 DSP_ReqBuf(const DSP_POOL_DESC_s *BufPool, UINT32 ReqSize, UINT32 *NewWp, UINT32 FullnessCheck)
{
    UINT32 Rval = OK;
    if ((BufPool == NULL) ||
        (NewWp == NULL)) {
        Rval = DSP_ERR_0000;
    } else if (ReqSize > BufPool->Size) {
        Rval = DSP_ERR_0001;
    } else {
        // we must use continues space for ucode in wrap around case
        if ((BufPool->Wp + ReqSize) >= BufPool->Size) {
            *NewWp = BufPool->StartAddr + ReqSize - 1U;
            if (FullnessCheck == 1U) {
                if (((BufPool->Rp > BufPool->Wp) && (*NewWp < BufPool->Rp)) ||
                    ((BufPool->Rp < BufPool->Wp) && (*NewWp >= BufPool->Rp))) {
                    Rval = DSP_ERR_0005;
                }
            }
        } else {
            *NewWp = BufPool->StartAddr + BufPool->Wp + ReqSize;
            if (FullnessCheck == 1U) {
                if (((BufPool->Rp > BufPool->Wp) && (*NewWp >= BufPool->Rp))) {
                    Rval = DSP_ERR_0005;
                }
            }
        }
    }

    return Rval;
}

UINT32 DSP_RelBuf(const DSP_POOL_DESC_s *BufPool, UINT32 RelSize, UINT32 *NewRp, UINT32 FullnessCheck)
{
    UINT32 Rval = OK;

    if ((BufPool == NULL) ||
        (NewRp == NULL)) {
        Rval = DSP_ERR_0000;
    } else if (RelSize > BufPool->Size) {
        Rval = DSP_ERR_0001;
    } else {
        if (RelSize == 1U) {
            if ((FullnessCheck == 1U) &&
                (BufPool->Rp == BufPool->Wp)) {
                Rval = DSP_ERR_0005;
            } else {
                *NewRp = (BufPool->Rp + 1U) % BufPool->Size;
                *NewRp += BufPool->StartAddr;
            }
        } else {
            //TBD
        }
    }

    return Rval;
}

UINT32 HL_MtxLock(osal_mutex_t *Mtx, const UINT8 MtxOpt)
{
    UINT32 Rval = OK;

    if ((MtxOpt == HL_MTX_OPT_ALL) || (MtxOpt == HL_MTX_OPT_GET)) {
        Rval = dsp_osal_mutex_lock(Mtx, MUTEX_TIMEOUT);
        if (Rval != OK) {
#if defined(CONFIG_THREADX)
            AmbaLL_LogStr5("%s forgets to SET after GET", Mtx->tx_mutex_name, NULL, NULL, NULL, NULL);
#else
            AmbaLL_LogUInt5("MtxLock Fail", 0U, 0U, 0U, 0U, 0U);
#endif
        }
    }

    return Rval;
}

UINT32 HL_MtxUnLock(osal_mutex_t *Mtx, const UINT8 MtxOpt)
{
    UINT32 Rval = OK;

    if ((MtxOpt == HL_MTX_OPT_ALL) || (MtxOpt == HL_MTX_OPT_SET)) {
        (void)dsp_osal_mutex_unlock(Mtx);
    }

    return Rval;
}

UINT8 HL_IS_ROTATE(UINT8 RotateFlip)
{
    UINT8 Enable = 0;

    if ((RotateFlip == AMBA_DSP_ROTATE_90) ||
        (RotateFlip == AMBA_DSP_ROTATE_90_VERT_FLIP) ||
        (RotateFlip == AMBA_DSP_ROTATE_270) ||
        (RotateFlip == AMBA_DSP_ROTATE_270_VERT_FLIP)) {
        Enable = 1;
    }

    return Enable;
}

UINT8 HL_IS_HFLIP(UINT8 RotateFlip)
{
    UINT8 Enable = 0;

    if ((RotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) ||
        (RotateFlip == AMBA_DSP_ROTATE_180) ||
        (RotateFlip == AMBA_DSP_ROTATE_270) ||
        (RotateFlip == AMBA_DSP_ROTATE_270_VERT_FLIP)) {
        Enable = 1;
    }

    return Enable;
}

UINT8 HL_IS_VFLIP(UINT8 RotateFlip)
{
    UINT8 Enable = 0;

    if ((RotateFlip == AMBA_DSP_ROTATE_90_VERT_FLIP) ||
        (RotateFlip == AMBA_DSP_ROTATE_180) ||
        (RotateFlip == AMBA_DSP_ROTATE_180_HORZ_FLIP) ||
        (RotateFlip == AMBA_DSP_ROTATE_270)) {
        Enable = 1;
    }

    return Enable;
}

UINT8 HL_ROTATE_FLIP_COMPOSITE(UINT8 Rotate, UINT8 Flip)
{
    UINT8 RotateFlip;

    if ((Rotate == DSP_NO_ROTATE) && (Flip == DSP_H_FLIP)) {
        RotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP;
    } else if ((Rotate == DSP_ROTATE_90_DEGREE) && (Flip == DSP_NO_FLIP)) {
        RotateFlip = AMBA_DSP_ROTATE_90;
    } else if ((Rotate == DSP_ROTATE_90_DEGREE) && (Flip == DSP_V_FLIP)) {
        RotateFlip = AMBA_DSP_ROTATE_90_VERT_FLIP;
    } else if ((Rotate == DSP_NO_ROTATE) && (Flip == DSP_V_H_FLIP)) {
        RotateFlip = AMBA_DSP_ROTATE_180;
    } else if ((Rotate == DSP_NO_ROTATE) && (Flip == DSP_V_FLIP)) {
        RotateFlip = AMBA_DSP_ROTATE_180_HORZ_FLIP;
    } else if ((Rotate == DSP_ROTATE_90_DEGREE) && (Flip == DSP_V_H_FLIP)) {
        RotateFlip = AMBA_DSP_ROTATE_270;
    } else if ((Rotate == DSP_ROTATE_90_DEGREE) && (Flip == DSP_H_FLIP)) {
        RotateFlip = AMBA_DSP_ROTATE_270_VERT_FLIP;
    } else {
        RotateFlip = AMBA_DSP_ROTATE_0;
    }

    return RotateFlip;
}

UINT8 HL_GET_ROTATE(UINT8 RotateFlip)
{
    UINT8 Rotate;

    if ((RotateFlip == AMBA_DSP_ROTATE_0) ||
        (RotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) ||
        (RotateFlip == AMBA_DSP_ROTATE_180) ||
        (RotateFlip == AMBA_DSP_ROTATE_180_HORZ_FLIP)) {
        Rotate = DSP_NO_ROTATE;
    } else {
        Rotate = DSP_ROTATE_90_DEGREE;
    }

    return Rotate;
}

UINT8 HL_GET_FLIP(UINT8 RotateFlip)
{
    UINT8 Flip;

    if ((RotateFlip == AMBA_DSP_ROTATE_0) ||
        (RotateFlip == AMBA_DSP_ROTATE_90)) {
        Flip = DSP_NO_FLIP;
    } else if ((RotateFlip == AMBA_DSP_ROTATE_180) ||
        (RotateFlip == AMBA_DSP_ROTATE_270)) {
        Flip = DSP_V_H_FLIP;
    } else if ((RotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) ||
        (RotateFlip == AMBA_DSP_ROTATE_270_VERT_FLIP)) {
        Flip = DSP_H_FLIP;
    } else {
        Flip = DSP_V_FLIP;
    }

    return Flip;
}


UINT32 HL_FRAME_RATE_MAP(const UINT8 Type, AMBA_DSP_FRAME_RATE_s Frate, UINT32 *NewFrate)
{
    UINT32 Rval = OK;
    UINT32 FrameRate = 0U;

    if (NewFrate == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (Type == VIN_ENC_FRAME_RATE) {
            FrameRate = Frate.TimeScale / Frate.NumUnitsInTick;
            if (FrameRate == 29U) {
                if (Frate.Interlace == 1U) {
                    FrameRate = VIN_ENC_FRAME_RATE_29_97_I;
                } else {
                    FrameRate = VIN_ENC_FRAME_RATE_29_97_P;
                }
            } else if (FrameRate == 59U) {
                if (Frate.Interlace == 1U) {
                    FrameRate = VIN_ENC_FRAME_RATE_59_94_I;
                } else {
                    FrameRate = VIN_ENC_FRAME_RATE_59_94_P;
                }
            } else if (FrameRate == 119U) {
                FrameRate = 120U;
            } else if (FrameRate == 239U) {
                FrameRate = 240U;
            } else if (FrameRate == 89U)  {
                FrameRate = 90U;
            } else if (FrameRate == 14U)  {
                FrameRate = 15U;
            } else {
               //
            }
        } else {
            FrameRate = Frate.TimeScale / Frate.NumUnitsInTick;
            if (FrameRate == 29U) {
                FrameRate = VOUT_FRAME_RATE_29_97;
            } else if (FrameRate == 59U) {
                FrameRate = VOUT_FRAME_RATE_59_94;
            } else {
               //
            }
        }

        *NewFrate = FrameRate;
    }

    return Rval;
}

#if 0
UINT32 HL_VOUT_YUV_COMPOSITE(UINT8 Y, UINT8 U, UINT8 V)
{
    UINT32 Value = 0U;
    UINT32 Tmp;

    Tmp = (UINT32)V;
    Value |= ((Tmp & 0xFFU));
    Tmp = (UINT32)U;
    Value |= ((Tmp & 0xFFU) << (8U*VOUT_GET_U));
    Tmp = (UINT32)Y;
    Value |= ((Tmp & 0xFFU) << (8U*VOUT_GET_Y));

    return Value;
}
#endif

UINT8 HL_VOUT_GET_YUV(const UINT8 Type, UINT32 Yuv)
{
    UINT8 Value = 0U;

    if (Type == VOUT_GET_Y) {
        Value = (UINT8)((Yuv >> (8U*VOUT_GET_Y)) & 0xFFU);
    } else if (Type == VOUT_GET_U) {
        Value = (UINT8)((Yuv >> (8U*VOUT_GET_U)) & 0xFFU);
    } else {
        Value = (UINT8)((Yuv) & 0xFFU);
    }

    return Value;
}

//UINT8 CtxCmdBufInfoNum = 0U;
//UINT8 CtxCmdBufInfoNumMax = 0U;

void HL_AcqCmdBuffer(UINT8 *Id, void **BufferAddr)
{
    UINT8 Idx;
    ULONG AcqBufferAddr = 0U;

    (void)HL_MtxLock(&CtxCmdBufInfo.PoolMtx, HL_MTX_OPT_GET);
    for (Idx=0U; Idx<CTX_CMD_BUF_POOL_NUM; Idx++) {
        if (CtxCmdBufInfo.PoolUsage[Idx] == 0U) {
            *Id = Idx;
            CtxCmdBufInfo.PoolUsage[Idx] = 1U;
            AcqBufferAddr = CtxCmdBufInfo.BaseAddr + (ULONG)(Idx*sizeof(dsp_cmd_t));
            dsp_osal_typecast(BufferAddr, &AcqBufferAddr);
            (void)dsp_osal_memset(*BufferAddr, 0, sizeof(dsp_cmd_t));
            break;
        }
    }
    (void)HL_MtxUnLock(&CtxCmdBufInfo.PoolMtx, HL_MTX_OPT_SET);
//    CtxCmdBufInfoNum++;
//    if (CtxCmdBufInfoNum >= CtxCmdBufInfoNumMax) {
//        CtxCmdBufInfoNumMax = CtxCmdBufInfoNum;
//    }
    if (AcqBufferAddr == 0U) {
        *Id = CTX_CMD_BUF_POOL_NUM;
        dsp_osal_typecast(BufferAddr, &HL_DefCtxCmdBufPtrSys);
        AmbaLL_LogUInt5("CtxCmdBufInfo pool full", 0U, 0U, 0U, 0U, 0U);
        dsp_osal_print_flush();
    }
}

void HL_RelCmdBuffer(const UINT8 Id)
{
    if (Id < CTX_CMD_BUF_POOL_NUM) {
        (void)HL_MtxLock(&CtxCmdBufInfo.PoolMtx, HL_MTX_OPT_GET);
        CtxCmdBufInfo.PoolUsage[Id] = 0U;
        (void)HL_MtxUnLock(&CtxCmdBufInfo.PoolMtx, HL_MTX_OPT_SET);
    }
//    CtxCmdBufInfoNum--;
}

