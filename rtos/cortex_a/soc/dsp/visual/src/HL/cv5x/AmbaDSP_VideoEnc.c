/**
 *  @file AmbaDSP_VideoEnc.c
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
 *  @details Implementation of SSP Video Encode API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "dsp_priv_api.h"

static inline UINT32 HL_ReconPostStart(UINT8 WriteMode, UINT16 StrmIdx)
{
    UINT8 ExitILoop = 0U;
    UINT32 Rval = OK;
    UINT16 i, Idx, ViewZoneId = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;

    HL_GetResourcePtr(&Resource);
    for (i=0U; i<Resource->MaxViewZoneNum; i++) {
        HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
        if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
            DSP_Bit2U16Idx(ViewZoneInfo->SourceVin, &Idx);
            if (StrmIdx == Idx) {
                ViewZoneId = i;
                ExitILoop = 1U;
                break;
            }
        }
    }
    if (ExitILoop == 1U) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindingCfg,
                                  BIND_VIDEO_RECON_TO_VPROC,
                                  1U/*IsBind*/,
                                  StrmIdx,
                                  0U/*DONT CARE*/,
                                  ViewZoneId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("FpBind filling fail", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    return Rval;
}

//#define DEBUG_FRATE_DIV
static inline void HL_VideoEnc_FrameRateDivisorDec(const UINT16 SourceVin,
                                                   const UINT16 StrmDecimation,
                                                   const UINT16 StrmFrateGcd,
                                                   const UINT32 VinDecimationRate,
                                                   const UINT16 StrmTimeScale,
                                                   UINT32* FrameRateDivisor)
{
    UINT16 DecIdx = 0U;
    UINT32 SrcFrateGcd, SrcNumUnitsInTick, SrcTimeScale;
    UINT32 FrateDividend, FrateDivisor;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

    DSP_Bit2U16Idx((UINT32)SourceVin, &DecIdx);
    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
    SrcNumUnitsInTick = (VidDecInfo.FrameRate.NumUnitsInTick > 0U) ? VidDecInfo.FrameRate.NumUnitsInTick : 1U;
    SrcNumUnitsInTick = (SrcNumUnitsInTick == 1001U) ? 1000U : SrcNumUnitsInTick;
    SrcTimeScale = (VidDecInfo.FrameRate.TimeScale > 0U) ? VidDecInfo.FrameRate.TimeScale : 1U;
    SrcFrateGcd = Gcd(SrcTimeScale, SrcNumUnitsInTick);
    if ((SrcFrateGcd > 0U) && (StrmDecimation > 0U) && (StrmFrateGcd > 0U)) {
        FrateDividend = ((SrcTimeScale/SrcFrateGcd)/VinDecimationRate)/StrmDecimation;
        FrateDivisor = (UINT32)StrmTimeScale / StrmFrateGcd;
    } else {
        FrateDividend = SrcTimeScale;
        FrateDivisor = StrmTimeScale;
    }
    if (FrateDivisor != 0U) {
        *FrameRateDivisor = FrateDividend / FrateDivisor;
    } else {
        *FrameRateDivisor = 0U;
    }
#ifdef DEBUG_FRATE_DIV
    AmbaLL_LogUInt5("HL_VideoEnc_FrameRateDivisorDec, Src N:%u/%u T:%u/%u SrcFrateGcd:%u",
            VidDecInfo.FrameRate.NumUnitsInTick, SrcNumUnitsInTick,
            VidDecInfo.FrameRate.TimeScale, SrcTimeScale, SrcFrateGcd);
    AmbaLL_LogUInt5("HL_VideoEnc_FrameRateDivisorDec FrateDividend:%u FrateDivisor:%u FrameRateDivisor:%u",
            FrateDividend, FrateDivisor, *FrameRateDivisor, 0U, 0U);
#endif
}

static inline void HL_VideoEnc_FrameRateDivisorEnc(const UINT16 SourceVin,
                                                   const UINT16 StrmDecimation,
                                                   const UINT16 StrmFrateGcd,
                                                   const UINT32 VinDecimationRate,
                                                   const UINT16 StrmTimeScale,
                                                   UINT32* FrameRateDivisor)
{
    UINT16 VinIdx = 0U;
    UINT32 SrcFrateGcd, SrcNumUnitsInTick, SrcTimeScale;
    UINT32 FrateDividend, FrateDivisor;
    CTX_VIN_INFO_s VinInfo = {0};

    DSP_Bit2U16Idx((UINT32)SourceVin, &VinIdx);
    HL_GetVinInfo(HL_MTX_OPT_ALL, (UINT16)VinIdx, &VinInfo);
    SrcNumUnitsInTick = (VinInfo.FrameRate.NumUnitsInTick > 0U) ? VinInfo.FrameRate.NumUnitsInTick : 1U;
    SrcNumUnitsInTick = (SrcNumUnitsInTick == 1001U) ? 1000U : SrcNumUnitsInTick;
    SrcTimeScale = (VinInfo.FrameRate.TimeScale > 0U) ? VinInfo.FrameRate.TimeScale : 1U;
    SrcFrateGcd = Gcd(SrcTimeScale, SrcNumUnitsInTick);

    if ((SrcFrateGcd > 0U) && (StrmDecimation > 0U) && (StrmFrateGcd > 0U)) {
        FrateDividend = ((SrcTimeScale/SrcFrateGcd)/VinDecimationRate)/StrmDecimation;
        FrateDivisor = (UINT32)StrmTimeScale / StrmFrateGcd;
    } else {
        FrateDividend = SrcTimeScale;
        FrateDivisor = StrmTimeScale;
    }
    if (FrateDivisor != 0U) {
        *FrameRateDivisor = FrateDividend / FrateDivisor;
    } else {
        *FrameRateDivisor = 0U;
    }
#ifdef DEBUG_FRATE_DIV
    AmbaLL_LogUInt5("HL_VideoEnc_FrameRateDivisorEnc, Src N:%u/%u T:%u/%u SrcFrateGcd:%u",
            VinInfo.FrameRate.NumUnitsInTick, SrcNumUnitsInTick,
            VinInfo.FrameRate.TimeScale, SrcTimeScale, SrcFrateGcd);
    AmbaLL_LogUInt5("HL_VideoEnc_FrameRateDivisorEnc FrateDividend:%u FrateDivisor:%u FrameRateDivisor:%u",
            FrateDividend, FrateDivisor, *FrameRateDivisor, 0U, 0U);
#endif
}
static inline void HL_VideoEnc_FrameRateDivisor(UINT16 StrmId, UINT32* FrameRateDivisor)
{
    CTX_STREAM_INFO_s StrmInfo = {0};

    // Get VinIdx from current stream ID
    HL_GetStrmInfo(HL_MTX_OPT_ALL, StrmId, &StrmInfo);
    if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        *FrameRateDivisor = 1;
    } else {
        UINT16 StrmDecimation, SrcViewZoneId;
        UINT32 StrmFrateGcd, StrmNumUnitsInTick, StrmTimeScale, VinDecimationRate;
        CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
        CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
        const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

        HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &pYuvStrmInfo);
        pYuvStrmLayout = &pYuvStrmInfo->Layout;
        StrmDecimation = (pYuvStrmInfo->DestDeciRate > 0U) ? (UINT16)DSP_GetBit(pYuvStrmInfo->DestDeciRate, 16U, 16U) : 1U;

        // Assume all the source viewzone has the same frame rate
        SrcViewZoneId = pYuvStrmLayout->ChanCfg[0].ViewZoneId;
        HL_GetViewZoneInfoPtr(SrcViewZoneId, &pViewZoneInfo);

        VinDecimationRate = (pViewZoneInfo->VinDecimationRate > 1U) ? (UINT32)pViewZoneInfo->VinDecimationRate : 1U;

        StrmNumUnitsInTick = (StrmInfo.FrameRate.NumUnitsInTick > 0U) ? StrmInfo.FrameRate.NumUnitsInTick : 1U;
        StrmNumUnitsInTick = (StrmNumUnitsInTick == 1001U) ? 1000U : StrmNumUnitsInTick;
        StrmTimeScale = (StrmInfo.FrameRate.TimeScale > 0U) ? StrmInfo.FrameRate.TimeScale : 1U;
        StrmFrateGcd = Gcd(StrmTimeScale, StrmNumUnitsInTick);

#ifdef DEBUG_FRATE_DIV
        AmbaLL_LogUInt5("HL_VideoEnc_FrameRateDivisor StrmId:%u Source:%u DestDeci:%u StrmDeci:%u VinDeci:%u",
                StrmId, StrmInfo.SourceYuvStrmId, pYuvStrmInfo->DestDeciRate, StrmDecimation, VinDecimationRate);
        AmbaLL_LogUInt5("HL_VideoEnc_FrameRateDivisor Strm: N:%u/%u T:%u/%u StrmFrateGcd:%u",
                StrmInfo.FrameRate.NumUnitsInTick, StrmNumUnitsInTick,
                StrmInfo.FrameRate.TimeScale, StrmTimeScale, StrmFrateGcd);
#endif
        /* No FrmRateDivisor in TimeLapse and TimeDevision */
        if ((StrmInfo.VideoTimeLapseActive > 0U) ||
            (pViewZoneInfo->SourceTdIdx != VIEWZONE_SRC_TD_IDX_NULL)) {
            *FrameRateDivisor = 1;

        } else if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON) {
            *FrameRateDivisor = 1;

        } else if (pViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
            HL_VideoEnc_FrameRateDivisorDec(pViewZoneInfo->SourceVin,
                                            StrmDecimation,
                                            (UINT16)StrmFrateGcd,
                                            VinDecimationRate,
                                            (UINT16)StrmTimeScale,
                                            FrameRateDivisor);
        } else {
            HL_VideoEnc_FrameRateDivisorEnc(pViewZoneInfo->SourceVin,
                                            StrmDecimation,
                                            (UINT16)StrmFrateGcd,
                                            VinDecimationRate,
                                            (UINT16)StrmTimeScale,
                                            FrameRateDivisor);
        }
    }
}

static inline UINT32 HL_VideoEnc_FillCrop(enc_frame_crop_t *DstCrop, const AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s *SrcCrop, UINT8 CodingFmt, const AMBA_DSP_WINDOW_s *Win)
{
    UINT32 Rval = OK;
    UINT32 Value;

    /* SPS */
    DstCrop->enable_flag = SrcCrop->CroppingFlag;

    if (SrcCrop->CroppingFlag == 0U) {
        if (CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
            if ((Win->Width == FHD_WIDTH) &&
                (Win->Height == FHD_HEIGHT)) {
                DstCrop->enable_flag = 1U;
                Value = 0U;
                DstCrop->offset_left = (UINT8)Value;

                Value = 0U;
                DstCrop->offset_right = (UINT8)Value;

                Value = 0U;
                DstCrop->offset_top = (UINT8)Value;

                Value = 4U;
                Value <<= 1U;
                DstCrop->offset_bottom = (UINT8)Value;
            } else if ((Win->Width == UHD_6M_WIDTH) &&
                (Win->Height == UHD_6M_HEIGHT)) {
                DstCrop->enable_flag = 1U;
                Value = 0U;
                DstCrop->offset_left = (UINT8)Value;

                Value = 0U;
                DstCrop->offset_right = (UINT8)Value;

                Value = 0U;
                DstCrop->offset_top = (UINT8)Value;

                Value = 4U;
                Value <<= 1U;
                DstCrop->offset_bottom = (UINT8)Value;
            } else {
                //TBD
            }
        }
    } else {
        Value = SrcCrop->CropLeftOffset;
        Value <<= 1U;
        DstCrop->offset_left= (UINT8)Value;

        Value = SrcCrop->CropRightOffset;
        Value <<= 1U;
        DstCrop->offset_right= (UINT8)Value;

        Value = SrcCrop->CropTopOffset;
        Value <<= 1U;
        DstCrop->offset_top= (UINT8)Value;

        Value = SrcCrop->CropBottomOffset;
        Value <<= 1U;
        DstCrop->offset_bottom= (UINT8)Value;
    }

    return Rval;
}

static inline UINT32 HL_VideoEnc_FillVui(h264_vui_t *DstVui, const AMBA_DSP_VIDEO_ENC_VUI_s *SrcVui)
{
    UINT32 Rval = OK;

    DstVui->vui_enable = SrcVui->VuiEnable;
    DstVui->aspect_ratio_info_present_flag = SrcVui->AspectRatioInfoPresentFlag;
    DstVui->overscan_info_present_flag = SrcVui->OverscanInfoPresentFlag;
    DstVui->overscan_appropriate_flag = SrcVui->OverscanAppropriateFlag;
    DstVui->video_signal_type_present_flag = SrcVui->VideoSignalTypePresentFlag;
    DstVui->video_full_range_flag = SrcVui->VideoFullRangeFlag;
    DstVui->colour_description_present_flag = SrcVui->ColourDescriptionPresentFlag;
    DstVui->chroma_loc_info_present_flag = SrcVui->ChromaLocInfoPresentFlag;
    DstVui->timing_info_present_flag = SrcVui->TimingInfoPresentFlag;
    DstVui->fixed_frame_rate_flag = SrcVui->FixedFrameRateFlag;
    DstVui->nal_hrd_parameters_present_flag = SrcVui->NalHrdParametersPresentFlag;
    DstVui->vcl_hrd_parameters_present_flag = SrcVui->VclHrdParametersPresentFlag;
    DstVui->low_delay_hrd_flag = SrcVui->LowDelayHrdFlag;
    DstVui->pic_struct_present_flag = SrcVui->PicStructPresentFlag;
    DstVui->bitstream_restriction_flag = SrcVui->BitstreamRestrictionFlag;
    if (SrcVui->BitstreamRestrictionFlag == 1U) {
        if (SrcVui->MaxDecFrameBuffering > 0U) {
            DstVui->custom_bitstream_restriction_cfg = 1U;
            DstVui->max_dec_frame_buffering = SrcVui->MaxDecFrameBuffering;
        } else {
            /* uCode decide it */
            DstVui->custom_bitstream_restriction_cfg = 0U;
            DstVui->max_dec_frame_buffering = 0U;
        }
    } else {
        DstVui->custom_bitstream_restriction_cfg = 0U;
        DstVui->max_dec_frame_buffering = 0U;
    }
    DstVui->motion_vectors_over_pic_boundaries_flag = SrcVui->MotionVectorsOverPicBoundariesFlag;
    DstVui->aspect_ratio_idc = SrcVui->AspectRatioIdc;
    DstVui->SAR_width = SrcVui->SarWidth;
    DstVui->SAR_height = SrcVui->SarHeight;
    DstVui->video_format = SrcVui->VideoFormat;

    DstVui->colour_primaries = SrcVui->ColourPrimaries;
    DstVui->transfer_characteristics = SrcVui->TransferCharacteristics;
    DstVui->matrix_coefficients = SrcVui->MatrixCoefficients;

    DstVui->chroma_sample_loc_type_top_field = SrcVui->ChromaSampleLocTypeTopField;
    DstVui->chroma_sample_loc_type_bottom_field = SrcVui->ChromaSampleLocTypeBottomField;

    DstVui->vbr_cbp_rate = SrcVui->VbrCbpRate;

    DstVui->max_bits_per_mb_denom = (UINT8)(SrcVui->MaxBitsPerMbDenom);
    DstVui->max_bytes_per_pic_denom = (UINT8)(SrcVui->MaxBytesPerPicDenom);
    DstVui->log2_max_mv_length_horizontal = SrcVui->Log2MaxMvLengthHorizontal;
    DstVui->log2_max_mv_length_vertical = SrcVui->Log2MaxMvLengthVertical;

    DstVui->num_reorder_frames = SrcVui->NumReorderFrames;

    return Rval;
}

