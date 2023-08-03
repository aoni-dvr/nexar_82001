/**
*  @file AmbaDSP_EncodeUtility.h
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

#ifndef AMBADSP_ENCODEUTILITY_H
#define AMBADSP_ENCODEUTILITY_H

#include "AmbaDSP_ContextUtility.h"

#define SUPPORT_EXT_MEM_NEW_APPEND
#ifdef SUPPORT_EXT_MEM_NEW_APPEND
#define DSP_FB_IDX_FOR_NEW2APPEND   (0xFFFFFFFEU) // Need this for New->Append->Append.. flow
#else
#define DSP_FB_IDX_FOR_NEW2APPEND   (0x0U)
#endif

extern UINT8 HL_IS_YUV420_INPUT(UINT32 VinDataFmt);
extern UINT8 HL_IS_YUV422_INPUT(UINT32 VinDataFmt);

typedef struct {
    UINT16 DramPar;
    UINT16 SmemPar;

    UINT16 SubPar;
    UINT16 SupPar;

    UINT16 Fbp;
    UINT16 Fb;

    UINT16 Dbp;
    UINT16 CBuf;

    UINT16 Bdt;
    UINT16 Bd;

    UINT16 ImgInf;
    UINT16 ExtFb;

    UINT16 Mcbl;
    UINT16 Mcb;

    UINT32 MbufPar;
    UINT32 Mbuf;
    UINT16 AikPar;
    UINT16 MFbp;

    UINT16 MFb;
    UINT16 MExtFb;

    UINT16 MImgInf;
    UINT16 PgSzLog2;

    UINT8 IdspSblk0;
    UINT8 IdspSblk;
    UINT16 Rsvdsblk;

    /* B[0]DramCache B[1]SmemCache */
    UINT32 FbpCache:4;
    UINT32 FbCache:4;
    UINT32 ImgInfCache:4;
    UINT32 MFbpCache:4;
    UINT32 MFbCache:4;
    UINT32 MImgInfCache:4;
    UINT32 OrcCodeMsgQ:8;

    UINT32 OrcAllMsgQ:8;
    UINT32 IdspTimeOut:16;
    UINT32 WarpDramOut:2;  /* 0:Auto 1:Dis 2:Enb */
    UINT32 DramOutRsvd:2;
    UINT32 C2YDramOut:2;   /* 0:Auto 1:Dis 2:Enb */
    UINT32 C2YBurstTile:2; /* 0:Auto 1:Dis 2:Enb */

    UINT32 AAAEnable:2; /* 0:Auto 1:Dis 2:Enb */
    UINT32 LiEnable:2; /* 0:Auto 1:Dis 2:Enb */
    UINT32 RawOverLap:8;
    UINT32 VinRep:5;
    UINT32 C2yFb:3;
    UINT32 TileDCrop:3; /* 0:Auto 1:Dis 2:Enb */
    UINT32 VinRepPortion:7; /* 0:Auto others: minus1 */
    UINT32 VoutDblCmd:2; /* 0:Auto 1:Dis 2:Enb */

    UINT32 ForbidIsoUpt; /* 16view, each view occupied two bits */
    UINT32 Enc512BXfer;

    UINT32 VprocIn:4;
    UINT32 MonoVdoEnc:2; /* 0:Auto 1:Dis 2:Enb */
    UINT32 idspDblBank:2; /* 0:Auto 1:Dis 2:Enb */
    UINT32 idspWinStrmNum:5; /* minus1 */
    UINT32 VprocRotate:2; /* 0:Auto 1:Dis 2:Enb */
    UINT32 VinAutoDrop:2; /* 0:Auto 1:Dis 2:Enb */
    UINT32 VprocInRsvd:15;

    UINT32 C2YDramPri:3;
    UINT32 WarpDramPri:3;
    UINT32 MctfDramPri:3;
    UINT32 PrevDramPri:3;
    UINT32 C2YInDramPri:3;
    UINT32 Pri0xfer:2;
    UINT32 Pri1xfer:2;
    UINT32 Pri2xfer:2;
    UINT32 Pri3xfer:2;
    UINT32 Pri4xfer:2;
    UINT32 Pri5xfer:2;
    UINT32 Pri6xfer:2;
    UINT32 Pri7xfer:2;
    UINT32 DramPriRsvd:1;

#define SYS_CFG_IDSP_AFF_LEN    (2U)
    UINT32 MaxIdspAff; /* 16view, each view occupied two bits */
    UINT32 IdspAff;    /* 16view, each view occupied two bits */
