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

static inline void HL_PrepareBaseParamBootTopHalf(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* Row[21] - init_dsp_mm() in dsp_boot_config() */
    /* frmbufpools */
    if (1U == DSP_GetBit(pBaseCfg->FbpCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->FbpCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }

    /* mfrmbufpools */
    if (1U == DSP_GetBit(pBaseCfg->MFbpCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->MFbpCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }

    /* frm_buffers[FB_INT_IDX] */
    if (1U == DSP_GetBit(pBaseCfg->FbCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->FbCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }

    /* frm_buffers[FB_EXT_IDX] */
    if (1U == DSP_GetBit(pBaseCfg->FbCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->FbCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }
}

static inline void HL_PrepareBaseParamBoot(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    HL_PrepareBaseParamBootTopHalf(pBaseCfg, pBaseParam);

    /* mfrm_buffers[FB_INT_IDX] */
    if (1U == DSP_GetBit(pBaseCfg->MFbCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->MFbCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }

    /* mfrm_buffers[FB_EXT_IDX], share MBb setting */
    if (1U == DSP_GetBit(pBaseCfg->MFbCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->MFbCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }

    /* frm_imginfos */
    if (1U == DSP_GetBit(pBaseCfg->ImgInfCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->ImgInfCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }

    /* frm_mimginfos */
    if (1U == DSP_GetBit(pBaseCfg->MImgInfCache, BASE_CFG_CACHE_DRAM_IDX, 1U)) {
        pBaseParam->DramParNum += 1U;
    }
    if (1U == DSP_GetBit(pBaseCfg->MImgInfCache, BASE_CFG_CACHE_SMEM_IDX, 1U)) {
        pBaseParam->SmemParNum += 1U;
    }

    /* bdt_sbase */
    // It is always true that max_bd_num is greater than 0
    pBaseParam->SmemParNum += 1U;

    /* Row[31] - init_orc_msg_q_allocator() in dsp_boot_config() */
    // It is always true that max_orc_all_msg_qs is greater than 0
    pBaseParam->SmemParNum += 1U;

    /* Row[35] - init_dsp_init_data_part2() */
    pBaseParam->DramParNum += 1U;
}

static inline void HL_PrepareBaseParamIeng(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
#define IDSP_JOB_Q_NUM              (3U)
#define IDSP_WIN_IN_STR_NUM         (3U)
#define IDSPENG_LN_SYNCCNT_TOT_NUM  (8U)

    /* Row[12] - ieng_init_smem_par() */
    pBaseParam->SmemParNum += 1U;

    /* Row[13] - ieng_init_in_sbuf() */
    pBaseParam->McblNum += 3U;
    pBaseParam->McbNum += 2U*((UINT16)pBaseCfg->LdStrNum[0] + (UINT16)pBaseCfg->LdStrNum[1] + (UINT16)pBaseCfg->LdStrNum[2]);

    /* Row[14] - ieng_init_in_win() */
    pBaseParam->McblNum += 1U;
    pBaseParam->McbNum += (UINT16)((UINT16)2U*IDSP_WIN_IN_STR_NUM);

    /* Row[15] - ieng_init_in_sync() */
    pBaseParam->McblNum += 1U;
    pBaseParam->McbNum += IDSPENG_LN_SYNCCNT_TOT_NUM;

    /* Row[16] - ieng_init_idsp_job() */
    pBaseParam->DramParNum += IDSP_JOB_Q_NUM;
    pBaseParam->OrccodeMsgNum += IDSP_JOB_Q_NUM;
}

static inline void HL_PrepareBaseParamVout(DSP_BASE_PARAM_s *pBaseParam)
{
    /* Row[3] - initMultiVoutCB() */
    pBaseParam->SmemParNum += 1U;

    /* Row[4] - initVout1() */
    pBaseParam->SmemParNum += 1U;

    /* Row[5] - initVout1Smem() */
    pBaseParam->SmemParNum += 1U;

    /* Row[6] - initVout0() */
    pBaseParam->SmemParNum += 1U;

    /* Raw[7] - initVout0Smem() */
    pBaseParam->SmemParNum += 1U;
}

static inline void HL_PrepareBaseParamVin(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
#define VIN_DEFAULT_RAW_FBP_SIZE    (7U)
    UINT16 i;
    UINT16 FrmBufNum;
    UINT8 IsYuvOutput, IsCeOut;

    /* Row[23] - vin_alloc_sys_sd_mem_usage() */
    pBaseParam->DramParNum += 1U;
    pBaseParam->SmemParNum += 1U;

    /* Row[26] - vin_async_alloc_roi_grp_cmd_mcbl() */
    pBaseParam->DramParNum += 1U;
    pBaseParam->McblNum += 1U;
    pBaseParam->McbNum += 32U;

    /* Row[27] - vin_alloc_sideband_info_mcbl() */
    pBaseParam->DramParNum += 1U;
    pBaseParam->McblNum += 1U;
    pBaseParam->McbNum += (((UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum)*pBaseCfg->MaxVinPinNum)*8U;

    /* Row[30] - vin_init_icb() */
    pBaseParam->DramParNum += (UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum;
    pBaseParam->OrccodeMsgNum += (UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum;
    pBaseParam->MFbpNum += ((UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum)*DSP_VIN_EXT_MEM_TYPE_NUM;
    pBaseParam->ExtMFbNum += ((((UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum)*DSP_VIN_EXT_MEM_TYPE_NUM)*VIN_DEFAULT_RAW_FBP_SIZE) +
                             (((UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum)*VIN_DEFAULT_RAW_FBP_SIZE);
    pBaseParam->MImgInfNum += ((UINT16)pBaseCfg->VinNum + pBaseCfg->VirtVinNum)*DSP_VIN_EXT_MEM_TYPE_NUM;

    /* Row[55] - vin_api_proc_dsp_vin_flow_max_cfg_cmd() */
    pBaseParam->SmemParNum += pBaseCfg->VinNum;

    /* Row[58] - vin_rm_init() */
    for (i = 0; i<DSP_VIN_MAX_NUM; i++) {
        if (0U == DSP_GetBit(pBaseCfg->VinBitMask, i, 1U)) {
            continue;
        }
        if (1U == DSP_GetBit(pBaseCfg->VinYuvBitMask, i, 1U)) {
            FrmBufNum = pBaseCfg->FrmBufNum[i]*2U;
        } else {
            FrmBufNum = pBaseCfg->FrmBufNum[i];
        }

        if (i < AMBA_DSP_MAX_VIN_NUM) {
            /* Row[58] - vin_cfg_fbp_id */
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += 3U;

            /* Row[59] - vin_raw_fbp_id or Vin_yuv422_out_fbp_id */
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += FrmBufNum;
            pBaseParam->MImgInfNum += 1U;

            /* Row[60] - default_raw_img_fbp_id */
            pBaseParam->MFbpNum += 1U;
            pBaseParam->ExtMFbNum += 1U;
            pBaseParam->MImgInfNum += 1U;
            if (1U == DSP_GetBit(pBaseCfg->VinCeBitMask, i, 1U)) {
                /* Row[61] - vin_ce_fbp_id */
                pBaseParam->DramParNum += 1U;
                pBaseParam->MFbpNum += 1U;
                pBaseParam->MFbSize += FrmBufNum;
                pBaseParam->MImgInfNum += FrmBufNum;

                /* Row[62] - default_ce_img_fbp_id */
                pBaseParam->MFbpNum += 1U;
                pBaseParam->ExtMFbNum += 1U;
                pBaseParam->MImgInfNum += 1U;
            }
        }

        /* Row[64] - vin_fov_fbp_ids[] Or Yuv_422 luma */
        pBaseParam->MFbpNum += pBaseCfg->MaxVinPinNum;
        pBaseParam->ExtMFbNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);
        pBaseParam->MImgInfNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);

        /* Row[65] - vin_ce_fov_fbp_ids[] or Yuv_422 chroma */
        IsYuvOutput = (UINT8)DSP_GetBit(pBaseCfg->VinYuvBitMask, i, 1U);
        IsCeOut = (UINT8)DSP_GetBit(pBaseCfg->VinCeBitMask, i, 1U);
        if ((1U == IsYuvOutput) ||
            (1U == IsCeOut)) {
            pBaseParam->MFbpNum += pBaseCfg->MaxVinPinNum;
            pBaseParam->ExtMFbNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);
            pBaseParam->MImgInfNum += ((UINT16)pBaseCfg->MaxVinPinNum*FrmBufNum);
        }

        /* Row[66] - vin_yuv_enc_fbp_id[]-420/422, After Rev_286448 */
        pBaseParam->FbpNum += 2U;
        pBaseParam->ExtFbNum += 8U;
        pBaseParam->ImgInfNum += 2U;

        /* ExtMe for TimeLapse style feature */
        pBaseParam->FbpNum += 1U;
        pBaseParam->ExtFbNum += 7U;
        pBaseParam->ImgInfNum += 1U;
    }
}

static inline void HL_PrepareBaseParamVprocTopHalf(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* Row[31] - create_vproc_dv */
    /* cmd_q */
    pBaseParam->DramParNum += (UINT16)pBaseCfg->MaxVprocNum + 1U;
    pBaseParam->OrccodeMsgNum += (UINT16)pBaseCfg->MaxVprocNum + 1U;
    /* pic_q */
    pBaseParam->DramParNum += pBaseCfg->MaxVprocNum;
    pBaseParam->OrccodeMsgNum += pBaseCfg->MaxVprocNum;
    /* msg_q */
    pBaseParam->DramParNum += pBaseCfg->MaxVprocNum;
    pBaseParam->OrccodeMsgNum += pBaseCfg->MaxVprocNum;
    /* icb->ext_buf_cb[EXT_MEM_TYPE_NUM] */
    pBaseParam->FbpNum += (UINT16)pBaseCfg->MaxVprocNum*NUM_VPROC_EXT_MEM_TYPE;
    pBaseParam->ExtFbNum += (DEFAULT_EXT_MEM_POOL_VPROC_BUF_NUM*(UINT16)pBaseCfg->MaxVprocNum)*NUM_VPROC_EXT_MEM_TYPE;

    /* vproc_smem_par_id */
    pBaseParam->SmemParNum += 1U;
    /* vproc_dram_par_id */
    pBaseParam->DramParNum += 1U;

    /* Row[38] - veffect_rm_init(), after 20190411 MM */
    /* s_str_bdt_id
     * VEFFECT_WARP_COPY_STR_NUM(SEC3_STR_NUM) (=7)+ VEFFECT_BLEND_S_STR_NUM(MCTF_S_STR_NUM) (=43)
     */
    if (pBaseCfg->VprocResEfct == 1U) {
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 50U;
    }

    /* d_str_bdt_id
     * VEFFECT_WARP_COPY_STR_NUM (=18)+ VEFFECT_BLEND_D_STR_NUM (=4)
     */
    if (pBaseCfg->VprocResEfct == 1U) {
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 44U;
    }

    /* smem_sub_par_id */
    if (pBaseCfg->VprocResEfct == 1U) {
        pBaseParam->SubParNum += 1U;
        pBaseParam->SupParNum += 1U;
    }

    /* Row[44] - vproc_flow_max_cfg_cmd() */
    /* vproc_sec2_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    /* vproc_sec4_cfg_fbp_id */
    if (pBaseCfg->VprocResHdr == 1U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    }
    /* vproc_c2y_dma_params_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    /* vproc_c2y_tile_params_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += 2U*(UINT16)pBaseCfg->MaxVprocNum;
    /* vproc_aaa_cfa_out_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    /* vproc_aaa_rgb_out_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    /* vproc_vin_stats_out_fbp_id*/
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(6U*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    /* vproc_sec3_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    if (pBaseCfg->VprocResEfct == 1U) {
        if (pBaseCfg->VprocResMaxEfctCopyNum > 0U) {
            pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxEfctCopyNum));
        } else {
            pBaseParam->MFbSize += 1U;
        }
    }
}

static inline void HL_PrepareBaseParamVproc(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    HL_PrepareBaseParamVprocTopHalf(pBaseCfg, pBaseParam);

    /* vproc_sec5_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    /* vproc_sec6_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    /* vproc_sec7_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    /* vproc_sec9_cfg_fbp_id */
    if (pBaseCfg->VprocResLndt == 1U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    }
    /* vproc_sec15_cfg_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    /* vproc_warp_dma_params_fbp_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->MFbpNum += 1U;
    pBaseParam->MFbSize += (UINT16)(2U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    /* vproc_warp_tile_params_fbp_id */
    if (pBaseCfg->VprocResTileMode == 1U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbSize += 2U*(UINT16)pBaseCfg->MaxVprocNum;
    }
    pBaseParam->MFbpNum += 1U;

    /* vproc_prev_fbp_id[0] */
    if (pBaseCfg->MaxVprocEfctComm0FrmBufNum > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->FbpNum += 1U;
    }
    pBaseParam->FbNum += pBaseCfg->MaxVprocEfctComm0FrmBufNum;
    pBaseParam->ImgInfNum += 2U;
    /* vproc_prev_fbp_id[1] */
    if (pBaseCfg->MaxVprocEfctComm1FrmBufNum > 0U) {
        pBaseParam->DramParNum += 1U;
        pBaseParam->FbpNum += 1U;
    }
    pBaseParam->FbNum += pBaseCfg->MaxVprocEfctComm1FrmBufNum;
    pBaseParam->ImgInfNum += 2U;
    /* vproc_c2y_secbb_dram_mcbl_id C2y total section num =2 */
    pBaseParam->DramParNum += 1U;
    pBaseParam->McblNum += 1U;
    pBaseParam->McbNum += (UINT16)(4U*(UINT16)(pBaseCfg->VprocResMaxC2YTileNum));
    /* vproc_c2y_secbb_dram_mcbl_id C2y total section num =7 */
    pBaseParam->DramParNum += 1U;
    pBaseParam->McbNum += (UINT16)(14U*(UINT16)(pBaseCfg->VprocResMaxTileNum));
    pBaseParam->McblNum += 1U;
    /* vproc_dzoom_mcbl_id */
    pBaseParam->DramParNum += 1U;
    pBaseParam->McblNum += 1U;
    pBaseParam->McbNum += (3U*(UINT16)pBaseCfg->MaxVprocNum);

    /* comm_main_fbp_id */
    if ((pBaseCfg->MaxVprocEfctMainFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        pBaseParam->DramParNum += 1U;
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->MaxVprocEfctMainFrmBufNum;
            pBaseParam->ImgInfNum += 2U;
        }
    }
    /* comm_main_me01_fbp_id */
    if ((pBaseCfg->MaxVprocEfctMainMeFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        pBaseParam->DramParNum += 1U;
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->MaxVprocEfctMainMeFrmBufNum;
            pBaseParam->ImgInfNum += 2U;
        }
    }
}

static inline void HL_PrepareBaseParamVprocEfctTopHalf(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* prim_output_work_fbp_id VPROC_MAIN_STREAM_IDX=0 */
    if ((pBaseCfg->MaxVprocEfctMainMeFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }
    /* sub_output_work_fbp_id VPROC_MAIN_STREAM_IDX=0 */
    if ((pBaseCfg->MaxVprocEfctMainMeFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctMainMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += pBaseCfg->MaxVprocNum;
            pBaseParam->ExtFbNum += 4U*(UINT16)pBaseCfg->MaxVprocNum;
            pBaseParam->ImgInfNum += 2U*(UINT16)pBaseCfg->MaxVprocNum;
        }
    }

    /* comm_pip_fbp_id */
    if ((pBaseCfg->MaxVprocEfctPipFrmBufNum > 0U) &&
        (pBaseCfg->MaxVprocEfctPipFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
        pBaseParam->DramParNum += 1U;
        if (pBaseCfg->VprocResEfct > 0U) {
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->MaxVprocEfctPipFrmBufNum;
            pBaseParam->ImgInfNum += 2U;
        }
    }
    /* comm_pip_me01_fbp_id */
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
}

static inline void HL_PrepareBaseParamVprocYuyv(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam, UINT16 ViewZoneId)
{
    if (1U == DSP_GetBit(pBaseCfg->VprocMipiYuyvBitMask, ViewZoneId, 1U)) {
        /* Row[147] - vproc_yuyv2y_rm_init() */
        /*
         * s_str_bdt_id SEC2_STR_NUM (=18) + SEC6_STR_NUM (=7) + SEC7_STR_NUM (=7)
         */
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 32U;

        /*
         * d_str_bdt_id SEC2_STR_NUM (=18) + SEC6_STR_NUM (=7) + SEC7_STR_NUM (=7)
         */
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 32U;

        /* smem_sub_par_id */
        pBaseParam->SubParNum += 1U;
        pBaseParam->SupParNum += 1U;

        /* Row[151] - vproc_yuyv2y_sec6_fbp_init() */
        /* vproc_luma_main_out_fbp_id (for sec6 luma out, Y data) */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += pBaseCfg->VprocC2YFrmBufNum[ViewZoneId];

        /* Row[152] - vproc_yuyv2y_sec6_fbp_init() */
        /* vproc_chroma_main_out_fbp_id (for sec7 luma out, UV data) */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += pBaseCfg->VprocC2YFrmBufNum[ViewZoneId];

        /* Row[153] - vproc_yuyv2y_sec2_fbp_init() */
        /* vproc_raw_fbp_id (for sec2 luma out, YUYV data) */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
#define YUYV2Y_SEC2_FB_NUM  (2U)
        pBaseParam->MFbSize += YUYV2Y_SEC2_FB_NUM;
    }
}


static inline void HL_PrepareBaseParamVprocNumTopHalf(UINT16 i, const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    /* Row[125] - vproc_c2y_rm_init() */
    /* s_str_bdt_id SEC2_STR_NUM (=18)+ SEC4_STR_NUM (=4) */
    if (0U == DSP_GetBit(pBaseCfg->VprocMipiYuyvBitMask, i, 1U)) {
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 18U;
    }
    if (1U == DSP_GetBit(pBaseCfg->VprocHdrBitMask, i, 1U)) {
        pBaseParam->BdNum += 4U;
    }
    /* d_str_bdt_id SEC2_STR_NUM (=18)+ SEC4_STR_NUM (=4) */
    if (0U == DSP_GetBit(pBaseCfg->VprocMipiYuyvBitMask, i, 1U)) {
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 18U;
    }
    if (1U == DSP_GetBit(pBaseCfg->VprocHdrBitMask, i, 1U)) {
        pBaseParam->BdNum += 4U;
    }
    /* smem_sub_par_id */
    pBaseParam->SubParNum += 1U;
    pBaseParam->SupParNum += 1U;

    /* Row[125] - vproc_c2y_sec2_fbp_init() */
    /* vproc_luma_main_out_fbp_id */
    if (0U == DSP_GetBit(pBaseCfg->VprocMipiYuyvBitMask, i, 1U)) {
        if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += pBaseCfg->VprocC2YFrmBufNum[i];
        }
    }

    /* vproc_chroma_main_out_fbp_id */
    if (0U == DSP_GetBit(pBaseCfg->VprocMipiYuyvBitMask, i, 1U)) {
        if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->MFbpNum += 1U;
            pBaseParam->MFbSize += pBaseCfg->VprocC2YFrmBufNum[i];
        }
    }

    /* vproc_ext_raw_out_fbp_id */
    if (0U == DSP_GetBit(pBaseCfg->VprocMipiYuyvBitMask, i, 1U)) {
        if (pBaseCfg->VprocC2YFrmBufNum[i] > 0U) {
            if (1U == DSP_GetBit(pBaseCfg->VprocCompOutBitMask, i, 1U)) {
                pBaseParam->DramParNum += 1U;
                pBaseParam->MFbpNum += (UINT16)1U;
                pBaseParam->MFbSize += (UINT16)pBaseCfg->VprocC2YFrmBufNum[i];
            }
        }
    }
}

static inline void HL_PrepareBaseParamVprocNum(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 i;

    /* Row[83] - create_cap_shared_smem */
    /* c2y_smem_par_id */
    pBaseParam->SmemParNum += 1U;
    /* warp_mctf_smem_par_id */
    pBaseParam->SmemParNum += 1U;
    /* yuyv2y_smem_par_id */
    if (pBaseCfg->VprocMipiYuyvBitMask > 0U) {
        pBaseParam->SmemParNum += 1U;
    }

    for (i = 0U; i < pBaseCfg->MaxVprocNum; i++) {

        HL_PrepareBaseParamVprocNumTopHalf(i, pBaseCfg, pBaseParam);

        /* Yuyv part */
        HL_PrepareBaseParamVprocYuyv(pBaseCfg, pBaseParam, i);

        /* Row[155] - vproc_rm_warp_mctf_init() */
        /*
         * s_str_bdt_id SEC3_STR_NUM (=7) + SEC5_STR_NUM (=9) + SEC6_STR_NUM (=7)
         *              SEC7_STR_NUM (=7) + SEC9_STR_NUM (=5) + SEC15_STR_NUM (=15) + MCTF_SMEM_STR_NUM (=37)
         */
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 87U;
        /*
         * s_str_bdt_id SEC3_STR_NUM (=7) + SEC5_STR_NUM (=9) + SEC6_STR_NUM (=7) +
         *              SEC7_STR_NUM (=7) + SEC9_STR_NUM (=5) + SEC15_STR_NUM (=15) + MCTF_DRAM_STR_NUM (=22)
         */
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 72U;
        /* smem_sub_par_id */
        pBaseParam->SubParNum += 1U;
        pBaseParam->SupParNum += 1U;
        /* vproc_main_fbp_id */
        if ((pBaseCfg->VprocMainFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocMainFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocMainFrmBufNum[i];
        }
        /* vproc_main_me_fbp_id */
        if ((pBaseCfg->VprocMainMeFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocMainMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocMainMeFrmBufNum[i];
        }
    }
}

static inline void HL_PrepareBaseParamVprocNumPrev(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 i;
    for (i = 0U; i < pBaseCfg->MaxVprocNum; i++) {
        /* vproc_prev_a_fbp_id */
        if ((pBaseCfg->VprocPrevAFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocPrevAFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevAFrmBufNum[i];
        }
        /* vproc_me_a_fbp_id */
        if ((pBaseCfg->VprocPrevAMeFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocPrevAMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevAMeFrmBufNum[i];
        }
        /* vproc_prev_b_fbp_id */
        if ((pBaseCfg->VprocPrevBFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocPrevBFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevBFrmBufNum[i];
        }

        /* vproc_me_b_fbp_id */
        if ((pBaseCfg->VprocPrevBMeFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocPrevBMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevBMeFrmBufNum[i];
        }
        /* vproc_prev_c_fbp_id */
        if ((pBaseCfg->VprocPrevCFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocPrevCFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevCFrmBufNum[i];
        }
        /* vproc_me_c_fbp_id */
        if ((pBaseCfg->VprocPrevCMeFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocPrevCMeFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocPrevCMeFrmBufNum[i];
        }
        /* vproc_ln_dec_fbp_id */
        if ((pBaseCfg->VprocLndtFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocLndtFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocLndtFrmBufNum[i];
        }

        /* vproc_hier_fbp_id */
        if ((pBaseCfg->VprocHierFrmBufNum[i] > 0U) &&
            (pBaseCfg->VprocHierFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            pBaseParam->DramParNum += 1U;
            pBaseParam->FbpNum += 1U;
            pBaseParam->FbNum += pBaseCfg->VprocHierFrmBufNum[i];
        }
    }
}

static inline void HL_PrepareBaseParamVprocHiso(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 HisoMaxTotalTilePerFrm = 0U;
    UINT16 WM = 0U;
    UINT16 WX = 0U;
    UINT16 WA = 0U;
    UINT16 WB = 0U;
    UINT16 WS = 0U;
    UINT16 W4 = 0U;
    UINT16 Wax8 = 0U;
    UINT16 W16 = 0U;
    UINT16 i;
#define HISO_SEC2_IDX   (0U)
#define HISO_SEC4_IDX   (1U)
#define HISO_SEC5_IDX   (2U)
#define HISO_SEC6_IDX   (3U)
#define HISO_SEC7_IDX   (4U)
#define HISO_SEC9_IDX   (5U)
#define HISO_IDX_NUM    (6U)
    UINT16 HisoStepTileNum[DSP_HISO_STEP_NUM][HISO_IDX_NUM];
    UINT16 HisoStepMaxTileNum[DSP_HISO_STEP_NUM];
    UINT16 HisoSecMaxTileNum[HISO_IDX_NUM];
    UINT16 Idx;

    if (pBaseCfg->VprocResHiso == (UINT8)1U) {
        /* Row[42] - create_sproc_dv() */
        /* sproc_secbb_dram_mcbl_id */
        pBaseParam->DramParNum += 1U;
        pBaseParam->McblNum += 1U;

        (void)dsp_osal_memset(&HisoStepTileNum[0U][0U], 0, (sizeof(UINT16)*DSP_HISO_STEP_NUM)*HISO_IDX_NUM);
        (void)dsp_osal_memset(&HisoStepMaxTileNum[0U], 0, sizeof(UINT16)*DSP_HISO_STEP_NUM);
        (void)dsp_osal_memset(&HisoSecMaxTileNum[0U], 0, sizeof(UINT16)*HISO_IDX_NUM);
        WM = ALIGN_NUM16(pBaseCfg->VprocHisoW0Max, 16U);

        WX = ALIGN_NUM16((WM>>1U), 16U);
        WX = MAX2_16(WX, 128U);

        WA = ALIGN_NUM16(WM, 16U);
        WA = MAX2_16(WA, 128U);

        WB = ALIGN_NUM16((WM>>1U), 16U);
        WB = MAX2_16(WB, 128U);

        WS = ALIGN_NUM16((WM>>2U), 16U);
        WS = MAX2_16(WS, 128U);

        W4 = ALIGN_NUM16((WM>>2U), 16U);
        W4 = MAX2_16(W4, 128U);

        Wax8 = ALIGN_NUM16((WM>>3U), 16U);
        Wax8 = MAX2_16(Wax8, 128U);

        W16 = ALIGN_NUM16((WM>>4U), 16U);
        W16 = MAX2_16(W16, 128U);

        Idx = 0U/*IMG_PASS_HISO_STEP_1*/;
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        HisoStepTileNum[Idx][HISO_SEC4_IDX] = HisoStepTileNum[Idx][HISO_SEC2_IDX];
        (void)HL_CalcVideoTile(WB, pBaseCfg->VprocHisoSec5Max, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WS, pBaseCfg->VprocHisoSec7Max, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 1U/*IMG_PASS_HISO_STEP_2*/;
        (void)HL_CalcVideoTile(WS, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec5Max, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile((WS*2U), pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WS, pBaseCfg->VprocHisoSec7Max, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 2U/*IMG_PASS_HISO_STEP_3*/;
        (void)HL_CalcVideoTile(WB, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(Wax8, pBaseCfg->VprocHisoSec7Max, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 3U/*DSP_IMG_PASS_HISO_STEP_4*/;
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W16, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile((W4/2U), pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W16, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 4/*DSP_IMG_PASS_HISO_STEP_4A*/;
        (void)HL_CalcVideoTile(Wax8, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(Wax8, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);

        Idx = 5U/*DSP_IMG_PASS_HISO_STEP_5*/;
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile((W4/2U), pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 6U/*DSP_IMG_PASS_HISO_STEP_6*/;
        (void)HL_CalcVideoTile(W16, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(Wax8, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(Wax8, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(Wax8, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 7U/*DSP_IMG_PASS_HISO_STEP_7*/;
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(W4, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);

        Idx = 8U/*DSP_IMG_PASS_HISO_STEP_8*/;
        (void)HL_CalcVideoTile(WB, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WB, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WB, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);

        Idx = 9U/*DSP_IMG_PASS_HISO_STEP_9*/;
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WX, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 10/*DSP_IMG_PASS_HISO_STEP_10*/ ;
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);

        Idx = 11/*DSP_IMG_PASS_HISO_STEP_11*/;
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        HisoStepTileNum[Idx][HISO_SEC4_IDX] = HisoStepTileNum[Idx][HISO_SEC2_IDX];
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        Idx = 12U/*DSP_IMG_PASS_HISO_STEP_12*/;
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        HisoStepTileNum[Idx][HISO_SEC4_IDX] = HisoStepTileNum[Idx][HISO_SEC2_IDX];
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoColRepMax, &HisoStepTileNum[Idx][HISO_SEC7_IDX], 0U/*Overlap*/);

        Idx = 13U/*DSP_IMG_PASS_HISO_STEP_13*/;
        (void)HL_CalcVideoTile(WA, pBaseCfg->VprocHisoSec2Max, &HisoStepTileNum[Idx][HISO_SEC2_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile((WA/2U), pBaseCfg->VprocHisoSec5Max, &HisoStepTileNum[Idx][HISO_SEC5_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WA, pBaseCfg->VprocHisoSec6Max, &HisoStepTileNum[Idx][HISO_SEC6_IDX], 0U/*Overlap*/);
        (void)HL_CalcVideoTile(WM, pBaseCfg->VprocHisoSec9Max, &HisoStepTileNum[Idx][HISO_SEC9_IDX], 0U/*Overlap*/);

        for (Idx = 0U; Idx<DSP_HISO_STEP_NUM; Idx++) {
            for (i = 0U; i<HISO_IDX_NUM; i++) {
                HisoStepMaxTileNum[Idx] = MAX2_16(HisoStepMaxTileNum[Idx], HisoStepTileNum[Idx][i]);
            }
            HisoStepMaxTileNum[Idx] += 1U;

            HisoMaxTotalTilePerFrm += HisoStepMaxTileNum[Idx];
        }
        pBaseParam->McbNum += HisoMaxTotalTilePerFrm;

        /* Row[45] - sproc_hi_rm_init() */
        /* s_str_bdt_id */
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 87U;
        /* d_str_bdt_id */
        pBaseParam->BdtNum += 1U;
        pBaseParam->BdNum += 72U;
        /* smem_sup_par_id */
        pBaseParam->SupParNum += 1U;
        /* smem_sub_par_id */
        pBaseParam->SubParNum += 1U;

        /* Row[51] - sproc_hi_icb_rm_init() */
        /* sproc_hi_common_fbp[SPROC_HI_FPB_WXHX_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 5U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_W4H4_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 6U;
        /* SPROC_HI_FPB_WBHB_2_IDX */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 1U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_W4_2_H4_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 3U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_WMHM_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 2U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_WBHB_2_UV_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 3U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_W4H4_2_UV_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 2U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_W8H8_2_UV_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 3U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_WAHA_2_UV_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 3U;
        /* sproc_hi_common_fbp[SPROC_HI_FPB_INTERMEDIATE_IDX] */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 5U;
        /* sproc_hi_strs_dma_params_fbp_id */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 14U;
        /* sproc_hi_tile_params_fbp_id */
        pBaseParam->DramParNum += 1U;
        pBaseParam->MFbpNum += 1U;
        pBaseParam->MFbSize += 14U;

        for (i = 0U; i<HISO_IDX_NUM; i++) {
            for (Idx = 0U; Idx<DSP_HISO_STEP_NUM; Idx++) {
                if (HisoStepTileNum[Idx][i] > 0U) {
                    HisoSecMaxTileNum[i] += HisoStepMaxTileNum[Idx];
                }
            }
        }
        /* sec2 idsp cfg */
        pBaseParam->MFbSize += HisoSecMaxTileNum[HISO_SEC2_IDX];

        /* sec4 idsp cfg */
        pBaseParam->MFbSize += HisoSecMaxTileNum[HISO_SEC4_IDX];

        /* sec5 idsp cfg */
        pBaseParam->MFbSize += HisoSecMaxTileNum[HISO_SEC5_IDX];

        /* sec6 idsp cfg */
        pBaseParam->MFbSize += HisoSecMaxTileNum[HISO_SEC6_IDX];

        /* sec7 idsp cfg */
        pBaseParam->MFbSize += HisoSecMaxTileNum[HISO_SEC7_IDX];

        /* sec9 idsp cfg */
        pBaseParam->MFbSize += HisoSecMaxTileNum[HISO_SEC9_IDX];

#if defined (CONFIG_SOC_CV28)
        /* 2022/07/22, ucode need 11 more after rev_289862 */
        pBaseParam->MFbSize += 11U;
#endif
    }
}

static inline void HL_PrepareBaseParamEncode(const DSP_BASE_CFG_s *pBaseCfg, DSP_BASE_PARAM_s *pBaseParam)
{
    UINT16 i;
    /* Row[13] - Create Encoder */
    /* s_bdt_id*/
    pBaseParam->BdtNum += (1U + (UINT16)pBaseCfg->EncNum);
    pBaseParam->BdNum += 68U*((UINT16)pBaseCfg->EncNum + 1U);
    /* d_bdt_id*/
    pBaseParam->BdtNum += (1U + (UINT16)pBaseCfg->EncNum);
    pBaseParam->BdNum += 48U*((UINT16)pBaseCfg->EncNum + 1U);
    pBaseParam->OrccodeMsgNum += pBaseCfg->EncNum;
    /* data_in_q_base */
    pBaseParam->SmemParNum += pBaseCfg->EncNum;
    pBaseParam->OrccodeMsgNum += pBaseCfg->EncNum;
    /* hlenc_dsp_cmd_q_id */
    pBaseParam->DramParNum += pBaseCfg->EncNum;
    /* Row[21] - Create Proxy */
    pBaseParam->SmemParNum += 1U;
    /* Row[31] - Create Eng0 */
    /* smem_bdt_id (ENG0_NUM_SMEM_BUFFERS = 9) */
    pBaseParam->BdtNum += 1U;
    pBaseParam->BdNum += 9U;
    /* smem_bdt_id (ENG0_NUM_SMEM_BUFFERS = 2) */
    pBaseParam->BdtNum += 1U;
    pBaseParam->BdNum += 2U;
    /* eng0_msg_in_q_id */
    pBaseParam->SmemParNum += 1U;
    pBaseParam->OrccodeMsgNum += 1U;
    /* eng0_smem_base */
    pBaseParam->SmemParNum += 1U;
    /* Row[41] - encoder_rm_init() */
    /* fbp_id_rec */
    pBaseParam->DramParNum += pBaseCfg->EncNum;
    pBaseParam->FbpNum += pBaseCfg->EncNum;
    for (i=0U; i<pBaseCfg->EncNum; i++) {
        pBaseParam->FbNum += pBaseCfg->ReconNum[i];
    }
    /* common_smem */
    pBaseParam->SmemParNum += 1U;
    /* common_dram*/
    pBaseParam->DramParNum += 1U;
    /* pjpeg_dram_base[] */
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
    }

    /* Row[19] - createHevcDecoder */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
        /* recon_ctx_dram_par */
        pBaseParam->DramParNum += 1U;

        /* recon_ctx_dbp */
        pBaseParam->DbpNum += 1U;

        /* out_fifo_q_id */
        pBaseParam->SmemParNum += pBaseCfg->DecNum;
        pBaseParam->OrccodeMsgNum += pBaseCfg->DecNum;

        /* cpb_id */
        pBaseParam->CBufNum += pBaseCfg->DecNum;

        /* hcb_id */
        pBaseParam->CBufNum += ((UINT16)3U*pBaseCfg->DecNum);

        /* fbp_dram_id */
        pBaseParam->FbpNum += pBaseCfg->DecNum;

        /* HEVC_MAX_NUM_OF_PIC_IN_DRAM_FBP */
        if (pBaseCfg->DecNum > 1U) {
            pBaseParam->FbNum += ((UINT16)12U*pBaseCfg->DecNum);
        } else {
            pBaseParam->FbNum += 16U;
        }

        /* fbp_imginfo */
        pBaseParam->ImgInfNum += pBaseCfg->DecNum;

        /* colo_imginfo */
        pBaseParam->ImgInfNum += pBaseCfg->DecNum;

        /* dbp_recon_cmd_id */
        pBaseParam->DbpNum += pBaseCfg->DecNum;
    }

    /* Row[29] - create_jpg_dec */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 2U/*JPG idx*/, 1U)) {
        /* buf_smem_par_id */
        pBaseParam->SmemParNum += 1U;
    }

    /* Row[30] - dec_init_flow_base */
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
    pBaseParam->ExtFbNum += 1U; // base on rev_287509_refined
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

    /* Row[42] - dec_create_fbp_dram_par */
    /* fbp_dram_par */
    pBaseParam->DramParNum += pBaseCfg->DecNum;

    /* Row[45] - config_avc_dec */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 0U/*AVC idx*/, 1U)) {
        /* recon_dram_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* sh_dram_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* hcb_dram_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* fbp_id */
        pBaseParam->FbpNum += 1U;

        /* MAX_DEC_FBUFS_IN_DRAM */
        pBaseParam->FbNum += ((UINT16)16U*pBaseCfg->DecNum);

        /* fbp_imginfo */
        pBaseParam->ImgInfNum += pBaseCfg->DecNum;

        /* cpb_id */
        pBaseParam->CBufNum += pBaseCfg->DecNum;

        /* hcb_id */
        pBaseParam->CBufNum += pBaseCfg->DecNum;
    }

    /* Row[53] - setupHevcDecoder */
    if (1U == DSP_GetU8Bit(pBaseCfg->DecFmt, 1U/*HEVC idx*/, 1U)) {
        /* sh_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* db_recon_cmdmsg_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* hdr_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* hcb_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* fb_colo_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* dram_mvedge_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;

        /* dpb_store_par_id */
        pBaseParam->DramParNum += pBaseCfg->DecNum;
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
        AmbaLL_LogUInt5("  VIN   : Num[%d %d %d] Mask[0x%X 0x%X]", pBaseCfg->VinNum, pBaseCfg->VirtVinNum,
                                                                  pBaseCfg->MaxVinPinNum, pBaseCfg->VinBitMask,
                                                                  pBaseCfg->VinCeBitMask);
        for (i=0U; i<DSP_VIN_MAX_NUM; i++) {
            if (1U == DSP_GetBit(pBaseCfg->VinBitMask, i, 1U)) {
                AmbaLL_LogUInt5("          [%d]FrmNum[%d]", i, pBaseCfg->FrmBufNum[i], 0U, 0U, 0U);
            }
        }
        AmbaLL_LogUInt5("          Mask[0x%X]", pBaseCfg->VinYuvBitMask, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  VOUT  : Mask[0x%X]", pBaseCfg->VoutBitMask, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  VPROC : Res[%d %d %d %d %d]", pBaseCfg->VprocResHdr, pBaseCfg->VprocResLndt,
                                                      pBaseCfg->VprocResTileMode, pBaseCfg->VprocResEfct,
                                                      pBaseCfg->VprocResHiso);
        AmbaLL_LogUInt5("          ExtMemMode[%d]", pBaseCfg->VprocExtMemAllocMode, 0U, 0U, 0U, 0U);
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
        AmbaLL_LogUInt5("          Mask[0x%X 0x%X 0x%X]", pBaseCfg->VprocCompOutBitMask, pBaseCfg->VprocHdrBitMask,
                                                          pBaseCfg->VprocMipiYuyvBitMask, 0U, 0U);
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
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
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
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
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
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
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
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

        /* Tab:Vproc */
        HL_PrepareBaseParamVproc(pBaseCfg, pBaseParam);
        HL_PrepareBaseParamVprocEfct(pBaseCfg, pBaseParam);
        HL_PrepareBaseParamVprocNum(pBaseCfg, pBaseParam);
        HL_PrepareBaseParamVprocNumPrev(pBaseCfg, pBaseParam);
        HL_PrepareBaseParamVprocHiso(pBaseCfg, pBaseParam);
#ifdef DEBUG_PARAM_CALC
        AmbaLL_LogUInt5("BaseParam[OUT]:VPROC", 0U, 0U, 0U, 0U, 0U);
        AmbaLL_LogUInt5("  [%d %d %d %d %d]", pBaseParam->AikParNum, pBaseParam->DramParNum,
                                              pBaseParam->SmemParNum, pBaseParam->SubParNum,
                                              pBaseParam->SupParNum);
        AmbaLL_LogUInt5("  [%d %d %d] [%d %d]", pBaseParam->FbpNum, pBaseParam->FbNum,
                                                pBaseParam->ExtFbNum, pBaseParam->MFbpNum,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
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
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
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
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
        AmbaLL_LogUInt5("  [%d %d] [%d %d] [%d]", pBaseParam->BdtNum, pBaseParam->BdNum,
                                                  pBaseParam->DbpNum, pBaseParam->CBufNum,
                                                  pBaseParam->ExtMFbNum);
        AmbaLL_LogUInt5("  [%d %d] [%d]", pBaseParam->ImgInfNum, pBaseParam->MImgInfNum,
                                          pBaseParam->OrccodeMsgNum, 0U, 0U);
#endif

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
        AmbaLL_LogUInt5("  [%d %d] [%d %d %d]", pBaseParam->McblNum, pBaseParam->McbNum,
                                                pBaseParam->MbufParSize, pBaseParam->MbufSize,
                                                pBaseParam->MFbSize);
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
                WorkSize = ((((UINT32)pDramCfg->MaxComm0Width*3U)/2U)*pDramCfg->MaxComm0Height)*pDramCfg->MaxVprocEfctComm0FrmBufNum;
            } else {
                WorkSize = (((UINT32)pDramCfg->MaxComm0Width*2U)*pDramCfg->MaxComm0Height)*pDramCfg->MaxVprocEfctComm0FrmBufNum;
            }
            *pDspWork += WorkSize;
        }
        /* Row[63] - vproc_prev_fbp_id[1] */
        if ((pDramCfg->MaxVprocEfctComm1FrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctComm1FrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            if (pDramCfg->VprocComm1Fmt == DSP_YUV_420) {
                WorkSize = ((((UINT32)pDramCfg->MaxComm1Width*3U)/2U)*pDramCfg->MaxComm1Height)*pDramCfg->MaxVprocEfctComm1FrmBufNum;
            } else {
                WorkSize = (((UINT32)pDramCfg->MaxComm1Width*2U)*pDramCfg->MaxComm1Height)*pDramCfg->MaxVprocEfctComm1FrmBufNum;
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
            WorkSize = ((((UINT32)pDramCfg->MaxEfctPipWidth*3U)/2U)*pDramCfg->MaxEfctPipHeight)*pDramCfg->MaxVprocEfctPipFrmBufNum;
            *pDspWork += WorkSize;
        }
        /* Row[76] - comm_pip_me01_fbp_id */
        if ((pDramCfg->MaxVprocEfctPipMeFrmBufNum > 0U) &&
            (pDramCfg->MaxVprocEfctPipMeFrmBufNum != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = (((UINT32)pDramCfg->MaxEfctPipWidth/4U)*((UINT32)pDramCfg->MaxEfctPipHeight/4U))*pDramCfg->MaxVprocEfctPipMeFrmBufNum;
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

    /* Row[151] - vproc_yuyv2y_sec6_fbp_init() */
    /* vproc_luma_main_out_fbp_id (for sec6 luma out, Y data) */
    if (1U == DSP_GetBit(pDramCfg->VprocMipiYuyvBitMask, i, 1U)) {
        if ((pDramCfg->VprocC2YFrmBufNum[i] > 0U) &&
            (pDramCfg->VprocC2YFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = (((UINT32)pDramCfg->MaxW0[i]*2U)*pDramCfg->MaxH0[i])*pDramCfg->VprocC2YFrmBufNum[i];
            *pDspWork += WorkSize;
        }
    }

    /* Row[152] - vproc_yuyv2y_sec6_fbp_init() */
    /* vproc_chroma_main_out_fbp_id (for sec7 luma out, UV data) */
    if (1U == DSP_GetBit(pDramCfg->VprocMipiYuyvBitMask, i, 1U)) {
        if ((pDramCfg->VprocC2YFrmBufNum[i] > 0U) &&
            (pDramCfg->VprocC2YFrmBufNum[i] != DSP_VPROC_FB_NUM_DISABLE)) {
            WorkSize = (((UINT32)pDramCfg->MaxW0[i]*2U)*pDramCfg->MaxH0[i])*pDramCfg->VprocC2YFrmBufNum[i];
            *pDspWork += WorkSize;
        }
    }

    /* Row[153] - vproc_yuyv2y_sec2_fbp_init() */
    /* vproc_raw_fbp_id (for sec2 luma out, YUYV data) */
    if (1U == DSP_GetBit(pDramCfg->VprocMipiYuyvBitMask, i, 1U)) {
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
            UINT32 EncH = ALIGN_NUM(pDramCfg->MaxEncHeight[i], 32U);
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
            AmbaLL_LogUInt5("  VPROC_COMM : Comm0[%d %d %d %d]", pDramCfg->MaxComm0Width, pDramCfg->MaxComm0Height,
                                                                 pDramCfg->MaxVprocEfctComm0FrmBufNum, pDramCfg->VprocComm0Fmt, 0U);
            AmbaLL_LogUInt5("               Comm1[%d %d %d %d]", pDramCfg->MaxComm1Width, pDramCfg->MaxComm1Height,
                                                                 pDramCfg->MaxVprocEfctComm1FrmBufNum, pDramCfg->VprocComm1Fmt, 0U);
            AmbaLL_LogUInt5("               Efct0[%d %d %d %d]", pDramCfg->MaxEfctMainWidth, pDramCfg->MaxEfctMainHeight,
                                                                 pDramCfg->MaxVprocEfctMainFrmBufNum, pDramCfg->MaxVprocEfctMainMeFrmBufNum, 0U);
            AmbaLL_LogUInt5("               Efct1[%d %d %d %d]", pDramCfg->MaxEfctPipWidth, pDramCfg->MaxEfctPipHeight,
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
                                                                               pDramCfg->MaxEncWidth[i], pDramCfg->MaxEncHeight[i], 0U);
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

