/**
 *  @file AmbaDSPSimilar_Context.h
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
 *  @details Definitions & constants for the APIs to generate DSP commands
 *
 */
#ifndef AMBA_DSP_SIMILAR_CONTEXT_H
#define AMBA_DSP_SIMILAR_CONTEXT_H

#include "dsp_osal.h"
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_ContextUtility.h"

extern void SIM_AcqCmdBuffer(UINT8 *Id, void **BufferAddr) GNU_WEAK_SYMBOL;
extern void SIM_RelCmdBuffer(const UINT8 Id) GNU_WEAK_SYMBOL;

extern cmd_dsp_config_t *SIM_DefCtxCmdBufPtrSys GNU_WEAK_SYMBOL;
extern cmd_dsp_hal_inf_t *SIM_DefCtxCmdBufPtrHal GNU_WEAK_SYMBOL;
extern cmd_binding_cfg_t *SIM_DefCtxCmdBufPtrBind GNU_WEAK_SYMBOL;
extern cmd_dsp_vproc_flow_max_cfg_t *SIM_DefCtxCmdBufPtrVpcMax GNU_WEAK_SYMBOL;
extern cmd_dsp_vin_flow_max_cfg_t *SIM_DefCtxCmdBufPtrVinMax GNU_WEAK_SYMBOL;
extern cmd_dsp_enc_flow_max_cfg_t *SIM_DefCtxCmdBufPtrEncMax GNU_WEAK_SYMBOL;
extern cmd_dsp_dec_flow_max_cfg_t *SIM_DefCtxCmdBufPtrDecMax GNU_WEAK_SYMBOL;

extern cmd_vproc_cfg_t *SIM_DefCtxCmdBufPtrVpc GNU_WEAK_SYMBOL;
extern cmd_vproc_setup_t *SIM_DefCtxCmdBufPtrVpcSetup GNU_WEAK_SYMBOL;
//extern cmd_vproc_ik_config_t *SIM_DefCtxCmdBufPtrVpcIkCfg GNU_WEAK_SYMBOL;
extern cmd_vproc_img_pyramid_setup_t *SIM_DefCtxCmdBufPtrVpcPymd GNU_WEAK_SYMBOL;
extern cmd_vproc_prev_setup_t *SIM_DefCtxCmdBufPtrVpcPrev GNU_WEAK_SYMBOL;
extern cmd_vproc_lane_det_setup_t *SIM_DefCtxCmdBufPtrVpcLndt GNU_WEAK_SYMBOL;
extern cmd_vproc_set_vproc_grping *SIM_DefCtxCmdBufPtrVpcGrp GNU_WEAK_SYMBOL;
extern cmd_vproc_multi_chan_proc_order_t *SIM_DefCtxCmdBufPtrVpcChOrd GNU_WEAK_SYMBOL;
extern cmd_vproc_pin_out_deci_t *SIM_DefCtxCmdBufPtrVpcDeci GNU_WEAK_SYMBOL;

extern cmd_vin_start_t *SIM_DefCtxCmdBufPtrVinStart GNU_WEAK_SYMBOL;
extern cmd_vin_ce_setup_t *SIM_DefCtxCmdBufPtrVinCe GNU_WEAK_SYMBOL;
extern cmd_vin_hdr_setup_t *SIM_DefCtxCmdBufPtrVinHdr GNU_WEAK_SYMBOL;
extern cmd_vin_set_fov_layout_t *SIM_DefCtxCmdBufPtrVinLayout GNU_WEAK_SYMBOL;
extern cmd_vin_initiate_raw_cap_to_ext_buf_t *SIM_DefCtxCmdBufPtrVinInitCap GNU_WEAK_SYMBOL;
extern cmd_vin_attach_sideband_info_to_cap_frm_t *SIM_DefCtxCmdBufPtrVinSidBend GNU_WEAK_SYMBOL;

extern cmd_vout_osd_setup_t *SIM_DefCtxCmdBufPtrVoutOsd GNU_WEAK_SYMBOL;

extern set_vin_config_t *SIM_DefCtxCmdBufPtrVinCfg GNU_WEAK_SYMBOL;

typedef struct {
    /* BatchCmdQ */
    DSP_POOL_DESC_s BatchQPoolDesc[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT16 MaxVprocTileWidth;
}CTX_SIMILAR_INFO_s;

extern UINT32 HL_SimilarCtxInit(void) GNU_WEAK_SYMBOL;
extern void HL_GetSimilarInfo(const UINT8 MtxOpt, CTX_SIMILAR_INFO_s *pInfo) GNU_WEAK_SYMBOL;
extern void HL_SetSimilarInfo(const UINT8 MtxOpt, const CTX_SIMILAR_INFO_s *pInfo) GNU_WEAK_SYMBOL;
extern void SIM_UpdateCtxViewZoneBatchQWPtr(const UINT32 ViewZoneId) GNU_WEAK_SYMBOL;
extern void SIM_UpdateCtxViewZoneBatchQRPtr(const UINT32 BatchCmdId) GNU_WEAK_SYMBOL;
extern void SIM_ResetViewZoneInfo(void) GNU_WEAK_SYMBOL;
extern void SIM_CtxLvReset(void) GNU_WEAK_SYMBOL;
extern UINT8 IsSimilarEnabled(void) GNU_WEAK_SYMBOL;
extern UINT32 SIM_CalcCheckSum32Add(const UINT32 *pBuffer, UINT32 Size, UINT32 CrcIn) GNU_WEAK_SYMBOL;

#endif  /* AMBA_DSP_SIMILAR_CONTEXT_H */
