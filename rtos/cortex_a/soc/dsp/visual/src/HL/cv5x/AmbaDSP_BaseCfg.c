/**
*  @file AmbaDSP_BaseCfg.c
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
*  @details DSP Base config realtive API, use for filling specific dsp configuration
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_BaseCfg.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_MsgDispatcher.h"

//#define DEBUG_PARAM_CALC
//#define DEBUG_DSP_WORK_CALC
//#define DEBUG_DRAM_BW_CALC

static inline void HL_PrepareBaseParamBoot(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT32 Mask, Cnt = 0U;

    /* Row[25] - dsp_boot_config() : init_dsp_mm */
    pBaseParam->DramParNum += 8U;
    pBaseParam->SmemParNum += 8U;

    /* init_orc_msg_q_allocator : q_alloc_all */
    // It is always true that max_orc_all_msg_qs is greater than 0
    pBaseParam->SmemParNum += 1U;

    /* init_boot_cmd_cache : dsp_cfg_cmd_daddr */
    pBaseParam->DramParNum += 1U;

    /* orcvin_build_dsp_flow_graph : flow_base_q_out_sbase */
    //pBaseParam->SmemParNum += 1U;  //max_data_out_num_per_vproc_strm

    /* create_enc_flow_base : alloc_orc_msg_q */
    pBaseParam->SmemParNum += pBaseCfg->EncNum;
    pBaseParam->OrccodeMsgNum += pBaseCfg->EncNum;

    /* create_enc_flow_base : alloc_dsp_cmd_qs */
    pBaseParam->DramParNum += pBaseCfg->EncNum;
    pBaseParam->OrccodeMsgNum += pBaseCfg->EncNum;

    /* create_eng0_flow_base : alloc_orc_msg_q */
    pBaseParam->SmemParNum += (pBaseCfg->EncNum > 0U)? 2U: 0U;
    pBaseParam->OrccodeMsgNum += (pBaseCfg->EncNum > 0U)? 2U: 0U;

    /* create_eng1_flow_base : alloc_orc_msg_q */
    pBaseParam->SmemParNum += (pBaseCfg->EncNum > 0U)? 2U: 0U;
    pBaseParam->OrccodeMsgNum += (pBaseCfg->EncNum > 0U)? 2U: 0U;

    /* create_dec_flow_base : alloc_orc_msg_q */
    pBaseParam->DramParNum += (pBaseCfg->DecNum > 0U)? 1U: 0U;
    pBaseParam->SmemParNum += (pBaseCfg->DecNum > 0U)? 1U: 0U;
    pBaseParam->OrccodeMsgNum += (pBaseCfg->DecNum > 0U)? 1U: 0U;

    /* create_dec_flow_base : alloc_dsp_cmd_qs */
    pBaseParam->DramParNum += (pBaseCfg->DecNum > 0U)? 1U: 0U;
    pBaseParam->OrccodeMsgNum += (pBaseCfg->DecNum > 0U)? 1U: 0U;

    /* create_vout_flow_base : alloc_orc_msg_q */
    pBaseParam->DramParNum += (pBaseCfg->VoutBitMask > 0U)? 1U: 0U;
    pBaseParam->OrccodeMsgNum += (pBaseCfg->VoutBitMask > 0U)? 1U: 0U;

    /* create_vout_flow_base : alloc_dsp_cmd_qs */
    Mask = pBaseCfg->VoutBitMask;
    DSP_Bit2Cnt(Mask, &Cnt);
    pBaseParam->DramParNum += (UINT16)Cnt;
    pBaseParam->OrccodeMsgNum += (UINT16)Cnt;

    /* create_vin_flow_base : alloc_dsp_cmd_qs */
    pBaseParam->DramParNum += (UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum;
    pBaseParam->OrccodeMsgNum += (UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum;

    /* create_vproc_flow_base : alloc_dsp_cmd_qs */
    pBaseParam->DramParNum += (UINT16)pBaseCfg->MaxVprocNum + 1U;
    pBaseParam->OrccodeMsgNum += (UINT16)pBaseCfg->MaxVprocNum + 1U;

    /* create_vproc_flow_base : alloc_orc_msg_q */
    pBaseParam->DramParNum += (pBaseCfg->MaxVprocNum);
    pBaseParam->OrccodeMsgNum += (pBaseCfg->MaxVprocNum);

    /* create_vproc_flow_base : alloc_pic_q */
    pBaseParam->DramParNum += (pBaseCfg->MaxVprocNum);
    pBaseParam->OrccodeMsgNum += (pBaseCfg->MaxVprocNum);

    /* Row[38] - init_dsp_init_data_part2 : internal_msg_holder_mode */
    pBaseParam->DramParNum += 1U;

    /* idspdrv_init : temp_buf_dram_addr */
    pBaseParam->DramParNum += 1U;
}

static inline void HL_PrepareBaseParamIeng(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
#define IDSP_JOB_Q_NUM              (4U)
#define IDSP_ENG_NUM                (2U)
    UINT16 i;

(void)pBaseCfg;

    for (i = 0U; i < IDSP_ENG_NUM; i++) {
        /* Row[12] - ieng_init_smem_par() */
        pBaseParam->SmemParNum += 1U;

        /* Row[13] - ieng_init_idsp_job() : idsp_job_cmdq_base */
        pBaseParam->DramParNum += IDSP_JOB_Q_NUM;
        pBaseParam->OrccodeMsgNum += IDSP_JOB_Q_NUM;

        /* ieng_init_idsp_job() : idsp_job_procq_base */
        pBaseParam->DramParNum += 1U;
        pBaseParam->OrccodeMsgNum += 1U;

        /* Row[15] - ieng_init_sect_cr() */
        pBaseParam->SmemParNum += 1U;
    }
}

static inline void HL_PrepareBaseParamVout(DSP_BASE_PARAM_s *pBaseParam)
{
    /* Row[3] - init_vout_cb : vout_back_up_mem */
    pBaseParam->SmemParNum += 1U;

    /* init_vout_cb : echo_cmd_info_pool_daddr */
    pBaseParam->DramParNum += 1U;

    /* Row[4] - initVout1() : internal pic_info q */
    pBaseParam->SmemParNum += 1U;

    /* Row[5] - initVout0() : internal pic_info q */
    pBaseParam->SmemParNum += 1U;

    /* vout_flow_max_cfg_cmd : vout_smem_free_start */
    pBaseParam->SmemParNum += 1U;

    /* vout_flow_max_cfg_cmd : vout_dram_free_start */
    pBaseParam->DramParNum += 1U;
}

static inline void HL_PrepareBaseParamVin(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
#define SUPPORT_VIN_DEF_RAW
#define VIN_DEFAULT_RAW_FBP_SIZE    (7U)
    UINT16 i;
    UINT16 FrmBufNum, ExtFrmBufNum;
    UINT8 IsYuvOutput, IsCeOut;

    /* Row[23] - vin_alloc_sys_sd_mem_usage() : vin_smem_free_start */
    pBaseParam->SmemParNum += 1U;

    /* vin_alloc_sys_sd_mem_usage() : vin_dram_free_start */
    pBaseParam->DramParNum += 1U;

    /* Row[25] - vin_alloc_sideband_info_mcbl() : sideband_info_mcbl_id */
    pBaseParam->McblNum += 1U;
    pBaseParam->McbNum += 128U;


    for (i = 0; i<DSP_VIN_MAX_NUM; i++) {
        if (0U == DSP_GetBit(pBaseCfg->VinBitMask, i, 1U)) {
            continue;
        }
        FrmBufNum = pBaseCfg->FrmBufNum[i];
        ExtFrmBufNum = pBaseCfg->ExtFrmBufNum[i];

        IsYuvOutput = (UINT8)DSP_GetBit(pBaseCfg->VinYuvBitMask, i, 1U);
        IsCeOut = (UINT8)DSP_GetBit(pBaseCfg->VinCeBitMask, i, 1U);

        if (i < AMBA_DSP_MAX_VIN_NUM) {
            /* Row[72] - vin_api_proc_dsp_vin_flow_max_cfg_cmd() : vin_data_smem_par_id */
            pBaseParam->SmemParNum += 1U;
        }

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : fbp_id[VIN_EXT_MEM_TYPE_RAW] */
        pBaseParam->MFbpNum += 1U;
        pBaseParam->ExtMFbNum += ExtFrmBufNum;
        pBaseParam->MImgInfNum += 1U;

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : fbp_id[VIN_EXT_MEM_TYPE_YUV] */
        pBaseParam->MFbpNum += 1U;
        pBaseParam->ExtMFbNum += ExtFrmBufNum;
        pBaseParam->MImgInfNum += 1U;

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : vin_yuv_enc_fbp_id[0] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += 4U;
        pBaseParam->ImgInfNum += 1U;

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : vin_yuv_enc_fbp_id[1] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += 4U;
        pBaseParam->ImgInfNum += 1U;

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : fbp_id[VIN_EXT_MEM_TYPE_HDS] */
        pBaseParam->MFbpNum += 1U;
        pBaseParam->ExtMFbNum += ExtFrmBufNum;
        pBaseParam->MImgInfNum += 1U;

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : fbp_id[VIN_EXT_MEM_TYPE_ME] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += ExtFrmBufNum;
        pBaseParam->ImgInfNum += 1U;

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : fbp_id[VIN_EXT_MEM_TYPE_AUX] */
        pBaseParam->MFbpNum += 1U;
        pBaseParam->ExtMFbNum += ExtFrmBufNum;
        pBaseParam->MImgInfNum += 1U;

        /* vin_api_proc_dsp_vin_flow_max_cfg_cmd() : fbp_id[VIN_EXT_MEM_TYPE_PREV] */
        // DO NOTHING

        if (i < AMBA_DSP_MAX_VIN_NUM) {
            /* Row[84] - vin_rm_init() : vin_cfg_fbp_id */
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += 3U;
        }

        /* vin_rm_init() : vin_raw_fbp_id or Vin_yuv422_out_fbp_id */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        if (1U == IsYuvOutput) {
            pBaseParam->MFbSize += (FrmBufNum*2U);
        } else {
            pBaseParam->MFbSize += FrmBufNum;
        }
        pBaseParam->MImgInfNum += 1U;

        /* vin_rm_init() : vin_ce_fbp_id */
        if (1U == IsCeOut) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += FrmBufNum;
            pBaseParam->MImgInfNum += FrmBufNum;
        }

        /* vin_rm_init() : vin_aux_out_fbp_id */
        if (1U == DSP_GetBit(pBaseCfg->VinAuxBitMask, i, 1U)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += FrmBufNum;
            pBaseParam->MImgInfNum += FrmBufNum;
        }

//FIXME
        /* vin_rm_init() : vin_prev_out_fbp_id */
        if (1U == DSP_GetBit(pBaseCfg->VinAux2BitMask, i, 1U)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += FrmBufNum;
            pBaseParam->MImgInfNum += FrmBufNum;
        }

#ifdef SUPPORT_VIN_DEF_RAW
        /* vin_rm_init() : default_raw_img_fbp_id */
        {
            pBaseParam->MFbpNum += 1U;
            pBaseParam->ExtMFbNum += 1U;
            pBaseParam->MImgInfNum += 1U;
        }

        /* vin_rm_init() : default_ce_img_fbp_id */
        if ((1U == IsYuvOutput) || (1U == IsCeOut)) {
            pBaseParam->MFbpNum += 1U;
            pBaseParam->ExtMFbNum += 1U;
            pBaseParam->MImgInfNum += 1U;
        }
#endif

        /* vin_rm_init() : vin_fov_fbp_ids[] or vin_fov_fbp_ids[]Y */
        pBaseParam->MFbpNum += pBaseCfg->MaxVinPinNum;
        if (1U == IsYuvOutput) {
            pBaseParam->ExtMFbNum += (((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum)*2U);
            pBaseParam->MImgInfNum += (((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum)*2U);
        } else {
            pBaseParam->ExtMFbNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);
            pBaseParam->MImgInfNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);
        }

        /* vin_rm_init() : vin_ce_fov_fbp_ids[] or vin_fov_fbp_ids[]UV */
        if ((1U == IsYuvOutput) || (1U == IsCeOut)) {
            pBaseParam->MFbpNum += pBaseCfg->MaxVinPinNum;
            if (1U == IsYuvOutput) {
                pBaseParam->ExtMFbNum += (((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum)*2U);
                pBaseParam->MImgInfNum += (((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum)*2U);
            } else {
                pBaseParam->ExtMFbNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);
                pBaseParam->MImgInfNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);
            }
        }
    }
}