static inline UINT32 HL_VideoEnc_ConvertFrameRate(AMBA_DSP_FRAME_RATE_s FrmRate, UINT32 *DspFrmRate)
{
    UINT32 Rval = OK, Value;
    UINT32 TimeScaleTruncate2U30, NewTimeScale;
    UINT32 FrateGcd;

    /* EncFrameRate
        B[31] Scan format. 0 is progressive, 1 is interlace
        B[30] NumUnitsInTick. 0 is 1000,  1 is 1001
        B[29:0] TimeScale

        if NumUnitsInTick=0, ucode will use TimeScale / 1000
        when user input 30/1, we will use 30/1*1000 = 30000
        if input 30000/1000, we will use 30000/1000*1000 = 30000
     */

    if (DspFrmRate == NULL) {
        AmbaLL_LogUInt5("HL_VideoEnc_ConvertFrameRate: Null input", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
//FIXME, Frame divisor

        *DspFrmRate = 0U; //reset

        Value = (FrmRate.Interlace == 1U)? ((UINT32)1U<<31U): (UINT32)0U;
        Value |= (FrmRate.NumUnitsInTick == 1001U)? ((UINT32)1U<<30U): (UINT32)0U;

        if ((FrmRate.NumUnitsInTick == 1001U) || (FrmRate.NumUnitsInTick == 1000U)) {
            NewTimeScale = FrmRate.TimeScale;
        } else {
            FrateGcd = Gcd(FrmRate.TimeScale, FrmRate.NumUnitsInTick);
            NewTimeScale = ((FrmRate.TimeScale / FrateGcd) * 1000U);
        }

        TimeScaleTruncate2U30 = (NewTimeScale & 0x3FFFFFFFU);
        Value |= TimeScaleTruncate2U30;
        *DspFrmRate = Value;
    }

    return Rval;
}

static inline void HL_FillEncJpgSetupObsoleted(cmd_encoder_jpeg_setup_t *pEncodeJpgSetup)
{
    //NotSupport
    pEncodeJpgSetup->target_bpp = 0U;
    pEncodeJpgSetup->tolerance = 0U;
    pEncodeJpgSetup->max_reenc_loops = 0U;
    pEncodeJpgSetup->rct_sample_num = 0U;
    pEncodeJpgSetup->rct_daddr = 0U;
    pEncodeJpgSetup->enc_frame_rate = 0U;

    pEncodeJpgSetup->stream_type = 0U;
    pEncodeJpgSetup->restart_interval = 0U;
    pEncodeJpgSetup->enc_src = 0U;
    pEncodeJpgSetup->efm_enable = 0U;
}

static inline UINT32 HL_FillVidEncCfgCmdOnJpg(UINT8 WriteMode, CTX_STREAM_INFO_s *pStrmInfo, UINT16 StrmNum, UINT16 StreamIdx)
{
    UINT32 Rval = OK;
    UINT16 i;
    UINT8 EnableVdoThm = 0U;
    CTX_DATACAP_INFO_s DataCapInfo = {0};
    ULONG ULAddr = 0U;
    UINT8 CmdBufferId = 0U;
    void *CmdBufferAddr = NULL;
    cmd_encoder_jpeg_setup_t *pEncodeJpgSetup = HL_DefCtxCmdBufPtrEncJpg;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&pEncodeJpgSetup, &CmdBufferAddr);

    pEncodeJpgSetup->stream_id = (UINT8)StreamIdx;

    pEncodeJpgSetup->coding_type = DSP_ENC_FMT_JPEG;
    pStrmInfo->IsMJPG = (UINT8)1U;

    for (i = 0U; i<AMBA_DSP_MAX_DATACAP_NUM; i++) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, i, &DataCapInfo);
        if ((DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) &&
            (DataCapInfo.Status == DATA_CAP_STATUS_2_RUN) &&
            (DataCapInfo.Cfg.Index == StreamIdx)) {
            EnableVdoThm = 1U;
            break;
        }
    }

    /*
     * Start/Stop method
     * In CVs uCode design, there are Start/Stop method restrictions
     * StartFromCmd + StopFromCmd [OK]
     * StartFromPic + StopFromPic [OK]
     * StartFromPic + StopFromCmd [OK]
     * StartFromCmd + StopFromPic [NG]
     */
    if ((EnableVdoThm > 0U) ||
        (StrmNum > 1U)) {
        pEncodeJpgSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
    } else {
        pEncodeJpgSetup->enc_start_method = DSP_ENC_START_FROM_CMD;
    }
    if ((pStrmInfo->VideoTimeLapseActive > 0U)) {
        pEncodeJpgSetup->enc_stop_method = DSP_ENC_STOP_FROM_PICINFO;
    } else {
        pEncodeJpgSetup->enc_stop_method = DSP_ENC_STOP_FROM_CMD;
    }
    pStrmInfo->StartMethod = pEncodeJpgSetup->enc_start_method;
    pStrmInfo->StopMethod = pEncodeJpgSetup->enc_stop_method;

    /* Window */
    pEncodeJpgSetup->encode_w = pStrmInfo->Window.Width;
    pEncodeJpgSetup->encode_h = pStrmInfo->Window.Height;
    pEncodeJpgSetup->encode_w_ofs = pStrmInfo->Window.OffsetX;
    pEncodeJpgSetup->encode_h_ofs = pStrmInfo->Window.OffsetY;
    if ((HL_IS_ROTATE(pStrmInfo->Rotate) == 1U)) {
        UINT16 EncodeW = pEncodeJpgSetup->encode_w;

        pEncodeJpgSetup->encode_w = pEncodeJpgSetup->encode_h;
        pEncodeJpgSetup->encode_h = EncodeW;
        pEncodeJpgSetup->encode_w_ofs = pStrmInfo->Window.OffsetY;
        pEncodeJpgSetup->encode_h_ofs = pStrmInfo->Window.OffsetX;
    }

    pEncodeJpgSetup->chroma_format = DSP_YUV_420;

    pEncodeJpgSetup->is_mjpeg = (UINT8)1U;

    /* Tile/Slice */
    pEncodeJpgSetup->slice_num = pStrmInfo->NumSlice;

    /* Rotate/Flip */
    pEncodeJpgSetup->hflip = (HL_IS_HFLIP(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;
    pEncodeJpgSetup->vflip = (HL_IS_VFLIP(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;
    pEncodeJpgSetup->rotate = (HL_IS_ROTATE(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;

    /* Bits buffer and information */
    (void)dsp_osal_virt2cli(pStrmInfo->BitsBufAddr, &pEncodeJpgSetup->bits_fifo_base);
    pEncodeJpgSetup->bits_fifo_size = pStrmInfo->BitsBufSize;
    pEncodeJpgSetup->bits_fifo_offset = 0U;

    /* Desc buffer, LL reset desc buffer every encode start, not yet */
    DSP_GetBitsDescBufferAddr(0U, &ULAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pEncodeJpgSetup->info_fifo_base);
    pEncodeJpgSetup->info_fifo_size = AMBA_DSP_VIDEOENC_DESC_BUF_SIZE;

    /* BitRate */
    pEncodeJpgSetup->initial_qlevel = (UINT8)pStrmInfo->JpgBitRateCtrl.QualityLevel;
    (void)dsp_osal_virt2cli(pStrmInfo->QuantMatrixAddr, &pEncodeJpgSetup->dqt_daddr);

    HL_FillEncJpgSetupObsoleted(pEncodeJpgSetup);

    Rval = AmbaHL_CmdEncoderJpegSetup(WriteMode, pEncodeJpgSetup);
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval == OK) {
        pStrmInfo->CfgStatus = ENC_STRM_CFG_DONE;
    }

    return Rval;
}

static inline UINT32 HL_VidEncCfgPreProcOnExtMem(UINT8 WriteMode, UINT8 VideoTimeLapseActive, UINT16 TimeLapseId, UINT16 StreamIdx)
{
    UINT32 Rval = OK;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;

    if (VideoTimeLapseActive > 0U) {
        if (TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
            CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};

            HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, TimeLapseId, &TimeLapseInfo);

            // Bind TimeLapseEncode to Virtual Vin
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
            Rval = HL_FillFpBindSetup(FpBindingCfg,
                                      BIND_VIDEO_INTERVAL_ENCODE,
                                      1U/*IsBind*/,
                                      TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM,/*VinId*/
                                      0U/*SrcPinId*/,
                                      StreamIdx);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
            } else {
                Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[%d]AmbaHL_CmdDspBindingCfg fail", __LINE__, 0U, 0U, 0U, 0U);
                } else {
                    HL_GetTimeLapseInfo(HL_MTX_OPT_GET, TimeLapseId, &TimeLapseInfo);
                    TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_RUN;
                    //Reset
                    (void)dsp_osal_memset(&TimeLapseInfo.LastYuvBuf , 0, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    TimeLapseInfo.LastMe1BufAddr = 0x0U;
                    HL_SetTimeLapseInfo(HL_MTX_OPT_SET, TimeLapseId, &TimeLapseInfo);
                }
            }
            HL_RelCmdBuffer(CmdBufferId);
        } else {
            AmbaLL_LogUInt5("[%d]Get TimeLapseInfo fail", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }

    return Rval;
}

static inline UINT32 HL_VidEncCfgPreProcOnTimeLapse(UINT8 WriteMode,
                                                    UINT16 TimeLapseId,
                                                    UINT16 ViewZoneId,
                                                    UINT16 StreamIdx,
                                                    UINT16 PinId)
{
    UINT32 Rval = OK;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
    UINT16 MinVinId;
#endif
    UINT8 NewWriteMode;

    if (TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
        CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};

        HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, TimeLapseId, &TimeLapseInfo);
        // Case 1: From VideoEncode to TimeLapseEncode
        // UnBind VideoEncode since interval record and normal record need different binding config
        if (TimeLapseInfo.Status == ENC_TIMELAPSE_STATUS_CONFIG) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
            Rval = HL_FillFpBindSetup(FpBindingCfg,
                                      BIND_VIDEO_ENCODE,
                                      0U/*IsBind*/,
                                      ViewZoneId,
                                      PinId,
                                      StreamIdx);
            if (Rval != OK) {
                AmbaLL_LogUInt5("FpBind filling fail[%d]", __LINE__, 0U, 0U, 0U, 0U);
                HL_RelCmdBuffer(CmdBufferId);
            } else {
                Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("AmbaHL_CmdDspBindingCfg fail[%d]", __LINE__, 0U, 0U, 0U, 0U);
                } else {
                    // Bind TimeLapseEncode to Virtual Vin
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
                    Rval = HL_FillFpBindSetup(FpBindingCfg,
                                              BIND_VIDEO_INTERVAL_ENCODE,
                                              1U/*IsBind*/,
                                              TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM,/*VinId*/
                                              0U/*SrcPinId*/,
                                              StreamIdx);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
                    } else {
                        /* Wait a VDSP event to ensure FpBindingCfg has been received */
                        Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1U, WAIT_INTERRUPT_TIMEOUT);
                        if (Rval == OK) {
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
                            MinVinId = HL_GetMinimalActiveVinId();
                            NewWriteMode = HL_GetVin2CmdNormalWrite(MinVinId);
#else
                            NewWriteMode = HL_GetVin2CmdNormalWrite(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM);
#endif
                            Rval = AmbaHL_CmdDspBindingCfg(NewWriteMode, FpBindingCfg);
                            if (Rval != OK) {
                                AmbaLL_LogUInt5("AmbaHL_CmdDspBindingCfg fail[%d]", __LINE__, 0U, 0U, 0U, 0U);
                            } else {
                                HL_GetTimeLapseInfo(HL_MTX_OPT_GET, TimeLapseId, &TimeLapseInfo);
                                TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_RUN;
                                HL_SetTimeLapseInfo(HL_MTX_OPT_SET, TimeLapseId, &TimeLapseInfo);
                            }
                        }
                    }
                }
                HL_RelCmdBuffer(CmdBufferId);
            }

        } else {
            // Case 2: From TimeLapseEncode to TimeLapseEncode
            // Do not repeat the binding
        }
    } else {
        AmbaLL_LogUInt5("[%d]Get TimeLapseInfo fail", __LINE__, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    }

    return Rval;
}

static inline UINT32 HL_VidEncCfgPreProcOnYuvStrm(UINT8 WriteMode,
                                                  UINT16 TimeLapseId,
                                                  UINT16 ViewZoneId,
                                                  UINT16 StreamIdx,
                                                  UINT16 PinId)
{
    UINT32 Rval = OK;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;
    UINT16 VinNum = 0;
    UINT8 NewWriteMode;

    if (TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
        CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};

        HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, TimeLapseId, &TimeLapseInfo);
        // Case 3: From TimeLapseEncode to VideoEncode
        // UnBind TimeLapseEncode since interval record and normal record need different binding config
        if (TimeLapseInfo.Status == ENC_TIMELAPSE_STATUS_COMPLETE) {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
            Rval = HL_FillFpBindSetup(FpBindingCfg,
                                      BIND_VIDEO_INTERVAL_ENCODE,
                                      0U/*IsBind*/,
                                      TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM,/*VinId*/
                                      0U/*SrcPinId*/,
                                      StreamIdx);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
            } else {
                NewWriteMode = HL_GetVin2CmdNormalWrite(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM);
                Rval = AmbaHL_CmdDspBindingCfg(NewWriteMode, FpBindingCfg);
                if (Rval == OK) {
                    /* Wait a VCAP event to ensure FpBindingCfg has been revceived */
                    (void)HL_GetVinNum(&VinNum);
                    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP1_EVENT, (UINT32)VinNum, WAIT_INTERRUPT_TIMEOUT);
                    if (Rval == OK) {
                        HL_GetTimeLapseInfo(HL_MTX_OPT_GET, TimeLapseId, &TimeLapseInfo);
                        TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_CONFIG;
                        HL_SetTimeLapseInfo(HL_MTX_OPT_SET, TimeLapseId, &TimeLapseInfo);
                    }
                }
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    if (Rval == OK) {
        // Bind VideoEncode to ViewZoneId
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindingCfg,
                                  BIND_VIDEO_ENCODE,
                                  1U/*IsBind*/,
                                  ViewZoneId,
                                  PinId,
                                  StreamIdx);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }
    return Rval;
}

static inline void HL_FillCodingType(cmd_encoder_setup_t *pEncodeSetup,
                                     CTX_STREAM_INFO_s *pStrmInfo)
{
    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        pEncodeSetup->coding_type = DSP_ENC_FMT_H264;
    } else if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->coding_type = DSP_ENC_FMT_H265;
    } else {
        pEncodeSetup->coding_type = DSP_ENC_FMT_H264;
    }
    pStrmInfo->IsMJPG = (UINT8)0U;
}

static inline void HL_FillTriggerMethod(cmd_encoder_setup_t *pEncodeSetup,
                                        const CTX_STREAM_INFO_s *pStrmInfo,
                                        UINT16 StrmNum,
                                        UINT8 EnableVdoThm)
{
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT8 IsLdPurpose;

    HL_GetYuvStrmInfoPtr(pStrmInfo->SourceYuvStrmId, &pYuvStrmInfo);
    IsLdPurpose = (1U == DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_LD_IDX, 1U))? (UINT8)1U: (UINT8)0U;

    if (pStrmInfo->VideoTimeLapseActive > 0U) {
        pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
        pEncodeSetup->enc_stop_method = DSP_ENC_STOP_FROM_PICINFO;
    } else if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_CMPR_STRO) {
        pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
        pEncodeSetup->enc_stop_method = DSP_ENC_START_FROM_PICINFO;
    } else {
        if ((EnableVdoThm > 0U) ||
            (StrmNum > 1U) ||
            (IsLdPurpose >= 1U)) {
            pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
        } else {
            pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_CMD;
        }

        if (IsLdPurpose >= 1U) {
            pEncodeSetup->enc_stop_method = DSP_ENC_STOP_FROM_PICINFO;
        } else {
            pEncodeSetup->enc_stop_method = DSP_ENC_STOP_FROM_CMD;
        }
    }
}

static inline void HL_FillEncWin(cmd_encoder_setup_t *pEncodeSetup,
                                 const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT16 ActWidth;
    UINT16 ActHeight;

    ActWidth = (HL_IS_ROTATE(pStrmInfo->Rotate) == 1U)? pStrmInfo->Window.Height: pStrmInfo->Window.Width;
    ActHeight = (HL_IS_ROTATE(pStrmInfo->Rotate) == 1U)? pStrmInfo->Window.Width: pStrmInfo->Window.Height;

    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        pEncodeSetup->encode_w_sz = ALIGN_NUM16(ActWidth, 16U);
        pEncodeSetup->encode_h_sz = ALIGN_NUM16(ActHeight, 16U);
    } else if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->encode_w_sz = ALIGN_NUM16(ActWidth, 8U);
        pEncodeSetup->encode_h_sz = ALIGN_NUM16(ActHeight, 8U);
    } else {
        pEncodeSetup->encode_w_sz = ActWidth;
        pEncodeSetup->encode_h_sz = ActHeight;
    }

    /* In CV2FS, offset is based before rotate/flip */
    if (HL_IS_ROTATE(pStrmInfo->Rotate) == 1U) {
        pEncodeSetup->encode_w_ofs = pStrmInfo->Window.OffsetY;
        pEncodeSetup->encode_h_ofs = pStrmInfo->Window.OffsetX;
    } else {
        pEncodeSetup->encode_w_ofs = pStrmInfo->Window.OffsetX;
        pEncodeSetup->encode_h_ofs = pStrmInfo->Window.OffsetY;
    }
}

static inline void HL_FillTile(cmd_encoder_setup_t *pEncodeSetup,
                               const CTX_STREAM_INFO_s *pStrmInfo)
{
    if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_MV) {
        //One tile only when MV
        pEncodeSetup->tile_num = 1U;
    } else {
        /* when dual-core-stripe, tile-num will be double,
         * App shall set even number and we divide it to make bitstrm tilenum meet App setting */
        if (pEncodeSetup->dual_core_mode == (UINT8)1U) {
            pEncodeSetup->tile_num = (UINT8)(pStrmInfo->NumTile >> 1U);
        } else {
            pEncodeSetup->tile_num = pStrmInfo->NumTile;
        }
    }
    pEncodeSetup->slice_num = pStrmInfo->NumSlice;
}

static inline void HL_FillRc(cmd_encoder_setup_t *pEncodeSetup,
                             const CTX_STREAM_INFO_s *pStrmInfo)
{
    pEncodeSetup->use_cabac = pStrmInfo->IsCabac;
    pEncodeSetup->quality_level = (UINT16)pStrmInfo->BitRateCfg.QualityLevel;
    /*
     * 2022/9/29, from SungShu,
     * we shall enable AVC tr8x8 to prevent wrong data when lower QP,
     * it may cause decoder stuck
     */
    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        UINT16 U16Val = pEncodeSetup->quality_level;
        DSP_SetU16Bit(&U16Val, 4U/*tr8x8*/);
        pEncodeSetup->quality_level = U16Val;
    }
    pEncodeSetup->target_bitrate = pStrmInfo->BitRateCfg.BitRate;
    pEncodeSetup->scalelist_opt = 0U;

    if ((pStrmInfo->FrameRate.TimeScale/pStrmInfo->FrameRate.NumUnitsInTick) >= 100U) {
        pEncodeSetup->rc_overlap_memd = (UINT8)1U;
    } else {
        pEncodeSetup->rc_overlap_memd = (UINT8)0U;
    }

#ifdef SUPPORT_DSP_MV_DUMP
    pEncodeSetup->is_dirt_detect = 0U;
#endif
//FIXME, SSP not support
    pEncodeSetup->initial_qp = 0U;

    if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_CBR) {
        pEncodeSetup->vbr_config = 0U;
        pEncodeSetup->rc_mode = DSP_ENC_RC_CBR;
    } else if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_VBR) {
//FIXME, VBR
        pEncodeSetup->vbr_config = 0U;
        pEncodeSetup->rc_mode = DSP_ENC_RC_VBR;
    } else if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_SMART_VBR) {
        pEncodeSetup->vbr_config = 0U;
        pEncodeSetup->rc_mode = DSP_ENC_RC_SIMPLE;
    } else if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_CMPR_STRO) {
        /*
         * Compress Stereo shall have following encode setting
         *  - I frm only, GOP_M = 1, GOP_N = 1
         *  - HEVC only
         *  - Luma only, Yuv400
         *  - ShortCutMode, 4KP60_FATEST_P1_P
         *  - Enable Scalelist = 1
         */

        pEncodeSetup->vbr_config = 0U;
        pEncodeSetup->rc_mode = DSP_ENC_RC_CMPR_STRO;
        pEncodeSetup->scalelist_opt = 1U;
    } else if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_MV) {
        /*
         * MV for DIS shall have following encode setting
         *  - RcMode = 0 (FIXED QP)
         *  - HEVC only
         *  - TileNum = 1
         *  - Enable DirtDetect
         *  - Enable IP infinite GOP
         */
        pEncodeSetup->vbr_config = 0U;
        pEncodeSetup->rc_mode = DSP_ENC_RC_OFF;
#ifdef SUPPORT_DSP_MV_DUMP
        pEncodeSetup->is_dirt_detect = (UINT8)1U;
#endif
        /*
         * 2019/11/13
         * Bits quality will be bad due to poor coding efficiency bases on IsDirtDetect = 1.
         *   it will disable me0/me1 and use ME3/4/5 start from [0,0],
         *   so search range may be smaller than before.
         * After that, Set InitQp=51 to make EncStrm get smallest EncBits for saving dram traffic.
         */
        pEncodeSetup->initial_qp = 51U;
    } else {
        pEncodeSetup->vbr_config = 0U;
        pEncodeSetup->rc_mode = DSP_ENC_RC_OFF;
    }