#define SYS_CFG_VDSP_AFF_LEN    (2U)
    UINT32 MaxEncVdspAff; /* 16Strm, each Strm occupied two bits */
    UINT32 MaxEncVdspAffOpt; /* 16Strm, each Strm occupied two bits */
    UINT16 MaxDecVdspAff; /* 8Cfg, each Cfg occupied two bits */
    UINT16 MaxDecVdspAffOpt; /* 8Cfg, each Cfg occupied two bits */
    UINT32 EncVdspAff; /* 16Strm, each Strm occupied two bits */
    UINT32 EncVdspAffOpt; /* 16Strm, each Strm occupied two bits */
    UINT16 DecVdspAff; /* 8Strm, each Strm occupied two bits */
    UINT16 DecVdspAffOpt; /* 8Strm, each Strm occupied two bits */
    UINT16 MaxDecVdspEngAff;
    UINT16 DecVdspEngAff; /* 8Strm, each Strm occupied two bits */
} HL_DSP_SYSTEM_CONFIG_s;
extern HL_DSP_SYSTEM_CONFIG_s TuneDspSystemCfg;

extern UINT32 HL_FillLiveviewSystemSetup(cmd_dsp_config_t *pSysSetup);
extern UINT32 HL_FillSystemHalSetup(cmd_dsp_hal_inf_t *pSysHalSetup);
extern UINT32 HL_FillSystemResourceEncodeSetup(cmd_dsp_enc_flow_max_cfg_t *pResEncSetup);
extern UINT32 HL_FillSystemResourceVprocSetup(cmd_dsp_vproc_flow_max_cfg_t *pResVprocSetup);
extern UINT32 HL_FillSystemResourceVinSetup(cmd_dsp_vin_flow_max_cfg_t *ResVinSetup, UINT16 VinId);
extern UINT32 HL_FillSystemResourceVoutSetup(cmd_dsp_vout_flow_max_cfg_t *pResVoutSetup, const CTX_RESOURCE_INFO_s *pResource);
extern UINT32 HL_FillFpBindSetup(cmd_binding_cfg_t *FpBindCfg, UINT8 BindPurpose, UINT8 IsBind, UINT16 SrcId, UINT16 SrcPinId, UINT16 StrmId);
extern UINT32 HL_FillVideoProcCfg(cmd_vproc_cfg_t *VidPrep, UINT16 ViewZoneId);
extern UINT32 HL_FillVideoProcSetup(cmd_vproc_setup_t *VprocSetup, UINT16 ViewZoneId);
extern UINT32 HL_FillVideoProcPyramidSetup(cmd_vproc_img_pyramid_setup_t *VprocPymdSetup, UINT16 ViewZoneId);
extern UINT32 HL_FillVideoProcLnDtSetup(cmd_vproc_lane_det_setup_t *LnDtSetup, UINT16 ViewZoneId);
extern UINT32 HL_FillVideoProcSideBandConfig(cmd_vin_attach_sideband_info_to_cap_frm_t *VinAttachSideband, UINT16 ViewZoneId);
extern UINT32 HL_FillVideoProcGrouping(cmd_vproc_set_vproc_grping *VprocGrping);
#ifndef SUPPORT_VPROC_GROUPING
extern UINT32 HL_FillVideoProcChanProcOrder(cmd_vproc_multi_chan_proc_order_t *ChanProcOrder);
#endif
extern UINT32 HL_FillVideoPreviewSetup(cmd_vproc_prev_setup_t *PrevSetup,
                                       UINT16 ViewZoneId,
                                       const UINT8 PrevId,
                                       UINT8 *CmdByPass,
                                       const CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout);
extern UINT32 HL_FillVideoProcDecimation(const UINT16 ViewZoneId,
                                         const UINT16 YuvStrmIdx,
                                         cmd_vproc_pin_out_deci_t *VprocDeci);
extern UINT32 HL_FillIsoCfgUpdate(UINT16 ViewZoneId, cmd_vproc_ik_config_t *IsoCfgUpdate);
extern UINT32 HL_FillVinConfig(set_vin_config_t *VinCfg, UINT16 VinId);
extern UINT32 HL_FillVinStartCfg(cmd_vin_start_t *VinStartCfg,
                                 UINT16 VinId,
                                 UINT8 Purpose,
                                 UINT16 CfgPtn,
                                 const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg);

/**
* Fill VinHDR setup cmd
* @param [in]  VinId Vin index
* @param [in/out]  HdrSetup hdr setup cmd
* @param [in]  CfgAddr dedicated IsoCfg address to be used
* @param [out]  SkipCmd 1 - Skip this cmd without sending, SkipCmd = 0xFF means force filling setup
* @return ErrorCode
*/
extern UINT32 HL_FillVinHdrSetup(UINT16 VinId, cmd_vin_hdr_setup_t *HdrSetup, ULONG CfgAddr, UINT8 *SkipCmd);

/**
* Fill Vin CE setup cmd
* @param [in]  VinId Vin index
* @param [in/out]  CeSetup CE setup cmd
* @param [out]  SkipCmd 1 - Skip this cmd without sending, SkipCmd = 0xFF means force filling setup
* @return ErrorCode
*/
extern UINT32 HL_FillVinCeSetup(UINT16 VinId, cmd_vin_ce_setup_t *CeSetup, UINT8 *SkipCmd);

/**
* Fill VinSetfovLayout cmd
* @param [in/out]  VinSetFovLayoutCfg Set fov layout cmd content
* @param [in]  ViewZoneId
* @param [in]  VinROI contains ViewZone's ROI on its source Vin
* @return ErrorCode
*/
extern UINT32 HL_FillVinSetFovLayout(cmd_vin_set_fov_layout_t *pSetFovLayout, UINT16 ViewZoneId, const AMBA_DSP_WINDOW_s* VinROI);