static inline void HL_PrepareBaseParamVproc(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
#define VPROC_SW_PPL_DEPTH_DEFAULT  (3U)
#define VPROC_TOTAL_PREV_SECTIONS   (5U)
#define VPROC_MAX_HDR_NUM           (3U)

    /* Row[42] - idspdrv_init (OrcIdsp0/1) : temp_buf_dram_addr */
    pBaseParam->DramParNum += 2U;

    /* Row[43] - create_vproc : vproc_smem_par_id */
    pBaseParam->SmemParNum += 1U;

    /* create_vproc : vproc_dram_par_id */
    pBaseParam->DramParNum += 1U;

    /* vproc_flow_max_cfg_cmd() : vproc_sec2_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));

    /* vproc_flow_max_cfg_cmd() : vproc_sec4_cfg_fbp_id */
    if (pBaseCfg->VprocResHdr == 1U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    }

    /* vproc_flow_max_cfg_cmd() : vproc_c2y_dma_params_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));

    /* vproc_flow_max_cfg_cmd() : vproc_aaa_cfa_out_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));

    /* vproc_flow_max_cfg_cmd() : vproc_aaa_rgb_out_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));

    /* vproc_flow_max_cfg_cmd() : vproc_vin_stats_out_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += ((VPROC_SW_PPL_DEPTH_DEFAULT*VPROC_MAX_HDR_NUM)*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));

    /* vproc_flow_max_cfg_cmd() : vproc_sec3_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxTileNum));

    /* vproc_flow_max_cfg_cmd() : vproc_sec11_cfg_fbp_id */
    if (pBaseCfg->VprocResHwarp > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    }

    /* vproc_flow_max_cfg_cmd() : vproc_sec18_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxTileNum));

    /* vproc_flow_max_cfg_cmd() : vproc_sec10_cfg_fbp_id */
    if (pBaseCfg->VprocResIdspMixer > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResIdspMixerNum));
    }

    /* vproc_flow_max_cfg_cmd() : comm_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += ((VPROC_SW_PPL_DEPTH_DEFAULT*VPROC_TOTAL_PREV_SECTIONS)*(UINT16)(pBaseCfg->VprocResMaxTileNum));

    /* vproc_flow_max_cfg_cmd() : comm2_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;

    /* vproc_flow_max_cfg_cmd() : sec9_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;

    /* vproc_flow_max_cfg_cmd() : y2y_dma_params_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (VPROC_SW_PPL_DEPTH_DEFAULT*(UINT16)(pBaseCfg->VprocResMaxTileNum));

    /* vproc_flow_max_cfg_cmd() : idsp_stitch_data_fbp_id */
    // DO NOTHING

    /* vproc_flow_max_cfg_cmd() : vproc_prev_fbp_id[0] */
    if (pBaseCfg->MaxVprocEfctComm0FrmBufNum > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->FbpNum += 1U;
        pBaseParam->ImgInfNum += 2U;
    }
    pBaseParam->FbNum += pBaseCfg->MaxVprocEfctComm0FrmBufNum;

    /* vproc_flow_max_cfg_cmd() : vproc_prev_fbp_id[1] */
    if (pBaseCfg->MaxVprocEfctComm1FrmBufNum > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->FbpNum += 1U;
        pBaseParam->ImgInfNum += 2U;
    }
    pBaseParam->FbNum += pBaseCfg->MaxVprocEfctComm1FrmBufNum;

    /* orcidsp_create_shared_smem() : orcidsp_create_shared_smem */
    pBaseParam->SmemParNum += 1U;

    /* orcidsp_create_shared_smem() : cap_y2y_smem_par_size */
    pBaseParam->SmemParNum += 1U;

    /* orcidsp_create_shared_smem() : cap_idsp_mixer_smem_par_size */
    pBaseParam->SmemParNum += 1U;

    /* vproc_init_idsp_mixer_cfg : idsp_mixer_cfg_dbase */
    if (pBaseCfg->VprocResIdspMixer > 0U) {
        pBaseParam->DramParNum += 1U;
    }

    /* vproc_flow_max_cfg_cmd : do_double_cmds */
    if (pBaseCfg->VprocResNeedDlbCmd > 0U) {
#define VPROC_DBL_CMD_Y2Y_PAR_NUM   (10U)
        pBaseParam->SupParNum += 1U;
        pBaseParam->SubParNum += VPROC_DBL_CMD_Y2Y_PAR_NUM;
    }
}