#ifdef SUPPORT_DSP_MV_DUMP
    if (pStrmInfo->MvBufBase > 0U) {
        pEncodeSetup->send_buf_status_msg = (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265)? (UINT8)1U: (UINT8)0U;

        (void)dsp_osal_virt2cli(pStrmInfo->MvBufBase, &pEncodeSetup->mvdump_daddr);
        (void)dsp_osal_virt2cli((pStrmInfo->MvBufBase + pStrmInfo->MvBufSize) - 1U, &pEncodeSetup->mvdump_fifo_limit);
        pEncodeSetup->mvdump_fifo_unit_sz = pStrmInfo->MvBufUnitSize;

        if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
            pEncodeSetup->mvdump_dpitch = 0U; //let ucode decide
        } else {
            UINT16 BufPitch, BufWidth, BufHeight;

            (void)AmbaDSP_CalcEncAvcMvBuf((UINT16)pEncodeSetup->encode_w_sz, (UINT16)pEncodeSetup->encode_h_sz, &BufPitch, &BufWidth, &BufHeight);
            pEncodeSetup->mvdump_dpitch = BufPitch; //let ucode decide
        }
    } else
#endif
    {
#ifdef SUPPORT_DSP_MV_DUMP
        pEncodeSetup->send_buf_status_msg = (UINT8)0U;
        pEncodeSetup->mvdump_dpitch = 0U;
#endif
        pEncodeSetup->mvdump_daddr = 0U;
        pEncodeSetup->mvdump_fifo_limit = 0U;
        pEncodeSetup->mvdump_fifo_unit_sz = 0U;
    }
}

static inline void HL_FillCpb(cmd_encoder_setup_t *pEncodeSetup,
                              const CTX_STREAM_INFO_s *pStrmInfo)
{
    pEncodeSetup->cpb_user_size = pStrmInfo->BitRateCfg.CPBSize;
    if (pEncodeSetup->cpb_user_size > 0U) {
        /* From uCode diag setting */
        pEncodeSetup->cpb_buf_idc = (UINT8)0x1FU;
        pEncodeSetup->cpb_cmp_idc = (UINT8)1U;
        pEncodeSetup->en_panic_rc = (UINT8)1U;
        pEncodeSetup->fast_rc_idc = (UINT8)2U;
        AmbaLL_LogUInt5("CPB size = %u", pEncodeSetup->cpb_user_size, 0U, 0U, 0U, 0U);
    } else {
        /* 2021/10/26, Per SungShu, let HL decide cpb buffer, so set cpb-buf-idc as 0 and cpb-cmp-idc as 1 */
        pEncodeSetup->cpb_buf_idc = (UINT8)0U;
        pEncodeSetup->en_panic_rc = (UINT8)0U;
        pEncodeSetup->cpb_cmp_idc = (UINT8)1U;
        pEncodeSetup->fast_rc_idc = (UINT8)0U;
    }

    if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_CBR) {
        pEncodeSetup->cpb_cmp_idc = (UINT8)1U;
    }
}

static inline void HL_FillDeblock(cmd_encoder_setup_t *pEncodeSetup,
                                  const CTX_STREAM_INFO_s *pStrmInfo)
{
    if (0U == DSP_GetU8Bit(pStrmInfo->DeblockFilter.EnableLoopFilter, AMBA_DSP_DEBLOCK_ENABLE_IDX, 1U)) {
        pEncodeSetup->custom_df_control_flag = 1U;
        pEncodeSetup->disable_df_idc = 0U;
        pEncodeSetup->df_param1 = 0U;
        pEncodeSetup->df_param2 = 0U;
    } else {
        if (0U == DSP_GetU8Bit(pStrmInfo->DeblockFilter.EnableLoopFilter, AMBA_DSP_DEBLOCK_EXT_PARAM_IDX, 1U)) {
            pEncodeSetup->custom_df_control_flag = 0U;
            pEncodeSetup->disable_df_idc = 0U;
            pEncodeSetup->df_param1 = 0U;
            pEncodeSetup->df_param2 = 0U;
        } else {
            pEncodeSetup->custom_df_control_flag = 1U;
            pEncodeSetup->disable_df_idc = 0U;
            pEncodeSetup->df_param1 = pStrmInfo->DeblockFilter.LoopFilterAlpha;
            pEncodeSetup->df_param2 = pStrmInfo->DeblockFilter.LoopFilterBeta;
        }
    }
}

static inline void HL_FillPriority(cmd_encoder_setup_t *pEncodeSetup,
                                   const CTX_STREAM_INFO_s *pStrmInfo)
{
#ifdef SUPPORT_ENC_Q_LD
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    if (pStrmInfo->SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        pEncodeSetup->enc_pic_q_low_delay = 0U;
    } else {
        HL_GetYuvStrmInfoPtr(pStrmInfo->SourceYuvStrmId, &pYuvStrmInfo);
        if (1U == DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_LD_IDX, 1U)) {
            pEncodeSetup->enc_pic_q_low_delay = 1U;
        } else {
            pEncodeSetup->enc_pic_q_low_delay = pStrmInfo->IsLowDlyQ;
        }
    }
#else
    (void)pStrmInfo;
#endif

    pEncodeSetup->per_slice_vdsp_interrupt = 1U; // we need bit-stream info every slice/tile

    //FIXME, for encode low delay and need accompany to MvinOpMode.hi_priority_stream_lowdelay/priority_encode_enable
    pEncodeSetup->is_high_priority_stream = 0U;
}

static inline void HL_FillIr(cmd_encoder_setup_t *pEncodeSetup,
                             const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT8 IrScanMode, IrScanDirection;

    if (pStrmInfo->IntraRefreshCtrl.IntraRefreshCycle > 0U) {
        pEncodeSetup->IR_cycle = pStrmInfo->IntraRefreshCtrl.IntraRefreshCycle;
        pEncodeSetup->IR_type = 0U; //TBD
        IrScanMode = (UINT8)DSP_GetU8Bit(pStrmInfo->IntraRefreshCtrl.IntraRefreshMode, 0U, 2U);
        IrScanDirection = (UINT8)DSP_GetU8Bit(pStrmInfo->IntraRefreshCtrl.IntraRefreshMode, 2U, 2U);
        (void)IrScanMode;
        (void)IrScanDirection;
#if defined(ENABLE_2ND_STRM_REFINE_CMD)
        pEncodeSetup->IR_no_overlap = 1U;
#endif
        AmbaLL_LogUInt5("No IR pattern now", 0U, 0U, 0U, 0U, 0U);
    } else {
        pEncodeSetup->IR_cycle = (UINT8)0U;
        pEncodeSetup->IR_type = (UINT8)0U; //TBD
    }

    pEncodeSetup->sps_pps_header_mode = pStrmInfo->SPSPPSHeaderInsertMode;
}

static inline void HL_FillPerfModeOnHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                         const CTX_STREAM_INFO_s *pStrmInfo)
{
    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->hevc_perf_mode = ENC_PERF_FAST_800; //ShortCutMode
    } else {
        pEncodeSetup->hevc_perf_mode = ENC_PERF_DEFAULT;
    }
}

static inline void HL_FillAffinity(cmd_encoder_setup_t *pEncodeSetup,
                                   const UINT16 StreamIdx,
                                   const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT8 AffinityEng = (UINT8)DSP_GetBit(pStrmInfo->Affinity, ENC_AFFINITY_ENG_BIT_IDX, ENC_AFFINITY_ENG_LEN);
    UINT8 Affinity = (UINT8)DSP_GetBit(pStrmInfo->Affinity, ENC_AFFINITY_CORE_BIT_IDX, ENC_AFFINITY_CORE_LEN);
    UINT8 AffinityOpt = (UINT8)DSP_GetBit(pStrmInfo->Affinity, ENC_AFFINITY_OPT_BIT_IDX, ENC_AFFINITY_OPT_LEN);
    UINT8 EncFlowMaxAffinity = (UINT8)DSP_GetBit(pStrmInfo->AffinityAssignation, ENC_AFFINITY_CORE_BIT_IDX, ENC_AFFINITY_CORE_LEN);
    UINT8 EncFlowMaxAffinityOpt = (UINT8)DSP_GetBit(pStrmInfo->AffinityAssignation, ENC_AFFINITY_OPT_BIT_IDX, ENC_AFFINITY_OPT_LEN);
    UINT32 TotalCoreNum;
    UINT8 SysCfgAffinity = (UINT8)DSP_GetBit(TuneDspSystemCfg.EncVdspAff, (UINT32)StreamIdx*SYS_CFG_VDSP_AFF_LEN, SYS_CFG_VDSP_AFF_LEN);
    UINT8 SysCfgAffinityOpt = (UINT8)DSP_GetBit(TuneDspSystemCfg.EncVdspAffOpt, (UINT32)StreamIdx*SYS_CFG_VDSP_AFF_LEN, SYS_CFG_VDSP_AFF_LEN);

    if (AffinityEng == (UINT8)0U) {
        pEncodeSetup->ceng_used = (UINT8)1U;
    } else {
        pEncodeSetup->ceng_used = AffinityEng;
    }
    if (Affinity == (UINT8)0U) {
        pEncodeSetup->core_used = EncFlowMaxAffinity;
    } else {
        pEncodeSetup->core_used = Affinity;
    }
    if (SysCfgAffinity > (UINT8)0U) {
        pEncodeSetup->core_used = SysCfgAffinity;
    }

    DSP_Bit2Cnt(pEncodeSetup->core_used, &TotalCoreNum);

    if (TotalCoreNum == 1U) {
        pEncodeSetup->dual_core_mode = 0U; //DONT CARE
    } else {
        if (AffinityOpt == (UINT8)0) {
            pEncodeSetup->dual_core_mode = (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265)? EncFlowMaxAffinityOpt: (UINT8)0U;
        } else {
            pEncodeSetup->dual_core_mode = (UINT8)(AffinityOpt - 1U);
        }
        if (SysCfgAffinityOpt > (UINT8)0U) {
            pEncodeSetup->dual_core_mode = (UINT8)(SysCfgAffinityOpt - 1U);
        }
    }
}

static inline void HL_FillQp(cmd_encoder_setup_t *pEncodeSetup,
                             const CTX_STREAM_INFO_s *pStrmInfo)
{
    if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_SMART_VBR) {
        // assign default QA
        pEncodeSetup->max_qp_i = (pStrmInfo->QpCfg.QpMaxIFrame == 255U)? (UINT8)51U: pStrmInfo->QpCfg.QpMaxIFrame;
        pEncodeSetup->min_qp_i = (pStrmInfo->QpCfg.QpMinIFrame == 255U)? (UINT8)14U: pStrmInfo->QpCfg.QpMinIFrame;
        pEncodeSetup->max_qp_p = (pStrmInfo->QpCfg.QpMaxPFrame == 255U)? (UINT8)51U: pStrmInfo->QpCfg.QpMaxPFrame;
        pEncodeSetup->min_qp_p = (pStrmInfo->QpCfg.QpMinPFrame == 255U)? (UINT8)17U: pStrmInfo->QpCfg.QpMinPFrame;
        if (pStrmInfo->QpCfg.Aqp == 255U) {
            pEncodeSetup->aqp = 2;
        } else {
            (void)dsp_osal_memcpy(&pEncodeSetup->aqp, &pStrmInfo->QpCfg.Aqp, sizeof(UINT8));
        }
        if (pStrmInfo->QpCfg.QpReduceIFrame == 255U) {
            pEncodeSetup->i_qp_reduce = 6;
        } else {
            (void)dsp_osal_memcpy(&pEncodeSetup->i_qp_reduce, &pStrmInfo->QpCfg.QpReduceIFrame, sizeof(UINT8));
        }
        if (pStrmInfo->QpCfg.QpReducePFrame == 255U) {
            pEncodeSetup->p_qp_reduce = 3;
        } else {
            (void)dsp_osal_memcpy(&pEncodeSetup->p_qp_reduce, &pStrmInfo->QpCfg.QpReducePFrame, sizeof(UINT8));
        }
    } else {
        pEncodeSetup->max_qp_i = 0U;
        pEncodeSetup->min_qp_i = 0U;
        pEncodeSetup->max_qp_p = 0U;
        pEncodeSetup->min_qp_p = 0U;
        pEncodeSetup->aqp = 0;
        pEncodeSetup->i_qp_reduce = 0;
        pEncodeSetup->p_qp_reduce = 0;
    }
}

static inline void HL_FillSmv(cmd_encoder_setup_t *pEncodeSetup,
                              const UINT16 StreamIdx,
                              const CTX_STREAM_INFO_s *pStrmInfo)
{
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&pResource);

    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->max_smvmax_scale = (UINT8)(pResource->MaxSearchRangeHevc[StreamIdx] >> 3U);
    } else if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        pEncodeSetup->max_smvmax_scale = (UINT8)(pResource->MaxSearchRange[StreamIdx] >> 3U);
    } else {
        pEncodeSetup->max_smvmax_scale = (UINT8)0U;
    }
}

static inline void HL_FillEncSetupObsoleted(cmd_encoder_setup_t *pEncodeSetup)
{
    pEncodeSetup->stream_type = 0U;
    pEncodeSetup->enc_src = 0U;
    pEncodeSetup->efm_enable = 0U;
    pEncodeSetup->beating_reduction = 0U;
    pEncodeSetup->log2_num_ltrp_per_gop = 0U;
    pEncodeSetup->two_lt_ref_mode = 0U;
    pEncodeSetup->aqp_lambda = 0U;
    pEncodeSetup->recon_offset_y = 0U;
    pEncodeSetup->is_recon_to_vout = 0U;
}

static inline UINT32 HL_FillVidEncCfgCmd(UINT8 WriteMode, CTX_STREAM_INFO_s *pStrmInfo, UINT16 StrmNum, UINT16 StreamIdx)
{
    UINT8 EnableVdoThm = 0U, U8Val;
    UINT8 CmdBufferId = 0U;
    void *CmdBufferAddr = NULL;
    UINT16 i;
    UINT32 Rval, U32Value;
    ULONG ULAddr;
    CTX_DATACAP_INFO_s DataCapInfo = {0};
    cmd_encoder_setup_t *pEncodeSetup = HL_DefCtxCmdBufPtrEnc;
    UINT8 AffinityEng = (UINT8)DSP_GetBit(pStrmInfo->Affinity, ENC_AFFINITY_ENG_BIT_IDX, ENC_AFFINITY_ENG_LEN);
    UINT16 EngId = 0U;

    DSP_U8Bit2U16Idx(AffinityEng, &EngId);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&pEncodeSetup, &CmdBufferAddr);
    /* Stream description */
    pEncodeSetup->stream_id = (UINT8)StreamIdx;

    HL_FillCodingType(pEncodeSetup, pStrmInfo);

    /* Profile/Level IDC */
    if (pStrmInfo->CodingFmt ==  AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->profile_idc = (pStrmInfo->ProfileIDC < (UINT8)2U/*Main10*/)? pStrmInfo->ProfileIDC: (UINT8)1U/*Main*/;
    } else {
        pEncodeSetup->profile_idc = pStrmInfo->ProfileIDC;
    }
    pEncodeSetup->level_idc = pStrmInfo->LevelIDC;
    pEncodeSetup->tier_idc = pStrmInfo->TierIDC;

    for (i = 0U; i < AMBA_DSP_MAX_DATACAP_NUM; i++) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, i, &DataCapInfo);
        if ((DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) &&
            (DataCapInfo.Status == DATA_CAP_STATUS_2_RUN) &&
            (DataCapInfo.Cfg.Index == StreamIdx)) {
            EnableVdoThm = 1U;
            break;
        }
    }

    /* Start/Stop method */
    HL_FillTriggerMethod(pEncodeSetup, pStrmInfo, StrmNum, EnableVdoThm);
    pStrmInfo->StartMethod = pEncodeSetup->enc_start_method;
    pStrmInfo->StopMethod = pEncodeSetup->enc_stop_method;

    /* Window */
    (void)HL_FillEncWin(pEncodeSetup, pStrmInfo);

    /* GOP */
    pEncodeSetup->M = pStrmInfo->GOPCfg.M;
    pEncodeSetup->N = pStrmInfo->GOPCfg.N;
    pEncodeSetup->idr_interval = pStrmInfo->GOPCfg.IdrInterval;
    pEncodeSetup->gop_structure = pStrmInfo->GOPCfg.GOPStruct;
    pEncodeSetup->numRef_P = pStrmInfo->NumPRef;
    pEncodeSetup->numRef_B = pStrmInfo->NumBRef;

    /* Affinity, 2021/5/14
     * affinity may change the total tile num when HEVC
     * so we shall fill affinity first, then tile
     * when enable dual-code-stripe mode, hevc tile numbe will X2, due to left/right stripe
     */
    HL_FillAffinity(pEncodeSetup, StreamIdx, pStrmInfo);

    /* Tile/Slice */
    HL_FillTile(pEncodeSetup, pStrmInfo);

    /* BRC */
    HL_FillRc(pEncodeSetup, pStrmInfo);

//FIXME, not used now
    pEncodeSetup->vbr_init_data_daddr = 0x0U;

    /* Bits buffer and inforamtion */
    (void)dsp_osal_virt2cli(pStrmInfo->BitsBufAddr, &pEncodeSetup->bits_fifo_base);
    pEncodeSetup->bits_fifo_size = pStrmInfo->BitsBufSize;

//FIXME, not used now
    pEncodeSetup->num_slices_per_bits_info = 0U;

    /* Desc buffer, LL reset desc buffer every encode start, not yet */
    DSP_GetBitsDescBufferAddr(EngId, &ULAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pEncodeSetup->info_fifo_base);
    pEncodeSetup->info_fifo_size = AMBA_DSP_VIDEOENC_DESC_BUF_SIZE;

    /* FrameRate and Pts */
    U32Value = 0U;
    (void)HL_VideoEnc_ConvertFrameRate(pStrmInfo->FrameRate, &U32Value);
    pEncodeSetup->enc_frame_rate = U32Value;
    U8Val = (pStrmInfo->FrameRate.Interlace == 1U)? (UINT8)DSP_ENC_PAFF_ALL_FLD: (UINT8)DSP_ENC_PAFF_ALL_FRM;
    pEncodeSetup->aff_mode = U8Val;
