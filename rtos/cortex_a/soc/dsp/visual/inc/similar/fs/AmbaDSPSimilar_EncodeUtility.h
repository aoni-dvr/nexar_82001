/**
*  @file AmbaDSPSimilar_EncodeUtility.h
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
*  @details Definitions & Constants for the APIs of SSP Encoder
*
*/

#ifndef AMBADSP_SIMILAR_ENCODEUTILITY_H
#define AMBADSP_SIMILAR_ENCODEUTILITY_H

#include "AmbaDSP_ContextUtility.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSPSimilar_Context.h"

#define SUPPORT_EXT_MEM_NEW_APPEND
#ifdef SUPPORT_EXT_MEM_NEW_APPEND
#define DSP_FB_IDX_FOR_NEW2APPEND   (0xFFFFFFFEU) // Need this for New->Append->Append.. flow
#else
#define DSP_FB_IDX_FOR_NEW2APPEND   (0x0U)
#endif

extern UINT8 SIMILARHL_IS_YUV420_INPUT(UINT32 VinDataFmt) GNU_WEAK_SYMBOL;
extern UINT8 SIMILARHL_IS_YUV422_INPUT(UINT32 VinDataFmt) GNU_WEAK_SYMBOL;

extern UINT32 SIM_FillLiveviewSystemSetup(cmd_dsp_config_t *SysSetup) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillSystemHalSetup(cmd_dsp_hal_inf_t *SysHalSetup) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillSystemResourceEncodeSetup(cmd_dsp_enc_flow_max_cfg_t *ResEncSetup) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillSystemResourceVprocSetup(cmd_dsp_vproc_flow_max_cfg_t *ResVprocSetup) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillSystemResourceVinSetup(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup, UINT16 VinId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillFpBindSetup(cmd_binding_cfg_t *FpBindCfg,
                                        UINT8 BindPurpose,
                                        UINT8 IsBind,
                                        UINT16 SrcId,
                                        UINT16 SrcPinId,
                                        UINT16 StrmId) GNU_WEAK_SYMBOL;

extern UINT32 SIM_FillVideoProcCfg(cmd_vproc_cfg_t *VidPrep, UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVideoProcSetup(cmd_vproc_setup_t *VprocSetup, UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVideoProcPyramidSetup(cmd_vproc_img_pyramid_setup_t *VprocPymdSetup, UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVideoProcLnDtSetup(cmd_vproc_lane_det_setup_t *LnDtSetup, UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVideoProcSideBandConfig(cmd_vin_attach_sideband_info_to_cap_frm_t *VinAttachSideband, UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVideoProcGrouping(cmd_vproc_set_vproc_grping *VprocGrping) GNU_WEAK_SYMBOL;
#ifndef SUPPORT_VPROC_GROUPING
extern UINT32 SIM_FillVideoProcChanProcOrder(cmd_vproc_multi_chan_proc_order_t *ChanProcOrder) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_FillVideoPreviewSetup(cmd_vproc_prev_setup_t *PrevSetup,
                                              UINT16 ViewZoneId,
                                              const UINT8 PrevId,
                                              UINT8 *CmdByPass,
                                              const CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout) GNU_WEAK_SYMBOL;

extern UINT32 SIM_FillVideoProcDecimation(const UINT16 ViewZoneId,
                                                const UINT16 YuvStrmIdx,
                                                cmd_vproc_pin_out_deci_t *VprocDeci) GNU_WEAK_SYMBOL;

extern UINT32 SIM_FillIsoCfgUpdate(UINT16 ViewZoneId, cmd_vproc_ik_config_t *IsoCfgUpdate) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVinConfig(set_vin_config_t *VinCfg, UINT16 VinId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVinStartCfg(cmd_vin_start_t *VinStartCfg,
                                        UINT16 VinId,
                                        UINT8 Purpose,
                                        UINT16 CfgPtn,
                                        const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg) GNU_WEAK_SYMBOL;


/**
* Fill VinHDR setup cmd
* @param [in]  VinId Vin index
* @param [in/out]  HdrSetup hdr setup cmd
* @param [in]  CfgAddr dedicated IsoCfg address to be used
* @param [out]  SkipCmd 1 - Skip this cmd without sending, SkipCmd = 0xFF means force filling setup
* @return ErrorCode
*/
extern UINT32 SIM_FillVinHdrSetup(UINT16 VinId, cmd_vin_hdr_setup_t *HdrSetup, ULONG CfgAddr, UINT8 *SkipCmd) GNU_WEAK_SYMBOL;

/**
* Fill Vin CE setup cmd
* @param [in]  VinId Vin index
* @param [in/out]  CeSetup CE setup cmd
* @param [out]  SkipCmd 1 - Skip this cmd without sending, SkipCmd = 0xFF means force filling setup
* @return ErrorCode
*/
extern UINT32 SIM_FillVinCeSetup(UINT16 VinId, cmd_vin_ce_setup_t *CeSetup, UINT8 *SkipCmd) GNU_WEAK_SYMBOL;

/**
* Fill VinSetfovLayout cmd
* @param [in/out]  VinSetFovLayoutCfg Set fov layout cmd content
* @param [in]  ViewZoneId
* @param [in]  VinROI contains ViewZone's ROI on its source Vin
* @return ErrorCode
*/
extern UINT32 SIM_FillVinSetFovLayout(cmd_vin_set_fov_layout_t *SetFovLayout, UINT16 ViewZoneId, const AMBA_DSP_WINDOW_s* VinROI) GNU_WEAK_SYMBOL;

/**
* Fill PpStrm Cfg
* @param [in]  YuvStrmIdx Yuv stream index
* @param [in/out]  PpStrmCfg PpStream config
* @param [in/out]  pExtStrmLayout external stream layout
* @return ErrorCode
*/
#if 0
extern UINT32 SIM_FillPpStrmCfg(UINT16 YuvStrmIdx,
                                      cmd_vproc_multi_stream_pp_t *PpStrmCfg,
                                      CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout);

extern UINT32 SIM_FillPpStrmCfgOrder(UINT16 YuvStrmIdx,
                                           const UINT16 *pYuvIdxOrder,
                                           cmd_vproc_multi_stream_pp_t *PpStrmCfg,
                                           CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout);


/**
* Fill EffectSyncJob into BatchCmdQ
* @param [in]  YuvStrmIdx Yuv stream index
* @param [in]  pYuvStrmInfo Yuv stream info
* @param [in]  pYuvStrmLayout Yuv stream layout
* @param [in]  ViewZoneId ViewZoneId
* @param [in]  LastViewZoneId Last ViewZoneId
* @param [in]  pSyncJobSet Sync Job Set
* @param [in]  pBatchQAddr BatchQBuf address
* @return ErrorCode
*/
extern UINT32 SIM_FillSyncJobIntoBatch(UINT16 YuvStrmIdx,
                                             const CTX_YUV_STRM_INFO_s *pYuvStrmInfo,
                                             CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                             UINT16 ViewZoneId,
                                             UINT16 LastViewZoneId,
                                             CTX_EFCT_SYNC_JOB_SET_s *pSyncJobSet,
                                             UINT32 *pBatchQAddr,
                                             UINT16 BatchCmdIdx);
#endif
/**
* Fill OSD Setup Cfg
* @param [in]  VoutIdx Vout index
* @param [in]  pOsdBufCfg OSD buffer config
* @param [in/out]  pOsdSetup Osd Cmd config
* @return ErrorCode
*/
extern UINT32 SIM_FillOsdSetup(UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pOsdBufCfg, cmd_vout_osd_setup_t *OsdSetup) GNU_WEAK_SYMBOL;

extern void SIM_CheckVinInfoUpdate(UINT16 VinId, CTX_VIN_INFO_s *pNewInfo, const CTX_VIN_INFO_s *pOldInfo) GNU_WEAK_SYMBOL;
extern void SIM_CheckViewZoneInfoUpdate(CTX_VIEWZONE_INFO_s *pNewInfo, const CTX_VIEWZONE_INFO_s *pOldInfo) GNU_WEAK_SYMBOL;
#if 0
extern void SIM_UpdateVprocChanProcOrder(UINT8 GrpID,
                                        UINT32 DisableViewZoneBitMask,
                                        cmd_vproc_multi_chan_proc_order_t* pChanProcOrder,
                                        UINT8 DisableVproc,
                                        UINT16 *ChanIdxArray);
extern void SIM_UpdateVprocChanProcOrderTout(const UINT8 GrpID,
                                            const UINT16 NewMasterViewZoneId,
                                            const UINT16 NumVproc,
                                            cmd_vproc_multi_chan_proc_order_t* pChanProcOrder);

extern UINT32 SIM_ReqTimeLapseId(UINT16 StrmId, UINT16 *TimeLapseId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FreeTimeLapseId(UINT16 TimeLapseId) GNU_WEAK_SYMBOL;

extern UINT32 SIM_LiveviewFillVinExtMem(const UINT32 VinId,
                                       const UINT32 IsYuvVin2Enc,
                                       const UINT8 IsFirstMem,
                                       const UINT8 MemType,
                                       const UINT32 ChromaFmt,
                                       const UINT32 OverFlowCtrl,
                                       const AMBA_DSP_BUF_s *pBuf,
                                       const AMBA_DSP_BUF_s *pAuxBuf,
                                       cmd_vin_set_ext_mem_t *VinExtMem);
#endif
extern void SIM_SetDspSystemCfg(UINT8 ParIdx, UINT32 Val) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillVinInitCapInRawCap(cmd_vin_initiate_raw_cap_to_ext_buf_t *VinInitRawCap,
                                           const CTX_DATACAP_INFO_s *pDataCapInfo,
                                           const CTX_VIN_INFO_s *pVinInfo) GNU_WEAK_SYMBOL;

#endif //AMBADSP_SIMILAR_ENCODEUTILITY_H