static inline void HL_PrepareBaseParamVprocEfctTopHalf(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* vproc_flow_max_cfg_cmd() : comm_main_fbp_id */
    if ((pBaseCfg->MaxVprocEfctMainFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        pBaseParam->DramParNum += 1U;
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->MaxVprocEfctMainFrmBufNum;
            pBaseParam->ImgInfNum += 2U;
        }
    }

    /* vproc_flow_max_cfg_cmd() : comm_main_me01_fbp_id */
    if ((pBaseCfg->MaxVprocEfctMainMeFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        pBaseParam->DramParNum += 1U;
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->MaxVprocEfctMainMeFrmBufNum;
            pBaseParam->ImgInfNum += 2U;
        }
    }

    /* vproc_flow_max_cfg_cmd() : prim_output_work_fbp_id VPROC_MAIN_STREAM_IDX=0 */
    if ((pBaseCfg->MaxVprocEfctMainMeFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }
    /* vproc_flow_max_cfg_cmd() : sub_output_work_fbp_id VPROC_MAIN_STREAM_IDX=0 */
    if ((pBaseCfg->MaxVprocEfctMainMeFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }

    /* vproc_flow_max_cfg_cmd() : comm_pip_fbp_id */
    if ((pBaseCfg->MaxVprocEfctPipFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctPipFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        pBaseParam->DramParNum += 1U;
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->MaxVprocEfctPipFrmBufNum;
            pBaseParam->ImgInfNum += 2U;
        }
    }

    /* vproc_flow_max_cfg_cmd() : comm_pip_me01_fbp_id */
    if ((pBaseCfg->MaxVprocEfctPipFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctPipFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        pBaseParam->DramParNum += 1U;
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->MaxVprocEfctPipFrmBufNum;
            pBaseParam->ImgInfNum += 2U;
        }
    }
}

static inline void HL_PrepareBaseParamVprocEfct(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    HL_PrepareBaseParamVprocEfctTopHalf(pBaseCfg, pBaseParam);

    /* prim_output_work_fbp_id VPROC_MAIN_STREAM_IDX=1 */
    if ((pBaseCfg->MaxVprocEfctPipFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctPipFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }
    /* sub_output_work_fbp_id VPROC_MAIN_STREAM_IDX=1 */
    if ((pBaseCfg->MaxVprocEfctPipFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctPipFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }
    /* prim_output_work_fbp_id VPROC_MAIN_STREAM_IDX=2 */
    if ((pBaseCfg->MaxVprocEfctComm0FrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctComm0FrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }
    /* prim_output_work_fbp_id VPROC_MAIN_STREAM_IDX=3 */
    if ((pBaseCfg->MaxVprocEfctComm1FrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctComm1FrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }

    if (pBaseCfg->VprocResEfct > 0U) {
        if (pBaseCfg->VprocNeedDummyPostPrevB > 0U) {
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
        if (pBaseCfg->VprocNeedDummyPostPrevC > 0U) {
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }

    /* veffect_rm_init() : smem_sub_par_id  */
    if (pBaseCfg->VprocResEfct > 0U) {
        pBaseParam->SubParNum += 2U;
        pBaseParam->SupParNum += 2U;
    }

    /* veffect_rm_init() : s_str_bdt_id  */
    if (pBaseCfg->VprocResEfct > 0U) {
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 14U;
    }

    /* veffect_rm_init() : d_str_bdt_id  */
    if (pBaseCfg->VprocResEfct > 0U) {
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 14U;
    }

    /* veffect temp fix for smem par increase 2 started from 72a2ea10  */
    if (pBaseCfg->VprocResEfct > 0U) {
        pBaseParam->SmemParNum += 2U;
    }
}

static inline void HL_PrepareBaseParamVprocNumTopHalf(UINT16 i, const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* Row[142] - vproc_c2y_rm_init() : s_str_bdt_id */
    pBaseParam->BdtNum += 1U;
    pBaseParam->BdNum += 22U;

    /* vproc_c2y_rm_init() : d_str_bdt_id */
    pBaseParam->BdNum += 22U;

    /* vproc_c2y_rm_init() : smem_sub_par_id */
    pBaseParam->SubParNum += 1U;
    pBaseParam->SupParNum += 1U;

    /* vproc_c2y_sec2_fbp_init() : c2y_main_y_out_fbp_id */
    if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += pBaseCfg->VprocC2YFrmBufNum[i];
    }

    /* vproc_c2y_sec2_fbp_init() : c2y_main_uv_out_fbp_id */
    if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += pBaseCfg->VprocC2YFrmBufNum[i];
    }

    /* vproc_c2y_sec2_fbp_init() : c2y_ext_raw_out_fbp_id */
    if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
        if (1U == DSP_GetBit(pBaseCfg->VprocCompOutBitMask, i, 1U)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += (UINT16)1U;
            pBaseParam->MFbSize += (UINT16)pBaseCfg->VprocC2YFrmBufNum[i];
        }
    }

    /* vproc_c2y_sec2_fbp_init() : c2y_me_fbp_id */
    if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
        if (1U == DSP_GetBit(pBaseCfg->VprocC2YMeBitMask, i, 1U)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += (UINT16)1U;
            pBaseParam->MFbSize += (UINT16)pBaseCfg->VprocC2YFrmBufNum[i];
        }
    }

    /* vproc_c2y_sec2_fbp_init() : c2y_ir_out_fbp_id */
    if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
        if (1U == DSP_GetBit(pBaseCfg->VprocC2YIrBitMask, i, 1U)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += (UINT16)1U;
            pBaseParam->MFbSize += (UINT16)pBaseCfg->VprocC2YFrmBufNum[i];
        }
    }

    /* vproc_c2y_sec2_fbp_init() : c2y_y12_out_fbp_id */
    if (pBaseCfg->VprocC2YY12FrmBufNum[i] > 0U) {
        if (1U == DSP_GetBit(pBaseCfg->VprocY12BitMask, i, 1U)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += (UINT16)1U;
            pBaseParam->MFbSize += (UINT16)pBaseCfg->VprocC2YY12FrmBufNum[i];
        } else if (1U == DSP_GetBit(pBaseCfg->VprocC2YY12BitMask, i, 1U)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += (UINT16)1U;
            pBaseParam->MFbSize += (UINT16)pBaseCfg->VprocC2YY12FrmBufNum[i];
        } else {
            // No addition
        }
    }
}

static inline void HL_PrepareBaseParamVprocNum(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 i;

    for (i = 0U; i < pBaseCfg->MaxVprocNum; i++) {
        HL_PrepareBaseParamVprocNumTopHalf(i, pBaseCfg, pBaseParam);

        /* vproc_y2y_rm_init() : s_str_bdt_id */
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 70U;

        /* vproc_y2y_rm_init() : d_str_bdt_id */
        pBaseParam->BdNum += 70U;

        /* vproc_y2y_rm_init() : smem_sub_par_id */
        pBaseParam->SubParNum += 1U;
        pBaseParam->SupParNum += 1U;

        /* vproc_y2y_rm_init() : main_fbp_id */
        if ((pBaseCfg->VprocMainFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocMainFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocMainFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : main_me_fbp_id */
        if ((pBaseCfg->VprocMainMeFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocMainMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocMainMeFrmBufNum[i];
        }

        /* vproc_y2y_init_mctf_cmpr_buf() : mctf compression buffer */
        pBaseParam->DramParNum += 1U;

        /* vproc_osd_blend_rm_init() : s_str_bdt_id */
        if (1U == DSP_GetBit(pBaseCfg->VprocOsdBlendBitMask, i, 1U)) {
            pBaseParam->BdtNum += 1U;
            pBaseParam->BdNum += 7U;
        }

        /* vproc_osd_blend_rm_init() : d_str_bdt_id */
        if (1U == DSP_GetBit(pBaseCfg->VprocOsdBlendBitMask, i, 1U)) {
            pBaseParam->BdNum += 7U;
        }

        /* vproc_osd_blend_rm_init() : smem_sub_par_id */
        if (1U == DSP_GetBit(pBaseCfg->VprocOsdBlendBitMask, i, 1U)) {
            pBaseParam->SubParNum += 1U;
            pBaseParam->SupParNum += 1U;
        }

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_MAIN] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PREV_A] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PREV_B] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PREV_C] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PREV_D] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_LN_DEC] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_HIER_0] */
        // DO NOTHING

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_COMP_RAW] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_MAIN_ME] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PIP_ME] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_HIER_Y12] */
        pBaseParam->MFbpNum += 1U;
        pBaseParam->ExtMFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_C2Y_Y12] */
        pBaseParam->MFbpNum += 1U;
        pBaseParam->ExtMFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PREV_D_ME] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PREV_B_ME] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_PREV_C_ME] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;

        /* ext_buf_cb() : fbp_id[EXT_MEM_TYPE_IR] */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM;
    }
}

static inline UINT8 DSP_PostiveFbNum(UINT8 FbNum)
{
    UINT8 Rval;

    if ((FbNum > 0U) &&
        (FbNum != DSP_VPROC_FB_NUM_DISABLE)) {
        Rval = (UINT8)1U;
    } else {
        Rval = (UINT8)0U;
    }

    return Rval;
}

static inline void HL_PrepareBaseParamVprocNumPrev(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 i;

    for (i = 0U; i < pBaseCfg->MaxVprocNum; i++) {
        /* vproc_y2y_rm_init() : vproc_prev_a_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocPrevAFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevAFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : vproc_me_a_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocPrevAMeFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevAMeFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : vproc_prev_b_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocPrevBFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevBFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : vproc_me_b_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocPrevBMeFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevBMeFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : vproc_prev_c_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocPrevCFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevCFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : vproc_me_c_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocPrevCMeFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevCMeFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : vproc_ln_dec_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocLndtFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocLndtFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : vproc_hier_fbp_id */
        if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocHierFrmBufNum[i])) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocHierFrmBufNum[i];
        }

        /* vproc_y2y_rm_init() : hier_y12_fbp_id */
        if (1U == DSP_GetBit(pBaseCfg->VprocY12BitMask, i, 1U)) {
            if ((UINT8)1U == DSP_PostiveFbNum(pBaseCfg->VprocHierY12FrmBufNum[i])) {
                pBaseParam->DramParNum += 1U;
                pBaseParam->MFbpNum += (UINT16)1U;
                pBaseParam->MFbSize += (UINT16)pBaseCfg->VprocHierY12FrmBufNum[i];
            }
        }
    }
}