#if 0 //FIXME
    pEncodeSetup->frame_rate_pts_delta = pStrmInfo->FrameRate.NumUnitsInTick;
#endif

    /* Rotate/Flip */
    pEncodeSetup->hflip = (HL_IS_HFLIP(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;
    pEncodeSetup->vflip = (HL_IS_VFLIP(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;
    pEncodeSetup->rotate = (HL_IS_ROTATE(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;

    /* Enc Engine Usage */
    pEncodeSetup->ceng_used = AffinityEng;

    /* CPB */
    HL_FillCpb(pEncodeSetup, pStrmInfo);

    /* Deblock filter */
    HL_FillDeblock(pEncodeSetup, pStrmInfo);

    /* Priority */
    HL_FillPriority(pEncodeSetup, pStrmInfo);

    /* IR */
    HL_FillIr(pEncodeSetup, pStrmInfo);

    /* HEVC performance mode */
    HL_FillPerfModeOnHEVC(pEncodeSetup, pStrmInfo);

    /* Qp */
    HL_FillQp(pEncodeSetup, pStrmInfo);

    pEncodeSetup->embed_code = pStrmInfo->EmbedCode;

//FIXME, only eanble when RC_SIMPE+1080p240
//          pEncodeSetup->out_loop_rc = 0U;

//          pEncodeSetup->use_local_enc_sw_option = 0U;
//          pEncodeSetup->local_enc_sw_option = 0U;

    pEncodeSetup->stat_fifo_base = 0U;
    pEncodeSetup->stat_fifo_limit = 0U;

    /* MaxSR */
    HL_FillSmv(pEncodeSetup, StreamIdx, pStrmInfo);

    /* Compression Stereo config */
#ifdef SUPPORT_DSP_CMPR_STRO
    if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_CMPR_STRO) {
        UINT16 CsLsb = 0U, CsResIdx = 0U;
        ULONG CsCfgAddr = 0U;

        Rval = HL_GetEncGrpCmprStroSetting(StreamIdx, &CsLsb, &CsResIdx, &CsCfgAddr);
        if (Rval == OK) {
            pEncodeSetup->cs_is_lsb = (UINT8)CsLsb;
            pEncodeSetup->cs_res_idx = (UINT8)CsResIdx;
            (void)dsp_osal_virt2cli(CsCfgAddr, &pEncodeSetup->cs_cfg_dbase);
        }
    } else {
        pEncodeSetup->cs_is_lsb = 0U;
        pEncodeSetup->cs_res_idx = 0U;
        pEncodeSetup->cs_cfg_dbase = 0U;
    }
#endif

    pEncodeSetup->is_mono = (UINT8)pStrmInfo->IsMono;
    if (TuneDspSystemCfg.MonoVdoEnc > (UINT8)0U) {
        pEncodeSetup->is_mono = (UINT8)(TuneDspSystemCfg.MonoVdoEnc - 1U);
    }

    HL_FillEncSetupObsoleted(pEncodeSetup);

    Rval = AmbaHL_CmdEncoderSetup(WriteMode, pEncodeSetup);
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval == OK) {
        pStrmInfo->CfgStatus = ENC_STRM_CFG_DONE;
    }

    return Rval;
}

static inline UINT32 HL_VideoStreamConfig(UINT16 StreamIdx, UINT32 StrmBitMask)
{
    UINT32 Rval = OK;
    CTX_STREAM_INFO_s StrmInfo;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VPROC_INFO_s VprocInfo = {0};
//FIXME, StreamSync
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 IsEncPurpose = (UINT8)0U;
    UINT16 i, PrevId = 0U, ViewZoneId = 0U, PinId = 0U, ReconPostStatus = 0U, StrmNum = 0U;
    UINT8 IsEfctYuvStrm;

    HL_GetStrmInfo(HL_MTX_OPT_ALL, StreamIdx, &StrmInfo);
    DSP_Bit2U16Cnt(StrmBitMask, &StrmNum);
    if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        /* ExtMem is sort of TimeLapse */
        Rval = HL_VidEncCfgPreProcOnExtMem(WriteMode, StrmInfo.VideoTimeLapseActive, StrmInfo.TimeLapseId, StreamIdx);
    } else {
        HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
        IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(StrmInfo.SourceYuvStrmId))? (UINT8)1U: (UINT8)0U;
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        IsEncPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;

        if (IsEncPurpose > 0U) {
            /* Fetch first chan as SourceViewZone */
            if (1U == IsEfctYuvStrm) {
#ifdef PPSTRM_SWITCH_BETWEEN_PASSTHROUGH_AND_BLENDING
                ViewZoneId = pYuvStrmLayout->EffectBufDesc.LastPsThIdx;
#else
                DSP_ReverseBit2U16Idx(YuvStrmInfo->MaxChanBitMask, &ViewZoneId);
#endif
            } else {
                ViewZoneId = pYuvStrmLayout->ChanCfg[0U].ViewZoneId;
            }

            HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
            for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
                if (DSP_GetBit(VprocInfo.PinUsage[i], StrmInfo.SourceYuvStrmId, 1U) > 0U) {
                    PrevId = i;
                    break;
                }
            }
            PinId = HL_CtxVprocPinDspPinMap[PrevId];

            if (StrmInfo.VideoTimeLapseActive > 0U) {
                Rval = HL_VidEncCfgPreProcOnTimeLapse(WriteMode, StrmInfo.TimeLapseId, ViewZoneId, StreamIdx, PinId);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("HL_VidEncCfgPreProcOnTimeLapse %u %x strm:%u",
                            __LINE__, Rval, StreamIdx, 0U, 0U);
                }
            } else {
//FIXME, TBD YuvEnc
                Rval = HL_VidEncCfgPreProcOnYuvStrm(WriteMode, StrmInfo.TimeLapseId, ViewZoneId, StreamIdx, PinId);
            }

            if (Rval == OK) {
                WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

                (void)HL_GetReconPostStatus(StreamIdx, &ReconPostStatus);
                if (1U == DSP_GetU16Bit(ReconPostStatus, RECON_POST_2_VPROC_BIT, 1U)) {
                    Rval = HL_ReconPostStart(WriteMode, StreamIdx);
                }
            }
        } else {
            // DO NOTHING
        }
    }

    if (Rval == OK) {
        if (StrmInfo.CfgStatus == ENC_STRM_CFG_UPDATE) {
            HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
            if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_MJPG) {
                Rval = HL_FillVidEncCfgCmdOnJpg(WriteMode, &StrmInfo, StrmNum, StreamIdx);
            } else {
                Rval = HL_FillVidEncCfgCmd(WriteMode, &StrmInfo, StrmNum, StreamIdx);
            }
            HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
        }
    }

    return Rval;
}

static inline UINT32 HL_FillVidEncStartCmd(cmd_encoder_start_t *pEncStart,
                                           const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                           const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval;

    /* start offset/length */
    if ((pStartConfig->EncDuration == 0U) || (pStartConfig->EncDuration == 0xFFFFFFFFU)) {
        pEncStart->encode_duration = 0U;
    } else {
        pEncStart->encode_duration = pStartConfig->EncDuration;
    }
    if ((pStartConfig->EncSkipFrame == 0U) || (pStartConfig->EncSkipFrame == 0xFFFFFFFFU)) {
        pEncStart->start_encode_frame_no = 0U;
    }

    /* header */
    pEncStart->gaps_in_frame_num_value_allowed_flag = 0U;
    pEncStart->force_annexb_long_start_code = 0U;
    if (pStrmInfo->AUDMode == 1U) {
        pEncStart->au_type = 3U;
    } else if (pStrmInfo->AUDMode == 2U) {
        pEncStart->au_type = 1U;
    } else {
        pEncStart->au_type = 0U;
    }

#if 0 //FIXME
    pEncStart->set_pic_order_cnt_type_is2 = (StrmInfo.PicOrderCntType == 1U)? 1U: 0U;
#endif
    Rval = HL_VideoEnc_FillCrop(&pEncStart->frame_crop, &pStrmInfo->FrameCrop, pStrmInfo->CodingFmt, &pStrmInfo->Window);
    if (Rval == OK) {
        (void)HL_VideoEnc_FillVui(&pEncStart->h264_vui, &pStrmInfo->Vui);

        pEncStart->h264_vui.num_units_in_tick = pStrmInfo->FrameRate.NumUnitsInTick;
        /* Base on H264 Spec, decoder will multiply NumUnitsInTick by 2 to calculate frame rate so we need to multiply TimeScale by 2 in advance */
        if ((pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) && (pStrmInfo->FrameRate.Interlace == 0U)) {
            pEncStart->h264_vui.time_scale = pStrmInfo->FrameRate.TimeScale << 1U;
        } else {
            pEncStart->h264_vui.time_scale = pStrmInfo->FrameRate.TimeScale;
        }
        pEncStart->h264_vui.vbr_cbp_rate = 0U; /* obsolete after uCode svn#279409 */

        /* IntraRefresh */
#if 0 //FIXME
        if (pStrmInfo->GOPCfg.N == ENC_GOP_INFINTE_N) {
            if (pStrmInfo->IntraRefreshCtrl.IntraRefreshCycle > ENC_MAX_IR_CYCLE) {
                pEncStart->sps_pps_insertion_rate = ENC_MAX_IR_CYCLE;
            } else {
                pEncStart->sps_pps_insertion_rate = pStrmInfo->IntraRefreshCtrl.IntraRefreshCycle;
            }
        }
#endif
    }

//FIXME, not used
    pEncStart->stream_type = 0;//Not used

    return Rval;
}

static inline UINT32 HL_VidEncStartFromVin(UINT16 NumStream,
                                           const UINT16 *pStreamIdx,
                                           UINT32 VinAttachMask,
                                           const UINT32 *pVinBitOnStrm,
                                           UINT8 *pVinAttachIssued)
{
    UINT32 Rval = OK;
    UINT8 WriteMode;
    UINT16 i, j, k;
    CTX_STREAM_INFO_s StrmInfo;
    UINT16 WaitEventVinNum = 0U;
    UINT16 WaitEventVinIdx[AMBA_DSP_MAX_VIN_NUM] = {0U};
    UINT8 LocalViewZoneNum;
    UINT32 EventMask;

    if (VinAttachMask > 0U) {
        for (j = 0U; j < AMBA_DSP_MAX_VIN_NUM; j++) {
            WriteMode = HL_GetVin2CmdNormalWrite((UINT16)j);

            if (DSP_GetBit(VinAttachMask, j, 1U) == 0U) {
                continue;
            }
            for (i = 0U; i < NumStream; i++) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;
                EventMask = 0U;

                if (DSP_GetBit(pVinBitOnStrm[i], j, 1U) == 0U) {
                    continue;
                }

                HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
                VinAttachEvent->vin_id = (UINT8)j;
                LocalViewZoneNum = (UINT8)HL_GetViewZoneNumOnVin(j);
                for (k = 0U; k<LocalViewZoneNum; k++) {
                    DSP_SetBit(&VinAttachEvent->fov_mask_valid, k);
                }

                if (pVinAttachIssued[j] == 0U) {
                    Rval = HL_GetVinAttachEventDefaultMask((UINT16)j,  &EventMask);
                    if (Rval == OK) {
                        DSP_SetBit(&EventMask, DSP_BIT_POS_ENC_START);
                        pVinAttachIssued[j] = 1U;
                    }
                }
                if (Rval == OK) {
                    if (EventMask > 0U) { //sanity check
                        (void)dsp_osal_memcpy(&VinAttachEvent->event_mask, &EventMask, sizeof(UINT32));
                        Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
                    }
                }
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval != OK) {
                    break;
                }
            }
        }

        if (Rval == OK) {
            /* We need to wait for a VCAP event interrupt to make sure the VinAttachEvent cmd is sent before stoping YUV capture */
            DSP_Bit2U16Cnt(VinAttachMask, &WaitEventVinNum);
            j = 0U;
            for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
                if (1U == DSP_GetBit(VinAttachMask, i, 1U)) {
                    WaitEventVinIdx[j] = i;
                    j++;
                }
            }
            Rval = DSP_WaitVcapEvent(WaitEventVinNum, WaitEventVinIdx, 1, WAIT_FLAG_TIMEOUT);
        }
    }

    return Rval;
}

static inline void HL_VideoStrmStartChkEnd(UINT16 NumStream, const UINT16 *pStreamIdx, UINT32 VinActiveBit,
                                             UINT16 Num, const UINT16 *pCapInstance, UINT32 VinAttachMask,
                                             const UINT32 *pVinBitOnStrm, UINT8 *pVinAttachIssued, UINT32 *pRval)
{
    UINT16 VinId;
    UINT8 WriteMode;

    if (*pRval == OK) {
    /* Wait a VDSP event to ensure EncoderSetup has been revceived */
#ifdef SUPPORT_ENC_VINATTACH_1FRM_DLY
        *pRval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);
#else
        *pRval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 3, WAIT_INTERRUPT_TIMEOUT);
#endif

        if (*pRval == OK) {
            for (VinId = 0U; VinId < AMBA_DSP_MAX_VIN_NUM; VinId++) {
                if (DSP_GetBit(VinActiveBit, VinId, 1U)==1U) {
                    WriteMode = HL_GetVin2CmdGroupWrite(VinId);
                    /* Process Encode start in the same cmd Q */
                    *pRval = DSP_ProcessBufCmds(WriteMode);
                }
                if (*pRval != OK) {
                    break;
                }
            }

            if (*pRval == OK) {
                if (Num > 0U) { //sanity check
                    *pRval = HL_StillCaptureYuvImpl(Num, pCapInstance);
                } else {
                    //FIXME, VinAttach + SyncedYuv case
                    // VinAttache once no SyncYuv needed
                    *pRval = HL_VidEncStartFromVin(NumStream,
                                                pStreamIdx,
                                                VinAttachMask,
                                                pVinBitOnStrm,
                                                pVinAttachIssued);
                }
            }
        }
    }
}

//FIXME, TimeLapse condition
static inline UINT32 HL_VideoStreamStart(UINT16 NumStream,
                                         const UINT16 *pStreamIdx,
                                         const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT8 EncState = 0U, IsEncodeBusy = 0U;
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE;
    UINT8 CmdBufferIdEncStart = 0U; void *CmdBufferAddrEncStart = NULL;
    UINT8 CmdBufferIdEncQt = 0U; void *CmdBufferAddrEncQt = NULL;
    UINT8 VinAttachIssued[AMBA_DSP_MAX_VIN_NUM];
    UINT16 i, j;
    UINT32 VinActiveBit = 0x0;
    UINT32 VinBitOnStrm[AMBA_DSP_MAX_STREAM_NUM];
    UINT32 U32Value;
    UINT32 Rval = OK;
    UINT32 VinAttachMask = 0U;
    CTX_STREAM_INFO_s StrmInfo;
    CTX_DATACAP_INFO_s DataCapInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_encoder_start_t *pEncStart = HL_DefCtxCmdBufPtrEncStart;
    encoder_realtime_setup_t *pEncQualitySetup = HL_DefCtxCmdBufPtrEncRt;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

//FIXME, Misra
    (void)pAttachedRawSeq;

    /* Initialize */
    (void)dsp_osal_memset(&VinBitOnStrm[0U], 0, sizeof(UINT32)*AMBA_DSP_MAX_STREAM_NUM);
    (void)dsp_osal_memset(&VinAttachIssued[0U], 0, sizeof(UINT8)*AMBA_DSP_MAX_VIN_NUM);

    for (i = 0U; i < NumStream; i++) {
        EncState = DSP_GetEncState(pStreamIdx[i]);
        if (EncState == DSP_ENC_STATUS_BUSY) {
            IsEncodeBusy = 1U;
            break;
        }
    }

    if (IsEncodeBusy == 1U) {
        Rval = DSP_ERR_0004;
    } else {
        UINT16 Num = 0U;
        UINT16 CapInst[AMBA_DSP_MAX_DATACAP_NUM] = {0};
        UINT16 VinId;

        for (i = 0U; i < NumStream; i++) {

            HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
            if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
//FIXME
                WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
            } else {
                HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
                pYuvStrmLayout = &YuvStrmInfo->Layout;
                HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &ViewZoneInfo);

                /* single yuv stream from VIN_SRC_FROM_DEC use vdsp */
                if ((pYuvStrmLayout->NumChan == 1U) && \
                    (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW)) {
                    WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
                } else {
                    for (j=0; j<pYuvStrmLayout->NumChan; j++) {
                        HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[j].ViewZoneId, &ViewZoneInfo);
                        if ((ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC) &&
                            (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON)) {
                            VinActiveBit |= (UINT32)ViewZoneInfo->SourceVin; // FIXME
                            VinBitOnStrm[i] |= (UINT32)ViewZoneInfo->SourceVin; // FIXME
                        }
                    }

                    /* Take the first channel's sourceVin as master */
                    DSP_Bit2U16Idx((UINT32)VinBitOnStrm[i], &VinId);
                    WriteMode = HL_GetVin2CmdGroupWrite(VinId);

                    if (StrmInfo.StartMethod == DSP_ENC_STOP_FROM_PICINFO) {
                        DSP_SetBit(&VinAttachMask, VinId);
                    }
                }
            }

            HL_AcqCmdBuffer(&CmdBufferIdEncStart, &CmdBufferAddrEncStart);
            dsp_osal_typecast(&pEncStart, &CmdBufferAddrEncStart);
            pEncStart->stream_id = (UINT8)pStreamIdx[i];
            Rval = HL_FillVidEncStartCmd(pEncStart, &pStartConfig[i], &StrmInfo);

            if (Rval == OK) {
                HL_AcqCmdBuffer(&CmdBufferIdEncQt, &CmdBufferAddrEncQt);
                dsp_osal_typecast(&pEncQualitySetup, &CmdBufferAddrEncQt);
                /* Issue Enc Frate Change accompany with Enc Start */
                pEncQualitySetup->stream_id = pEncStart->stream_id;

                pEncQualitySetup->enable_flags |= ENC_PARAM_FRAME_RATE;
                pEncQualitySetup->multiplication_factor = 1U;

                U32Value = 0U;
                HL_VideoEnc_FrameRateDivisor((UINT16)pStreamIdx[i], &U32Value);
                pEncQualitySetup->division_factor = (UINT8)U32Value;

                U32Value = 0U;
                Rval = HL_VideoEnc_ConvertFrameRate(StrmInfo.FrameRate, &U32Value);
                if (Rval == OK) {
                    pEncQualitySetup->enc_frame_rate = U32Value;

                    /* Issue mvdump for AVC */
#ifdef SUPPORT_DSP_MV_DUMP
                    if ((StrmInfo.MvBufBase > 0U) &&
                        (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264)) {
                        pEncQualitySetup->enable_flags |= ENC_PARAM_MVDUMP_ENABLE;
                        pEncQualitySetup->mvdump_enable = (UINT8)1U;
                    }
#endif
                    Rval = AmbaHL_CmdEncoderRealtimeSetup(WriteMode, pEncQualitySetup);

                    /* Issue Enc Start */
                    if (Rval == OK) {
                        Rval = AmbaHL_CmdEncoderStart(WriteMode, pEncStart);
                        if (Rval == OK) {
                            /* Issue SetExtmem for VdoThm */
                            for (j = 0U; j<AMBA_DSP_MAX_DATACAP_NUM; j++) {
                                HL_GetDataCapInfo(HL_MTX_OPT_ALL, j, &DataCapInfo);

                                if ((DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) &&
                                    (DataCapInfo.Status == DATA_CAP_STATUS_2_RUN) &&
                                    (DataCapInfo.Cfg.Index == pStreamIdx[i])) {
                                    CapInst[Num] = j;
                                    Num++;
                                }
                            }
                        }
                    }
                }
                HL_RelCmdBuffer(CmdBufferIdEncQt);
            }
            HL_RelCmdBuffer(CmdBufferIdEncStart);
        }

        HL_VideoStrmStartChkEnd(NumStream, pStreamIdx, VinActiveBit, Num, &CapInst[0U],
                                VinAttachMask, &VinBitOnStrm[0U], &VinAttachIssued[0U], &Rval);

    }

    return Rval;
}

