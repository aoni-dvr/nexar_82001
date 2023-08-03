/**
*  @file AmbaDSP_EncodeAPI.h
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

#ifndef AMBADSP_ENCODEAPI_H
#define AMBADSP_ENCODEAPI_H

#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_CommonAPI.h"
#include "AmbaDSP_EncodeAPI_Def.h"

typedef struct {
    UINT16 Den;
    UINT16 Num;
    UINT16 Mantissa;
    UINT16 Block;
} RAW_BIT_RATE_t;

typedef struct {
    UINT16 Mantissa;
    UINT16 Block;
} RAW_CMPT_RATE_t;

//extern const UINT16 HL_MaxVprocPinOutputWidthMap[DSP_VPROC_PIN_NUM];
//extern const UINT16 HL_MaxVprocPinOutput2XWidthMap[DSP_VPROC_PIN_NUM];
extern const UINT16 HL_MaxVprocPinOutputRatioMap[DSP_VPROC_PIN_NUM];
//extern const UINT8 HL_VprocPinUpscaleCapabilityMap[DSP_VPROC_PIN_NUM];
extern const UINT16 HL_CtxVprocPinDspPrevMap[DSP_VPROC_PIN_MAIN];
extern const UINT16 HL_DspPrevCtxVprocPinMap[DSP_VPROC_PREV_D];
extern const UINT16 HL_CtxVprocPinDspPinMap[DSP_VPROC_PIN_NUM];
//extern const UINT16 HL_VprocMemTypeVprocPinMap[MAX_VPROC_EXT_MEM_TYPE_NUM];
//extern const UINT16 HL_VprocPinVprocMemBitMap[DSP_VPROC_PIN_NUM];
extern const UINT16 HL_VprocMemBitMemTypeMap[DSP_BIT_POS_EXT_NUM];
extern const UINT16 HL_VprocPinVprocMemTypePoolMap[DSP_VPROC_PIN_NUM];
extern const UINT16 HL_VprocPinVprocMemMeTypeMap[DSP_VPROC_PIN_NUM];
extern const UINT16 HL_VprocPinVinAttachBitMap[DSP_VPROC_PIN_NUM];
extern const UINT16 HL_VprocPinVinAttachMeBitMap[DSP_VPROC_PIN_NUM];
extern const UINT16 HL_VprocPinPrevSyncMap[DSP_VPROC_PIN_NUM];
//extern const RAW_BIT_RATE_t RawBitRateTable[RAW_COMPRESS_NUM];
extern const RAW_CMPT_RATE_t RawCmptRateTable[RAW_COMPACT_NUM];
//extern const UINT8 RawCmptBitLengthTable[RAW_COMPACT_NUM];
extern const UINT16 HL_HierUserIdx2VprocIdx[AMBA_DSP_MAX_HIER_NUM];
extern const UINT8 HL_RotateFlipMap[AMBA_DSP_ROTATE_NUM];
extern const UINT16 HL_Hier2ShiftNumSqrt[AMBA_DSP_MAX_HIER_NUM];
extern const UINT8 YuvFmtTable[3U];
extern const UINT8 HL_DecCodec2DspDecCodec[AMBA_DSP_DEC_BITS_FORMAT_NUM];
extern UINT32 HL_PollingEncodeTimerMode(UINT8 VinId, UINT8 WriteMode, UINT8 TimerScale, UINT32 WaitMs, UINT8 WaitType);
//extern UINT32 HL_ConvVideoModeFlag(UINT32 PipeLine, UINT32 MctfEnable, UINT32 EnaOsdVoutMixer, UINT32 *pModeFlag);
//extern UINT32 HL_FrameRateConvert(UINT32 Interlace, UINT32 Den, UINT32 Num);
extern UINT32 HL_GetBatchCmdNumber(ULONG BatchAddr);
extern UINT32 HL_VinPostCfgHandler(const UINT16 VinOI);
extern void HL_DisassembleVinCompression(UINT32 Compression, UINT8 *pCompressedMode, UINT8 *pCompactMode);
extern UINT32 HL_GetCmprRawBufInfo(UINT16 Width,
                                   UINT8 CmprRate, UINT8 CmptMode,
                                   UINT16 *pRawWidth, UINT16 *pRawPitch,
                                   UINT16 *pMantissa, UINT16 *pBlkSz);
extern UINT32 HL_GetViewZoneLocalIndexOnVin(UINT16 ViewZoneId);
extern UINT32 HL_GetViewZoneNumOnVin(UINT16 VinId);
extern UINT32 HL_GetViewZoneVprocPinMaxWin(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *Width, UINT16 *Height);
extern UINT32 HL_GetVprocPinMaxWidth(UINT16 PrevIdx, const AMBA_DSP_WINDOW_s *Input, const AMBA_DSP_WINDOW_s *Output, UINT16 *MaxWidth);
extern UINT32 HL_CalcVprocGroupNum(void);
extern UINT32 HL_GetVprocGroupNum(UINT16 *Num);
extern UINT32 HL_GetVprocGroupFreeRun(UINT16 *pFreeRunBit);
extern UINT32 HL_GetVprocGroupIdx(UINT16 VprocId, UINT16 *pIdx, UINT8 IsStlProc);
extern UINT32 HL_GetViewZonePrevDest(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *pDest, UINT32 *pDestIdBit);
extern UINT32 HL_GetViewZonePrevSync(UINT16 ViewZoneId, UINT8 *pPrevSync, UINT8 *pPrevVoutPreSend);
#define DSP_EFCT_SHARE_NONE             (0U)
#define DSP_EFCT_SHARE_PREVB_TO_VOUT0   (1U) // means effect case prevB output to vout0
#define DSP_EFCT_SHARE_PREVC_TO_VOUT1   (2U) // means effect case prevC output to vout1
extern UINT32 HL_GetPinOutDestBufNum(UINT16 Dest, UINT16 VprocPin, UINT16 *pNum, UINT16 *pAuxNum, UINT8 *pEfctSharedMode);
extern UINT32 HL_GetPinOutDestChFmt(UINT16 Dest, UINT16 VprocPin, UINT8 *pFmt);
extern UINT32 HL_GetViewZonePinFbNum(UINT16 ViewZoneId, UINT16 VprocPin, UINT16 *pNum, UINT16 *pAuxNum, UINT8 IsExtFb);
extern UINT32 HL_GetViewZoneVinId(UINT16 ViewZoneId, UINT16* VinId);
extern UINT32 HL_GetVinViewZoneId(UINT16 VinId, UINT16 *pViewZoneId);
extern void HL_GetGroupAliveViewZoneId(UINT8 GrpID, UINT16 MasterVinId, UINT32 DisableViewZoneBitMask, UINT16 *pMasterViewZoneId, UINT16 *pNumVproc);
extern void HL_GetGroupMasterVprocId(const UINT16 GrpId, UINT16 *pMasterVprocId);
extern void HL_GetGroupFirstVprocId(const UINT16 GrpId, UINT16 *pFirstVprocId);
//extern void HL_GetGroupVZMask(const UINT16 GrpId, UINT32 *pGroupVZMask);
extern UINT32 HL_GetViewZoneAffinity(const UINT16 ViewZoneId, const CTX_VIEWZONE_INFO_s *pViewZoneInfo, UINT8 *pAffinity);
extern UINT32 HL_GetRescEncStrmAffinity(const UINT16 StrmIdx,
                                        const AMBA_DSP_WINDOW_DIMENSION_s *pWin,
                                        UINT8 *pAffinity);
#define ENC_MAX_WIN_TYPE_AVC    (0U)
#define ENC_MAX_WIN_TYPE_HEVC   (1U)
#define ENC_MAX_WIN_TYPE_JPG    (2U)
#define ENC_MAX_WIN_TYPE_ALL    (3U)
#define ENC_MAX_WIN_TYPE_NUM    (4U)
extern UINT32 HL_GetEncMaxWindow(UINT16 Type, UINT16 *pWidth, UINT16 *pHeight);
extern UINT32 HL_GetLevelIdc(const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *StrmCfg, UINT8 *pLevelIdc, UINT8 *pTierIdc);
extern UINT32 HL_GetHevcMaxTileNum(UINT8 *TileNum, UINT16 EncodeWidth);
extern UINT32 HL_GetCommBufMaxWindow(UINT16 Dest, UINT16 PinId, UINT16 *pWidth, UINT16 *pHeight);
extern UINT32 HL_GetVoutTotalBit(void);
extern UINT32 HL_GetVoutRotateRescRow(UINT16 Height, UINT8 *pRow);
extern UINT32 HL_GetEncFmtTotalBit(void);
extern UINT32 HL_GetEffectEnableOnYuvStrm(const UINT16 YuvStrmId);
extern UINT32 HL_GetEffectChannelEnable(void);
extern UINT8 HL_GetDefaultRawEnable(void);
extern UINT32 HL_GetProcRawDramOutEnable(UINT8 *pEnable, UINT8 *pStlRawInEn);
extern UINT32 HL_GetEffectChannelPostBlendNum(UINT16 YuvStrmIdx, const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout, UINT16 *TotalPostBldNum);
#if defined(UCODE_SUPPORT_EFFECT_PREV_BC_REARRANGE) || defined(UCODE_SUPPORT_EFFECT_PREV_ABC_REARRANGE)
extern UINT32 HL_GetEffectChannelVprocPin(UINT16 Purpose,
                                          UINT16 DestVout,
                                          UINT32 DestEnc,
                                          UINT16 *pCandNum,
                                          UINT16 *pCandPinId,
                                          UINT8 IsDramMipiYuv,
                                          UINT8 ChromaFmt);
#else
extern UINT32 HL_GetEffectChannelVprocPin(UINT16 Purpose,
                                          UINT16 DestVout,
                                          UINT32 DestEnc,
                                          UINT16 *pPinId,
                                          UINT8 IsDramMipiYuv);
#endif
extern UINT32 HL_CalcEffectPostPJob(UINT16 YuvStrmIdx, CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout);
extern UINT32 HL_EffPostPReBase(UINT16 YuvStrmIdx, const UINT16 *pYuvIdxOrder, CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout);
//extern UINT32 HL_FindOverlapeRegion(const AMBA_DSP_WINDOW_s *WinA, const AMBA_DSP_WINDOW_s *WinB, AMBA_DSP_WINDOW_s *WinOverlap, UINT16 *IsOverLap);
extern UINT32 HL_ComposeEfctSyncJobId(DSP_EFCT_SYNC_JOB_ID_s *JobId, UINT16 YuvStrmIdx, UINT16 SeqIndx, UINT16 TypeBit, UINT16 SubJobIdx);
extern UINT32 HL_VprocUpscaleCheck(UINT16 PrevIdx,
                                   const AMBA_DSP_WINDOW_s *In,
                                   const AMBA_DSP_WINDOW_s *Out,
                                   UINT8 *pCheckPass,
                                   UINT8 IsDecVout);
extern UINT32 HL_FindMinimalVprocPin(const AMBA_DSP_WINDOW_s *Output, const AMBA_DSP_WINDOW_s *ROI,
                                     UINT16 MaxTileNum, UINT16 VprocId, UINT16* VprocPin,
                                     UINT8 *pFound, UINT16 LowDlyPurpose, UINT8 IsEncSync, UINT8 IsDramMipiYuv,
                                     UINT8 IsSingleDecVoutPurpose);
extern UINT32 HL_IsVirtualVinIdx(UINT16 VinId, UINT8 *IsVirtVin);
extern void HL_GetVirtualVinNum(UINT16 *pVirtVinNum, UINT16 *pTdVirtVinNum);
extern UINT32 HL_GetTimeDivisionVirtVinInfo(UINT16 VinId, UINT16 *PhysicalVin);
extern UINT32 HL_GetVprocNum(UINT16 *Num);
extern UINT32 HL_GetVprocMipiYuyvNumber(UINT32 ViewZoneOI, UINT16 *YuyvNumber);
extern UINT32 HL_GetVinMipiYuyvInfo(UINT16 VinId, const CTX_VIN_INFO_s *pVinInfo, UINT16 *YuyvEnable);
//#define DEBUG_TILE_CALC
extern UINT32 HL_GetViewZoneWarpOverlap(const UINT16 ViewZoneId, UINT16 *pOverLapX, UINT16 *pOverLapY);
extern UINT32 HL_GetViewZoneWarpOverlapX(const UINT16 ViewZoneId, UINT16 *pOverLapX);
//extern UINT32 HL_GetViewZoneWarpOverlapXMax(const UINT16 ViewZoneId, UINT16 *pOverLapX);
extern UINT32 HL_GetSystemWarpWarpOverlap(UINT16 *WarpOverLap);
extern UINT32 HL_GetViewZoneRawOverlap(const UINT16 ViewZoneId, UINT16 *pOverLapX);
extern UINT32 HL_GetLuma12Info(UINT8 *pMainY12Enable, UINT8 *pHierY12Enable);
extern UINT32 HL_GetMainIrInfo(UINT8 *pMainIrEnable);
extern UINT32 HL_GetDramMipiYuvEnable(void);
#define SLICE_MODE_HOR_IDX  (0x0U)  // for smaller width
#define SLICE_MODE_VER_IDX  (0x1U)  // for LowDelay
extern UINT32 HL_IsSliceMode(UINT8 *IsSliceMode);
extern UINT32 HL_IsTileMode(UINT8 *pIsTileMode);
extern UINT32 HL_FillSliceCapLine(UINT16 ViewZoneId, UINT16 *pCapLineCfg, UINT16 IntcNumMinusOne);
#ifdef SUPPORT_DSP_LDY_SLICE
extern UINT32 HL_FillSliceLayoutCfg(UINT16 ColNum, UINT16 RowNum, UINT16 Width, UINT16 Height, DSP_SLICE_LAYOUT_s *pSliceLayout);
#endif
#define RECON_POST_2_VPROC_BIT  (0U)
extern UINT32 HL_GetReconPostStatus(UINT16 StrmIdx, UINT16 *Status);
extern UINT32 HL_ComposeStlProcJobId(DSP_STL_PROC_JOB_ID_s *JobId, UINT16 DatFmt, UINT16 OutputPinMask, UINT16 IsExtMem, UINT16 VprocId);
extern UINT32 HL_GetSystemVinMaxViewZoneNum(UINT16 VinId);
extern void HL_GetVinExistence(UINT16 VinId, UINT16 *Exist);
extern void HL_GetSystemVprocInputMaxWindow(UINT16 *pWidth, UINT16 *pHeight);
extern UINT32 HL_GetSystemVprocPinMaxWindow(UINT8 PinType, UINT16 *Width, UINT16 *Height);
extern UINT32 HL_GetVinAttachEventDefaultMask(UINT16 VinId, UINT32 *pMask);
extern UINT32 HL_GetSystemPymdMaxWindow(UINT16 *Width, UINT16 *Height);
extern UINT32 HL_GetViewZonePymdMaxWindow(UINT16 *Width, UINT16 *Height, UINT16 ViewZoneId);
extern UINT32 HL_GetSystemLndtMaxWindow(UINT16 *Width, UINT16 *Height);
extern UINT32 HL_GetViewZoneLndtMaxWindow(UINT16 *Width, UINT16 *Height, UINT16 ViewZoneId);
extern UINT32 HL_GetSystemHdrSetting(UINT8 *MaxHdrExpNumMiunsOne, UINT8 *LinearCeEnable, UINT16 *MaxHdrBlendHeight);
extern UINT32 HL_GetSystemHdrSettingOnVin(UINT16 VinId, UINT8 *MaxHdrExpNumMiunsOne, UINT8 *LinearCeEnable, UINT16 *MaxHdrBlendHeight);
extern UINT32 HL_GetSystemMctfSetting(UINT8 *pMctfEnable, UINT8 *pMctsEnable, UINT8 *pMctfCmpr);
extern void HL_GetWarpChromaDma(UINT16 LumaDmaSize, UINT16 *pChromaDmaSize);
extern UINT32 HL_GetSystemWarpDmaSetting(UINT16 *pLumaDmaSize, UINT16 *pChromaDmaSize);
extern UINT32 HL_GetSystemWarpWaitLineSetting(UINT16 *pLumaWaitLine, UINT16 *pChromaWaitLine);
extern UINT32 HL_GetSystemPostBlendSetting(UINT16 *pBlendWidth);
extern UINT32 HL_CalcVideoTile(UINT16 Width, UINT16 TileWidth, UINT16 *pTileNum, UINT16 Overlap);
extern UINT32 HL_CalcVideoTileC2Y(UINT16 ViewZoneId,
                                  UINT16 Width,
                                  UINT16 MaxTileWidth,
                                  UINT32 TileWidthExp,
                                  UINT8 FixedOverlap,
                                  UINT8 ChkSmem,
                                  UINT16 *pTileNum,
                                  UINT16 *pOverlap);
extern UINT32 HL_CalcVideoTileWidth(UINT16 Width,
                                    UINT16 TileNum,
                                    UINT16 *pTileWidth);
extern UINT32 HL_CalcVideoTileHeight(UINT16 Height, UINT16 TileNum, UINT16 *TileHeight);
extern UINT32 HL_GetViewZoneMctfEnable(UINT16 ViewZoneId,
                                       UINT8 *pMctfEnable,
                                       UINT8 *pMctsEnable,
                                       UINT8 *pMctsOutEnable,
                                       UINT8 *pMctfCmpr,
                                       UINT8 *pMctfRefIoEnable);
extern UINT32 HL_GetEncMaxTimeLapseWindow(UINT16 *Width, UINT16 *Height);
extern UINT32 HL_GetEncGrpCmprStroSetting(UINT16 StrmIdx, UINT16 *pCsLsb, UINT16 *pCsResIdx, ULONG *pCsCfgULAddr);
extern UINT32 HL_GetVinNum(UINT16 *Num);
extern UINT32 HL_CheckPrevROI(AMBA_DSP_WINDOW_s ROI, AMBA_DSP_WINDOW_s Main);
extern UINT32 HL_GetDramToWarpEnable(void);
extern UINT32 HL_GetSmemToWarpEnable(void);
extern UINT32 HL_GetExtraHorWarpEnable(void);
extern UINT32 HL_GetViewZoneDramToWarpEnable(UINT16 ViewZoneId);
extern UINT32 HL_GetViewZoneSmemToWarpEnable(UINT16 ViewZoneId);
extern UINT8  HL_GetYuvStrmIsEffectOut(UINT16 MaxChanNum, UINT16 WindowOffsetX, UINT16 WindowOffsetY);
extern UINT32 HL_GetDecIdxFromVinId(UINT16 VinId, UINT16 *pIdx);
extern UINT32 HL_GetDecIdxFromDecTypeId(UINT16 DecTypeId, UINT32 BitsFmt, UINT16 *pIdx);
extern UINT32 HL_GetOctaveSize(UINT8 OctaveMode, UINT16 Input, UINT16 *pOutput);


extern UINT32 HL_GetVinWaitFlagTotalMask(UINT32 *pMask);
extern UINT32 HL_GetVoutWaitFlagTotalMask(UINT32 *pMask);
extern UINT32 HL_GetVinOutputPinNum(UINT16 VinId, UINT16 VirtChanId, const CTX_RESOURCE_INFO_s *pResource, UINT8 *pPinNum);
extern UINT8  HL_GetVin2CmdNormalWrite(UINT16 VinId);
extern UINT8  HL_GetVin2CmdGroupWrite(UINT16 VinId);
extern UINT32 HL_GetVinSecCfgSize(UINT16 VinId);

extern UINT32 HL_GetVinHdsCmprCmdCode(UINT16 VinId, UINT32 *pCmdCode);

#ifndef SUPPORT_MAX_UCODE
extern UINT32 HL_CalHierWindow(UINT8 OctaveMode, UINT8 HierIdx,
                               const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                               UINT16 *pHierWidth, UINT16 *pHierHeight);
#endif

extern UINT32 HL_GetVoutPhysId(UINT16 VoutId, UINT16 *pPhysId);
extern UINT32 HL_GetVoutVirtId(UINT16 VoutId, UINT16 *pVirtId);
extern UINT16 HL_GetMinimalActiveVinId(void);

extern UINT32 HL_VideoDecVprocSetupImpl(UINT16 StreamIdx, const AMBA_DSP_WINDOW_DIMENSION_s *pExtMainWin);


#endif //AMBADSP_ENCODEAPI_H