static inline void HL_PrepareBaseParamEncode(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 i;
//FIXME
    UINT16 CoreNum = 2U;

    /* Row[13] - create_encoders() : s_bdt_id */
    pBaseParam->BdtNum += (((CoreNum + 1U) * (UINT16)pBaseCfg->EncNum) + 1U);
    pBaseParam->BdNum += 68U*pBaseParam->BdtNum;

    /* create_encoders() : d_bdt_id */
    pBaseParam->BdtNum += (((CoreNum + 1U) * (UINT16)pBaseCfg->EncNum) + 1U);
    pBaseParam->BdNum += 40U*pBaseParam->BdtNum;

    /* create_proxy() */
    //DO NOTHING

    /* create_eng0() : smem_bdt_id */
    pBaseParam->BdtNum += 1U;
    pBaseParam->BdNum += 16U;

    /* create_eng0() : dram_bdt_id */
    pBaseParam->BdtNum += 1U;
    pBaseParam->BdNum += 4U;

    /* encoder_rm_init() : fbp_id_rec */
    pBaseParam->FbpNum += pBaseCfg->EncNum;
    for (i = 0U; i < pBaseCfg->EncNum; i++) {
        pBaseParam->FbNum += pBaseCfg->ReconNum[i];
    }
    pBaseParam->ImgInfNum += pBaseCfg->EncNum;

    /* create_encoder_smem_par : common_smem */
    pBaseParam->SmemParNum += 1U;

    /* create_encoder_dram : common_dram */
    pBaseParam->DramParNum += 1U;
}

static inline void HL_PrepareBaseParamDecode(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* Row[16] - create_avc_dec */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 0U/*AVC idx*/, 1U)) {
        /* recon_ctx_dram_par */
        pBaseParam->DramParNum += 1U;

        /* recon_ctx_dbp */
        pBaseParam->DbpNum += 1U;

        /* out_fifo_q_id */
        pBaseParam->SmemParNum += pBaseCfg->DecNum;
        pBaseParam->OrccodeMsgNum += pBaseCfg->DecNum;

        /* dbp_recon_cmd_id */
        pBaseParam->DbpNum += pBaseCfg->DecNum;

        /* dbp_recon_msg_id */
        pBaseParam->DbpNum += pBaseCfg->DecNum;

        /* dbp_recon_mvdec_edges_id */
        pBaseParam->DbpNum += pBaseCfg->DecNum;

        /* create_avc_dec() : he_job_q_id */
        pBaseParam->SmemParNum += ((UINT16)pBaseCfg->DecNum*2U);
        pBaseParam->OrccodeMsgNum += ((UINT16)pBaseCfg->DecNum*2U);
    }

    /* Row[23] - createHevcDecoder */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
        /* recon_ctx_dram_par */
        pBaseParam->DramParNum += 1U;

        /* recon_ctx_dbp */
        pBaseParam->DbpNum += 1U;

        /* out_fifo_q_id */
        pBaseParam->SmemParNum += pBaseCfg->DecNum;
        pBaseParam->OrccodeMsgNum += pBaseCfg->DecNum;

        /* fbp_dram_id */
        pBaseParam->FbpNum += pBaseCfg->DecNum;

        /* HEVC_MAX_NUM_OF_PIC_IN_DRAM_FBP */
        pBaseParam->FbNum += ((UINT16)16U*pBaseCfg->DecNum);

        /* fbp_imginfo */
        pBaseParam->ImgInfNum += pBaseCfg->DecNum;

        /* colo_imginfo */
        pBaseParam->ImgInfNum += pBaseCfg->DecNum;

        /* dbp_recon_cmd_id */
        pBaseParam->DbpNum += pBaseCfg->DecNum;

        /* createHevcDecoder(): fbp_dram_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* createHevcDecoder(): he_job_q_id */
        pBaseParam->SmemParNum += ((UINT16)pBaseCfg->DecNum*2U);
    }

    /* Row[33] - create_jpg_dec */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 2U/*JPG idx*/, 1U)) {
        /* buf_smem_par_id */
        pBaseParam->SmemParNum += 1U;
    }

    /* Row[82] - jpegdec */
    if (pBaseCfg->DecNum > 0U) {
        pBaseParam->McblNum += 1U;
        pBaseParam->McbNum += 4U;
    }

    /* Row[34] - dec_init_flow_base */
    if (pBaseCfg->DecNum > 0U) {
        /* msg_q_in */
        pBaseParam->SmemParNum += 1U;
        pBaseParam->OrccodeMsgNum += 1U;

        /* dec_dsp_cmd_q_id */
        pBaseParam->DramParNum += 1U;
        pBaseParam->OrccodeMsgNum += 1U;

        /* to_coding_q_id */
        pBaseParam->SmemParNum += 1U;
        pBaseParam->OrccodeMsgNum += 1U;

        /* fr_coding_q_id */
        pBaseParam->SmemParNum += 1U;
        pBaseParam->OrccodeMsgNum += 1U;

        /* dec_ext_fbp_id */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += 1U;
        pBaseParam->ImgInfNum += 1U;

        if (pBaseCfg->IsDuplexMode > (UINT8)0U) {
            /* Row[36] - create_duplex_shared_smem */
            /* Row[37] - dec_create_mem_partitions */
            pBaseParam->SmemParNum += 1U;
        }

        /* Row[37] - dec_create_mem_partitions */
        /* dec_sup_par_id */
        pBaseParam->SupParNum += 1U;

        /* rec_smem_par_id */
        pBaseParam->SubParNum += 1U;

        /* ref_smem_par_id */
        pBaseParam->SubParNum += 1U;

        /* buf_smem_par_id */
        pBaseParam->SmemParNum += 1U;

        /* Row[46] - alloc_avc_dec_nonshare_mem_par */
        /* cod_dma_smem_par_id */
        if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 0U/*AVC idx*/, 1U)) {
            pBaseParam->SmemParNum += 1U;
        }
        if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
            pBaseParam->SmemParNum += 1U;
        }

        /* dram_sup_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;
        pBaseParam->SubParNum += ((UINT16)15U*pBaseCfg->DecNum);
        if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 0U/*AVC idx*/, 1U)) {
            /* alloc_avc_dec_nonshare_mem_par */
            pBaseParam->SupParNum += 1U;
        }
        if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
            /* alloc_hevc_dec_nonshare_mem_par */
            pBaseParam->SupParNum += 1U;
        }

        /* for multiple decode */
        pBaseParam->SupParNum += (pBaseCfg->DecNum - (UINT16)1U);

        /* sh_sub_par_id */
        pBaseParam->SubParNum += pBaseCfg->DecNum;

        /* db_recon_cmdmsg_sub_par_id */
        pBaseParam->SubParNum += pBaseCfg->DecNum;

        /* hdr_sub_par_id */
        pBaseParam->SubParNum += pBaseCfg->DecNum;

        /* hcb_sub_par_id */
        pBaseParam->SubParNum += 4U;

        /* fb_colo_par_id */
        pBaseParam->SubParNum += 1U;
        pBaseParam->ImgInfNum += 1U;
        pBaseParam->MFbSize += 6U;
        pBaseParam->MImgInfNum += 1U;

        /* dsp_csw_par_id */
        pBaseParam->SubParNum += 1U;
        /* echo command */
        pBaseParam->DramParNum += 1U;

        /* Row[65] - dec_create_fbp_dram_par */
        /* fbp_dram_par */
        pBaseParam->DramParNum += pBaseCfg->DecNum;
    }

    /* Row[64] - config_avc_dec */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 0U/*AVC idx*/, 1U)) {
        /* fbp_id */
        pBaseParam->FbpNum += pBaseCfg->DecNum;

        /* MAX_DEC_FBUFS_IN_DRAM */
        pBaseParam->FbNum += ((UINT16)16U*pBaseCfg->DecNum);

        /* fbp_imginfo */
        pBaseParam->ImgInfNum += pBaseCfg->DecNum;

        /* cpb_id */
        pBaseParam->CBufNum += pBaseCfg->DecNum;

        /* config_avc_dec(): hcb_id */
        pBaseParam->CBufNum += ((UINT16)pBaseCfg->DecNum)*4U;
    }

    /* Row[71] - setupHevcDecoder */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
        /* cpb_id */
        pBaseParam->CBufNum += pBaseCfg->DecNum;

        /* setupHevcDecoder(): hcb_id */
        pBaseParam->CBufNum += ((UINT16)pBaseCfg->DecNum*20U);
    }
}

static inline void HL_PrepareBaseParamSproc(const DSP_BASE_CFG_s *pDspBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 MaxMainWidth = 0U, MaxMainHeight = 0U, SprocHiW0Max;

    (void)HL_GetSystemVprocPinMaxWindow(DSP_VPROC_PIN_MAIN, &MaxMainWidth, &MaxMainHeight);
    SprocHiW0Max = (pDspBaseCfg->VprocHisoW0Max > 0U) ? pDspBaseCfg->VprocHisoW0Max : MaxMainWidth;

    /* 20221004 According to ChenHan, */
    /* 105 + ((sproc_hi_W_max+2047/2048)+1)*(14+3+12) for sproc. */
    pBaseParam->MFbSize += 105U + ((((SprocHiW0Max + 2047U) / 2048U) + 1U) * (29U));

}