static inline UINT32 HL_VidEncStopFromDecPreProc(const CTX_VID_DEC_INFO_s *pVidDecInfo,
                                                 UINT16 *pDecSrcOnStrm,
                                                 const CTX_STREAM_INFO_s *pStrmInfo,
                                                 UINT16 StrmIdx)
{
    UINT32 Rval;
    UINT8 WriteMode;
    UINT16 PinIdIdx, PrevId = 0U, PinId;
    CTX_VPROC_INFO_s VprocInfo = {0};
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;
    UINT16 VinId;

    WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    *pDecSrcOnStrm = 1U;

    /* unbind decoder's vproc to encoder */
    VinId = pVidDecInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;

    /* Get vrpoc output pin */
    HL_GetVprocInfo(HL_MTX_OPT_ALL, pVidDecInfo->ViewZoneId, &VprocInfo);
    for (PinIdIdx=0U; PinIdIdx<DSP_VPROC_PIN_NUM; PinIdIdx++) {
        if (DSP_GetBit(VprocInfo.PinUsage[PinIdIdx], pStrmInfo->SourceYuvStrmId, 1U) > 0U) {
            PrevId = PinIdIdx;
            break;
        }
    }
    PinId = HL_CtxVprocPinDspPinMap[PrevId];
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindingCfg,
                              BIND_VIDEO_ENCODE_VENC,
                              0U/*IsBind*/,
                              pVidDecInfo->ViewZoneId/*SrcId -> src_ch_id*/,
                              PinId/*SrcPinId*/,
                              StrmIdx/*StrmId -> dst_ch_id*/);
    if (Rval == OK) {
        Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
    } else {
        AmbaLL_LogUInt5("HL_VideoStreamStop %u HL_FillFpBindSetup %x strm:%u",
                __LINE__, Rval, StrmIdx, 0U, 0U);
    }
    HL_RelCmdBuffer(CmdBufferId);

    if (Rval == OK) {
        /* Wait VDSP event to ensure FpBindingCfg has been received */
        Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
        if (Rval == OK) {
            /* bind decoder's virtual vin to encoder */
            WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
            Rval = HL_FillFpBindSetup(FpBindingCfg,
                                      BIND_STILL_ENCODE,
                                      1U/*IsBind*/,
                                      VinId/*SrcId -> src_fp_id*/,
                                      0U/*SrcPinId -> xx*/,
                                      StrmIdx/*StrmId -> dst_ch_id*/);
            if (Rval == OK) {
                Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindingCfg);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[%d]VidStrmStop:CmdDspBindingCfg 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
                }
            } else {
                AmbaLL_LogUInt5("[%d]VidStrmStop:FillFpBindSetup 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
            }
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                /* Wait VDSP event to ensure FpBindingCfg has been received */
                Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
            }
        }
    }

    return Rval;
}

#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
static inline UINT32 HL_VidEncStopFromTimeLapse(const CTX_STREAM_INFO_s *pStrmInfo,
                                                UINT16 StrmIdx,
                                                UINT8 IsExtMemEnc)
#else
static inline UINT32 HL_VidEncStopFromTimeLapse(UINT8 WriteMode,
                                                const CTX_STREAM_INFO_s *pStrmInfo,
                                                UINT16 StrmIdx,
                                                UINT8 IsExtMemEnc)
#endif
{
    UINT32 Rval = OK;
    CTX_TIMELAPSE_INFO_s TimeLapseInfo;
    UINT16 VinId;
    UINT8 IsFirstMem;
    AMBA_DSP_BUF_s ExtBuf = {0};
    AMBA_DSP_BUF_s AuxBuf = {0};
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
    UINT8 WriteMode;
#endif

    HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
    VinId = TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM;
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
    WriteMode = HL_GetVin2CmdNormalWrite(VinId);
#endif
    if (0U == TimeLapseInfo.TotalIssuedMemEncodeNum) {
        IsFirstMem = 1U;
    } else {
        IsFirstMem = 0U;
    }

    (void)dsp_osal_memset(&ExtBuf, 0, sizeof(AMBA_DSP_BUF_s));
    if (IsExtMemEnc == (UINT8)1U) {
        ExtBuf.Pitch         = TimeLapseInfo.LastYuvBuf.Pitch;
        ExtBuf.Window.Width  = TimeLapseInfo.LastYuvBuf.Window.Width;
        ExtBuf.Window.Height = TimeLapseInfo.LastYuvBuf.Window.Height;
    } else {
        Rval = HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId, DSP_DATACAP_CALTYPE_LUMA, &ExtBuf);
    }

    if (Rval == OK) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
        /* Wait until EncStop received */
        (void)DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP1_EVENT, (UINT32)1U, WAIT_FLAG_TIMEOUT);