/**
* Fill Vin HDS compression cmd
* @param [in]  VinId Vin index
* @param [in/out]  pHdsCmpr hds compression cmd
* @return ErrorCode
*/
extern UINT32 HL_FillVinHdsCmpr(const UINT16 VinId,
                                const UINT8 Purpose,
                                const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg,
                                lossy_compression_t *pHdsCmpr,
                                UINT8 *SkipCmd);

/**
* Fill PpStrm Cfg
* @param [in]  YuvStrmIdx Yuv stream index
* @param [in/out]  PpStrmCfg PpStream config
* @param [in/out]  pExtStrmLayout external stream layout
* @return ErrorCode
*/
extern UINT32 HL_FillPpStrmCfg(UINT16 YuvStrmIdx, cmd_vproc_multi_stream_pp_t *PpStrmCfg, CTX_YUV_STRM_LAYOUT_s *pExtStrmLayout);
extern void HL_FillPpBufImgSz(UINT16 YuvStrmIdx,
                              cmd_vproc_set_effect_buf_img_sz_t *pVprocEfctBuf);
extern UINT32 HL_FillPpStrmCfgOrder(UINT16 YuvStrmIdx,
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
extern UINT32 HL_FillSyncJobIntoBatch(UINT16 YuvStrmIdx,
                                      CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout,
                                      UINT16 ViewZoneId,
                                      UINT16 LastViewZoneId,
                                      CTX_EFCT_SYNC_JOB_SET_s *pSyncJobSet,
                                      const CTX_YUV_STRM_LAYOUT_s *pSyncJobSetLayout,
                                      UINT32 *pBatchQAddr,
                                      UINT16 BatchCmdIdx);

/**
* Fill OSD Setup Cfg
* @param [in]  VoutIdx Vout index
* @param [in]  pOsdBufCfg OSD buffer config
* @param [in/out]  pOsdSetup Osd Cmd config
* @return ErrorCode
*/
extern UINT32 HL_FillOsdSetup(UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pOsdBufCfg, cmd_vout_osd_setup_t *pOsdSetup);

extern UINT32 HL_VideoProcBinding(void);

extern void HL_CheckVinInfoUpdate(CTX_VIN_INFO_s *pNewInfo, const CTX_VIN_INFO_s *pOldInfo);
extern void HL_CheckVinInfoUpdatePush(CTX_VIN_INFO_s *pInfo);
extern void HL_CheckViewZoneInfoUpdatePush(CTX_VIEWZONE_INFO_s *pInfo);
extern void HL_CheckViewZoneInfoUpdate(CTX_VIEWZONE_INFO_s *pInfo);
extern void HL_UpdateVprocChanProcOrder(UINT8 GrpID,
                                        UINT32 DisableViewZoneBitMask,
                                        cmd_vproc_multi_chan_proc_order_t* pChanProcOrder,
                                        UINT8 DisableVproc,
                                        UINT16 *ChanIdxArray);
extern void HL_UpdateVprocChanProcOrderTout(const UINT8 GrpID,
                                            const UINT16 NewMasterViewZoneId,
                                            const UINT16 NumVproc,
                                            cmd_vproc_multi_chan_proc_order_t* pChanProcOrder);

extern UINT32 HL_ReqTimeLapseId(UINT16 StrmId, UINT16 *TimeLapseId);
extern UINT32 HL_FreeTimeLapseId(UINT16 TimeLapseId);

extern UINT32 HL_LvVprocSetupOnPreview(const UINT8 WriteMode,
                                       UINT16 ViewZoneId);
extern UINT32 HL_LiveviewFillVinExtMem(const UINT32 VinId,
                                       const UINT32 IsYuvVin2Enc,
                                       const UINT8 IsFirstMem,
                                       const UINT8 MemType,
                                       const UINT32 ChromaFmt,
                                       const UINT32 OverFlowCtrl,
                                       const AMBA_DSP_BUF_s *pBuf,
                                       const AMBA_DSP_BUF_s *pAuxBuf,
                                       cmd_vin_set_ext_mem_t *pVinExtMem);

extern void HL_SetDspSystemCfg(UINT8 ParIdx, UINT32 Val);

extern void HL_FillVinInitCapInRawCap(cmd_vin_initiate_raw_cap_to_ext_buf_t *pVinInitRawCap,
                                      const CTX_DATACAP_INFO_s *pDataCapInfo,
                                      const CTX_VIN_INFO_s *pVinInfo,
                                      const CTX_VIN_INFO_s *pMasterVinInfo,
                                      const UINT8 ExpId);
extern UINT8 HL_FetchFirstViewZoneInfo(const UINT16 VinIdx,
                                       UINT16 *pViewZoneId,
                                       CTX_VIEWZONE_INFO_s **pViewZoneInfo);

#endif //AMBADSP_ENCODEUTILITY_H