static inline void HL_PrepareBaseParamTestFrm(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* 20210129, cureently DSP allocate fixed resource when is-testfrm-on */
    if ((pBaseCfg->TestIdspC2YNum > (UINT8)0U) ||
        (pBaseCfg->TestIdspY2YNum > (UINT8)0U)) {
        /* each c2y need 2mcb, y2y need 8mcb, currenlty 4 C2Y and 13 Y2Y = 4X2+13X8 = 112  */
        pBaseParam->McbNum += 112U;

        /* c2y and y2y both need one sub and sup */
        pBaseParam->SubParNum += 2U;
        pBaseParam->SupParNum += 2U;
    }
}

/*
 * This API come from "CV2_MM_Spreadsheet"
 *  - the purpose is to relief DspCfgCmd tuning effort
 *  - Per Ucode, Effect case is not well consider
 */
UINT32 HL_PrepareBaseParam(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT32 Rval = OK;

    if ((pBaseCfg == NULL) || (pBaseParam == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
#ifdef DEBUG_PARAM_CALC
        UINT16 i;
        AmbaLL_LogUInt5("BaseCfg[IN]", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  BOOT  : Cache[%d %d %d %d %d]", pBaseCfg->FbpCache, pBaseCfg->MFbpCache,
                                                           pBaseCfg->FbCache, pBaseCfg->MFbCache,
                                                           pBaseCfg->ImgInfCache);
        AmbaLL_LogUInt5("          Cache[%d]", pBaseCfg->MImgInfCache, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  IENG  : LdStr[%d %d %d]", pBaseCfg->LdStrNum[0U], pBaseCfg->LdStrNum[1U],
                                                    pBaseCfg->LdStrNum[2U], 0U, 0U);
        AmbaLL_LogUInt5("  VIN   : Num[%d %d %d] VinMask[0x%X] AuxMask[0x%X]", pBaseCfg->VinNum, pBaseCfg->VirtVinNum,
                                                                               pBaseCfg->MaxVinPinNum, pBaseCfg->VinBitMask,
                                                                               pBaseCfg->VinAuxBitMask);
        AmbaLL_LogUInt5("          CeMask[0x%X] YuvMask[0x%X] Aux2Mask[0x%X]", pBaseCfg->VinCeBitMask, pBaseCfg->VinYuvBitMask,
                                                                               pBaseCfg->VinAux2BitMask, 0U, 0U);
        for (i=0U; i<DSP_VIN_MAX_NUM; i++) {
            if (1U == DSP_GetBit(pBaseCfg->VinBitMask, i, 1U)) {
                AmbaLL_LogUInt5("          [%d]FrmNum[%d %d]", i, pBaseCfg->FrmBufNum[i], pBaseCfg->ExtFrmBufNum[i], 0U, 0U);
            }
        }
        AmbaLL_LogUInt5("  VOUT  : Mask[0x%X]", pBaseCfg->VoutBitMask, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  VPROC : Res[%d %d %d %d %d]", pBaseCfg->VprocResHdr,
                                                         pBaseCfg->VprocResLndt,
                                                         pBaseCfg->VprocResTileMode,
                                                         pBaseCfg->VprocResEfct,
                                                         pBaseCfg->VprocResHiso);
        AmbaLL_LogUInt5("          Res[%d %d %d] DmyPostPrev[%d %d]", pBaseCfg->VprocResHwarp,
                                                                      pBaseCfg->VprocResIdspMixer,
                                                                      pBaseCfg->VprocResIdspMixerNum,
                                                                      pBaseCfg->VprocNeedDummyPostPrevB,
                                                                      pBaseCfg->VprocNeedDummyPostPrevC);
        AmbaLL_LogUInt5("          ExtMemMode[%d] DblCmd[%d]", pBaseCfg->VprocExtMemAllocMode, pBaseCfg->VprocResNeedDlbCmd, 0U, 0U, 0U);
        AmbaLL_LogUInt5("          Tile[%d %d] Num[%d] NumC[%d %d]", pBaseCfg->VprocResMaxC2YTileNum,
                                                                     pBaseCfg->VprocResMaxTileNum,
                                                                     pBaseCfg->MaxVprocNum,
                                                                     pBaseCfg->MaxVprocEfctComm0FrmBufNum,
                                                                     pBaseCfg->MaxVprocEfctComm1FrmBufNum);
        AmbaLL_LogUInt5("          NumM[%d %d] NumP[%d %d] Cpy[%d]", pBaseCfg->MaxVprocEfctMainFrmBufNum,
                                                                     pBaseCfg->MaxVprocEfctMainMeFrmBufNum,
                                                                     pBaseCfg->MaxVprocEfctPipFrmBufNum,
                                                                     pBaseCfg->MaxVprocEfctPipMeFrmBufNum,
                                                                     pBaseCfg->VprocResMaxEfctCopyNum);
        AmbaLL_LogUInt5("          CompMask[0x%X] HdrMask[0x%X] YuyvMask[0x%X] Y12Mask[0x%X] Blend[%d]", pBaseCfg->VprocCompOutBitMask,
                                                                                                         pBaseCfg->VprocHdrBitMask,
                                                                                                         pBaseCfg->VprocMipiYuyvBitMask,
                                                                                                         pBaseCfg->VprocY12BitMask,
                                                                                                         pBaseCfg->VprocResMaxEfctBlendNum);
        AmbaLL_LogUInt5("          C2YY12Mask[0x%X] C2YMeMask[0x%X] C2YIrMask[0x%X] OsdBldMask[0x%X]", pBaseCfg->VprocC2YY12BitMask,
                                                                                                       pBaseCfg->VprocC2YMeBitMask,
                                                                                                       pBaseCfg->VprocC2YIrBitMask,
                                                                                                       pBaseCfg->VprocOsdBlendBitMask, 0U);
        for (i=0U; i<pBaseCfg->MaxVprocNum; i++) {
            AmbaLL_LogUInt5("          [%d] FrmNum[%d] [%d %d]", i,
                                                                 pBaseCfg->VprocC2YFrmBufNum[i], pBaseCfg->VprocMainFrmBufNum[i],
                                                                 pBaseCfg->VprocMainMeFrmBufNum[i], 0U);
            AmbaLL_LogUInt5("              [%d %d] [%d %d]", pBaseCfg->VprocPrevAFrmBufNum[i], pBaseCfg->VprocPrevAMeFrmBufNum[i],
                                                             pBaseCfg->VprocPrevBFrmBufNum[i], pBaseCfg->VprocPrevBMeFrmBufNum[i],
                                                             0U);
            AmbaLL_LogUInt5("              [%d %d] [%d] [%d]", pBaseCfg->VprocPrevCFrmBufNum[i], pBaseCfg->VprocPrevCMeFrmBufNum[i],
                                                               pBaseCfg->VprocLndtFrmBufNum[i], pBaseCfg->VprocHierFrmBufNum[i],
                                                               0U);
            AmbaLL_LogUInt5("              ExtMem[%d %d %d %d %d]", pBaseCfg->VprocExtMemBufNum[i][0U], pBaseCfg->VprocExtMemBufNum[i][1U],
                                                                    pBaseCfg->VprocExtMemBufNum[i][2U], pBaseCfg->VprocExtMemBufNum[i][3U],
                                                                    pBaseCfg->VprocExtMemBufNum[i][4U]);
            AmbaLL_LogUInt5("                    [%d %d %d %d]", pBaseCfg->VprocExtMemBufNum[i][5U], pBaseCfg->VprocExtMemBufNum[i][6U],
                                                                 pBaseCfg->VprocExtMemBufNum[i][7U], pBaseCfg->VprocExtMemBufNum[i][8U],
                                                                 0U);
        }
        AmbaLL_LogUInt5("          Hiso[%d] [%d %d %d %d]", pBaseCfg->VprocHisoW0Max, pBaseCfg->VprocHisoSec2Max,
                                                            pBaseCfg->VprocHisoSec4Max, pBaseCfg->VprocHisoSec5Max,
                                                            pBaseCfg->VprocHisoSec6Max);
        AmbaLL_LogUInt5("                   [%d %d %d]", pBaseCfg->VprocHisoSec7Max, pBaseCfg->VprocHisoSec9Max,
                                                         pBaseCfg->VprocHisoColRepMax, 0U, 0U);

        AmbaLL_LogUInt5("  ENC   : Num[%d]", pBaseCfg->EncNum, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  DEC   : Num[%d] Fmt[0x%X]", pBaseCfg->DecNum, pBaseCfg->DecFmt, 0U, 0U, 0U);
#endif

        /* Tab:Boot */
        HL_PrepareBaseParamBoot(pBaseCfg, pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:BOOT", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

        /* Tab:IENG */
        HL_PrepareBaseParamIeng(pBaseCfg, pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:IENG", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

        /* Tab:Vout */
        HL_PrepareBaseParamVout(pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:VOUT", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

        /* Tab:Vin */
        HL_PrepareBaseParamVin(pBaseCfg, pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:VIN", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

        /* Tab:Vproc */
        HL_PrepareBaseParamVproc(pBaseCfg, pBaseParam); // All vproc
        HL_PrepareBaseParamVprocEfct(pBaseCfg, pBaseParam); // All vproc
        HL_PrepareBaseParamVprocNum(pBaseCfg, pBaseParam); // per Vproc
        HL_PrepareBaseParamVprocNumPrev(pBaseCfg, pBaseParam); // per Vproc
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:VPROC", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

        /* Tab:Encode */
        HL_PrepareBaseParamEncode(pBaseCfg, pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:ENCODE", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);

#endif

        /* Tab:Decode */
        HL_PrepareBaseParamDecode(pBaseCfg, pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:DEC", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif
        /* Tab:Sproc */
        HL_PrepareBaseParamSproc(pBaseCfg, pBaseParam);

        /* Misc */
        {
            //TBD
        }
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:MISC", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif
        /* TestFrm */
        HL_PrepareBaseParamTestFrm(pBaseCfg, pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:TestFrm", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

        /* PostProc */
        /* Row[25] - dsp_boot_config() : init_dsp_mm */
        pBaseParam->SmemParNum += pBaseParam->DbpNum;
        pBaseParam->SmemParNum += pBaseParam->CBufNum;
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:PostProc", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                             pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                             0U);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif
    }

    return Rval;
}

static inline void HL_CalcWorkingBufferVin(UINT32 AOI, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork)
{
    UINT16 i;
    UINT32 WorkSize;

    if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_VIN_BIT, 1U)) {
        /* Virtual Vin use external memory */
        for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
            /* Row[59] - vin_raw_fbp_id or Vin_yuv422_out_fbp_id */
            if (1U == DSP_GetBit(pDramCfg->VinBitMask, i, 1U)) {
                if (pDramCfg->VinCmpr[i] == 0U) {
                    WorkSize = (((UINT32)pDramCfg->VinMaxCapOutWidth[i]*2U)*pDramCfg->VinMaxCapOutHeight[i])*pDramCfg->CapOutBufNum[i];
                } else {
                    WorkSize = ((UINT32)pDramCfg->VinMaxCapOutWidth[i]*pDramCfg->VinMaxCapOutHeight[i])*pDramCfg->CapOutBufNum[i];
                }
                *pDspWork += WorkSize;
            }

            /* Row[61] - vin_ce_fbp_id */
            if (1U == DSP_GetBit(pDramCfg->VinCeBitMask, i, 1U)) {
                WorkSize = (((UINT32)pDramCfg->VinMaxCeOutWidth[i]*2U)*pDramCfg->VinMaxCeOutHeight[i])*pDramCfg->CeOutBufNum[i];
                *pDspWork += WorkSize;
            }
        }
    }
}

static inline void HL_CalcWorkingBufferVprocComm(UINT32 AOI, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork)
{
    UINT32 WorkSize;

    if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_VPROC_COMM_BIT, 1U)) {
        /* Row[62] - vproc_prev_fbp_id[0] */
        if ((pDramCfg->MaxVprocEfctComm0FrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctComm0FrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            if (pDramCfg->VprocComm0Fmt == DSP_YUV_420) {//Yuv420
                WorkSize = ((((UINT32)pDramCfg->MaxComm0Width*3U)/2U)*pDramCfg->MaxComm0Hieght)*pDramCfg->MaxVprocEfctComm0FrmBufNum;
            } else {
                WorkSize = (((UINT32)pDramCfg->MaxComm0Width*2U)*pDramCfg->MaxComm0Hieght)*pDramCfg->MaxVprocEfctComm0FrmBufNum;
            }
            *pDspWork += WorkSize;
        }
        /* Row[63] - vproc_prev_fbp_id[1] */
        if ((pDramCfg->MaxVprocEfctComm1FrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctComm1FrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            if (pDramCfg->VprocComm1Fmt == DSP_YUV_420) {
                WorkSize = ((((UINT32)pDramCfg->MaxComm1Width*3U)/2U)*pDramCfg->MaxComm1Hieght)*pDramCfg->MaxVprocEfctComm1FrmBufNum;
            } else {
                WorkSize = (((UINT32)pDramCfg->MaxComm1Width*2U)*pDramCfg->MaxComm1Hieght)*pDramCfg->MaxVprocEfctComm1FrmBufNum;
            }
            *pDspWork += WorkSize;
        }

        /* Row[70] - comm_main_fbp_id */
        if ((pDramCfg->MaxVprocEfctMainFrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctMainFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = ((((UINT32)pDramCfg->MaxEfctMainWidth*3U)/2U)*pDramCfg->MaxEfctMainHeight)*pDramCfg->MaxVprocEfctMainFrmBufNum;
            *pDspWork += WorkSize;
        }
        /* Row[71] - comm_main_me01_fbp_id */
        if ((pDramCfg->MaxVprocEfctMainMeFrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctMainMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = (((UINT32)pDramCfg->MaxEfctMainWidth/4U)*((UINT32)pDramCfg->MaxEfctMainHeight/4U))*pDramCfg->MaxVprocEfctMainMeFrmBufNum;
            *pDspWork += WorkSize;
        }

        /* Row[75] - comm_pip_fbp_id */
        if ((pDramCfg->MaxVprocEfctPipFrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctPipFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = ((((UINT32)pDramCfg->MaxEfctPipWidth*3U)/2U)*pDramCfg->MaxEfctPipHieght)*pDramCfg->MaxVprocEfctPipFrmBufNum;
            *pDspWork += WorkSize;
        }
        /* Row[76] - comm_pip_me01_fbp_id */
        if ((pDramCfg->MaxVprocEfctPipMeFrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctPipMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = (((UINT32)pDramCfg->MaxEfctPipWidth/4U)*((UINT32)pDramCfg->MaxEfctPipHieght/4U))*pDramCfg->MaxVprocEfctPipMeFrmBufNum;
            *pDspWork += WorkSize;
        }
    }
}

static inline void HL_CalcWorkingBufferVprocTopHalf(UINT16 i, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork)
{
    UINT32 WorkSize;

    /* Row[142] - vproc_luma(chroma)_main_out_fbp_id */
    if ((pDramCfg->VprocC2YFrmBufNum[i] > 0U) &&
        (pDramCfg->VprocC2YFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
        WorkSize = (((UINT32)pDramCfg->MaxC2YWidth[i]*2U)*pDramCfg->MaxC2YHeight[i])*pDramCfg->VprocC2YFrmBufNum[i];
        *pDspWork += WorkSize;
    }

    /* Row[144] - vproc_ext_raw_out_fbp_id */
    if (1U == DSP_GetBit(pDramCfg->VprocCompOutBitMask, i, 1U)) {
        if ((pDramCfg->VprocC2YFrmBufNum[i] > 0U) &&
            (pDramCfg->VprocC2YFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = (((UINT32)pDramCfg->MaxW0[i]*2U)*pDramCfg->MaxH0[i])*pDramCfg->VprocC2YFrmBufNum[i];
            *pDspWork += WorkSize;
        }
    }

    /* Row[159] - vproc_main_fbp_id */
    if ((pDramCfg->VprocMainFrmBufNum[i] > 0U) &&
        (pDramCfg->VprocMainFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
        UINT32 MaxMainWidthAlign = ALIGN_NUM((UINT32)pDramCfg->MaxMainWidth[i], 32U);
        UINT32 MaxMainHeightAlign = ALIGN_NUM((UINT32)pDramCfg->MaxMainHeight[i] + 16U, 32U);

        WorkSize = (((MaxMainWidthAlign*3U)/2U)*(MaxMainHeightAlign))*pDramCfg->VprocMainFrmBufNum[i];
        *pDspWork += WorkSize;
    }
    /* Row[160] - vproc_main_me_fbp_id */
    if ((pDramCfg->VprocMainMeFrmBufNum[i] > 0U) &&
        (pDramCfg->VprocMainMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
        UINT32 W = ALIGN_NUM(pDramCfg->MaxMainWidth[i], 32U)/4U;
        UINT32 H = ALIGN_NUM(pDramCfg->MaxMainHeight[i], 32U)/4U;
        WorkSize = (W*H)*pDramCfg->VprocMainMeFrmBufNum[i];
        WorkSize += ((W/2U)*(H/2U))*pDramCfg->VprocMainMeFrmBufNum[i];
        *pDspWork += WorkSize;
    }
}

static inline void HL_CalcWorkingBufferVproc(UINT32 AOI, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork)
{
    UINT16 i;
    UINT32 WorkSize;

    if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_VPROC_BIT, 1U)) {
        for (i=0U; i<pDramCfg->MaxVprocNum; i++) {
            if (1U == DSP_GetBit(pDramCfg->VprocBitMask, i, 1U)) {

                HL_CalcWorkingBufferVprocTopHalf(i, pDramCfg, pDspWork);

                /* Row[161] - vproc_prev_a_fbp_id */
                if ((pDramCfg->VprocPrevAFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocPrevAFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 MaxPrevAWidthAlign = ALIGN_NUM(pDramCfg->MaxPrevAWidth[i], 32U);
                    UINT32 MaxPrevAHeightAlign = ALIGN_NUM((UINT32)pDramCfg->MaxPrevAHeight[i] + 16U, 32U);

                    WorkSize = (((MaxPrevAWidthAlign*3U)/2U)*(MaxPrevAHeightAlign))*pDramCfg->VprocPrevAFrmBufNum[i];
                    *pDspWork += WorkSize;
                }
                /* Row[162] - vproc_me_a_fbp_id */
                if ((pDramCfg->VprocPrevAMeFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocPrevAMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 W = ALIGN_NUM(pDramCfg->MaxPrevAWidth[i], 32U)/4U;
                    UINT32 H = ALIGN_NUM(pDramCfg->MaxPrevAHeight[i], 32U)/4U;
                    WorkSize = (W*H)*pDramCfg->VprocPrevAMeFrmBufNum[i];
                    WorkSize += ((W/2U)*(H/2U))*pDramCfg->VprocPrevAMeFrmBufNum[i];
                    *pDspWork += WorkSize;
                }

                /* Row[163] - vproc_prev_b_fbp_id */
                if ((pDramCfg->VprocPrevBFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocPrevBFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 MaxPrevBWidthAlign = ALIGN_NUM(pDramCfg->MaxPrevBWidth[i], 32U);
                    UINT32 MaxPrevBHeightAlign = ALIGN_NUM((UINT32)pDramCfg->MaxPrevBHeight[i] + 16U, 32U);

                    WorkSize = (((MaxPrevBWidthAlign*3U)/2U)*(MaxPrevBHeightAlign))*pDramCfg->VprocPrevBFrmBufNum[i];
                    *pDspWork += WorkSize;
                }
                /* Row[164] - vproc_me_b_fbp_id */
                if ((pDramCfg->VprocPrevBMeFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocPrevBMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 W = ALIGN_NUM(pDramCfg->MaxPrevBWidth[i], 32U)/4U;
                    UINT32 H = ALIGN_NUM(pDramCfg->MaxPrevBHeight[i], 32U)/4U;
                    WorkSize = (W*H)*pDramCfg->VprocPrevBMeFrmBufNum[i];
                    WorkSize += ((W/2U)*(H/2U))*pDramCfg->VprocPrevBMeFrmBufNum[i];
                    *pDspWork += WorkSize;
                }

                /* Row[165] - vproc_prev_c_fbp_id */
                if ((pDramCfg->VprocPrevCFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocPrevCFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 MaxPrevCWidthAlign = ALIGN_NUM(pDramCfg->MaxPrevCWidth[i], 32U);
                    UINT32 MaxPrevCHeightAlign = ALIGN_NUM((UINT32)pDramCfg->MaxPrevCHeight[i] + 16U, 32U);
                    WorkSize = (((MaxPrevCWidthAlign*3U)/2U)*(MaxPrevCHeightAlign))*pDramCfg->VprocPrevCFrmBufNum[i];
                    *pDspWork += WorkSize;
                }
                /* Row[166] - vproc_me_c_fbp_id */
                if ((pDramCfg->VprocPrevCMeFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocPrevCMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 W = ALIGN_NUM(pDramCfg->MaxPrevCWidth[i], 32U)/4U;
                    UINT32 H = ALIGN_NUM(pDramCfg->MaxPrevCHeight[i], 32U)/4U;
                    WorkSize = (W*H)*pDramCfg->VprocPrevCMeFrmBufNum[i];
                    WorkSize += ((W/2U)*(H/2U))*pDramCfg->VprocPrevCMeFrmBufNum[i];
                    *pDspWork += WorkSize;
                }

                /* Row[167] - vproc_ln_dec_fbp_id */
                if ((pDramCfg->VprocLndtFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocLndtFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 MaxLndtWidthAlign = ALIGN_NUM(pDramCfg->MaxLndtWidth[i], 32U);
                    UINT32 MaxLndtHeightAlign = ALIGN_NUM((UINT32)pDramCfg->MaxLndtHeight[i] + 16U, 32U);
                    WorkSize = (((MaxLndtWidthAlign*3U)/2U)*(MaxLndtHeightAlign))*pDramCfg->VprocLndtFrmBufNum[i];
                    *pDspWork += WorkSize;
                }
                /* Row[168] - vproc_hier_fbp_id */
                if ((pDramCfg->VprocHierFrmBufNum[i] > 0U) &&
                    (pDramCfg->VprocHierFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
                    UINT32 MaxHierBufWidthAlign = ALIGN_NUM(pDramCfg->MaxHierBufWidth[i], 32U);
                    UINT32 MaxHierBufHeightAlign = ALIGN_NUM((UINT32)pDramCfg->MaxHierBufHeight[i] + 16U, 32U);
                    WorkSize = (((MaxHierBufWidthAlign*3U)/2U)*(MaxHierBufHeightAlign))*pDramCfg->VprocHierFrmBufNum[i];
                    *pDspWork += WorkSize;
                }
            }
        }
    }
}

static inline void HL_CalcWorkingBufferEnc(UINT32 AOI, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork)
{
    UINT16 i;
    UINT32 WorkSize;

    if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_ENC_BIT, 1U)) {
        /* Row[41] - fbp_id_rec */
        for (i=0U; i<pDramCfg->EncNum; i++) {
            UINT32 EncW = ALIGN_NUM(pDramCfg->MaxEncWidth[i], 32U);
            UINT32 EncH = ALIGN_NUM(pDramCfg->MaxEncHieght[i], 32U);
            WorkSize = (((EncW*3U)/2U)*EncH)*pDramCfg->ReconNum[i];
            WorkSize += (((((EncW/32U) + 1U)*48U) + (EncW/4U))*(EncH/16U))*pDramCfg->ReconNum[i];
            *pDspWork += WorkSize;
        }

        /* Row[44] - pjpeg_dram_base[] */
        WorkSize = pDramCfg->PjpgSize + (pDramCfg->AvcPjpgSize*3U);
        *pDspWork += WorkSize;
    }
}

static inline void HL_CalcWorkingBufferDec(UINT32 AOI, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork)
{
    UINT32 WorkSize;

    if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_DEC_BIT, 1U)) {
        /* Row[16] - create_avc_dec() */
        if (1U == DSP_GetU8Bit(pDramCfg->DecFmt, 0U/*AVC idx*/, 1U)) {
            /* recon_ctx_dram_par */
            *pDspWork += 8192U;
        }

        /* Row[19] - createHevcDecoder() */
        if (1U == DSP_GetU8Bit(pDramCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
            /* recon_ctx_dram_par */
            *pDspWork += 8704U;
        }

        /* Row[30] - dec_init_flow_base() */
        /* dec_dsp_cmd_q_id */
        *pDspWork += 4096U;

        /* Row[42] - dec_create_fbp_dram_par() */
        /* fbp_dram_par */
        WorkSize = ((UINT32)pDramCfg->DecMaxWidth*pDramCfg->DecMaxHeight)*pDramCfg->DecMaxDpb;
        WorkSize *= 3U;
        WorkSize /= 2U;
        WorkSize += (((UINT32)pDramCfg->DecMaxWidth/2U)*((UINT32)pDramCfg->DecMaxHeight/16U))*((UINT32)pDramCfg->DecMaxDpb + 4U);
        WorkSize *= pDramCfg->DecNum;
        *pDspWork += WorkSize;

        /* Row[45] - config_avc_dec() */
        if (1U == DSP_GetU8Bit(pDramCfg->DecFmt, 0U/*AVC idx*/, 1U)) {
            /* recon_dram_par_id */
            WorkSize = ((UINT32)36864U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* sh_dram_par_id */
            WorkSize = ((UINT32)25344U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* hcb_dram_par_id */
            WorkSize = (16777216U*pDramCfg->DecNum);
            *pDspWork += WorkSize;
        }

        /* Row[53] - setupHevcDecoder() */
        if (1U == DSP_GetU8Bit(pDramCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
            /* sh_par_id */
            WorkSize = ((UINT32)35100U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* db_recon_cmdmsg_par_id */
            WorkSize = (131072U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* hdr_par_id */
            WorkSize = ((UINT32)48960U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* hcb_par_id */
            WorkSize = (18874368U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* fb_colo_par_id */
            WorkSize = (13369344U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* dram_mvedge_par_id */
            WorkSize = (65536U*pDramCfg->DecNum);
            *pDspWork += WorkSize;

            /* dpb_store_par_id */
            WorkSize = ((UINT32)8192U*pDramCfg->DecNum);
            *pDspWork += WorkSize;
        }
    }
}

UINT32 HL_CalcWorkingBuffer(UINT32 AOI, const DSP_DRAM_CFG_s *pDramCfg, UINT32 *pDspWork)
{
    UINT32 Rval = OK;

    if ((pDramCfg == NULL) ||
        (pDspWork == NULL)) {
        Rval = DSP_ERR_0000;
    } else if ((AOI == 0x0U) ||
               (AOI > DSP_MEM_AREA_ALL_MASK)) {
        Rval = DSP_ERR_0001;
    } else {
#ifdef DEBUG_DSP_WORK_CALC
        UINT16 i;

        AmbaLL_LogUInt5("DramCfg[IN]  AOI[0x%X] Size[%d]", AOI, *pDspWork, 0U, 0U, 0U);
        if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_VIN_BIT, 1U)) {
            AmbaLL_LogUInt5("  VIN : Bit[0x%X 0x%X]", pDramCfg->VinBitMask, pDramCfg->VinCeBitMask, 0U, 0U, 0U);
            for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
                AmbaLL_LogUInt5("        Idx[%d]", i, 0U, 0U, 0U, 0U);
                if (1U == DSP_GetBit(pDramCfg->VinBitMask, i, 1U)) {
                    AmbaLL_LogUInt5("          Win[%d %d] Cmpr[%d] Num[%d]", pDramCfg->VinMaxCapOutWidth[i], pDramCfg->VinMaxCapOutHeight[i], pDramCfg->VinCmpr[i], pDramCfg->CapOutBufNum[i], 0U);
                }
                if (1U == DSP_GetBit(pDramCfg->VinBitMask, i, 1U)) {
                    AmbaLL_LogUInt5("          AuxWin[%d %d] Num[%d]", pDramCfg->VinMaxCeOutWidth[i], pDramCfg->VinMaxCeOutHeight[i], pDramCfg->CeOutBufNum[i], 0U, 0U);
                }
            }
        }
        if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_VPROC_COMM_BIT, 1U)) {
            AmbaLL_LogUInt5("  VPROC_COMM : Comm0[%d %d %d %d]", pDramCfg->MaxComm0Width, pDramCfg->MaxComm0Hieght,
                                                                 pDramCfg->MaxVprocEfctComm0FrmBufNum, pDramCfg->VprocComm0Fmt, 0U);
            AmbaLL_LogUInt5("               Comm1[%d %d %d %d]", pDramCfg->MaxComm1Width, pDramCfg->MaxComm1Hieght,
                                                                 pDramCfg->MaxVprocEfctComm1FrmBufNum, pDramCfg->VprocComm1Fmt, 0U);
            AmbaLL_LogUInt5("               Efct0[%d %d %d %d]", pDramCfg->MaxEfctMainWidth, pDramCfg->MaxEfctMainHieght,
                                                                 pDramCfg->MaxVprocEfctMainFrmBufNum, pDramCfg->MaxVprocEfctMainMeFrmBufNum, 0U);
            AmbaLL_LogUInt5("               Efct1[%d %d %d %d]", pDramCfg->MaxEfctPipWidth, pDramCfg->MaxEfctPipHieght,
                                                                 pDramCfg->MaxVprocEfctPipFrmBufNum, pDramCfg->MaxVprocEfctPipMeFrmBufNum, 0U);
        }
        if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_VPROC_BIT, 1U)) {
            AmbaLL_LogUInt5("  VPROC :      Num[%d]", pDramCfg->MaxVprocNum, 0U, 0U, 0U, 0U);
            for (i=0U; i<pDramCfg->MaxVprocNum; i++) {
                if (1U == DSP_GetBit(pDramCfg->VprocBitMask, i, 1U)) {
                    AmbaLL_LogUInt5("               Idx[%d] C2Y[%d %d %d]", i, pDramCfg->MaxC2YWidth[i], pDramCfg->MaxC2YHeight[i],
                                                                            pDramCfg->VprocC2YFrmBufNum[i], 0U);
                    if (1U == DSP_GetBit(pDramCfg->VprocCompOutBitMask, i, 1U)) {
                        AmbaLL_LogUInt5("                       CE[%d %d %d]", pDramCfg->MaxW0[i], pDramCfg->MaxH0[i],
                                                                               pDramCfg->VprocC2YFrmBufNum[i], 0U, 0U);
                    }
                    AmbaLL_LogUInt5("                       Main[%d %d %d %d]", pDramCfg->MaxMainWidth[i], pDramCfg->MaxMainHeight[i],
                                                                                pDramCfg->VprocMainFrmBufNum[i], pDramCfg->VprocMainMeFrmBufNum[i], 0U);
                    AmbaLL_LogUInt5("                       PrevA[%d %d %d %d]", pDramCfg->MaxPrevAWidth[i], pDramCfg->MaxPrevAHeight[i],
                                                                                 pDramCfg->VprocPrevAFrmBufNum[i], pDramCfg->VprocPrevAMeFrmBufNum[i], 0U);
                    AmbaLL_LogUInt5("                       PrevB[%d %d %d %d]", pDramCfg->MaxPrevBWidth[i], pDramCfg->MaxPrevBHeight[i],
                                                                                 pDramCfg->VprocPrevBFrmBufNum[i], pDramCfg->VprocPrevBMeFrmBufNum[i], 0U);
                    AmbaLL_LogUInt5("                       PrevC[%d %d %d %d]", pDramCfg->MaxPrevCWidth[i], pDramCfg->MaxPrevCHeight[i],
                                                                                 pDramCfg->VprocPrevCFrmBufNum[i], pDramCfg->VprocPrevCMeFrmBufNum[i], 0U);
                    AmbaLL_LogUInt5("                       Lndt[%d %d %d]", pDramCfg->MaxLndtWidth[i], pDramCfg->MaxLndtHeight[i],
                                                                                pDramCfg->VprocLndtFrmBufNum[i], 0U, 0U);
                    AmbaLL_LogUInt5("                       Hier[%d %d %d]", pDramCfg->MaxHierBufWidth[i], pDramCfg->MaxHierBufHeight[i],
                                                                                pDramCfg->VprocHierFrmBufNum[i], 0U, 0U);
                }
            }
        }
        if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_ENC_BIT, 1U)) {
            AmbaLL_LogUInt5("  ENC :        Num[%d] Pjpg[%d %d]", pDramCfg->EncNum, pDramCfg->PjpgSize,
                                                                  pDramCfg->AvcPjpgSize, 0U, 0U);
            for (i=0U; i<pDramCfg->EncNum; i++) {
                AmbaLL_LogUInt5("               Idx[%d] Recon[%d] Win[%d %d]", i, pDramCfg->ReconNum[i],
                                                                               pDramCfg->MaxEncWidth[i], pDramCfg->MaxEncHieght[i], 0U);
            }
        }
        if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_DEC_BIT, 1U)) {
            AmbaLL_LogUInt5("  DEC :        Num[%d] Fmt[0x%X] Win[%d %d] Dpb[%d]", pDramCfg->DecNum, pDramCfg->DecFmt,
                                                                                   pDramCfg->DecMaxWidth, pDramCfg->DecMaxHeight,
                                                                                   pDramCfg->DecMaxDpb);
        }
        if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_MISC_BIT, 1U)) {
            //TBD
        }
#endif

        HL_CalcWorkingBufferVin(AOI, pDramCfg, pDspWork);
        HL_CalcWorkingBufferVprocComm(AOI, pDramCfg, pDspWork);
        HL_CalcWorkingBufferVproc(AOI, pDramCfg, pDspWork);
        HL_CalcWorkingBufferEnc(AOI, pDramCfg, pDspWork);
        HL_CalcWorkingBufferDec(AOI, pDramCfg, pDspWork);

//        if (1U == DSP_GetBit(AOI, DSP_MEM_AREA_MISC_BIT, 1U)) {
            //TBD
//        }
    }

#ifdef DEBUG_DSP_WORK_CALC
    AmbaLL_LogUInt5("DramCfg[OUT] AOI[0x%X] Size[%d]", AOI, *pDspWork, 0U, 0U, 0U);
#endif

    return Rval;
}

UINT32 HL_CalcDramBandWidth(UINT32 AOI, const DSP_DRAM_BW_CFG_s *pDramBwCfg, const UINT32 *pBandWidth)
{
    UINT32 Rval = OK;
#ifdef DEBUG_DRAM_BW_CALC
    AmbaLL_LogUInt5("BWCfg[IN]  AOI[0x%X] BW[%d]", AOI, *pBandWidth, 0U, 0U, 0U);
#endif

    if ((pDramBwCfg == NULL) ||
        (pBandWidth == NULL)) {
        Rval = DSP_ERR_0000;
    } else if ((AOI == 0x0U) ||
               (AOI > DSP_BW_AREA_ALL_MASK)) {
        Rval = DSP_ERR_0001;
    } else {
        if (1U == DSP_GetBit(AOI, DSP_BW_AREA_VIN_BIT, 1U)) {
            /* Row[60] - raw_out */
            /* ce_out */
        }
        if (1U == DSP_GetBit(AOI, DSP_BW_AREA_VPROC_BIT, 1U)) {
            /* Row[66] - raw_in */
            /* Row[67] - c2y_out/c2y_in */
            /* Row[68] - mctf_ref_in/mctf_out */
            /* Row[69] - mcts_y_out */
            /* Row[70] - prev_a_out */
            /* Row[71] - prev_b_out */
            /* Row[72] - prev_c_out */
            /* Row[73] - prev_ln_out */
            /* Row[74] - prev_hier_out */
        }
        if (1U == DSP_GetBit(AOI, DSP_BW_AREA_VOUT_BIT, 1U)) {
            /* Row[86] - vout_in */
            /* OSD */
        }
        if (1U == DSP_GetBit(AOI, DSP_BW_AREA_ENC_BIT, 1U)) {
            /* Row[80] - enc_in/enc_ref_in/enc_recon_out */
        }
        if (1U == DSP_GetBit(AOI, DSP_BW_AREA_EFCT_BIT, 1U)) {
            /* Row[92] - copy_in/copy_out */
            /* Row[98] - mctf_tar/mctf_ref/mctf_output */
            /* Row[99] - alpha_in */
        }
        if (1U == DSP_GetBit(AOI, DSP_BW_AREA_DEC_BIT, 1U)) {
            //TBD
        }
        if (1U == DSP_GetBit(AOI, DSP_BW_AREA_MISC_BIT, 1U)) {
            //TBD
        }
    }
#ifdef DEBUG_DRAM_BW_CALC
    AmbaLL_LogUInt5("BWCfg[OUT] AOI[0x%X] BW[%d]", AOI, *pBandWidth, 0U, 0U, 0U);
#endif

    return Rval;
}