#endif

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        Rval = HL_LiveviewFillVinExtMem(VinId,
                                        1U/*IsYuvVin2Enc*/,
                                        IsFirstMem,
                                        DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                        DSP_YUV_420 /* ChromaFmt */,
                                        VIN_EXT_MEM_CTRL_WAIT_APPEND,
                                        &ExtBuf,
                                        NULL,
                                        VinExtMem);

        if (Rval == OK) {
            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[%d]VidStrmStop:CmdVinSetExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
            }
        } else {
            AmbaLL_LogUInt5("[%d]VidStrmStop:LiveviewFillVinExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
        }
        HL_RelCmdBuffer(CmdBufferId);

        if (Rval == OK) {
            /* Me1 */
            (void)dsp_osal_memset(&ExtBuf, 0, sizeof(AMBA_DSP_BUF_s));
            (void)dsp_osal_memset(&AuxBuf, 0, sizeof(AMBA_DSP_BUF_s));
            if (IsExtMemEnc == (UINT8)1U) {
                ExtBuf.Pitch         = ALIGN_NUM16((TimeLapseInfo.LastYuvBuf.Pitch >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
                ExtBuf.Window.Width  = ALIGN_NUM16((TimeLapseInfo.LastYuvBuf.Window.Width >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
                ExtBuf.Window.Height = ALIGN_NUM16((TimeLapseInfo.LastYuvBuf.Window.Height >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
                AuxBuf.Pitch         = ALIGN_NUM16((TimeLapseInfo.LastYuvBuf.Pitch >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
                AuxBuf.Window.Width  = ALIGN_NUM16((TimeLapseInfo.LastYuvBuf.Window.Width >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
                AuxBuf.Window.Height = ALIGN_NUM16((TimeLapseInfo.LastYuvBuf.Window.Height >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
            } else {
                Rval = HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId, DSP_DATACAP_CALTYPE_ME1, &ExtBuf);
                if (Rval == OK) {
                    Rval = HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId, DSP_DATACAP_CALTYPE_ME0, &AuxBuf);
                }
            }

            if (Rval == OK) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
                Rval = HL_LiveviewFillVinExtMem(VinId,
                                                1U/*IsYuvVin2Enc*/,
                                                IsFirstMem,
                                                DSP_VIN_EXT_MEM_TYPE_ME /* MemType */,
                                                DSP_YUV_MONO /* ChromaFmt */,
                                                VIN_EXT_MEM_CTRL_WAIT_APPEND,
                                                &ExtBuf,
                                                &AuxBuf,
                                                VinExtMem);
                if (Rval == OK) {
                    Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[%d]VidStrmStop:CmdVinSetExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
                    }
                } else {
                    AmbaLL_LogUInt5("[%d]VidStrmStop:LiveviewFillVinExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
                }
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval == OK) {
                    UINT8 U8Val;
                    UINT8 CmdBufferIdVinFeed = 0U; void *CmdBufferAddrVinFeed = NULL;
                    cmd_vin_send_input_data_t *VinFeedData = HL_DefCtxCmdBufPtrVinSndData;

                    //Send Input Data for Vin
                    HL_AcqCmdBuffer(&CmdBufferIdVinFeed, &CmdBufferAddrVinFeed);
                    dsp_osal_typecast(&VinFeedData, &CmdBufferAddrVinFeed);
                    U8Val = (UINT8)(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM);
                    VinFeedData->vin_id = U8Val;
                    VinFeedData->chan_id = 0U; //obsoleted
                    VinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_ENC;
                    VinFeedData->encode_start_idc = (0U == TimeLapseInfo.TotalIssuedMemEncodeNum)? 1U: 0U;
                    VinFeedData->encode_stop_idc = 1U;
                    VinFeedData->input_data_type = (UINT32)DSP_VIN_SEND_IN_DATA_YUV420; //TBD
                    VinFeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
                    VinFeedData->ext_ce_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
                    VinFeedData->hw_pts = DSP_ENC_DUMMY_MARK; // Dummy frame

                    Rval = AmbaHL_CmdVinSendInputData(WriteMode, VinFeedData);
                    if (Rval == OK) {
                        HL_GetTimeLapseInfo(HL_MTX_OPT_GET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
                        TimeLapseInfo.TotalIssuedMemEncodeNum = 0U;
                        TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_COMPLETE;
                        HL_SetTimeLapseInfo(HL_MTX_OPT_SET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
                    }
                    HL_RelCmdBuffer(CmdBufferIdVinFeed);
                }
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_VidEncStopFromDec(const CTX_VID_DEC_INFO_s *pVidDecInfo,
                                          UINT16 StrmIdx,
                                          UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 VinId;
    UINT32 Rval;
    AMBA_DSP_BUF_s ExtBuf = {0};
    AMBA_DSP_BUF_s AuxBuf = {0};
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

    VinId = pVidDecInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;

    /* pVidDecInfo->LastYuvBuf is updated by AmbaDSP_Event.c: VideoDecodePicPreProcFunc */
    (void)dsp_osal_memset(&ExtBuf, 0, sizeof(AMBA_DSP_BUF_s));
    ExtBuf.BaseAddr       = pVidDecInfo->LastYuvBuf.BaseAddrY;
    ExtBuf.Pitch          = pVidDecInfo->LastYuvBuf.Pitch;
    ExtBuf.Window.Width   = pVidDecInfo->LastYuvBuf.Window.Width;
    ExtBuf.Window.Height  = pVidDecInfo->LastYuvBuf.Window.Height;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
    Rval = HL_LiveviewFillVinExtMem(VinId,
                                    1U /* IsYuvVin2Enc */,
                                    1U /* IsFirstMem */,
                                    DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                    DSP_YUV_420 /* ChromaFmt */,
                                    VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                    &ExtBuf,
                                    NULL,
                                    VinExtMem);
    if (Rval == OK) {
        Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]VidStrmStop:CmdVinSetExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("[%d]VidStrmStop:LiveviewFillVinExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
    }
    HL_RelCmdBuffer(CmdBufferId);

    if (Rval == OK) {
        (void)dsp_osal_memset(&ExtBuf, 0, sizeof(AMBA_DSP_BUF_s));
        (void)dsp_osal_memset(&AuxBuf, 0, sizeof(AMBA_DSP_BUF_s));
        /* pVidDecInfo->LastYuvBuf is updated by AmbaDSP_Event.c: VideoDecodePicPreProcFunc */
        ExtBuf.Pitch          = ALIGN_NUM16((pVidDecInfo->LastYuvBuf.Pitch >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
        ExtBuf.Window.Width   = (UINT16)ALIGN_NUM((pVidDecInfo->LastYuvBufWidth  >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT32)DSP_BUF_ALIGNMENT);
        ExtBuf.Window.Height  = (UINT16)ALIGN_NUM((pVidDecInfo->LastYuvBufHeight >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT32)DSP_BUF_ALIGNMENT);
        AuxBuf.Pitch          = (UINT16)ALIGN_NUM((pVidDecInfo->LastYuvBufWidth  >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT32)DSP_BUF_ALIGNMENT);
        AuxBuf.Window.Width   = (UINT16)ALIGN_NUM((pVidDecInfo->LastYuvBufHeight >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT32)DSP_BUF_ALIGNMENT);
        AuxBuf.Window.Height  = ALIGN_NUM16((pVidDecInfo->LastYuvBuf.Pitch >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        Rval = HL_LiveviewFillVinExtMem(VinId,
                                        1U /* IsYuvVin2Enc */,
                                        1U /* IsFirstMem */,
                                        DSP_VIN_EXT_MEM_TYPE_ME /* MemType */,
                                        DSP_YUV_MONO /* ChromaFmt */,
                                        VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                        &ExtBuf,
                                        &AuxBuf,
                                        VinExtMem);
        if (Rval == OK) {
            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[%d]VidStrmStop:AmbaHL_CmdVinSetExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
            }
        } else {
            AmbaLL_LogUInt5("[%d]VidStrmStop:LiveviewFillVinExtMem 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
        }
        HL_RelCmdBuffer(CmdBufferId);

        if (Rval == OK) {
            UINT8 CmdBufferIdVinFeed = 0U; void *CmdBufferAddrVinFeed = NULL;
            cmd_vin_send_input_data_t *VinFeedData = HL_DefCtxCmdBufPtrVinSndData;

            /* Send Input Data for Vin */
            HL_AcqCmdBuffer(&CmdBufferIdVinFeed, &CmdBufferAddrVinFeed);
            dsp_osal_typecast(&VinFeedData, &CmdBufferAddrVinFeed);
            VinFeedData->vin_id = (UINT8)VinId;
            VinFeedData->chan_id = 0U; //obsoleted
            VinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_ENC;
            VinFeedData->encode_start_idc = 0U;
            VinFeedData->encode_stop_idc = 1U;
            VinFeedData->input_data_type = DSP_VIN_SEND_IN_DATA_YUV420; //TBD
            VinFeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
            VinFeedData->ext_ce_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
            VinFeedData->hw_pts = DSP_ENC_DUMMY_MARK; // Dummy frame

            Rval = AmbaHL_CmdVinSendInputData(WriteMode, VinFeedData);
            HL_RelCmdBuffer(CmdBufferIdVinFeed);
            if ((Rval == OK) && (IsGroupWriteMode(WriteMode) == 1U)) {
                Rval = DSP_ProcessBufCmds(WriteMode);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[%d]VidStrmStop:DSP_ProcessBufCmds 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
                }
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_VidEncStopFromVin(UINT16 NumStream,
                                          const UINT16 *pStreamIdx,
                                          UINT32 VinActiveBit,
                                          const UINT32 *pVinBitOnStrm,
                                          UINT8 *pVinAttachIssued)
{
    UINT32 Rval = OK;
    UINT16 i, j, k;
    UINT8 WriteMode;
    CTX_STREAM_INFO_s StrmInfo;
    UINT8 LocalViewZoneNum;
    UINT32 EventMask;

    for (j=0U; j<AMBA_DSP_MAX_VIN_NUM; j++) {
        WriteMode = HL_GetVin2CmdNormalWrite((UINT16)j);

        if (DSP_GetBit(VinActiveBit, j, 1U) == 0U) {
            continue;
        }
        for (i = 0; i<NumStream; i++) {
            if (DSP_GetBit(pVinBitOnStrm[i], j, 1U) == 0U) {
                continue;
            }

            HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
            if (StrmInfo.StopMethod == DSP_ENC_STOP_FROM_PICINFO/*|| HL_IS_VDOTHM_FIRST_FRAME(VideoThmb[i])*/) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;
                EventMask = 0U;
//FIXME. MultiChan
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
                VinAttachEvent->vin_id = (UINT8)j;
                LocalViewZoneNum = (UINT8)HL_GetViewZoneNumOnVin(j);
                for (k = 0U; k<LocalViewZoneNum; k++) {
                    DSP_SetBit(&VinAttachEvent->fov_mask_valid, k);
                }

                if ((StrmInfo.VideoTimeLapseActive == 0U) && (pVinAttachIssued[j] == 0U)) {
                    Rval = HL_GetVinAttachEventDefaultMask((UINT16)j, &EventMask);
                    if (Rval == OK) {
                        DSP_SetBit(&EventMask, DSP_BIT_POS_ENC_STOP);
                        pVinAttachIssued[j] = 1U;
                    }
                }

                if (Rval == OK) {
                    /* Sanity check */
                    if (EventMask > 0U) {
                        (void)dsp_osal_memcpy(&VinAttachEvent->event_mask, &EventMask, sizeof(UINT32));
                        Rval = AmbaHL_CmdVinAttachEventToRaw(WriteMode, VinAttachEvent);
                    }
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
        //DSP_ProcessBufCmds(WriteMode);
    }

    return Rval;
}

static inline UINT32 HL_VidEncStopPostProc(UINT16 NumStream,
                                           const UINT16 *pStreamIdx,
                                           const UINT16 *pDecSrcOnStrm)
{
    UINT32 Rval = OK;
    UINT16 i;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT16 PinIdIdx, PrevId = 0U, PinId = 0U;
    CTX_VPROC_INFO_s VprocInfo = {0};
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;
    CTX_STREAM_INFO_s StrmInfo;

    for (i = 0U; i<NumStream; i++) {
        /* VIN_SRC_FROM_DEC, bind dec's vproc back after encoder stop*/
        if (pDecSrcOnStrm[i] == 1U) {
            HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);
            Rval = DSP_WaitEncState(pStreamIdx[i], DSP_ENC_STATUS_IDLE, WAIT_FLAG_TIMEOUT);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_VideoStreamStop %u DSP_WaitEncState %x strm:%u", __LINE__, Rval, pStreamIdx[i], 0U, 0U);
            } else {
                /* get vrpoc output pin */
                HL_GetVprocInfo(HL_MTX_OPT_ALL, VidDecInfo.ViewZoneId, &VprocInfo);
                HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
                for (PinIdIdx=0U; PinIdIdx<DSP_VPROC_PIN_NUM; PinIdIdx++) {
                    if (DSP_GetBit(VprocInfo.PinUsage[PinIdIdx], StrmInfo.SourceYuvStrmId, 1U) > 0U) {
                        PrevId = PinIdIdx;
                        break;
                    }
                }
                PinId = HL_CtxVprocPinDspPinMap[PrevId];
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
                Rval = HL_FillFpBindSetup(FpBindingCfg,
                                          BIND_STILL_ENCODE,
                                          0U/*IsBind*/,
                                          VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM/*SrcId -> src_fp_id*/,
                                          0U/*SrcPinId -> xx*/,
                                          pStreamIdx[i]/*StrmId -> dst_ch_id*/);
                if (Rval == OK) {
                    Rval = AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, FpBindingCfg);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[%d]VidStrmStop:AmbaHL_CmdDspBindingCfg 0x%X Strm[%d]", __LINE__, Rval, pStreamIdx[i], 0U, 0U);
                    }
                } else {
                    AmbaLL_LogUInt5("[%d]VidStrmStop:HL_FillFpBindSetup 0x%X Strm[%d]", __LINE__, Rval, pStreamIdx[i], 0U, 0U);
                }
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval == OK) {
                    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);

                    if (Rval == OK) {
                        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                        dsp_osal_typecast(&FpBindingCfg, &CmdBufferAddr);
                        Rval = HL_FillFpBindSetup(FpBindingCfg,
                                                  BIND_VIDEO_ENCODE_VENC,
                                                  1U/*IsBind*/,
                                                  VidDecInfo.ViewZoneId/*SrcId -> src_ch_id*/,
                                                  PinId/*SrcPinId -> xx*/,
                                                  pStreamIdx[i]/*StrmId -> dst_ch_id*/);
                        if (Rval == OK) {
                            Rval = AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindingCfg);
                            if (Rval != OK) {
                                AmbaLL_LogUInt5("[%d]VidStrmStop:CmdDspBindingCfg 0x%X Strm[%d]", __LINE__, Rval, pStreamIdx[i], 0U, 0U);
                            }
                        } else {
                            AmbaLL_LogUInt5("[%d]VidStrmStop:FillFpBindSetup 0x%X Strm[%d]", __LINE__, Rval, pStreamIdx[i], 0U, 0U);
                        }
                        HL_RelCmdBuffer(CmdBufferId);
                        if (Rval == OK) {
                            /* Wait a VDSP event to ensure FpBindingCfg has been received */
                            Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2U, WAIT_INTERRUPT_TIMEOUT);
                            if (Rval == OK) {
                                /* Reset YuvFrameCnt to make next feed yuv use VIN_EXT_MEM_MODE_NEW */
                                HL_GetVidDecInfo(HL_MTX_OPT_GET, 0U, &VidDecInfo);
                                VidDecInfo.YuvFrameCnt = 0U;
                                HL_SetVidDecInfo(HL_MTX_OPT_SET, 0U, &VidDecInfo);
                            }
                        }
                    }
                }
            }
        }
        if (Rval != OK) {
            break;
        }
    }

    return Rval;
}

static inline void HL_VideoStrmStopChkEnd(UINT16 NumStream, const UINT16 *pStreamIdx, UINT32 VinActiveBit,
                                            const UINT32 *pVinBitOnStrm, UINT8 *pVinAttachIssued, const UINT16 *pDecSrcOnStrm, UINT32 *pRval)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE;
    UINT32 VinId;

    if (*pRval == OK) {
        /* Process for physical vin */
        for (VinId=0; VinId<AMBA_DSP_MAX_VIN_NUM; VinId++) {
            if (DSP_GetBit(VinActiveBit, VinId, 1U)==1U) {
                WriteMode = HL_GetVin2CmdGroupWrite((UINT16)VinId);
                /* wait one VDSP event to prevent multiple EncStart taking effect on different frame */
                *pRval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP1_EVENT, (UINT32)1U, WAIT_FLAG_TIMEOUT);
                if (*pRval == OK) {
                    /* Process Encode start in the same cmd Q */
                    *pRval = DSP_ProcessBufCmds(WriteMode);
                    if (*pRval != OK) {
                        AmbaLL_LogUInt5("[%d]VidStrmStop:ProcessBufCmds 0x%X VinId[%d]", __LINE__, *pRval, VinId, 0U, 0U);
                    }
                }
            }
            if (*pRval != OK) {
                break;
            }
        }

        if (*pRval == OK) {
            /* VideoEncode stop From Vin Cmd */
            *pRval = HL_VidEncStopFromVin(NumStream, pStreamIdx, VinActiveBit, pVinBitOnStrm, pVinAttachIssued);
            if (*pRval == OK) {
                /* VideoEncode stop postproc */
                *pRval = HL_VidEncStopPostProc(NumStream, pStreamIdx, pDecSrcOnStrm);
            }
        }
    }
}

//FIXME, TimeLapse condition
static inline UINT32 HL_VideoStreamStop(const UINT16 NumStream,
                                        const UINT16 *pStreamIdx,
                                        const UINT8 *pStopOption,
                                        const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, i, j;
    UINT8 EncState;
    UINT32 VinActiveBit = 0x0;
    UINT32 VinBitOnStrm[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 DecSrcOnStrm[AMBA_DSP_MAX_STREAM_NUM];
    UINT8 VinAttachIssued[AMBA_DSP_MAX_VIN_NUM];
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE, IsExtMemEnc = (UINT8)0U;
    CTX_STREAM_INFO_s StrmInfo;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

//FIXME, StreamSync
//FIXME, Misra
    (void)pAttachedRawSeq;

    /* Initialize */
//FIXME, DecSrcOnStrm, with pip
//FIXME, DecSrcOnStrm, feed yuv at begin, frame size not match
//FIXME, DecSrcOnStrm, dec -> feed yuv -> stop
    (void)dsp_osal_memset(&VinBitOnStrm[0U], 0, sizeof(UINT32)*AMBA_DSP_MAX_STREAM_NUM);
    (void)dsp_osal_memset(&DecSrcOnStrm[0U], 0, sizeof(UINT16)*AMBA_DSP_MAX_STREAM_NUM);
    (void)dsp_osal_memset(&VinAttachIssued[0U], 0, sizeof(UINT8)*AMBA_DSP_MAX_VIN_NUM);
    HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);

    for (i = 0U; i<NumStream; i++) {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
        if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
            IsExtMemEnc = (UINT8)1U;
//FIXME
            WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
        } else {
            UINT16 YuvStrmLayoutNumChan;
            UINT16 ViewZoneInfoInputFromMemory;

            IsExtMemEnc = (UINT8)0U;

            HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
            pYuvStrmLayout = &YuvStrmInfo->Layout;
            YuvStrmLayoutNumChan = pYuvStrmLayout->NumChan;

            HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &ViewZoneInfo);
            ViewZoneInfoInputFromMemory = ViewZoneInfo->InputFromMemory;

            /* VIN_SRC_FROM_DEC */
            if ((YuvStrmLayoutNumChan == 1U) && \
                (ViewZoneInfoInputFromMemory == VIN_SRC_FROM_DEC)) {
                HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);
                Rval = HL_VidEncStopFromDecPreProc(&VidDecInfo, &DecSrcOnStrm[i], &StrmInfo, pStreamIdx[i]);
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
                WriteMode = HL_GetVin2CmdNormalWrite(VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
#else
                WriteMode = HL_GetVin2CmdGroupWrite(VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM);
#endif
            /* other yuv stream use vdsp */
            } else if ((YuvStrmLayoutNumChan == 1U) && \
                (ViewZoneInfoInputFromMemory != VIN_SRC_FROM_HW)) {
                WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
            } else {
                UINT16 CurrentVinId = 0U;
                for (j=0; j<YuvStrmLayoutNumChan; j++) {
                    HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[j].ViewZoneId, &ViewZoneInfo);
                    if ((ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC) &&
                        (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON)) {
                        VinActiveBit |= (UINT32)ViewZoneInfo->SourceVin; //FIXME
                        VinBitOnStrm[i] |= (UINT32)ViewZoneInfo->SourceVin; //FIXME
                    }
                }

                /* Take the first channel's sourceVin as master */
                DSP_Bit2U16Idx((UINT32)VinBitOnStrm[i], &CurrentVinId);
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
                WriteMode = HL_GetVin2CmdNormalWrite(CurrentVinId);
#else
                WriteMode = HL_GetVin2CmdGroupWrite(CurrentVinId);
#endif
            }
        }

        EncState = DSP_GetEncState(pStreamIdx[i]);
        if (EncState == DSP_ENC_STATUS_BUSY) {
            UINT8 CmdBufferId = 0U;
            void *CmdBufferAddr = NULL;
            cmd_encoder_stop_t *pEncStop = HL_DefCtxCmdBufPtrEncStop;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&pEncStop, &CmdBufferAddr);
            pEncStop->channel_id = (UINT8)pStreamIdx[i];
            pEncStop->stop_method = pStopOption[i];
            StrmInfo.CfgStatus = ENC_STRM_CFG_UPDATE;
            HL_SetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
            Rval = AmbaHL_CmdEncoderStop(WriteMode, pEncStop);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[%d]VidStrmStop:CmdEncoderStop 0x%X strm[%d]", __LINE__, Rval, pStreamIdx[i], 0U, 0U);
            }
        } else {
            Rval = DSP_ERR_0004;
        }

        if (Rval == OK) {
            if ((StrmInfo.VideoTimeLapseActive == 1U) && (StrmInfo.TimeLapseId != DSP_TIMELAPSE_IDX_INVALID)) {
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
                Rval = HL_VidEncStopFromTimeLapse(&StrmInfo, pStreamIdx[i], IsExtMemEnc);
#else
                Rval = HL_VidEncStopFromTimeLapse(WriteMode, &StrmInfo, pStreamIdx[i], IsExtMemEnc);
#endif
            }

            if (Rval == OK) {
                /* VIN_SRC_FROM_DEC, feed last frame to trigger encoder stop */
                if (DecSrcOnStrm[i] == 1U) {
                    Rval = HL_VidEncStopFromDec(&VidDecInfo, pStreamIdx[i], WriteMode);
                }
            }
        }
        if (Rval != OK) {
            break;
        }
    }
    HL_VideoStrmStopChkEnd(NumStream, pStreamIdx, VinActiveBit, &VinBitOnStrm[0], &VinAttachIssued[0], &DecSrcOnStrm[0], &Rval);

    return Rval;
}

static inline UINT32 HL_VideoEncConfigChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncConfig(NumStream, pStreamIdx, pStreamConfig);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncConfig(NumStream, pStreamIdx, pStreamConfig);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncConfig(NumStream, pStreamIdx, pStreamConfig);
    }

    return Rval;
}

static UINT32 HL_VideoEncConfig1stHalf(const UINT32 i,
                                       const UINT16 *pStreamIdx,
                                       UINT32 *pErrLine,
                                       CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    if (1U == DSP_GetU8Bit(Resource->MaxStrmFmt[pStreamIdx[i]], 7U/*TimeLapse*/ , 1U)) {
        if (pStrmInfo->TimeLapseId == DSP_TIMELAPSE_IDX_INVALID) {
            UINT16 TimeLapseId = 0U;

            Rval = HL_ReqTimeLapseId(pStreamIdx[i], &TimeLapseId);
            if (Rval != OK) {
                *pErrLine = __LINE__;
            } else {
                pStrmInfo->TimeLapseId = (UINT8)TimeLapseId;
            }
        }
    } else {
        if (pStrmInfo->TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
            Rval = HL_FreeTimeLapseId(pStrmInfo->TimeLapseId);
            if (Rval != OK) {
                *pErrLine = __LINE__;
            } else {
                pStrmInfo->TimeLapseId = DSP_TIMELAPSE_IDX_INVALID;
            }
        }
    }

    return Rval;
}

/**
* VideoEncode config function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pStreamConfig stream configuration information
* @return ErrorCode
*/
UINT32 dsp_video_enc_cfg(UINT16 NumStream,
                         const UINT16 *pStreamIdx,
                         const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval, i, ErrLine = 0U;
    CTX_STREAM_INFO_s StrmInfo;
    UINT8 CalcLevelIdc = 0U, CalcTierIdc = 0U;
    UINT8 AffinityEng;
    UINT16 EngId = 0U;

    Rval = HL_VideoEncConfigChk(NumStream, pStreamIdx, pStreamConfig);

    /* Body */
    if (Rval == OK) {
        /* VideoEncode or Xcode*/
        UINT8 DspProfState = DSP_GetProfState();

        if (DspProfState == DSP_PROF_STATUS_CAMERA) {
            for (i=0U; i<NumStream; i++) {
                HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);

                StrmInfo.CodingFmt = pStreamConfig[i].CodingFmt;
                StrmInfo.SourceYuvStrmId = pStreamConfig[i].SrcYuvStreamId; //Update

                (void)dsp_osal_memcpy(&StrmInfo.Window, &pStreamConfig[i].Window, sizeof(AMBA_DSP_WINDOW_s));
                (void)dsp_osal_memcpy(&StrmInfo.FrameRate, &pStreamConfig[i].FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

                StrmInfo.Rotate = pStreamConfig[i].Rotate;
                StrmInfo.VideoTimeLapseActive = pStreamConfig[i].IntervalCapture;
                StrmInfo.Affinity = pStreamConfig[i].Affinity;
                AffinityEng = (UINT8)DSP_GetBit(StrmInfo.Affinity, ENC_AFFINITY_ENG_BIT_IDX, ENC_AFFINITY_ENG_LEN);
                DSP_U8Bit2U16Idx(AffinityEng, &EngId);

                Rval = HL_VideoEncConfig1stHalf(i, pStreamIdx, &ErrLine, &StrmInfo);

                if (Rval == OK) {
                    StrmInfo.ProfileIDC = pStreamConfig[i].EncConfig.ProfileIDC;
                    (void)HL_GetLevelIdc(&pStreamConfig[i], &CalcLevelIdc, &CalcTierIdc);
                    if ((pStreamConfig[i].EncConfig.LevelIDC > 0U) &&
                        (pStreamConfig[i].EncConfig.LevelIDC < CalcLevelIdc)) {
                        AmbaLL_LogUInt5("LevelIDC[%d] adjust to [%d]", pStreamConfig[i].EncConfig.LevelIDC, CalcLevelIdc, 0U, 0U, 0U);
                        StrmInfo.LevelIDC = CalcLevelIdc;
                    } else if (pStreamConfig[i].EncConfig.LevelIDC == 0U) { /* Use Ambarella default setting */
                        StrmInfo.LevelIDC = (UINT8)0U;
                    } else {
                        StrmInfo.LevelIDC = pStreamConfig[i].EncConfig.LevelIDC;
                    }

                    if (StrmInfo.LevelIDC != (UINT8)0U) {
                        StrmInfo.TierIDC = CalcTierIdc;
                    }
                    StrmInfo.IsCabac = pStreamConfig[i].EncConfig.IsCabac;

                    (void)dsp_osal_memcpy(&StrmInfo.GOPCfg, &pStreamConfig[i].EncConfig.GOPCfg, sizeof(AMBA_DSP_VIDEO_GOP_CONFIG_s));
                    StrmInfo.NumPRef = pStreamConfig[i].EncConfig.NumPRef;
                    StrmInfo.NumBRef = pStreamConfig[i].EncConfig.NumBRef;
                    StrmInfo.FirstGOPStartB = pStreamConfig[i].EncConfig.FirstGOPStartB;
                    (void)dsp_osal_memcpy(&StrmInfo.DeblockFilter, &pStreamConfig[i].EncConfig.DeblockFilter, sizeof(AMBA_DSP_VIDEO_DEBLOCK_CTRL_s));
                    (void)dsp_osal_memcpy(&StrmInfo.FrameCrop, &pStreamConfig[i].EncConfig.FrameCrop, sizeof(AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s));
                    (void)dsp_osal_memcpy(&StrmInfo.Vui, &pStreamConfig[i].EncConfig.Vui, sizeof(AMBA_DSP_VIDEO_ENC_VUI_s));
                    (void)dsp_osal_memcpy(&StrmInfo.BitRateCfg, &pStreamConfig[i].EncConfig.BitRateCfg, sizeof(AMBA_DSP_VIDEO_BIT_RATE_CFG_s));

                    StrmInfo.BitsBufAddr = pStreamConfig[i].EncConfig.BitsBufAddr;
                    StrmInfo.BitsBufSize = pStreamConfig[i].EncConfig.BitsBufSize;
                    if ((StrmInfo.BitsBufAddr > 0U) && (StrmInfo.BitsBufSize > 0U)) {
                        LL_SetMsgDispatcherBitsInfo(EngId, StrmInfo.BitsBufAddr, StrmInfo.BitsBufSize);
                    }

                    StrmInfo.NumSlice = pStreamConfig[i].EncConfig.NumSlice;
                    StrmInfo.NumTile = pStreamConfig[i].EncConfig.NumTile;
                    StrmInfo.NumExtraRecon = pStreamConfig[i].EncConfig.NumExtraRecon;

                    (void)dsp_osal_memcpy(&StrmInfo.IntraRefreshCtrl, &pStreamConfig[i].EncConfig.IntraRefreshCtrl, sizeof(AMBA_DSP_VIDEO_INTRA_REFRESH_CTRL_s));;

                    StrmInfo.PicOrderCntType = pStreamConfig[i].EncConfig.PicOrderCntType;
                    StrmInfo.SPSPPSHeaderInsertMode = pStreamConfig[i].EncConfig.SPSPPSHeaderInsertMode;
                    StrmInfo.AUDMode = pStreamConfig[i].EncConfig.AUDMode;
                    StrmInfo.NonRefPIntvl = pStreamConfig[i].EncConfig.NonRefPIntvl;

                    StrmInfo.QuantMatrixAddr = pStreamConfig[i].EncConfig.QuantMatrixAddr;
                    (void)dsp_osal_memcpy(&StrmInfo.JpgBitRateCtrl, &pStreamConfig[i].EncConfig.JpgBitRateCtrl, sizeof(AMBA_DSP_JPEG_BIT_RATE_CTRL_s));

                    StrmInfo.IsMono = (UINT8)DSP_GetU8Bit(pStreamConfig->EncConfig.EncOption, ENC_OPT_MONO_BIT_IDX, ENC_OPT_MONO_LEN);
                    StrmInfo.IsLowDlyQ = (UINT8)DSP_GetU8Bit(pStreamConfig->EncConfig.EncOption, ENC_OPT_LDQ_BIT_IDX, ENC_OPT_LDQ_LEN);

                    StrmInfo.CfgStatus = ENC_STRM_CFG_UPDATE;
                    HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                } else {
                    break;
                }
            }
        } else if (DspProfState == DSP_PROF_STATUS_XCODE) {
            //TBD
            //_Xcode_VideoEncStreamConfig
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else {
            //TBD
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncConfig Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncStartChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncStart(NumStream, pStreamIdx, pStartConfig, pAttachedRawSeq);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncStart(NumStream, pStreamIdx, pStartConfig, pAttachedRawSeq);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncStart(NumStream, pStreamIdx, pStartConfig, pAttachedRawSeq);
    }

    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoEncStart(NumStream, pStreamIdx, pStartConfig, pAttachedRawSeq);
    }

    return Rval;
}

/**
* VideoEncode start function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pStartConfig start configuration
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_video_enc_start(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval, i, StrmBitMask = 0U, ErrLine = 0U;

    Rval = HL_VideoEncStartChk(NumStream, pStreamIdx, pStartConfig, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        /* VideoEncode or Xcode*/
        UINT8 ProfState = DSP_GetProfState();

        if (ProfState == DSP_PROF_STATUS_CAMERA) {
            /* Filling BitMask */
            for (i = 0U; i < NumStream; i++) {
                DSP_SetBit(&StrmBitMask, pStreamIdx[i]);
            }

            /* Stream config */
            for (i = 0U; i < NumStream; i++) {
                Rval = HL_VideoStreamConfig(pStreamIdx[i], StrmBitMask);
                if (Rval != OK) {
                    break;
                }
            }

            /* Stream start */
            if (Rval == OK) {
                Rval = HL_VideoStreamStart(NumStream, pStreamIdx, pStartConfig, pAttachedRawSeq);
                if (Rval != OK) {
                    ErrLine = __LINE__;
                }
            }
        } else if (ProfState == DSP_PROF_STATUS_XCODE) {
            //TBD
            //_Xcode_VideoEncStreamConfig
            //_Xcode_VideoEncStart
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else {
            //TBD
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncStart Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncStopChk(const UINT16 NumStream,
                                        const UINT16 *pStreamIdx,
                                        const UINT8 *pStopOption,
                                        const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    //FIXME, Misra
    (void)pAttachedRawSeq;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncStop(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncStop(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncStop(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);
    }

    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoEncStop(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);
    }

    return Rval;
}

/**
* VideoEncode stop function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pStopOption stop option
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_video_enc_stop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval, ErrLine = 0U;

    Rval = HL_VideoEncStopChk(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
        /* VideoEncode or Xcode*/
        UINT8 ProfState = DSP_GetProfState();

        if (ProfState == DSP_PROF_STATUS_CAMERA) {
            /* Stream stop */
            Rval = HL_VideoStreamStop(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);
        } else if (ProfState == DSP_PROF_STATUS_XCODE) {
            //TBD
            //_Xcode_VideoEncStop
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        } else {
            //TBD
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncStop Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncControlFrameRateChk(const UINT16 NumStream,
                                                    const UINT16 *pStreamIdx,
                                                    const UINT32 *pDivisor,
                                                    const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    (void)pAttachedRawSeq;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncControlFrameRate(NumStream, pStreamIdx, pDivisor, pAttachedRawSeq);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncControlFrameRate(NumStream, pStreamIdx, pDivisor, pAttachedRawSeq);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncControlFrameRate(NumStream, pStreamIdx, pDivisor, pAttachedRawSeq);
    }

    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoEncControlFrameRate(NumStream, pStreamIdx, pDivisor, pAttachedRawSeq);
    }

    return Rval;
}

/**
* VideoEncode frame-rate control function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pDivisor frame-rate divisor
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_video_enc_ctrl_framerate(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, UINT64 *pAttachedRawSeq)
{
    UINT8 CmdBufferIdEncQt = 0U; void *CmdBufferAddrEncQt = NULL;
    UINT16 i;
    UINT32 Rval, ErrLine = 0U;
    UINT32 U32Value;
    CTX_STREAM_INFO_s StrmInfo;
    encoder_realtime_setup_t *pEncQualitySetup = HL_DefCtxCmdBufPtrEncRt;
    AMBA_DSP_FRAME_RATE_s FrmRate;

    Rval = HL_VideoEncControlFrameRateChk(NumStream, pStreamIdx, pDivisor, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {
//FIXME, issued before encode started or accompany to encode start
//       flag take effect in which frm?
        UINT8 DspProfState = DSP_GetProfState();
        UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

        if (DspProfState == DSP_PROF_STATUS_CAMERA) {
            for (i = 0U; i < NumStream; i++) {
                HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
                HL_AcqCmdBuffer(&CmdBufferIdEncQt, &CmdBufferAddrEncQt);
                dsp_osal_typecast(&pEncQualitySetup, &CmdBufferAddrEncQt);

                pEncQualitySetup->stream_id = (UINT8)pStreamIdx[i];

                pEncQualitySetup->enable_flags |= ENC_PARAM_FRAME_RATE;
                pEncQualitySetup->multiplication_factor = 1U;

                U32Value = 0U;
                HL_VideoEnc_FrameRateDivisor((UINT16)pEncQualitySetup->stream_id, &U32Value);
                if (pDivisor[i] > 1U) {
                    U32Value *= pDivisor[i];
                }
                pEncQualitySetup->division_factor = (UINT8)U32Value;

                U32Value = 0U;
                (void)dsp_osal_memcpy(&FrmRate, &StrmInfo.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
                if (pDivisor[i] > 1U) {
                    FrmRate.TimeScale /= pDivisor[i];
                }
                Rval = HL_VideoEnc_ConvertFrameRate(FrmRate, &U32Value);
                if (Rval != OK) {
                    ErrLine = __LINE__;
                } else {
                    pEncQualitySetup->enc_frame_rate = U32Value;
                    Rval = AmbaHL_CmdEncoderRealtimeSetup(WriteMode, pEncQualitySetup);
                    if (Rval != OK) {
                        ErrLine = __LINE__;
                    } else {
                        HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);
                        StrmInfo.FrmRateDivisor = pDivisor[i];
                        HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                    }
                }
                HL_RelCmdBuffer(CmdBufferIdEncQt);
                if (Rval != OK) {
                    break;
                }
            }
        } else {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncControlFrameRate Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncControlRepeatDropChk(const UINT16 NumStream,
                                                     const UINT16* pStreamIdx,
                                                     const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg,
                                                     const UINT64* pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    (void)pAttachedRawSeq;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncControlRepeatDrop(NumStream, pStreamIdx, pRepeatDropCfg, pAttachedRawSeq);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncControlRepeatDrop(NumStream, pStreamIdx, pRepeatDropCfg, pAttachedRawSeq);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncControlRepeatDrop(NumStream, pStreamIdx, pRepeatDropCfg, pAttachedRawSeq);
    }

    return Rval;
}

/**
* VideoEncode Drop-repeat control function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pUpSamplingRate upsampling rate information
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_video_enc_ctrl_repeat_drop(UINT16 NumStream, const UINT16* pStreamIdx, const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg, const UINT64* pAttachedRawSeq)
{
    UINT32 Rval;

    (void)pAttachedRawSeq;

    Rval = HL_VideoEncControlRepeatDropChk(NumStream, pStreamIdx, pRepeatDropCfg, pAttachedRawSeq);

    /* Body */
    if (Rval == OK) {

    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncControlRepeatDrop Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* VideoEncode slowshutter control function
* @param [in]  NumVin number of vin
* @param [in]  pVinIdx Vin index
* @param [in]  pUpSamplingRate upsampling rate information
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_video_enc_ctrl_slowshutter(UINT16 NumVin,
                                      const UINT16 *pVinIdx,
                                      const UINT32 *pUpSamplingRate,
                                      const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK;

    (void)NumVin;
    (void)pVinIdx;
    (void)pUpSamplingRate;
    (void)pAttachedRawSeq;

    AmbaLL_LogUInt5("AmbaDSP_VideoEncControlSlowShutter is not supported anymore, please use AmbaDSP_LiveviewSlowShutterCtrl", 0U, 0U, 0U, 0U, 0U);

    return Rval;
}

static inline UINT32 HL_VideoEncControlBlendChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncControlBlend(NumStream, pStreamIdx, pBlendCfg);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncControlBlend(NumStream, pStreamIdx, pBlendCfg);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncControlBlend(NumStream, pStreamIdx, pBlendCfg);
    }

    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoEncControlBlend(NumStream, pStreamIdx, pBlendCfg);
    }

    return Rval;
}

/**
* VideoEncode blending control function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pBlendCfg blend configuration information
* @return ErrorCode
*/
UINT32 dsp_video_enc_ctrl_blend(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT16 PrevId = 0U;
    UINT32 Rval, i, j, ErrLine = 0U;
    CTX_STREAM_INFO_s StrmInfo;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

    Rval = HL_VideoEncControlBlendChk(NumStream, pStreamIdx, pBlendCfg);

    /* Body */
    if (Rval == OK) {
        UINT8 DspProfState = DSP_GetProfState();
        UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

        if (DspProfState == DSP_PROF_STATUS_CAMERA) {
            /* Blend effect is executed in dram when vproc push data out before MEMD */
            for (i = 0U; i < NumStream; i++) {
                if (pBlendCfg[i].BlendAreaIdx == AMBA_DSP_OSD_BLEND_AREA_EMBED_CODE) {
                    /* Only set EmbedCode */
                    HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);
                    StrmInfo.EmbedCode = pBlendCfg[i].Enable;
                    HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                } else {
                    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                    cmd_vproc_osd_blend_t *OsdBlend = HL_DefCtxCmdBufPtrVpcOsd;

                    HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
                    if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
                        AmbaLL_LogUInt5("No Blend support in TimeLapse[%d]!", pStreamIdx[i], 0U, 0U, 0U, 0U);
                        continue;
                    }
                    HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
                    pYuvStrmLayout = &YuvStrmInfo->Layout;

                    /* Fetch first chan as SourceViewZone */
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&OsdBlend, &CmdBufferAddr);
                    OsdBlend->channel_id = (UINT8)pYuvStrmLayout->ChanCfg[pYuvStrmLayout->NumChan-1U].ViewZoneId;
                    HL_GetVprocInfo(HL_MTX_OPT_ALL, pYuvStrmLayout->ChanCfg[pYuvStrmLayout->NumChan-1U].ViewZoneId, &VprocInfo);
                    for (j=0U; j<DSP_VPROC_PIN_NUM; j++) {
                        if (DSP_GetBit(VprocInfo.PinUsage[j], StrmInfo.SourceYuvStrmId, 1U) > 0U) {
                            PrevId = (UINT16)j;
                            break;
                        }
                    }
                    OsdBlend->stream_id = (UINT8)(HL_CtxVprocPinDspPinMap[PrevId]);

                    OsdBlend->blending_area_id = pBlendCfg[i].BlendAreaIdx;
                    OsdBlend->cap_seq_no = 0U; // start right away
                    OsdBlend->enable = pBlendCfg[i].Enable;

                    (void)dsp_osal_virt2cli(pBlendCfg[i].BlendYuvBuf.BaseAddrY, &OsdBlend->osd_addr_y);
                    (void)dsp_osal_virt2cli(pBlendCfg[i].BlendYuvBuf.BaseAddrUV, &OsdBlend->osd_addr_uv);
                    (void)dsp_osal_virt2cli(pBlendCfg[i].AlphaBuf.BaseAddrY, &OsdBlend->alpha_addr_y);
                    (void)dsp_osal_virt2cli(pBlendCfg[i].AlphaBuf.BaseAddrUV, &OsdBlend->alpha_addr_uv);

                    OsdBlend->osd_pitch = pBlendCfg[i].BlendYuvBuf.Pitch;
                    OsdBlend->osd_width = pBlendCfg[i].BlendYuvBuf.Window.Width;
                    OsdBlend->osd_height = pBlendCfg[i].BlendYuvBuf.Window.Height;
                    OsdBlend->osd_start_x = pBlendCfg[i].BlendYuvBuf.Window.OffsetX;
                    OsdBlend->osd_start_y = pBlendCfg[i].BlendYuvBuf.Window.OffsetY;

                    if (OsdBlend->enable> 0U) {
                        UINT32 IsWAlign = IsAligned16(pBlendCfg[i].BlendYuvBuf.Window.Width);
                        UINT32 IsHAlign = IsAligned16(pBlendCfg[i].BlendYuvBuf.Window.Height);
                        if ((pBlendCfg[i].BlendYuvBuf.Window.Width < 64U) ||
                            (IsWAlign == 0U) || (IsHAlign == 0U)) {
                            AmbaLL_LogUInt5("Blend Width[%d] < 64 or not 16 alignment or Height[%d] not 8 alignment!", pBlendCfg[i].BlendYuvBuf.Window.Width, pBlendCfg[i].BlendYuvBuf.Window.Height, 0U, 0U, 0U);
                        }
                    }
                    Rval = AmbaHL_CmdVprocOsdBlend(WriteMode, OsdBlend);
                    HL_RelCmdBuffer(CmdBufferId);
                }
                if (Rval != OK) {
                    ErrLine = __LINE__;
                    break;
                }
            }
        } else {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncControlBlend Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncControlQualityChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncControlQuality(NumStream, pStreamIdx, pQCtrl);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncControlQuality(NumStream, pStreamIdx, pQCtrl);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncControlQuality(NumStream, pStreamIdx, pQCtrl);
    }

    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoEncControlQuality(NumStream, pStreamIdx, pQCtrl);
    }

    return Rval;
}

/**
* VideoEncode quality control function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pQCtrl quality control information
* @return ErrorCode
*/
UINT32 dsp_video_enc_ctrl_quality(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    UINT8 CmdBufferIdEncQt = 0U; void *CmdBufferAddrEncQt = NULL;
    UINT32 Rval, ErrLine = 0U;
    UINT16 i;
#ifdef SUPPORT_ENC_QP_ROI
    UINT16 j;
#endif

    CTX_STREAM_INFO_s StrmInfo;
    encoder_realtime_setup_t *EncQualitySetup = HL_DefCtxCmdBufPtrEncRt;

    Rval = HL_VideoEncControlQualityChk(NumStream, pStreamIdx, pQCtrl);

    /* Body */
    if (Rval == OK) {
//FIXME, issued before encode started or accompany to encode start
        UINT8 DspProfState = DSP_GetProfState();
        UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

        if (DspProfState == DSP_PROF_STATUS_CAMERA) {
            /* Blend effect is executed in dram when vproc push data out before MEMD */
            for (i = 0U; i < NumStream; i++) {
                HL_AcqCmdBuffer(&CmdBufferIdEncQt, &CmdBufferAddrEncQt);
                dsp_osal_typecast(&EncQualitySetup, &CmdBufferAddrEncQt);

                EncQualitySetup->stream_id = (UINT8)pStreamIdx[i];
                if (pQCtrl[i].DemandIDR > 0U) {
                    EncQualitySetup->enable_flags |= ENC_PARAM_INSERT_IDR;

                    EncQualitySetup->force_idr = (UINT8)1U;
                }

                if (pQCtrl[i].BitRateCtrl > 0U) {
                    EncQualitySetup->enable_flags |= ENC_PARAM_BITRATE_CHANGE;

                    EncQualitySetup->target_bitrate = pQCtrl[i].BitRate;
                }

                if (pQCtrl[i].GOPCtrl > 0U) {
                    EncQualitySetup->enable_flags |= ENC_PARAM_GOP;

                    EncQualitySetup->gop_n_new = pQCtrl[i].GOPCfg.N;
                    EncQualitySetup->idr_interval_new = (UINT8)pQCtrl[i].GOPCfg.IdrInterval;
                }

                if (pQCtrl[i].QPCtrl > 0U) {
                    EncQualitySetup->enable_flags |= ENC_PARAM_QP_LIMIT;

                    EncQualitySetup->qp_min_on_I = pQCtrl[i].QpCfg.QpMinIFrame;
                    EncQualitySetup->qp_max_on_I = pQCtrl[i].QpCfg.QpMaxIFrame;
                    EncQualitySetup->qp_min_on_P = pQCtrl[i].QpCfg.QpMinPFrame;
                    EncQualitySetup->qp_max_on_P = pQCtrl[i].QpCfg.QpMaxPFrame;
                    (void)dsp_osal_memcpy(&EncQualitySetup->i_qp_reduce, &pQCtrl[i].QpCfg.QpReduceIFrame, sizeof(UINT8));
                    (void)dsp_osal_memcpy(&EncQualitySetup->p_qp_reduce, &pQCtrl[i].QpCfg.QpReducePFrame, sizeof(UINT8));
                    EncQualitySetup->aqp = pQCtrl[i].QpCfg.Aqp;

                    /* default value of others */
                    EncQualitySetup->qp_min_on_B = 21U;
                    EncQualitySetup->qp_max_on_B = 51U;
                    EncQualitySetup->qp_min_on_C = 21U;
                    EncQualitySetup->qp_max_on_C = 51U;
                    EncQualitySetup->qp_min_on_D = 21U;
                    EncQualitySetup->qp_max_on_D = 51U;
                    EncQualitySetup->qp_min_on_Q = 21U;
                    EncQualitySetup->qp_max_on_Q = 51U;
                    EncQualitySetup->b_qp_reduce = 0;
                    EncQualitySetup->c_qp_reduce = 3;
                    EncQualitySetup->q_qp_reduce = 6;
                }

#ifdef SUPPORT_ENC_QP_ROI
                if (pQCtrl->RoiCtrl > 0U) {
                    EncQualitySetup->enable_flags |= ENC_PARAM_MDSWCAT;

                    (void)dsp_osal_virt2cli(pQCtrl->RoiCfg.IFrmMapAddr, &EncQualitySetup->mdSwCat_daddr[0U]);
                    (void)dsp_osal_virt2cli(pQCtrl->RoiCfg.PFrmMapAddr, &EncQualitySetup->mdSwCat_daddr[1U]);
                    (void)dsp_osal_virt2cli(pQCtrl->RoiCfg.BFrmMapAddr, &EncQualitySetup->mdSwCat_daddr[2U]);

                    HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);

                    if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
                        EncQualitySetup->intra_bias[0U] = pQCtrl->RoiCfg.IntraBias[0U];
                        EncQualitySetup->intra_bias[1U] = pQCtrl->RoiCfg.IntraBias[1U];
                        EncQualitySetup->direct_bias[0U] = pQCtrl->RoiCfg.DirectBias[0U];
                        EncQualitySetup->direct_bias[1U] = pQCtrl->RoiCfg.DirectBias[1U];
                    }
                    if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
                        for (j = 0U; j < MD_CATEGORY_MAX_NUM; j++) {
                            EncQualitySetup->md_category_lookup[j] = pQCtrl->RoiCfg.MdCatLUT[j];
                        }
                    }
                }
#endif

                Rval = AmbaHL_CmdEncoderRealtimeSetup(WriteMode, EncQualitySetup);
                HL_RelCmdBuffer(CmdBufferIdEncQt);
                if (Rval != OK) {
                    ErrLine = __LINE__;
                    break;
                } else {
                    if (pQCtrl[i].BitRateCtrl > 0U) {
                        HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);
                        StrmInfo.BitRateCtrlStatus = ENC_RT_CTRL_ISSUED;
                        StrmInfo.BitRateCfg.BitRate = pQCtrl[i].BitRate;
                        HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                    }

                    if (pQCtrl[i].GOPCtrl > 0U) {
                        HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);
                        StrmInfo.GopCtrlStatus = ENC_RT_CTRL_ISSUED;
                        StrmInfo.GOPCfg.N = pQCtrl[i].GOPCfg.N;
                        StrmInfo.GOPCfg.IdrInterval = pQCtrl[i].GOPCfg.IdrInterval;
                        HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                    }

                    if (pQCtrl[i].QPCtrl > 0U) {
                        HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);
                        StrmInfo.QpCtrlStatus = ENC_RT_CTRL_ISSUED;
                        (void)dsp_osal_memcpy(&StrmInfo.QpCfg, &pQCtrl[i].QpCfg, sizeof(AMBA_DSP_VIDEO_QP_CONFIG_s));
                        HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                    }

                    if (pQCtrl->RoiCtrl > 0U) {
                        HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);
                        StrmInfo.RoiCtrlStatus = ENC_RT_CTRL_ISSUED;
                        (void)dsp_osal_memcpy(&StrmInfo.RoiCfg, &pQCtrl->RoiCfg, sizeof(AMBA_DSP_VIDEO_ROI_CONFIG_s));
                        HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                    }
                }
            }
        } else {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncControlQuality Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncExecIntervalCapChk(UINT16 NumStream, const UINT16 *pStreamIdx)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncExecIntervalCap(NumStream, pStreamIdx);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncExecIntervalCap(NumStream, pStreamIdx);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncExecIntervalCap(NumStream, pStreamIdx);
    }

    return Rval;
}

