/**
*  @file AmbaDSPSimilar_EncodeAPI.h
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

#ifndef AMBADSP_SIMILAR_ENCODEAPI_H
#define AMBADSP_SIMILAR_ENCODEAPI_H

#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_CommonAPI.h"
#include "AmbaDSP_EncodeAPI.h"

extern UINT32 SIM_PollingEncodeTimerMode(UINT8 VinId, UINT8 WriteMode, UINT8 TimerScale, UINT32 WaitMs, UINT8 WaitType) GNU_WEAK_SYMBOL;
//extern UINT32 SIM_ConvVideoModeFlag(UINT32 PipeLine, UINT32 MctfEnable, UINT32 EnaOsdVoutMixer, UINT32 *pModeFlag);
//extern UINT32 SIM_FrameRateConvert(UINT32 Interlace, UINT32 Den, UINT32 Num);
extern UINT32 SIM_GetBatchCmdNumber(ULONG BatchAddr) GNU_WEAK_SYMBOL;
extern UINT32 SIM_VinPostCfgHandler(const UINT16 VinOI) GNU_WEAK_SYMBOL;
#if 0
extern void SIM_DisassembleVinCompression(UINT32 Compression, UINT8 *pCompressedMode, UINT8 *pCompactMode) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_GetCmprRawBufInfo(UINT16 Width,
                                   UINT8 CmprRate, UINT8 CmptMode,
                                   UINT16 *pRawWidth, UINT16 *pRawPitch,
                                   UINT16 *pMantissa, UINT16 *pBlkSz);
extern UINT32 SIM_GetViewZoneLocalIndexOnVin(UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneNumOnVin(UINT16 VinId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneVprocPinMaxWin(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *Width, UINT16 *Height) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_GetVprocPinMaxWidth(UINT16 PrevIdx, const AMBA_DSP_WINDOW_s *Input, const AMBA_DSP_WINDOW_s *Output, UINT16 *MaxWidth) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_CalcVprocGroupNum(void) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVprocGroupNum(UINT16 *Num) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVprocGroupFreeRun(UINT16 *pFreeRunBit) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVprocGroupIdx(UINT16 VprocId, UINT16 *pIdx, UINT8 IsStlProc) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZonePrevDest(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *Dest) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetPinOutDestBufNum(UINT16 Dest, UINT16 VprocPin, UINT16 *pNum, UINT16 *pAuxNum) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetPinOutDestChFmt(UINT16 Dest, UINT16 VprocPin, UINT8 *pFmt) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZonePinFbNum(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *pNum, UINT16 *pAuxNum, UINT8 IsExtFb) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneVinId(UINT16 ViewZoneId, UINT16* VinId) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_GetVinViewZoneId(UINT16 VinId, UINT16 *pViewZoneId) GNU_WEAK_SYMBOL;
extern void SIM_GetGroupAliveViewZoneId(UINT8 GrpID, UINT16 MasterVinId, UINT32 DisableViewZoneBitMask, UINT16 *pMasterViewZoneId, UINT16 *pNumVproc) GNU_WEAK_SYMBOL;
extern void SIM_GetGroupMasterVprocId(const UINT16 GrpId, UINT16 *pMasterVprocId) GNU_WEAK_SYMBOL;
extern void SIM_GetGroupFirstVprocId(const UINT16 GrpId, UINT16 *pFirstVprocId) GNU_WEAK_SYMBOL;
#endif
//extern void SIM_GetGroupVZMask(const UINT16 GrpId, UINT32 *pGroupVZMask);
extern UINT32 SIM_GetEncMaxWindow(UINT16 *Width, UINT16 *Height) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_GetLevelIdc(const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *StrmCfg, UINT8 *pLevelIdc, UINT8 *pTierIdc) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetHevcMaxTileNum(UINT8 *TileNum, UINT16 EncodeWidth) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_GetCommBufMaxWindow(UINT16 Dest, UINT16 PinId, UINT16 *pWidth, UINT16 *pHeight) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVoutTotalBit(void) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVoutRotateRescRow(UINT16 Height, UINT8 *pRow) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_GetEncFmtTotalBit(void) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_GetEffectEnableOnYuvStrm(const CTX_YUV_STRM_INFO_s *pYuvStrm) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetEffectChannelEnable(void) GNU_WEAK_SYMBOL;
extern UINT8 SIM_GetDefaultRawEnable(void) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetProcRawDramOutEnable(UINT8 *pEnable) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetEffectChannelPostBlendNum(UINT16 YuvStrmIdx, const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout, UINT16 *TotalPostBldNum) GNU_WEAK_SYMBOL;
#if 0
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
extern UINT32 SIM_GetEffectChannelVprocPin(UINT16 Purpose,
                                          UINT16 DestVout,
                                          UINT32 DestEnc,
                                          UINT16 *pCandNum,
                                          UINT16 *pCandPinId,
                                          UINT8 IsDramMipiYuv,
                                          UINT8 ChromaFmt);
#else
extern UINT32 SIM_GetEffectChannelVprocPin(UINT16 Purpose,
                                          UINT16 DestVout,
                                          UINT32 DestEnc,
                                          UINT16 *pPinId,
                                          UINT8 IsDramMipiYuv);
#endif
extern UINT32 SIM_CalcEffectPostPJob(UINT16 YuvStrmIdx, CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout) GNU_WEAK_SYMBOL;
extern UINT32 SIM_EffPostPReBase(UINT16 YuvStrmIdx, const UINT16 *pYuvIdxOrder, CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout) GNU_WEAK_SYMBOL;
//extern UINT32 SIM_FindOverlapeRegion(const AMBA_DSP_WINDOW_s *WinA, const AMBA_DSP_WINDOW_s *WinB, AMBA_DSP_WINDOW_s *WinOverlap, UINT16 *IsOverLap);
extern UINT32 SIM_ComposeEfctSyncJobId(DSP_EFCT_SYNC_JOB_ID_s *JobId, UINT16 YuvStrmIdx, UINT16 SeqIndx, UINT16 TypeBit, UINT16 SubJobIdx) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_VprocUpscaleCheck(UINT16 PrevIdx,
                                   const AMBA_DSP_WINDOW_s *In,
                                   const AMBA_DSP_WINDOW_s *Out,
                                   UINT8 *pCheckPass);
#if 0
extern UINT32 SIM_FindMinimalVprocPin(const AMBA_DSP_WINDOW_s *Output, const AMBA_DSP_WINDOW_s *ROI,
                                     UINT16 MaxTileNum, UINT16 VprocId, UINT16* VprocPin,
                                     UINT8 *pFound, UINT16 LowDlyPurpose, UINT8 IsEncSync, UINT8 IsDramMipiYuv);
#endif
extern UINT32 SIM_IsVirtualVinIdx(UINT16 VinId, UINT8 *IsVirtVin) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVirtualVinNum(UINT16 *Num) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetTimeDivisionVirtVinInfo(UINT16 VinId, UINT16 *PhysicalVin) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVprocNum(UINT16 *Num) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVprocMipiYuyvNumber(UINT32 ViewZoneOI, UINT16 *YuyvNumber) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVinMipiYuyvInfo(UINT16 VinId, UINT16 *YuyvEnable) GNU_WEAK_SYMBOL;
//#define DEBUG_TILE_CALC
extern UINT32 SIM_GetViewZoneWarpOverlap(const UINT16 ViewZoneId, UINT16 *pOverLapX, UINT16 *pOverLapY) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneWarpOverlapX(const UINT16 ViewZoneId, UINT16 *pOverLapX) GNU_WEAK_SYMBOL;
//extern UINT32 SIM_GetViewZoneWarpOverlapXMax(const UINT16 ViewZoneId, UINT16 *pOverLapX);
extern UINT32 SIM_GetSystemWarpWarpOverlap(UINT16 *WarpOverLap) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneRawOverlap(const UINT16 ViewZoneId, UINT16 *pOverLapX) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetLuma12Info(UINT8 *pMainY12Enable, UINT8 *pHierY12Enable) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_GetDramMipiYuvEnable(void) GNU_WEAK_SYMBOL;
#endif
#define SLICE_MODE_HOR_IDX  (0x0U)  // for smaller width
#define SLICE_MODE_VER_IDX  (0x1U)  // for LowDelay
extern UINT32 SIM_IsSliceMode(UINT8 *IsSliceMode) GNU_WEAK_SYMBOL;
extern UINT32 SIM_IsTileMode(UINT8 *IsTileMode) GNU_WEAK_SYMBOL;
extern UINT32 SIM_FillSliceCapLine(UINT16 ViewZoneId, UINT16 *pCapLineCfg, UINT16 IntcNumMinusOne) GNU_WEAK_SYMBOL;
#ifdef SUPPORT_DSP_LDY_SLICE
extern UINT32 SIM_FillSliceLayoutCfg(UINT16 ColNum, UINT16 RowNum, UINT16 Width, UINT16 Height, DSP_SLICE_LAYOUT_s *pSliceLayout) GNU_WEAK_SYMBOL;
#endif
#define RECON_POST_2_VPROC_BIT  (0U)
extern UINT32 SIM_GetReconPostStatus(UINT16 StrmIdx, UINT16 *Status) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_ComposeStlProcJobId(DSP_STL_PROC_JOB_ID_s *JobId, UINT16 DatFmt, UINT16 OutputPinMask, UINT16 IsExtMem, UINT16 VprocId) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_GetSystemVinMaxViewZoneNum(UINT16 VinId) GNU_WEAK_SYMBOL;
extern void SIM_GetVinExistence(UINT16 VinId, UINT16 *Exist) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemVinMaxWindow(UINT16 *Width, UINT16 *Height) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemVprocPinMaxWindow(UINT8 PinType, UINT16 *Width, UINT16 *Height) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_GetVinAttachEventDefaultMask(UINT16 VinId, UINT32 *pMask) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_GetSystemPymdMaxWindow(UINT16 *Width, UINT16 *Height) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZonePymdMaxWindow(UINT16 *Width, UINT16 *Height, UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemLndtMaxWindow(UINT16 *Width, UINT16 *Height) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemHdrSetting(UINT8 *MaxHdrExpNumMiunsOne, UINT8 *LinearCeEnable, UINT16 *MaxHdrBlendHeight) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemHdrSettingOnVin(UINT16 VinId, UINT8 *MaxHdrExpNumMiunsOne, UINT8 *LinearCeEnable, UINT16 *MaxHdrBlendHeight) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemMctfSetting(UINT8 *pMctfEnable, UINT8 *pMctsEnable, UINT8 *pMctfCmpr) GNU_WEAK_SYMBOL;
extern void SIM_GetWarpChromaDma(UINT16 LumaDmaSize, UINT16 *pChromaDmaSize) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemWarpDmaSetting(UINT16 *pLumaDmaSize, UINT16 *pChromaDmaSize) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSystemWarpWaitLineSetting(UINT16 *pLumaWaitLine, UINT16 *pChromaWaitLine) GNU_WEAK_SYMBOL;
extern UINT32 SIM_CalcVideoTile(UINT16 Width, UINT16 TileWidth, UINT16 *TileNum, UINT16 Overlap) GNU_WEAK_SYMBOL;
extern UINT32 SIM_CalcVideoTileC2Y(UINT16 ViewZoneId,
                                  UINT16 Width,
                                  UINT16 MaxTileWidth,
                                  UINT32 TileWidthExp,
                                  UINT8 FixedOverlap,
                                  UINT8 ChkSmem,
                                  UINT16 *TileNum,
                                  UINT16 *Overlap);
extern UINT32 SIM_CalcVideoTileWidth(UINT16 Width,
                                    UINT16 TileNum,
                                    UINT16 *TileWidth);
extern UINT32 SIM_CalcVideoTileHeight(UINT16 Height, UINT16 TileNum, UINT16 *TileHeight) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneMctfEnable(UINT16 ViewZoneId, UINT8 *pMctfEnable, UINT8 *pMctsEnable, UINT8 *pMctsOutEnable, UINT8 *pMctfCmpr) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetEncMaxTimeLapseWindow(UINT16 *Width, UINT16 *Height) GNU_WEAK_SYMBOL;
#if 0
extern UINT32 SIM_GetEncGrpCmprStroSetting(UINT16 StrmIdx, UINT16 *pCsLsb, UINT16 *pCsResIdx, ULONG *pCsCfgULAddr) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVinNum(UINT16 *Num) GNU_WEAK_SYMBOL;
#endif
extern UINT32 SIM_CheckPrevROI(AMBA_DSP_WINDOW_s ROI, AMBA_DSP_WINDOW_s Main) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetDramToWarpEnable(void) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetSmemToWarpEnable(void) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneDramToWarpEnable(UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetViewZoneSmemToWarpEnable(UINT16 ViewZoneId) GNU_WEAK_SYMBOL;
extern UINT8  SIM_GetYuvStrmIsEffectOut(UINT16 MaxChanNum, UINT16 WindowOffsetX, UINT16 WindowOffsetY, UINT8 RotateFlip) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetDecIdxFromVinId(UINT16 VinId, UINT16 *pIdx) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetTestEncHeader(test_binary_header_t *pEncHdr, const CTX_RESOURCE_INFO_s *pResource) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetTestDecHeader(dec_test_bin_inf_t *pDecHdr, const CTX_RESOURCE_INFO_s *pResource) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetOctaveSize(UINT8 OctaveMode, UINT16 Input, UINT16 *pOutput) GNU_WEAK_SYMBOL;

extern UINT32 SIM_GetVinWaitFlagTotalMask(UINT32 *pMask) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVoutWaitFlagTotalMask(UINT32 *pMask) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVinOutputPinNum(UINT16 VinId, UINT16 VirtChanId, const CTX_RESOURCE_INFO_s *pResource, UINT8 *pPinNum) GNU_WEAK_SYMBOL;
extern UINT8  SIM_GetVin2CmdNormalWrite(UINT16 VinId) GNU_WEAK_SYMBOL;
extern UINT8  SIM_GetVin2CmdGroupWrite(UINT16 VinId) GNU_WEAK_SYMBOL;
extern UINT32 SIM_GetVinSecCfgSize(UINT16 VinId) GNU_WEAK_SYMBOL;

#ifndef SUPPORT_MAX_UCODE
extern UINT32 SIM_CalHierWindow(UINT8 OctaveMode, UINT8 HierIdx,
                               const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                               UINT16 *pHierWidth, UINT16 *pHierHeight);
#endif

#endif //AMBADSP_SIMILAR_ENCODEAPI_H