/**
* VideoEncode interval capture execute function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @return ErrorCode
*/
UINT32 dsp_video_enc_exec_intervalcap(UINT16 NumStream, const UINT16 *pStreamIdx)
{
    UINT32 Rval;

    Rval = HL_VideoEncExecIntervalCapChk(NumStream, pStreamIdx);

    /* Body */
    if (Rval == OK) {

    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncExecIntervalCap Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncFeedYuvDataChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncFeedYuvData(NumStream, pStreamIdx, pExtYuvBuf);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncFeedYuvData(NumStream, pStreamIdx, pExtYuvBuf);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncFeedYuvData(NumStream, pStreamIdx, pExtYuvBuf);
    }

    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoEncFeedYuvData(NumStream, pStreamIdx, pExtYuvBuf);
    }

    return Rval;
}

static UINT32 HL_VideoEncFeedYuvData(UINT16 i, UINT8 WriteMode, UINT32 *pErrLine,
                                     const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf, UINT8 IsExtMemEnc, AMBA_DSP_BUF_s *pCalcExtBuf,
                                     const CTX_STREAM_INFO_s *pStrmInfo, cmd_vin_set_ext_mem_t *pVinExtMem,
                                     CTX_TIMELAPSE_INFO_s *pTimeLapseInfo)
{
    UINT32 Rval = OK;
    ULONG ULAddr = 0x0U;
    UINT32 PhysAddr = OK;
    UINT32 *pBufTblAddr = NULL;
    UINT8 U8Val;

    if (IsExtMemEnc == (UINT8)1U) {
        pCalcExtBuf->Pitch = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Pitch >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
        pCalcExtBuf->Window.Width = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Width >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
        pCalcExtBuf->Window.Height = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Height >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
    } else {
        Rval = HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId, DSP_DATACAP_CALTYPE_ME1, pCalcExtBuf);
    }
    if (Rval != OK) {
        *pErrLine = __LINE__;
    } else {
        pVinExtMem->buf_pitch = pCalcExtBuf->Pitch;
        pVinExtMem->buf_width = pCalcExtBuf->Window.Width;
        pVinExtMem->buf_height = pCalcExtBuf->Window.Height;
        pVinExtMem->chroma_format = YUV_MONO;

        HL_GetPointerToDspExtRawBufArray(pTimeLapseInfo->VirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                         1U/*Me*/,
                                         &pBufTblAddr);
        (void)dsp_osal_virt2cli(pExtYuvBuf->pExtME1Buf->BaseAddr, &PhysAddr);
        pBufTblAddr[0U] = PhysAddr;
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &pVinExtMem->buf_addr);

        /* Fill Aux for Me0 */
        if (IsExtMemEnc == (UINT8)1U) {
            pCalcExtBuf->Pitch = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Pitch >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
            pCalcExtBuf->Window.Width = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Width >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
            pCalcExtBuf->Window.Height = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Height >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
        } else {
            Rval = HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId, DSP_DATACAP_CALTYPE_ME0, pCalcExtBuf);
        }

        if (Rval != OK) {
            *pErrLine = __LINE__;
        } else {
            pVinExtMem->aux_pitch = pCalcExtBuf->Pitch;
            pVinExtMem->aux_width = pCalcExtBuf->Window.Width;
            pVinExtMem->aux_height = pCalcExtBuf->Window.Height;
            /*
             * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
             * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
             * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
            pVinExtMem->vin_yuv_enc_fbp_disable = 0U;
#endif
            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, pVinExtMem);
            if (Rval != OK) {
                *pErrLine = __LINE__;
            } else {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_send_input_data_t *VinFeedData = HL_DefCtxCmdBufPtrVinSndData;

                //Send Input Data for Vin
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinFeedData, &CmdBufferAddr);
                U8Val = (UINT8)(pTimeLapseInfo->VirtVinId + AMBA_DSP_MAX_VIN_NUM);
                VinFeedData->vin_id = U8Val;
                VinFeedData->chan_id = 0U; //obsoleted
                VinFeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_ENC;
                VinFeedData->encode_start_idc = (0U == pTimeLapseInfo->TotalIssuedMemEncodeNum)? 1U: 0U;
                VinFeedData->encode_stop_idc = 0U; // Control by VideoStop cmd
                if ((AMBA_DSP_YUV420 == pExtYuvBuf[i].ExtYuvBuf.DataFmt) ||
                    (AMBA_DSP_YUV400 == pExtYuvBuf[i].ExtYuvBuf.DataFmt)) {
                    VinFeedData->input_data_type = DSP_VIN_SEND_IN_DATA_YUV420;
                } else {
                    VinFeedData->input_data_type = DSP_VIN_SEND_IN_DATA_YUV422;
                }
                VinFeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
                VinFeedData->ext_ce_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;
                VinFeedData->hw_pts = 0U;

                Rval = AmbaHL_CmdVinSendInputData(WriteMode, VinFeedData);
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval != OK) {
                    *pErrLine = __LINE__;
                } else {
                    HL_GetTimeLapseInfo(HL_MTX_OPT_GET, (UINT16)pStrmInfo->TimeLapseId, pTimeLapseInfo);
                    pTimeLapseInfo->TotalIssuedMemEncodeNum++;
                    (void)dsp_osal_memcpy(&pTimeLapseInfo->LastYuvBuf, &pExtYuvBuf[i].ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    pTimeLapseInfo->LastMe1BufAddr = pExtYuvBuf[i].pExtME1Buf->BaseAddr;
                    HL_SetTimeLapseInfo(HL_MTX_OPT_SET, (UINT16)pStrmInfo->TimeLapseId, pTimeLapseInfo);
                }
            }
        }
    }
    return Rval;
}

/**
* VideoEncode feeding Yuv function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pExtYuvBuf yuv buffer information
* @return ErrorCode
*/
UINT32 dsp_video_enc_feed_yuv_data(UINT16 NumStream,
                                   const UINT16 *pStreamIdx,
                                   const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    UINT8 IsExtMemEnc = 0U, U8Val;
    UINT16 i;
    UINT32 PhysAddr = 0x0U;
    UINT32 Rval, ErrLine = 0U;
    UINT32 *pBufTblAddr;
    ULONG ULAddr = 0x0U;
    AMBA_DSP_BUF_s CalcExtBuf = {0U};
    CTX_STREAM_INFO_s StrmInfo = {0U};
    CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};
#ifndef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
    UINT16 ViewZoneId, VinId;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
#endif
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

    Rval = HL_VideoEncFeedYuvDataChk(NumStream, pStreamIdx, pExtYuvBuf);

    /* Body */
    if (Rval == OK) {
        for (i = 0U; i < NumStream; i++) {
            HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);

            if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
                IsExtMemEnc = (UINT8)1U;
            } else {
                IsExtMemEnc = (UINT8)0U;
            }

            if ((StrmInfo.VideoTimeLapseActive == 1U) && (StrmInfo.TimeLapseId != DSP_TIMELAPSE_IDX_INVALID)) {
                HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, (UINT16)StrmInfo.TimeLapseId, &TimeLapseInfo);

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
                U8Val = (UINT8)(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM);
                VinExtMem->vin_id_or_chan_id = U8Val;
                VinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
                VinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
                VinExtMem->allocation_mode = (0U == TimeLapseInfo.TotalIssuedMemEncodeNum)? VIN_EXT_MEM_MODE_NEW: VIN_EXT_MEM_MODE_APPEND;
                VinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
                VinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
                if (VinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
                    AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
                }
#endif
                if (IsExtMemEnc == (UINT8)1U) {
                    CalcExtBuf.Pitch = pExtYuvBuf->ExtYuvBuf.Pitch;
                    CalcExtBuf.Window.Width = pExtYuvBuf->ExtYuvBuf.Window.Width;
                    CalcExtBuf.Window.Height = pExtYuvBuf->ExtYuvBuf.Window.Height;

                    WriteMode = HL_GetVin2CmdNormalWrite((UINT16)(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM));
                } else {
                    (void)HL_StillCaptureCalcExtBufImpl(StrmInfo.SourceYuvStrmId, DSP_DATACAP_CALTYPE_LUMA, &CalcExtBuf);

                    /* Update WriteMode */
#ifdef UCODE_HANDLE_VIRT_VIN_CMD_IN_ASYNC
                    WriteMode = HL_GetVin2CmdNormalWrite((UINT16)(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM));
#else
                    HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
                    ViewZoneId = YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId;
                    (void)HL_GetViewZoneVinId(ViewZoneId, &VinId);
                    WriteMode = HL_GetVin2CmdNormalWrite(VinId);
#endif
                }
                VinExtMem->buf_pitch = CalcExtBuf.Pitch;
                VinExtMem->buf_width = CalcExtBuf.Window.Width;
                VinExtMem->buf_height = CalcExtBuf.Window.Height;
                if (AMBA_DSP_YUV420 == pExtYuvBuf[i].ExtYuvBuf.DataFmt) {
                    VinExtMem->chroma_format = DSP_YUV_420;
                } else if (AMBA_DSP_YUV400 == pExtYuvBuf[i].ExtYuvBuf.DataFmt) {
                    VinExtMem->chroma_format = DSP_YUV_MONO;
                } else {
                    VinExtMem->chroma_format = DSP_YUV_422;
                }

                /*
                 * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
                 * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
                 * */
#ifdef SUPPORT_DSP_VIN_YUV_ENC_FBP_CTRL
                VinExtMem->vin_yuv_enc_fbp_disable = 0U;
#endif
                HL_GetPointerToDspExtRawBufArray(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM, 0U/*Raw*/, &pBufTblAddr);
                (void)dsp_osal_virt2cli(pExtYuvBuf->ExtYuvBuf.BaseAddrY, &PhysAddr);
                pBufTblAddr[0U] = PhysAddr;
                dsp_osal_typecast(&ULAddr, &pBufTblAddr);
                (void)dsp_osal_virt2cli(ULAddr, &VinExtMem->buf_addr);
                Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);

                if (Rval != OK) {
                    ErrLine = __LINE__;
                } else {
                    /* Me1 */
                    (void)dsp_osal_memset(VinExtMem, 0, sizeof(cmd_vin_set_ext_mem_t));
                    U8Val = (UINT8)TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM;
                    VinExtMem->vin_id_or_chan_id = U8Val;
                    VinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_ME;
                    VinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
                    VinExtMem->allocation_mode = (0U == TimeLapseInfo.TotalIssuedMemEncodeNum)? VIN_EXT_MEM_MODE_NEW: VIN_EXT_MEM_MODE_APPEND;
                    VinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
                    VinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
                    if (VinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
                        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
                    }
#endif
                    Rval = HL_VideoEncFeedYuvData(i, WriteMode, &ErrLine, pExtYuvBuf, IsExtMemEnc, &CalcExtBuf,
                                                  &StrmInfo, VinExtMem, &TimeLapseInfo);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncFeedYuvData Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncGrpConfigChk(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncGrpConfig(pGrpCfg);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncGrpConfig(pGrpCfg);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncGrpConfig(pGrpCfg);
    }

    return Rval;
}

/**
* VideoEncode group config
* @param [in]  pGrpCfg group config
* @return ErrorCode
*/
UINT32 dsp_video_enc_grp_cfg(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    UINT32 Rval;
    CTX_ENC_GRP_INFO_s EncGrpInfo = {0};
    UINT16 i;

    Rval = HL_VideoEncGrpConfigChk(pGrpCfg);

    /* Body */
    if (Rval == OK) {
        HL_GetEncGrpInfo(HL_MTX_OPT_GET, pGrpCfg->GrpIdx, &EncGrpInfo);
        // Reset
        for (i = 0U; i<AMBA_DSP_MAX_ENC_GRP_STRM_NUM; i++) {
            EncGrpInfo.StrmIdx[i] = DSP_ENC_GRP_STRM_IDX_INVALID;
        }

        // Fill
        EncGrpInfo.Purpose = pGrpCfg->Purpose;
        EncGrpInfo.StrmNum = pGrpCfg->NumStrm;
        EncGrpInfo.Stride = (UINT8)DSP_GetU8Bit(pGrpCfg->Stride, 0U, 7U);
        EncGrpInfo.MonoIncrement = (UINT8)DSP_GetU8Bit(pGrpCfg->Stride, 7U, 1U);
        EncGrpInfo.RcCfgAddr = pGrpCfg->RcCfgAddr;

        for (i=0U; i<pGrpCfg->NumStrm; i++) {
            EncGrpInfo.StrmIdx[i] = pGrpCfg->pStrmIdx[i];
        }
        HL_SetEncGrpInfo(HL_MTX_OPT_SET, pGrpCfg->GrpIdx, &EncGrpInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncGrpConfig Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

#ifdef SUPPORT_DSP_MV_DUMP
static inline UINT32 HL_VideoEncMvConfigChk(UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncMvConfig(StreamIdx, pMvCfg);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncMvConfig(StreamIdx, pMvCfg);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncMvConfig(StreamIdx, pMvCfg);
    }

    return Rval;
}
#endif

/**
* VideoEncode config MV
* @param [in]  StreamIdx encode stream index
* @param [in]  pMvCfg MV information
* @return ErrorCode
*/
UINT32 dsp_video_enc_mv_cfg(UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
#ifdef SUPPORT_DSP_MV_DUMP
    UINT32 Rval;
    CTX_STREAM_INFO_s StrmInfo;

    Rval = HL_VideoEncMvConfigChk(StreamIdx, pMvCfg);

    /* Body */
    if (Rval == OK) {
        HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
        StrmInfo.MvBufBase = pMvCfg->BufAddr;
        StrmInfo.MvBufSize = pMvCfg->BufSize;
        StrmInfo.MvBufUnitSize = pMvCfg->UnitSize;
        StrmInfo.MvOption = pMvCfg->Option;
        HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncMvConfig Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }
    return Rval;
#else
(void)StreamIdx;
(void)pMvCfg;
    AmbaLL_LogUInt5("No MV Dump in this chip", 0U, 0U, 0U, 0U, 0U);
    return ERR_NA;
#endif

}

static inline UINT32 HL_VideoEncDescFmtConfigChk(UINT16 StreamIdx, UINT16 CatIdx, UINT32 OptVal)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoEncDescFmtConfig(StreamIdx, CatIdx, OptVal);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoEncDescFmtConfig(StreamIdx, CatIdx, OptVal);
    }

    /* Logic sanity check */
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoEncDescFmtConfig(StreamIdx, CatIdx, OptVal);
    }

    return Rval;
}

/**
* Config Video data descriptor format, carried by AMBA_DSP_EVENT_VIDEO_DATA_RDY, only valid before encode start
* @param [in]  StreamIdx encode stream index
* @param [in]  CatId category index
* @param [in]  OptVal Option setting
* @return ErrorCode
*/
UINT32 dsp_video_enc_desc_fmt_cfg(UINT16 StreamIdx, UINT16 CatIdx, UINT32 OptVal)
{
    UINT32 Rval;
    CTX_STREAM_INFO_s StrmInfo;

    Rval = HL_VideoEncDescFmtConfigChk(StreamIdx, CatIdx, OptVal);

    /* Body */
    if (Rval == OK) {
        if (CatIdx == DSP_ENC_DESC_CAT_CAPTS) {
            HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
            StrmInfo.PicRdyInfoOpt = (UINT8)OptVal;
            HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncDescFmtConfig Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}
