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

static inline void HL_VideoEnc_FrameRateDivisor(UINT16 StrmId, UINT32* FrameRateDivisor)
{
    CTX_STREAM_INFO_s StrmInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT32 VinIdx = 0U;
    UINT16 SrcViewZoneId;
    UINT32 VinFrateGcd, VinNumUnitsInTick;
    UINT32 EncStmFrateGcd, StrmNumUnitsInTick;
    UINT16 StrmDecimation;

    // Get VinIdx from current stream ID
    HL_GetStrmInfo(HL_MTX_OPT_ALL, StrmId, &StrmInfo);
    if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        *FrameRateDivisor = 1;
    } else {
        HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;

        // Assume all the source viewzone has the same framerate
        SrcViewZoneId = pYuvStrmLayout->ChanCfg[0].ViewZoneId;
        HL_GetViewZoneInfoPtr(SrcViewZoneId, &ViewZoneInfo);
        DSP_Bit2Idx((UINT32)ViewZoneInfo->SourceVin, &VinIdx);
        HL_GetVinInfo(HL_MTX_OPT_ALL, (UINT16)VinIdx, &VinInfo);

        VinNumUnitsInTick = (VinInfo.FrameRate.NumUnitsInTick > 0U) ? VinInfo.FrameRate.NumUnitsInTick : 1U;
        VinFrateGcd = Gcd(VinInfo.FrameRate.TimeScale, VinNumUnitsInTick);
        StrmNumUnitsInTick = (StrmInfo.FrameRate.NumUnitsInTick > 0U) ? StrmInfo.FrameRate.NumUnitsInTick : 1U;
        EncStmFrateGcd = Gcd(StrmInfo.FrameRate.TimeScale, StrmNumUnitsInTick);

        if (YuvStrmInfo->DestDeciRate > 0U) {
            StrmDecimation = (UINT16)DSP_GetBit(YuvStrmInfo->DestDeciRate, 16U, 16U);
        } else {
            StrmDecimation = 1U;
        }

        /* No FrmRateDivisor in TimeLapse and TimeDevision */
        if ((StrmInfo.VideoTimeLapseActive > 0U) ||
            (ViewZoneInfo->SourceTdIdx != VIEWZONE_SRC_TD_IDX_NULL)) {
            *FrameRateDivisor = 1;
        } else if ((ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) ||
                   (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_RECON)) {
            *FrameRateDivisor = 1;
        } else {
            UINT32 FrateDividend, FrateDivisor;
            UINT32 VinDecimationRate = (ViewZoneInfo->VinDecimationRate > 1U) ? (UINT32)ViewZoneInfo->VinDecimationRate : 1U;

            if ((VinFrateGcd > 0U) &&
                (StrmDecimation > 0U) &&
                (EncStmFrateGcd > 0U)) {
                FrateDividend = ((VinInfo.FrameRate.TimeScale/VinFrateGcd)/VinDecimationRate)/StrmDecimation;
                FrateDivisor = StrmInfo.FrameRate.TimeScale/EncStmFrateGcd;
            } else {
                FrateDividend = VinInfo.FrameRate.TimeScale;
                FrateDivisor = StrmInfo.FrameRate.TimeScale;
            }
            if (FrateDivisor != 0U) {
                *FrameRateDivisor = FrateDividend / FrateDivisor;
            } else {
                *FrameRateDivisor = 0U;
            }
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
        if (CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
            Value = SrcCrop->CropLeftOffset;
            DstCrop->offset_left= (UINT8)Value;

            Value = SrcCrop->CropRightOffset;
            Value <<= 1U;
            DstCrop->offset_right= (UINT8)Value;

            Value = SrcCrop->CropTopOffset;
            DstCrop->offset_top= (UINT8)Value;

            Value = SrcCrop->CropBottomOffset;
            Value <<= 1U;
            DstCrop->offset_bottom= (UINT8)Value;
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

static inline UINT32 HL_FillVidEncCfgCmdOnJpg(cmd_encoder_jpeg_setup_t *pEncodeJpgSetup,
                                              CTX_STREAM_INFO_s *pStrmInfo,
                                              UINT16 StrmNum,
                                              UINT16 StreamIdx,
                                              UINT8 IsVertSliceEnable)
{
    UINT32 Rval = OK;
    UINT16 i;
    UINT8 EnableVdoThm = 0U;
    CTX_DATACAP_INFO_s DataCapInfo = {0};
    ULONG ULAddr = 0U;

    pEncodeJpgSetup->stream_id = (UINT8)StreamIdx;
    pEncodeJpgSetup->stream_type = 0U; //TBD
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
        (IsVertSliceEnable > 0U) ||
        (StrmNum > 1U)) {
        pEncodeJpgSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
    } else {
        pEncodeJpgSetup->enc_start_method = DSP_ENC_START_FROM_CMD;
    }
    if ((pStrmInfo->VideoTimeLapseActive > 0U) ||
        (IsVertSliceEnable > 0U)) {
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
    pEncodeJpgSetup->enc_hflip = (HL_IS_HFLIP(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;
    pEncodeJpgSetup->enc_vflip = (HL_IS_VFLIP(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;
    pEncodeJpgSetup->enc_rotation = (HL_IS_ROTATE(pStrmInfo->Rotate) == 1U)? (UINT8)1U: (UINT8)0U;

    /* Bits buffer and information */
    (void)dsp_osal_virt2cli(pStrmInfo->BitsBufAddr, &pEncodeJpgSetup->bits_fifo_base);
    pEncodeJpgSetup->bits_fifo_size = pStrmInfo->BitsBufSize;
    pEncodeJpgSetup->bits_fifo_offset = 0U;

    /* Desc buffer, LL reset desc buffer every encode start, not yet */
    DSP_GetBitsDescBufferAddr(&ULAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pEncodeJpgSetup->info_fifo_base);
    pEncodeJpgSetup->info_fifo_size = AMBA_DSP_VIDEOENC_DESC_BUF_SIZE;

    /* BitRate */
    pEncodeJpgSetup->initial_qlevel = (UINT8)pStrmInfo->JpgBitRateCtrl.QualityLevel;
    (void)dsp_osal_virt2cli(pStrmInfo->QuantMatrixAddr, &pEncodeJpgSetup->dqt_daddr);

    //NotSupport
    pEncodeJpgSetup->target_bpp = 0U;
    pEncodeJpgSetup->tolerance = 0U;
    pEncodeJpgSetup->max_reenc_loops = 0U;
    pEncodeJpgSetup->rct_sample_num = 0U;
    pEncodeJpgSetup->rct_daddr = 0U;
    pEncodeJpgSetup->frame_rate = 0U;

    //Obsolete
    pEncodeJpgSetup->restart_interval = 0U;
    pEncodeJpgSetup->enc_src = 0U;
    pEncodeJpgSetup->efm_enable = 0U;

    return Rval;
}

static inline UINT32 HL_VidEncCfgPreProcOnExtMem(UINT8 WriteMode, const CTX_STREAM_INFO_s *pStrmInfo, UINT16 StreamIdx)
{
    UINT32 Rval = OK;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;

    if (pStrmInfo->VideoTimeLapseActive > 0U) {
        if (pStrmInfo->TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
            CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};

            HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);

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
                    HL_GetTimeLapseInfo(HL_MTX_OPT_GET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
                    TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_RUN;
                    //Reset
                    (void)dsp_osal_memset(&TimeLapseInfo.LastYuvBuf , 0, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    TimeLapseInfo.LastMe1BufAddr = 0x0U;
                    HL_SetTimeLapseInfo(HL_MTX_OPT_SET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
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
                                                    const CTX_STREAM_INFO_s *pStrmInfo,
                                                    UINT16 ViewZoneId,
                                                    UINT16 StreamIdx,
                                                    UINT16 PinId)
{
    UINT32 Rval = OK;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;
    UINT8 NewWriteMode;

    if (pStrmInfo->TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
        CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};

        HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
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
                            NewWriteMode = HL_GetVin2CmdNormalWrite(TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM);
                            Rval = AmbaHL_CmdDspBindingCfg(NewWriteMode, FpBindingCfg);
                            if (Rval != OK) {
                                AmbaLL_LogUInt5("AmbaHL_CmdDspBindingCfg fail[%d]", __LINE__, 0U, 0U, 0U, 0U);
                            } else {
                                HL_GetTimeLapseInfo(HL_MTX_OPT_GET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
                                TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_RUN;
                                HL_SetTimeLapseInfo(HL_MTX_OPT_SET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
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
                                                  const CTX_STREAM_INFO_s *pStrmInfo,
                                                  UINT16 ViewZoneId,
                                                  UINT16 StreamIdx,
                                                  UINT16 PinId)
{
    UINT32 Rval = OK;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindingCfg = HL_DefCtxCmdBufPtrBind;
    UINT16 VinNum = 0;
    UINT8 NewWriteMode;

    if (pStrmInfo->TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
        CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};

        HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
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
                        HL_GetTimeLapseInfo(HL_MTX_OPT_GET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
                        TimeLapseInfo.Status = ENC_TIMELAPSE_STATUS_CONFIG;
                        HL_SetTimeLapseInfo(HL_MTX_OPT_SET, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
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

static inline UINT32 HL_FillCodingTpyeOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                                  CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        pEncodeSetup->coding_type = DSP_ENC_FMT_H264;
    } else if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->coding_type = DSP_ENC_FMT_H265;
    } else {
        pEncodeSetup->coding_type = DSP_ENC_FMT_H264;
    }
    pStrmInfo->IsMJPG = (UINT8)0U;

    return Rval;
}

static inline UINT32 HL_FillTriggerMethodOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                                     const CTX_STREAM_INFO_s *pStrmInfo,
                                                     UINT16 StrmNum,
                                                     UINT8 EnableVdoThm)
{
    UINT32 Rval = OK;

    if (pStrmInfo->VideoTimeLapseActive > 0U) {
        pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
        pEncodeSetup->enc_stop_method = DSP_ENC_STOP_FROM_PICINFO;
    } else if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_CMPR_STRO) {
        pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
        pEncodeSetup->enc_stop_method = DSP_ENC_START_FROM_PICINFO;
    } else {
        if ((EnableVdoThm > 0U) ||
            (StrmNum > 1U)) {
            pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_PICINFO;
        } else {
            pEncodeSetup->enc_start_method = DSP_ENC_START_FROM_CMD;
        }
        pEncodeSetup->enc_stop_method = DSP_ENC_STOP_FROM_CMD;
    }

    return Rval;
}

static inline UINT32 HL_FillEncWinOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                              const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    pEncodeSetup->encode_w_sz = pStrmInfo->Window.Width;
    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
        pEncodeSetup->encode_h_sz = ALIGN_NUM16(pStrmInfo->Window.Height, 16U);
    } else if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->encode_h_sz = ALIGN_NUM16(pStrmInfo->Window.Height, 8U);
    } else {
        pEncodeSetup->encode_h_sz = pStrmInfo->Window.Height;
    }
    pEncodeSetup->encode_w_ofs = pStrmInfo->Window.OffsetX;
    pEncodeSetup->encode_h_ofs = pStrmInfo->Window.OffsetY;

    if ((HL_IS_ROTATE(pStrmInfo->Rotate) == 1U)) {
        UINT16 EncodeW = pEncodeSetup->encode_w_sz;

        pEncodeSetup->encode_w_sz = pEncodeSetup->encode_h_sz;
        pEncodeSetup->encode_h_sz = EncodeW;
        pEncodeSetup->encode_w_ofs = pStrmInfo->Window.OffsetY;
        pEncodeSetup->encode_h_ofs = pStrmInfo->Window.OffsetX;
    }

    return Rval;
}

static inline UINT32 HL_FillTileOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                            const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    if (pStrmInfo->BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_MV) {
        //One tile only when MV
        pEncodeSetup->tile_num = 1U;
    } else {
        pEncodeSetup->tile_num = pStrmInfo->NumTile;
    }
    pEncodeSetup->slice_num = pStrmInfo->NumSlice;

    return Rval;
}

static inline UINT32 HL_FillRcOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                          const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    pEncodeSetup->use_cabac = pStrmInfo->IsCabac;
    pEncodeSetup->quality_level = (UINT16)pStrmInfo->BitRateCfg.QualityLevel;
    pEncodeSetup->target_bitrate = pStrmInfo->BitRateCfg.BitRate;
    pEncodeSetup->chroma_format = 0U; //0: 420, 1: 400 (mono), default as Yuv420
    pEncodeSetup->scalelist_opt = 0U;
    pEncodeSetup->is_dirt_detect = 0U;
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
        pEncodeSetup->chroma_format = (UINT8)1U; //running mono in CMPR_STRO
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
        pEncodeSetup->is_dirt_detect = (UINT8)1U;
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

    pEncodeSetup->send_buf_status_msg = 1U; // for reporting recon buffers

    if (pStrmInfo->MvBufBase > 0U) {
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
    } else {
        pEncodeSetup->mvdump_daddr = 0U;
        pEncodeSetup->mvdump_fifo_limit = 0U;
        pEncodeSetup->mvdump_fifo_unit_sz = 0U;
        pEncodeSetup->mvdump_dpitch = 0U;
    }

    if (pStrmInfo->ActBufBase > 0U) {
        /* we shall use rc-mode = 0 */
#ifdef SUPPORT_DSP_AVC_ENC_ACT_STAT
        pEncodeSetup->rc_mode = DSP_ENC_RC_OFF;
        pEncodeSetup->initial_qp = 0U; //30 when AVC, 32 when HEVC

        (void)dsp_osal_virt2cli(pStrmInfo->ActBufBase, &pEncodeSetup->stat_fifo_base);
        (void)dsp_osal_virt2cli((pStrmInfo->ActBufBase + pStrmInfo->ActBufSize) - 1U, &pEncodeSetup->stat_fifo_limit);
#endif
    } else {
        pEncodeSetup->stat_fifo_base = 0U;
        pEncodeSetup->stat_fifo_limit = 0U;
    }

    return Rval;
}

static inline UINT32 HL_FillCpbOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                           const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    pEncodeSetup->cpb_user_size = pStrmInfo->BitRateCfg.CPBSize;
    if (pEncodeSetup->cpb_user_size > 0U) {
        /* From uCode diag setting */
        pEncodeSetup->cpb_buf_idc = (UINT8)0x1FU;
        pEncodeSetup->cpb_cmp_idc = (UINT8)1U;
        pEncodeSetup->en_panic_rc = (UINT8)1U;
        pEncodeSetup->fast_rc_idc = (UINT8)2U;
        AmbaLL_LogUInt5("CPB size = %u", pEncodeSetup->cpb_user_size, 0U, 0U, 0U, 0U);
    } else {
        pEncodeSetup->cpb_buf_idc = (UINT8)1U;
        pEncodeSetup->en_panic_rc = (UINT8)0U;
        pEncodeSetup->cpb_cmp_idc = (UINT8)0U;
        pEncodeSetup->fast_rc_idc = (UINT8)0U;
    }

    return Rval;
}

static inline UINT32 HL_FillDeblockOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                               const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

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

    return Rval;
}

static inline UINT32 HL_FillIrOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                          const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;
    UINT8 IrScanMode, IrScanDirection;

    if (pStrmInfo->IntraRefreshCtrl.IntraRefreshCycle > 0U) {
        pEncodeSetup->IR_cycle = pStrmInfo->IntraRefreshCtrl.IntraRefreshCycle;
        pEncodeSetup->IR_type = 0U; //TBD
        IrScanMode = (UINT8)DSP_GetU8Bit(pStrmInfo->IntraRefreshCtrl.IntraRefreshMode, 0U, 2U);
        IrScanDirection = (UINT8)DSP_GetU8Bit(pStrmInfo->IntraRefreshCtrl.IntraRefreshMode, 2U, 2U);
        (void)IrScanMode;
        (void)IrScanDirection;
#if defined(ENABLE_2ND_STRM_REFINE_CMD)
        pEncodeSetup->lambda_scalefactor = 51U;
        pEncodeSetup->IR_intra_qp_adj = 0U;
        pEncodeSetup->IR_tuning_mode = 1U;
        pEncodeSetup->IR_no_overlap = 1U;
        pEncodeSetup->IR_max_qp = 50U;
        pEncodeSetup->IR_max_qp_spread = 4U;

        pEncodeSetup->gmv_enable = 1U;
        pEncodeSetup->gmv_threshold_enable = 1U;
        pEncodeSetup->gmv_threshold = 1U;
#endif
        AmbaLL_LogUInt5("No IR pattern now", 0U, 0U, 0U, 0U, 0U);
    } else {
        pEncodeSetup->IR_cycle = (UINT8)0U;
        pEncodeSetup->IR_type = (UINT8)0U; //TBD
    }

    pEncodeSetup->sps_pps_header_mode = pStrmInfo->SPSPPSHeaderInsertMode;

    return Rval;
}

static inline UINT32 HL_FillPerfModeOnHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                           const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
        pEncodeSetup->hevc_perf_mode = ENC_PERF_4KP60_FASTEST_P1_P; //ShortCutMode
    } else {
        pEncodeSetup->hevc_perf_mode = ENC_PERF_DEFAULT;
    }

    return Rval;
}

static inline UINT32 HL_FillVidEncCfgCmdOnAVCorHEVC(cmd_encoder_setup_t *pEncodeSetup,
                                                    CTX_STREAM_INFO_s *pStrmInfo,
                                                    UINT16 StrmNum,
                                                    UINT16 StreamIdx)
{
    UINT32 Rval = OK, U32Value;
    UINT16 i;
    UINT8 EnableVdoThm = 0U, U8Val;
    CTX_DATACAP_INFO_s DataCapInfo = {0};
    ULONG ULAddr = 0U;

    /* Stream description */
    pEncodeSetup->stream_id = (UINT8)StreamIdx;
    pEncodeSetup->stream_type = 0U; //TBD

    (void)HL_FillCodingTpyeOnAVCorHEVC(pEncodeSetup, pStrmInfo);

    /* Profile/Level IDC */
    pEncodeSetup->profile_idc = pStrmInfo->ProfileIDC;
    pEncodeSetup->level_idc = pStrmInfo->LevelIDC;
    pEncodeSetup->tier_idc = pStrmInfo->TierIDC;

    /* Source */
    pEncodeSetup->enc_src = 0U; /* Obsolete */
    pEncodeSetup->efm_enable = 0U; //TBD, 0=encode from enc_src, 1=encode from memory

    for (i = 0U; i<AMBA_DSP_MAX_DATACAP_NUM; i++) {
        HL_GetDataCapInfo(HL_MTX_OPT_ALL, i, &DataCapInfo);
        if ((DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) &&
            (DataCapInfo.Status == DATA_CAP_STATUS_2_RUN) &&
            (DataCapInfo.Cfg.Index == StreamIdx)) {
            EnableVdoThm = 1U;
            break;
        }
    }

    /* Start/Stop method */
    (void)HL_FillTriggerMethodOnAVCorHEVC(pEncodeSetup, pStrmInfo, StrmNum, EnableVdoThm);
    pStrmInfo->StartMethod = pEncodeSetup->enc_start_method;
    pStrmInfo->StopMethod = pEncodeSetup->enc_stop_method;

    /* Window */
    (void)HL_FillEncWinOnAVCorHEVC(pEncodeSetup, pStrmInfo);

    /* GOP */
    pEncodeSetup->M = pStrmInfo->GOPCfg.M;
    pEncodeSetup->N = pStrmInfo->GOPCfg.N;
    pEncodeSetup->idr_interval = pStrmInfo->GOPCfg.IdrInterval;
    pEncodeSetup->gop_structure = pStrmInfo->GOPCfg.GOPStruct;
    pEncodeSetup->numRef_P = pStrmInfo->NumPRef;
    pEncodeSetup->numRef_B = pStrmInfo->NumBRef;
//FIXME, reference frame
    pEncodeSetup->max_num_ltrs = 0U; //TBD
    pEncodeSetup->log2_num_ltrp_per_gop = 0U; // only support at quality_level bit 6 set as "1"
    pEncodeSetup->two_lt_ref_mode = 0U;

    /* Tile/Slice */
    (void)HL_FillTileOnAVCorHEVC(pEncodeSetup, pStrmInfo);

    /* BRC */
    (void)HL_FillRcOnAVCorHEVC(pEncodeSetup, pStrmInfo);

//FIXME, not used now
    pEncodeSetup->vbr_init_data_daddr = 0x0U;

    /* Bits buffer and inforamtion */
    (void)dsp_osal_virt2cli(pStrmInfo->BitsBufAddr, &pEncodeSetup->bits_fifo_base);
    pEncodeSetup->bits_fifo_size = pStrmInfo->BitsBufSize;
    pEncodeSetup->bits_fifo_offset = 0U;

//FIXME, not used now
    pEncodeSetup->num_slices_per_bits_info = 0U;

    /* Desc buffer, LL reset desc buffer every encode start, not yet */
    DSP_GetBitsDescBufferAddr(&ULAddr);
    (void)dsp_osal_virt2cli(ULAddr, &pEncodeSetup->info_fifo_base);
    pEncodeSetup->info_fifo_size = AMBA_DSP_VIDEOENC_DESC_BUF_SIZE;

    /* FrameRate and Pts */
    U32Value = 0U;
    (void)HL_ConvertDspFrameRate(pStrmInfo->FrameRate, &U32Value);
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

    /* CPB */
    (void)HL_FillCpbOnAVCorHEVC(pEncodeSetup, pStrmInfo);

    /* Deblock filter */
    (void)HL_FillDeblockOnAVCorHEVC(pEncodeSetup, pStrmInfo);

    //FIXME, for encode low delay and need accompany to MvinOpMode.hi_priority_stream_lowdelay/priority_encode_enable
    pEncodeSetup->is_high_priority_stream = 0U;

    /* IR */
    (void)HL_FillIrOnAVCorHEVC(pEncodeSetup, pStrmInfo);

    /* HEVC performance mode */
    (void)HL_FillPerfModeOnHEVC(pEncodeSetup, pStrmInfo);

    pEncodeSetup->embed_code = pStrmInfo->EmbedCode;

//FIXME, only eanble when RC_SIMPE+1080p240
//          pEncodeSetup->out_loop_rc = 0U;

//          pEncodeSetup->use_local_enc_sw_option = 0U;
//          pEncodeSetup->local_enc_sw_option = 0U;

#define DEFAULT_HEVC_AQP_LAMBDA (2U)    //uCode will ignore it when AVC
    pEncodeSetup->aqp_lambda = DEFAULT_HEVC_AQP_LAMBDA;
    pEncodeSetup->force_intlc_tb_iframe = 0U;
    pEncodeSetup->beating_reduction = 0U;
    pEncodeSetup->fast_seek_interval = (pEncodeSetup->gop_structure == 4U/*GOP_FAST_SEEK_SIMPLE*/)? (UINT8)1U: (UINT8)0U;
    pEncodeSetup->recon_offset_y = 0U;
    pEncodeSetup->is_recon_to_vout = 0U;
    pEncodeSetup->disable_sao = 0U; //HEVC I beating tuning tool

    /* Compression Stereo config */
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

    return Rval;
}

static inline UINT32 HL_VideoStreamConfigPreProc(const UINT16 StreamIdx,
                                                 const CTX_STREAM_INFO_s *pStrmInfo,
                                                 UINT8 *pIsVertSliceEnable)
{
    //FIXME, StreamSync
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 IsEfctYuvStrm, IsEncPurpose;
    UINT8 IsVertSliceEnable = *pIsVertSliceEnable;
    UINT16 i, PrevId = 0U, ViewZoneId = 0U, PinId = 0U;
    UINT16 ReconPostStatus = 0U;
    UINT32 Rval = OK;
    CTX_VPROC_INFO_s VprocInfo = {0};
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

    HL_GetResourcePtr(&Resource);
    HL_GetYuvStrmInfoPtr(pStrmInfo->SourceYuvStrmId, &YuvStrmInfo);
    pYuvStrmLayout = &YuvStrmInfo->Layout;
    IsEfctYuvStrm = (1U == HL_GetEffectEnableOnYuvStrm(pStrmInfo->SourceYuvStrmId))? (UINT8)1U: (UINT8)0U;
    IsEncPurpose = (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_ENCODE_IDX, 1U))? 1U: 0U;

    if (IsEncPurpose > 0U) {
        /* Fetch first chan as SourceViewZone */
        if (1U == IsEfctYuvStrm) {
            if (pYuvStrmLayout->EffectBufDesc.LastPsThIdx != DSP_EFCT_INVALID_IDX) {
                ViewZoneId = pYuvStrmLayout->EffectBufDesc.LastPsThIdx;
            } else {
                DSP_ReverseBit2U16Idx(YuvStrmInfo->MaxChanBitMask, &ViewZoneId);
            }
        } else {
            ViewZoneId = pYuvStrmLayout->ChanCfg[0U].ViewZoneId;
        }

        HL_GetVprocInfo(HL_MTX_OPT_ALL, ViewZoneId, &VprocInfo);
        HL_GetViewZoneInfoPtr(ViewZoneId, &ViewZoneInfo);
        for (i=0U; i<DSP_VPROC_PIN_NUM; i++) {
            if (DSP_GetBit(VprocInfo.PinUsage[i], pStrmInfo->SourceYuvStrmId, 1U) > 0U) {
                PrevId = i;
                break;
            }
        }
        PinId = HL_CtxVprocPinDspPinMap[PrevId];

        if ((ViewZoneInfo->SliceNumRow > 1U) &&
            (PinId == DSP_VPROC_OUT_STRM_PREV_B)) {
            IsVertSliceEnable = 1U;
        } else {
            IsVertSliceEnable = 0U;
        }

        /* 20190703 from uCoder[ChenHan]
         * In CV series Encode LowDelay(For curise) is only support in following case
         *  - Must be StartFromPic
         *  - Must be MJPG
         *  - Must come from PrevB
         *  - Must enable prev_b_sync_to_enc
         *
         * In current uCode design, once StartEncIdc(No matter for low delay strm or not) appears,
         * uCode will send Pic to LowDlyStrm(No matter LowDlyStrm is configed/Binded/Started),
         * which leads EncPicQ FULL.
         *
         * In short,
         * when MJPG-LowDly is needed,
         * we can't support features which need StartFromPic method of NonLowDlyStrm is LowDlyStrm is not running(or will start at same time)
         *  - VdoThm/TimeLapse all need StartFromPic method
         */
//TBD, add check for above limitation

        if (pStrmInfo->VideoTimeLapseActive > 0U) {
            Rval = HL_VidEncCfgPreProcOnTimeLapse(WriteMode,
                                                  pStrmInfo,
                                                  ViewZoneId,
                                                  StreamIdx,
                                                  PinId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_VidEncCfgPreProcOnTimeLapse %u %x strm:%u",
                        __LINE__, Rval, StreamIdx, 0U, 0U);
            }
        } else {
//FIXME, TBD YuvEnc
            Rval = HL_VidEncCfgPreProcOnYuvStrm(WriteMode,
                                                pStrmInfo,
                                                ViewZoneId,
                                                StreamIdx,
                                                PinId);
        }

        if (Rval == OK) {
            WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

            (void)HL_GetReconPostStatus(StreamIdx, &ReconPostStatus);
            if (1U == DSP_GetU16Bit(ReconPostStatus, RECON_POST_2_VPROC_BIT, 1U)) {
                Rval = HL_ReconPostStart(WriteMode, StreamIdx);
            }
        }
    }
    *pIsVertSliceEnable = IsVertSliceEnable;
    return Rval;
}

static inline void HL_UpdateWriteModeOnStrmCfg(UINT8 *pWriteMode, const CTX_STREAM_INFO_s *pStrmInfo)
{
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 PhysVinId = 0U;

    HL_GetYuvStrmInfoPtr(pStrmInfo->SourceYuvStrmId, &pYuvStrmInfo);
    pYuvStrmLayout = &pYuvStrmInfo->Layout;
    HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &pViewZoneInfo);

    /* single yuv stream from VIN_SRC_FROM_DEC use vdsp */
    if ((pYuvStrmLayout->NumChan == 1U) && \
        (pViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW)) {
        if ((pViewZoneInfo->IsUpStrmSrc > (UINT8)0U) &&
            (pStrmInfo->StartMethod == DSP_ENC_STOP_FROM_PICINFO)) {
            HL_GetViewZonePhysVinId(pViewZoneInfo, &PhysVinId);
            *pWriteMode = HL_GetVin2CmdNormalWrite(PhysVinId);
        }
    }
}

static inline UINT32 HL_VideoStreamConfig(UINT16 StreamIdx, UINT32 StrmBitMask)
{
//FIXME, StreamSync
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 IsVertSliceEnable = 0U;
    UINT16 StrmNum = 0U;
    UINT32 Rval = OK;
    CTX_STREAM_INFO_s StrmInfo;

    HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
    DSP_Bit2U16Cnt(StrmBitMask, &StrmNum);
    if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        /* ExtMem is sort of TimeLapse */
        Rval = HL_VidEncCfgPreProcOnExtMem(WriteMode, &StrmInfo, StreamIdx);
        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_VidEncCfgPreProcOnExtMem strm:%u Error %x",
                    StreamIdx, Rval, 0U, 0U, 0U);
        }
    } else {
        Rval = HL_VideoStreamConfigPreProc(StreamIdx, &StrmInfo, &IsVertSliceEnable);
        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_VideoStreamConfigPreProc strm:%u Error %x",
                    StreamIdx, Rval, 0U, 0U, 0U);
        }
    }

    if (Rval == OK) {
        if (StrmInfo.CfgStatus == ENC_STRM_CFG_UPDATE) {
            if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_MJPG) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_encoder_jpeg_setup_t *EncodeJpgSetup = HL_DefCtxCmdBufPtrEncJpg;

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&EncodeJpgSetup, &CmdBufferAddr);
                (void)HL_FillVidEncCfgCmdOnJpg(EncodeJpgSetup,
                                               &StrmInfo,
                                               StrmNum,
                                               StreamIdx,
                                               IsVertSliceEnable);
                Rval = AmbaHL_CmdEncoderJpegSetup(WriteMode, EncodeJpgSetup);
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval == OK) {
                    StrmInfo.CfgStatus = ENC_STRM_CFG_DONE;
                }
            } else {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_encoder_setup_t *EncodeSetup = HL_DefCtxCmdBufPtrEnc;

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&EncodeSetup, &CmdBufferAddr);
                Rval = HL_FillVidEncCfgCmdOnAVCorHEVC(EncodeSetup, &StrmInfo, StrmNum, StreamIdx);
                if (Rval == OK) {
                    HL_UpdateWriteModeOnStrmCfg(&WriteMode, &StrmInfo);
                    Rval = AmbaHL_CmdEncoderSetup(WriteMode, EncodeSetup);
                    if (Rval == OK) {
                        StrmInfo.CfgStatus = ENC_STRM_CFG_DONE;
                    }
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
        if (Rval == OK) {
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

    if (pStrmInfo->InsertSeiUserData == 1U) {
        pEncStart->append_user_data_sei = 1U;
    } else {
        pEncStart->append_user_data_sei = 0U;
    }
#if 0 //FIXME
EncStart.set_pic_order_cnt_type_is2 = (StrmInfo.PicOrderCntType == 1U)? 1U: 0U;
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
                                           const UINT16 *pVinBitOnStrm,
                                           UINT8 *pVinAttachIssued)
{
    UINT32 Rval = OK;
    UINT8 WriteMode;
    UINT16 i, j;
    CTX_STREAM_INFO_s StrmInfo;
    UINT16 WaitEventVinNum = 0U;
    UINT16 WaitEventVinIdx[AMBA_DSP_MAX_VIN_NUM] = {0U};
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 VinId = 0U;

    if (VinAttachMask > 0U) {
        for (j = 0U; j < (AMBA_DSP_MAX_VIN_NUM + AMBA_DSP_MAX_VIRT_VIN_NUM); j++) {

            WriteMode = HL_GetVin2CmdNormalWrite((UINT16)j);

            if (DSP_GetBit(VinAttachMask, j, 1U) == 0U) {
                continue;
            }
            for (i = 0U; i < NumStream; i++) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_attach_event_to_raw_t *VinAttachEvent = NULL;

                if (DSP_GetU16Bit(pVinBitOnStrm[i], j, 1U) == 0U) {
                    continue;
                }

                HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
                HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &pYuvStrmInfo);
                pYuvStrmLayout = &pYuvStrmInfo->Layout;
                HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &pViewZoneInfo);

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
                if ((pViewZoneInfo->IsUpStrmSrc > (UINT8)0U)||
                    ((pViewZoneInfo->InputFromMemory >= VIN_SRC_FROM_DRAM) &&
                     (pViewZoneInfo->InputFromMemory <= VIN_SRC_FROM_DRAM_420))) {
                    HL_GetViewZoneVinId(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &VinId);
                    VinAttachEvent->vin_id = (UINT8)VinId;
                    AmbaLL_LogUInt5("HL_VidEncStartFromVin strm:%u VinId %d",
                            pStreamIdx[i], VinId, 0U, 0U, 0U);
                } else {
                    VinAttachEvent->vin_id = (UINT8)j;
                }
                VinAttachEvent->ch_id = 0x0U; //dont care now

                if (pVinAttachIssued[j] == 0U) {
                    Rval = HL_GetVinAttachEventDefaultMask((UINT16)j, &VinAttachEvent->event_mask);
                    if (Rval == OK) {
                        DSP_SetBit(&VinAttachEvent->event_mask, DSP_BIT_POS_ENC_START);
                        pVinAttachIssued[j] = 1U;
                    }
                }
                if (Rval == OK) {
                    if (VinAttachEvent->event_mask > 0U) { //sanity check
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

static inline UINT32 HL_VideoStrmStartChkEnd(UINT16 NumStream, const UINT16 *pStreamIdx, UINT16 VinActiveBit,
                                             UINT16 Num, const UINT16 *pCapInstance, UINT32 VinAttachMask,
                                             const UINT16 *pVinBitOnStrm, UINT8 *pVinAttachIssued)
{
    UINT32 Rval;
    UINT16 VinId;
    UINT8 WriteMode;

    /* Wait a VDSP event to ensure EncoderSetup has been revceived */
    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);

    if (Rval == OK) {
        for (VinId=0U; VinId<AMBA_DSP_MAX_VIN_NUM; VinId++) {
            if (DSP_GetBit(VinActiveBit, VinId, 1U)==1U) {
                WriteMode = HL_GetVin2CmdGroupWrite(VinId);
                /* Process Encode start in the same cmd Q */
                Rval = DSP_ProcessBufCmds(WriteMode);
            }
            if (Rval != OK) {
                break;
            }
        }

        if (Rval == OK) {
            if (Num > 0U) { //sanity check
                Rval = HL_StillCaptureYuvImpl(Num, pCapInstance);
            } else {
                //FIXME, VinAttach + SyncedYuv case
                // VinAttache once no SyncYuv needed
                Rval = HL_VidEncStartFromVin(NumStream,
                                            pStreamIdx,
                                            VinAttachMask,
                                            pVinBitOnStrm,
                                            pVinAttachIssued);
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_VdoStrmStartPreProc(const CTX_STREAM_INFO_s *pStrmInfo,
                                            UINT8 *pWriteMode,
                                            UINT16 *pVinActiveBit,
                                            UINT16 *pVinBitOnStrm,
                                            UINT32 *pVinAttachMask)
{
    UINT8 WriteMode = *pWriteMode;
    UINT16 Idx, VinId;
    UINT32 Rval = OK;
    UINT16 VinActiveBit = *pVinActiveBit;
    UINT16 VinBitOnStrm = *pVinBitOnStrm;
    UINT32 VinAttachMask = *pVinAttachMask;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 PhysVinId = 0U;

    if (pStrmInfo->SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    } else {
        HL_GetYuvStrmInfoPtr(pStrmInfo->SourceYuvStrmId, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &ViewZoneInfo);

        /* single yuv stream from VIN_SRC_FROM_DEC use vdsp */
        if ((pYuvStrmLayout->NumChan == 1U) && \
            (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW)) {
            if (pStrmInfo->StartMethod == DSP_ENC_STOP_FROM_PICINFO) {
                HL_GetViewZonePhysVinId(ViewZoneInfo, &PhysVinId);
                WriteMode = HL_GetVin2CmdGroupWrite(PhysVinId);
                DSP_SetBit(&VinAttachMask, PhysVinId);
                DSP_SetU16Bit(&VinActiveBit, PhysVinId);
                DSP_SetU16Bit(&VinBitOnStrm, PhysVinId);
            } else {
                WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
            }
        } else {
            for (Idx = 0U; Idx < pYuvStrmLayout->NumChan; Idx++) {
                HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[Idx].ViewZoneId, &ViewZoneInfo);
                if ((ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC) &&
                    (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON)) {
                    VinActiveBit |= ViewZoneInfo->SourceVin;
                    VinBitOnStrm |= ViewZoneInfo->SourceVin;
                }
            }

            /* Take the first channel's sourceVin as master */
            DSP_Bit2U16Idx((UINT32)VinBitOnStrm, &VinId);
            WriteMode = HL_GetVin2CmdGroupWrite(VinId);

            if (pStrmInfo->StartMethod == DSP_ENC_STOP_FROM_PICINFO) {
                DSP_SetBit(&VinAttachMask, VinId);
            }
        }
    }

    *pWriteMode = WriteMode;
    *pVinActiveBit = VinActiveBit;
    *pVinBitOnStrm = VinBitOnStrm;
    *pVinAttachMask = VinAttachMask;
    return Rval;
}

static inline UINT32 HL_VdoStrmStartExec(const UINT16 StreamIdx,
                                         const UINT8 WriteMode,
                                         const CTX_STREAM_INFO_s *pStrmInfo,
                                         const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                         UINT16 *CapInst,
                                         UINT16 *pCapNum)
{
    UINT8 CmdBufferIdEncStart = 0U; void *CmdBufferAddrEncStart = NULL;
    UINT8 CmdBufferIdEncQt = 0U; void *CmdBufferAddrEncQt = NULL;
    UINT16 CapNum = *pCapNum;
    UINT32 Rval;
    UINT32 U32Value;
    cmd_encoder_start_t *EncStart = HL_DefCtxCmdBufPtrEncStart;
    ipcam_real_time_encode_param_setup_t *EncQualitySetup = HL_DefCtxCmdBufPtrEncRt;

    HL_AcqCmdBuffer(&CmdBufferIdEncStart, &CmdBufferAddrEncStart);
    dsp_osal_typecast(&EncStart, &CmdBufferAddrEncStart);
    EncStart->stream_id = (UINT8)StreamIdx;
    Rval = HL_FillVidEncStartCmd(EncStart, pStartConfig, pStrmInfo);

    if (Rval == OK) {
        HL_AcqCmdBuffer(&CmdBufferIdEncQt, &CmdBufferAddrEncQt);
        dsp_osal_typecast(&EncQualitySetup, &CmdBufferAddrEncQt);

        /* Issue Enc Frate Change accompany with Enc Start */
        EncQualitySetup->stream_id = EncStart->stream_id;

        EncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_FRAME_RATE;
        EncQualitySetup->multiplication_factor = 1U;

        U32Value = 0U;
        HL_VideoEnc_FrameRateDivisor((UINT16)EncQualitySetup->stream_id, &U32Value);
        EncQualitySetup->division_factor = (UINT8)U32Value;

        U32Value = 0U;
        Rval = HL_ConvertDspFrameRate(pStrmInfo->FrameRate, &U32Value);
        if (Rval == OK) {
            EncQualitySetup->enc_frame_rate = U32Value;

            /* Issue mvdump for AVC */
            if ((pStrmInfo->MvBufBase > 0U) &&
                (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264)) {
                EncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_MVDUMP_ENABLE;
                EncQualitySetup->mvdump_enable = (UINT8)1U;
            }

            if ((pStrmInfo->ActBufBase > 0U) &&
                (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264)) {
                EncQualitySetup->enable_flags |= STATS_INIT_ACTTHRESH;
                /* fill default value */
                EncQualitySetup->ActThresh[0U] = 800U;
                EncQualitySetup->ActThresh[1U] = 1600U;
                EncQualitySetup->ActThresh[2U] = 4000U;
            }

            Rval = AmbaHL_CmdEncoderRealtimeEncodeSetup(WriteMode, EncQualitySetup);

            /* Issue Enc Start */
            if (Rval == OK) {
                Rval = AmbaHL_CmdEncoderStart(WriteMode, EncStart);

                if (Rval == OK) {
                    UINT16 Idx;
                    /* Issue SetExtmem for VdoThm */
                    for (Idx = 0U; Idx<AMBA_DSP_MAX_DATACAP_NUM; Idx++) {
                        CTX_DATACAP_INFO_s DataCapInfo = {0};

                        HL_GetDataCapInfo(HL_MTX_OPT_ALL, Idx, &DataCapInfo);
                        if ((DataCapInfo.Cfg.CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) &&
                            (DataCapInfo.Status == DATA_CAP_STATUS_2_RUN) &&
                            (DataCapInfo.Cfg.Index == StreamIdx)) {
                            CapInst[CapNum] = Idx;
                            CapNum++;
                        }
                    }
                }
            }
        }
        HL_RelCmdBuffer(CmdBufferIdEncQt);
    }
    HL_RelCmdBuffer(CmdBufferIdEncStart);
    *pCapNum = CapNum;

    return Rval;
}

//FIXME, TimeLapse condition
static inline UINT32 HL_VideoStreamStart(UINT16 NumStream,
                                         const UINT16 *pStreamIdx,
                                         const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE;
    UINT8 EncState = 0U, IsEncodeBusy = 0U;
    UINT32 Rval = OK;
    UINT16 i;

//FIXME, Misra
    (void)pAttachedRawSeq;

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
        UINT8 VinAttachIssued[AMBA_DSP_MAX_VIN_NUM + AMBA_DSP_MAX_VIRT_VIN_NUM];
        UINT16 Num = 0U;
        UINT16 VinActiveBit = 0x0;
        UINT16 CapInst[AMBA_DSP_MAX_DATACAP_NUM] = {0};
        UINT16 VinBitOnStrm[AMBA_DSP_MAX_STREAM_NUM];
        UINT32 VinAttachMask = 0U;
        CTX_STREAM_INFO_s StrmInfo;

        /* Initialize */
        (void)dsp_osal_memset(&VinBitOnStrm[0U], 0, sizeof(UINT16)*AMBA_DSP_MAX_STREAM_NUM);
        (void)dsp_osal_memset(&VinAttachIssued[0U], 0, sizeof(UINT8)*(AMBA_DSP_MAX_VIN_NUM +AMBA_DSP_MAX_VIRT_VIN_NUM));

        for (i = 0U; i < NumStream; i++) {
            HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
            Rval = HL_VdoStrmStartPreProc(&StrmInfo,
                                          &WriteMode,
                                          &VinActiveBit,
                                          &VinBitOnStrm[i],
                                          &VinAttachMask);
            AmbaLL_LogUInt5("HL_VdoStrmStartPreProc strm:%u VinAttachMask:0x%x",
                    pStreamIdx[i], VinAttachMask, 0U, 0U, 0U);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_VdoStrmStartPreProc strm:%u Error:0x%x",
                        pStreamIdx[i], Rval, 0U, 0U, 0U);
            }


            Rval = HL_VdoStrmStartExec(pStreamIdx[i],
                                       WriteMode,
                                       &StrmInfo,
                                       &pStartConfig[i],
                                       CapInst,
                                       &Num);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_VideoStreamStartExec strm:%u Error:0x%x",
                        pStreamIdx[i], Rval, 0U, 0U, 0U);
            }

        }

        if (Rval == OK) {
            Rval = HL_VideoStrmStartChkEnd(NumStream,
                                           pStreamIdx,
                                           VinActiveBit,
                                           Num,
                                           &CapInst[0U],
                                           VinAttachMask,
                                           &VinBitOnStrm[0U],
                                           &VinAttachIssued[0U]);
        }
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

static inline UINT32 HL_FillVidEncStopCmd(cmd_encoder_stop_t *pEncStop,
                                          UINT8 StopOption,
                                          const CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_MJPG) {
        pEncStop->stop_method = AMBA_DSP_VIDEO_ENC_STOP_IMMEDIATELY;
    } else {
        pEncStop->stop_method = StopOption;
    }
    pEncStop->stream_type = 0; /* obsolete */

    return Rval;
}

static inline UINT32 HL_VidEncStopFromTimeLapse(UINT8 WriteMode,
                                                const CTX_STREAM_INFO_s *pStrmInfo,
                                                UINT16 StrmIdx,
                                                UINT8 IsExtMemEnc)
{
    UINT8 IsFirstMem;
    UINT16 VinId;
    UINT32 Rval = OK;
    AMBA_DSP_BUF_s ExtBuf = {0};
    AMBA_DSP_BUF_s AuxBuf = {0};
    CTX_TIMELAPSE_INFO_s TimeLapseInfo;

    HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, (UINT16)pStrmInfo->TimeLapseId, &TimeLapseInfo);
    VinId = TimeLapseInfo.VirtVinId + AMBA_DSP_MAX_VIN_NUM;
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

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
        Rval = HL_LiveviewFillVinExtMem(VinId,
                                        1U/*IsYuvVin2Enc*/,
                                        IsFirstMem,
                                        DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                        DSP_YUV_420,
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
                                                YUV_MONO,
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
                                          UINT16 StrmIdx)
{
    UINT8 WriteMode;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT16 VinId;
    UINT32 Rval;
    AMBA_DSP_BUF_s ExtBuf = {0};
    AMBA_DSP_BUF_s AuxBuf = {0};
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

    WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE;
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
                                    DSP_YUV_422 /* ChromaFmt */,
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
                                        AMBA_DSP_YUV420 /* ChromaFmt */,
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
            if (Rval == OK) {
                Rval = DSP_ProcessBufCmds(WriteMode);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[%d]VidStrmStop:DSP_ProcessBufCmds 0x%X strm[%d]", __LINE__, Rval, StrmIdx, 0U, 0U);
                }
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_VidEncStopFromVirtVin(UINT16 NumStream,
                                              const UINT16 *pStrmIdx)
{
    UINT8 WriteMode;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT8 IsFirstMem;
    UINT16 i, VinId, ViewZoneId;
    UINT32 Rval = OK, NewWp;
    AMBA_DSP_BUF_s ExtBuf = {0};
    AMBA_DSP_BUF_s AuxBuf = {0};
    CTX_STREAM_INFO_s StrmInfo = {0};
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;
    cmd_vin_send_input_data_t *FeedData = HL_DefCtxCmdBufPtrVinSndData;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

    for (i=0U; i<NumStream; i++) {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, pStrmIdx[i], &StrmInfo);
        HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &ViewZoneInfo);

        if ((pYuvStrmLayout->NumChan == 1U) && \
            ((ViewZoneInfo->InputFromMemory >= VIN_SRC_FROM_DRAM) &&
             (ViewZoneInfo->InputFromMemory <= VIN_SRC_FROM_DRAM_420))) {
            WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
            ViewZoneId = pYuvStrmLayout->ChanCfg[0U].ViewZoneId;

            HL_GetViewZoneVinId(ViewZoneId, &VinId);
            HL_GetViewZoneInfoPtr(ViewZoneId, &pViewZoneInfo);

            /* pVidDecInfo->LastYuvBuf is updated by AmbaDSP_Event.c: VideoDecodePicPreProcFunc */
            (void)dsp_osal_memset(&ExtBuf, 0, sizeof(AMBA_DSP_BUF_s));
            ExtBuf.BaseAddr       = pViewZoneInfo->ExtYuvBuf.BaseAddrY;
            ExtBuf.Pitch          = pViewZoneInfo->ExtYuvBuf.Pitch;
            ExtBuf.Window.Width   = pViewZoneInfo->ExtYuvBuf.Window.Width;
            ExtBuf.Window.Height  = pViewZoneInfo->ExtYuvBuf.Window.Height;
            AuxBuf.BaseAddr = pViewZoneInfo->ExtYuvBuf.BaseAddrUV;

            IsFirstMem = (pViewZoneInfo->YuvFrameCnt > 0U)? 0U: 1U;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
            Rval = HL_LiveviewFillVinExtMem(VinId,
                                            0U /* IsYuvVin2Enc */,
                                            IsFirstMem,
                                            DSP_VIN_EXT_MEM_TYPE_YUV422 /* MemType */,
                                            YuvFmtTable[pViewZoneInfo->ExtYuvBuf.DataFmt] /* ChromaFmt */,
                                            VIN_EXT_MEM_CTRL_WAIT_APPEND /* OverFlowCtrl */,
                                            &ExtBuf,
                                            &AuxBuf,
                                            VinExtMem);
            if (Rval == OK) {
                Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[%d]VidStrmStop:CmdVinSetExtMem 0x%X strm[%d]", __LINE__, Rval, *pStrmIdx, 0U, 0U);
                }
            } else {
                AmbaLL_LogUInt5("[%d]VidStrmStop:LiveviewFillVinExtMem 0x%X strm[%d]", __LINE__, Rval, *pStrmIdx, 0U, 0U);
            }
            HL_RelCmdBuffer(CmdBufferId);

            if (Rval == OK) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&FeedData, &CmdBufferAddr);
                FeedData->vin_id = (UINT8)VinId;
                FeedData->chan_id = 0U; //obsoleted
                FeedData->send_data_dest = DSP_VIN_SEND_IN_DATA_DEST_VPROC;
                FeedData->is_compression_en = 0U;
                FeedData->blk_sz = 0U;
                FeedData->mantissa = 0U;
                FeedData->raw_frm_cap_cnt = pViewZoneInfo->YuvFrameCnt;
                FeedData->input_data_type = (pViewZoneInfo->ExtYuvBuf.DataFmt == AMBA_DSP_YUV420) ? (UINT32)DSP_VIN_SEND_IN_DATA_YUV420: (UINT32)DSP_VIN_SEND_IN_DATA_YUV422;
                FeedData->ext_fb_idx = DSP_FB_IDX_FOR_NEW2APPEND;

                HL_GetViewZoneInfoLock(ViewZoneId, &pViewZoneInfo);
                /* Request BatchCmdQ buffer */
                Rval = DSP_ReqBuf(&pViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
                if (Rval != OK) {
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                    AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_BATCHQ_FULL, "[LiveviewFeedYuvData] ViewZone[%d] batch pool is full"
                            , ViewZoneId, 0U, 0U, 0U, 0U);
                } else {
                    UINT32 *pBatchQAddr, BatchCmdId;
                    process_idsp_iso_config_cmd_t *pIsoCfgCmd;
                    ULONG BatchQAddr = 0U;
                    pViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                    HL_GetPointerToDspBatchQ(ViewZoneId, (UINT16)pViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

                    /* Reset New BatchQ after Wp advanced */
                    HL_ResetDspBatchQ(pBatchQAddr);

                    /* IsoCfgCmd occupied first CmdSlot */
                    dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                    (void)HL_FillIsoCfgUpdate(ViewZoneId, pIsoCfgCmd);
                    (void)dsp_osal_virt2cli(pViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->iso_cfg_daddr);

                    dsp_osal_typecast(&BatchQAddr, &pBatchQAddr);
                    (void)dsp_osal_virt2cli(BatchQAddr, &FeedData->batch_cmd_set_info.addr);
                    HL_SetDspBatchQInfo(BatchCmdId, 1/*AutoReset*/, BATCHQ_INFO_ISO_CFG, pViewZoneInfo->StartIsoCfgIndex);
                    FeedData->batch_cmd_set_info.id = BatchCmdId;
                    FeedData->batch_cmd_set_info.size = HL_GetBatchCmdNumber(BatchQAddr)*CMD_SIZE_IN_BYTE;

                    FeedData->encode_start_idc = IsFirstMem;

                    (void)AmbaHL_CmdVinSendInputData(WriteMode, FeedData);

                    HL_GetViewZoneInfoLock(ViewZoneId, &pViewZoneInfo);
                    pViewZoneInfo->YuvFrameCnt++;
                    HL_GetViewZoneInfoUnLock(ViewZoneId);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_VidEncStopFromVin(UINT16 NumStream,
                                          const UINT16 *pStreamIdx,
                                          UINT16 VinActiveBit,
                                          const UINT16 *pVinBitOnStrm,
                                          UINT8 *pVinAttachIssued)
{
    UINT32 Rval = OK;
    UINT16 i, j;
    UINT8 WriteMode;
    CTX_STREAM_INFO_s StrmInfo;

    for (j=0U; j<AMBA_DSP_MAX_VIN_NUM; j++) {
        WriteMode = HL_GetVin2CmdNormalWrite((UINT16)j);

        if (DSP_GetU16Bit(VinActiveBit, j, 1U) == 0U) {
            continue;
        }
        for (i = 0; i<NumStream; i++) {
            if (DSP_GetU16Bit(pVinBitOnStrm[i], j, 1U) == 0U) {
                continue;
            }

            HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
            if (StrmInfo.StopMethod == DSP_ENC_STOP_FROM_PICINFO/*|| HL_IS_VDOTHM_FIRST_FRAME(VideoThmb[i])*/) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vin_attach_event_to_raw_t *VinAttachEvent = HL_DefCtxCmdBufPtrVinAttRaw;
//FIXME. MultiChan
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VinAttachEvent, &CmdBufferAddr);
                VinAttachEvent->vin_id = (UINT8)j;
                VinAttachEvent->ch_id = 0x0U; //dont care now
                if ((StrmInfo.VideoTimeLapseActive == 0U) && (pVinAttachIssued[j] == 0U)) {
                    Rval = HL_GetVinAttachEventDefaultMask((UINT16)j, &VinAttachEvent->event_mask);
                    if (Rval == OK) {
                        DSP_SetBit(&VinAttachEvent->event_mask, DSP_BIT_POS_ENC_STOP);
                        pVinAttachIssued[j] = 1U;
                    }
                }

                if (Rval == OK) {
                    /* Sanity check */
                    if (VinAttachEvent->event_mask > 0U) {
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
                                           const UINT16 *pDecSrcOnStrm,
                                           UINT16 VinId)
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
                                          VinId/*SrcId -> src_fp_id*/,
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

static inline UINT32 HL_VideoStrmStopChkEnd(UINT16 NumStream,
                                            const UINT16 *pStreamIdx,
                                            UINT16 VinId,
                                            UINT16 VinActiveBit,
                                            const UINT16 *pVinBitOnStrm,
                                            UINT8 *pVinAttachIssued,
                                            const UINT16 *pDecSrcOnStrm)
{
    UINT32 Rval;

    /* VideoEncode stop From Vin Cmd */
    Rval = HL_VidEncStopFromVin(NumStream,
                                pStreamIdx,
                                VinActiveBit,
                                pVinBitOnStrm,
                                pVinAttachIssued);
    if (Rval == OK) {
        /* VideoEncode stop when source YuvStream is from DRAM */
        Rval = HL_VidEncStopFromVirtVin(NumStream, pStreamIdx);
    }
    if (Rval == OK) {
        /* VideoEncode stop postproc */
        Rval = HL_VidEncStopPostProc(NumStream,
                                     pStreamIdx,
                                     pDecSrcOnStrm,
                                     VinId);
    }

    return Rval;
}

static inline UINT32 HL_VideoStreamStopPreProc(const UINT16 StreamIdx,
                                               const UINT8 StopOption,
                                               CTX_VID_DEC_INFO_s *pVidDecInfo,
                                               UINT16 *pVinBitOnStrm,
                                               UINT16 *pDecSrcOnStrm,
                                               UINT16 *pVinActiveBit)
{
    UINT8 EncState, IsExtMemEnc = (UINT8)0U;
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_GROUP_WRITE;
    UINT16 VinBitOnStrm = *pVinBitOnStrm;
    UINT16 DecSrcOnStrm = *pDecSrcOnStrm;
    UINT16 VinActiveBit = *pVinActiveBit;
    UINT32 Rval = OK;
    CTX_STREAM_INFO_s StrmInfo = {0};
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;

    HL_GetStrmInfo(HL_MTX_OPT_ALL, StreamIdx, &StrmInfo);
    if (StrmInfo.SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        IsExtMemEnc = (UINT8)1U;
        WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    } else {
        IsExtMemEnc = (UINT8)0U;

        HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &YuvStrmInfo);
        pYuvStrmLayout = &YuvStrmInfo->Layout;
        HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[0U].ViewZoneId, &ViewZoneInfo);

        /* VIN_SRC_FROM_DEC */
        if ((pYuvStrmLayout->NumChan == 1U) && \
            (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC)) {

            HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, pVidDecInfo);
            Rval = HL_VidEncStopFromDecPreProc(pVidDecInfo, &DecSrcOnStrm, &StrmInfo, StreamIdx);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[%d]VidStrmStop:HL_VidEncStopFromDecPreProc 0x%X strm[%d]",
                        __LINE__, Rval, StreamIdx, 0U, 0U);
            }
        /* other yuv stream use vdsp */
        } else if ((pYuvStrmLayout->NumChan == 1U) && \
                   (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_HW)) {
            WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
        } else {
            UINT16 j, CurrentVinId = 0U;

            for (j=0; j<pYuvStrmLayout->NumChan; j++) {
                HL_GetViewZoneInfoPtr(pYuvStrmLayout->ChanCfg[j].ViewZoneId, &ViewZoneInfo);
                if ((ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_DEC) &&
                    (ViewZoneInfo->InputFromMemory != VIN_SRC_FROM_RECON)) {
                    VinActiveBit |= ViewZoneInfo->SourceVin;
                    VinBitOnStrm |= ViewZoneInfo->SourceVin;
                }
            }

            /* Take the first channel's sourceVin as master */
            DSP_Bit2U16Idx((UINT32)VinBitOnStrm, &CurrentVinId);
            WriteMode = HL_GetVin2CmdGroupWrite(CurrentVinId);
        }
    }

    EncState = DSP_GetEncState(StreamIdx);
    if (EncState == DSP_ENC_STATUS_BUSY) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_encoder_stop_t *EncStop = HL_DefCtxCmdBufPtrEncStop;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&EncStop, &CmdBufferAddr);
        EncStop->channel_id = (UINT8)StreamIdx;
        (void)HL_FillVidEncStopCmd(EncStop, StopOption, &StrmInfo);
        StrmInfo.CfgStatus = ENC_STRM_CFG_UPDATE;
        HL_SetStrmInfo(HL_MTX_OPT_ALL, StreamIdx, &StrmInfo);
        Rval = AmbaHL_CmdEncoderStop(WriteMode, EncStop);
        HL_RelCmdBuffer(CmdBufferId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]VidStrmStop:CmdEncoderStop 0x%X strm[%d]",
                    __LINE__, Rval, StreamIdx, 0U, 0U);
        }
    } else {
        Rval = DSP_ERR_0004;
    }

    if (Rval == OK) {
        if ((StrmInfo.VideoTimeLapseActive == 1U) &&
            (StrmInfo.TimeLapseId != DSP_TIMELAPSE_IDX_INVALID)) {
            Rval = HL_VidEncStopFromTimeLapse(WriteMode, &StrmInfo, StreamIdx, IsExtMemEnc);
        }

        if (Rval == OK) {
            /* VIN_SRC_FROM_DEC, feed last frame to trigger encoder stop */
            if (DecSrcOnStrm == 1U) {
                Rval = HL_VidEncStopFromDec(pVidDecInfo, StreamIdx);
            }
        }
    }
    *pVinBitOnStrm = VinBitOnStrm;
    *pDecSrcOnStrm = DecSrcOnStrm;
    *pVinActiveBit = VinActiveBit;
    return Rval;
}

//FIXME, TimeLapse condition
static inline UINT32 HL_VideoStreamStop(const UINT16 NumStream,
                                        const UINT16 *pStreamIdx,
                                        const UINT8 *pStopOption,
                                        const UINT64 *pAttachedRawSeq)
{

    UINT8 VinAttachIssued[AMBA_DSP_MAX_VIN_NUM];
    UINT16 VinActiveBit = 0x0;
    UINT16 VinBitOnStrm[AMBA_DSP_MAX_STREAM_NUM];
    UINT16 DecSrcOnStrm[AMBA_DSP_MAX_STREAM_NUM];
    UINT32 Rval = OK, i;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

//FIXME, StreamSync
//FIXME, Misra
    (void)pAttachedRawSeq;

    /* Initialize */
//FIXME, DecSrcOnStrm, with pip
//FIXME, DecSrcOnStrm, feed yuv at begin, frame size not match
//FIXME, DecSrcOnStrm, dec -> feed yuv -> stop
    (void)dsp_osal_memset(&VinBitOnStrm[0U], 0, sizeof(UINT16)*AMBA_DSP_MAX_STREAM_NUM);
    (void)dsp_osal_memset(&DecSrcOnStrm[0U], 0, sizeof(UINT16)*AMBA_DSP_MAX_STREAM_NUM);
    (void)dsp_osal_memset(&VinAttachIssued[0U], 0, sizeof(UINT8)*AMBA_DSP_MAX_VIN_NUM);
    HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);

    for (i = 0U; i<NumStream; i++) {
        Rval = HL_VideoStreamStopPreProc(pStreamIdx[i],
                                         pStopOption[i],
                                         &VidDecInfo,
                                         &VinBitOnStrm[i],
                                         &DecSrcOnStrm[i],
                                         &VinActiveBit);
        if (Rval != OK) {
            break;
        }
    }

    if (Rval == OK) {
        UINT16 VinId;

        /* Process for physical vin */
        for (VinId=0; VinId<AMBA_DSP_MAX_VIN_NUM; VinId++) {
            if (DSP_GetBit(VinActiveBit, VinId, 1U)==1U) {
                UINT8 WriteMode = HL_GetVin2CmdGroupWrite(VinId);

                /* wait one VDSP event to prevent multiple EncStart taking effect on different frame */
                Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP1_EVENT, 1U, WAIT_FLAG_TIMEOUT);
                if (Rval == OK) {
                    /* Process Encode start in the same cmd Q */
                    Rval = DSP_ProcessBufCmds(WriteMode);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[%d]VidStrmStop:ProcessBufCmds 0x%X VinId[%d]",
                                __LINE__, Rval, VinId, 0U, 0U);
                    }
                }
            }
            if (Rval != OK) {
                break;
            }
        }

    //FIXME, wait CmdSend to prevent EncStart and VinAttachEvent Racing
        /* WaitVdsp? */
        if (Rval == OK) {
            Rval = HL_VideoStrmStopChkEnd(NumStream,
                                          pStreamIdx,
                                          VinId,
                                          VinActiveBit,
                                          &VinBitOnStrm[0U],
                                          &VinAttachIssued[0U],
                                          &DecSrcOnStrm[0U]);
        }
    }

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

static inline UINT32 HL_EncCfgStrmInfoTimlapse(const UINT16 StreamIdx,
                                               const UINT8 MaxStrmFmt,
                                               const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig,
                                               CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT32 Rval = OK;

    pStrmInfo->VideoTimeLapseActive = pStreamConfig->IntervalCapture;
    if (1U == DSP_GetU8Bit(MaxStrmFmt, ENC_STRM_FMT_TIMELAPSE_BIT_IDX, ENC_STRM_FMT_TIMELAPSE_LEN)) {
        if (pStrmInfo->TimeLapseId == DSP_TIMELAPSE_IDX_INVALID) {
            UINT16 TimeLapseId = 0U;

            Rval = HL_ReqTimeLapseId(StreamIdx, &TimeLapseId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaDSP_VideoEncConfig Error[0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            } else {
                pStrmInfo->TimeLapseId = (UINT8)TimeLapseId;
            }
        }
    } else {
        if (pStrmInfo->TimeLapseId != DSP_TIMELAPSE_IDX_INVALID) {
            Rval = HL_FreeTimeLapseId(pStrmInfo->TimeLapseId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("AmbaDSP_VideoEncConfig Error[0x%X][%d]", Rval, __LINE__, 0U, 0U, 0U);
            } else {
                pStrmInfo->TimeLapseId = DSP_TIMELAPSE_IDX_INVALID;
            }
        }
    }

    return Rval;
}

static inline UINT32 HL_EncCfgStrmInfo(const UINT16 StreamIdx,
                                       const UINT8 MaxStrmFmt,
                                       const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig,
                                       CTX_STREAM_INFO_s *pStrmInfo)
{
    UINT8 CalcLevelIdc = 0U, CalcTierIdc = 0U;
    UINT32 Rval;

    pStrmInfo->CodingFmt = pStreamConfig->CodingFmt;
    pStrmInfo->SourceYuvStrmId = pStreamConfig->SrcYuvStreamId; //Update

    (void)dsp_osal_memcpy(&pStrmInfo->Window, &pStreamConfig->Window, sizeof(AMBA_DSP_WINDOW_s));
    (void)dsp_osal_memcpy(&pStrmInfo->FrameRate, &pStreamConfig->FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));

    pStrmInfo->Rotate = pStreamConfig->Rotate;
    Rval = HL_EncCfgStrmInfoTimlapse(StreamIdx,
                                     MaxStrmFmt,
                                     pStreamConfig,
                                     pStrmInfo);
    if (Rval == OK) {
        pStrmInfo->ProfileIDC = pStreamConfig->EncConfig.ProfileIDC;
        (void)HL_GetLevelIdc(pStreamConfig, &CalcLevelIdc, &CalcTierIdc);
        if ((pStreamConfig->EncConfig.LevelIDC > 0U) &&
            (pStreamConfig->EncConfig.LevelIDC < CalcLevelIdc)) {
            AmbaLL_LogUInt5("LevelIDC[%d] adjust to [%d]",
                            pStreamConfig->EncConfig.LevelIDC, CalcLevelIdc, 0U, 0U, 0U);
            pStrmInfo->LevelIDC = CalcLevelIdc;
        } else if (pStreamConfig->EncConfig.LevelIDC == 0U) { /* Use Ambarella default setting */
            pStrmInfo->LevelIDC = (UINT8)0U;
        } else {
            pStrmInfo->LevelIDC = pStreamConfig->EncConfig.LevelIDC;
        }

        if (pStrmInfo->LevelIDC != (UINT8)0U) {
            pStrmInfo->TierIDC = CalcTierIdc;
        }
        pStrmInfo->IsCabac = pStreamConfig->EncConfig.IsCabac;

        (void)dsp_osal_memcpy(&pStrmInfo->GOPCfg,
                              &pStreamConfig->EncConfig.GOPCfg,
                              sizeof(AMBA_DSP_VIDEO_GOP_CONFIG_s));
        pStrmInfo->NumPRef = pStreamConfig->EncConfig.NumPRef;
        pStrmInfo->NumBRef = pStreamConfig->EncConfig.NumBRef;
        pStrmInfo->FirstGOPStartB = pStreamConfig->EncConfig.FirstGOPStartB;
        (void)dsp_osal_memcpy(&pStrmInfo->DeblockFilter,
                              &pStreamConfig->EncConfig.DeblockFilter,
                              sizeof(AMBA_DSP_VIDEO_DEBLOCK_CTRL_s));
        (void)dsp_osal_memcpy(&pStrmInfo->FrameCrop,
                              &pStreamConfig->EncConfig.FrameCrop,
                              sizeof(AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s));
        (void)dsp_osal_memcpy(&pStrmInfo->Vui,
                              &pStreamConfig->EncConfig.Vui,
                              sizeof(AMBA_DSP_VIDEO_ENC_VUI_s));
        (void)dsp_osal_memcpy(&pStrmInfo->BitRateCfg,
                              &pStreamConfig->EncConfig.BitRateCfg,
                              sizeof(AMBA_DSP_VIDEO_BIT_RATE_CFG_s));

        pStrmInfo->BitsBufAddr = pStreamConfig->EncConfig.BitsBufAddr;
        pStrmInfo->BitsBufSize = pStreamConfig->EncConfig.BitsBufSize;
        if ((pStrmInfo->BitsBufAddr > 0U) && (pStrmInfo->BitsBufSize > 0U)) {
            LL_SetMsgDispatcherBitsInfo(pStrmInfo->BitsBufAddr, pStrmInfo->BitsBufSize);
        }

        pStrmInfo->NumSlice = pStreamConfig->EncConfig.NumSlice;
        /* Tile number is only for HEVC encode */
        if (pStrmInfo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
            UINT8 MaxTileNum = 0U;
            Rval = HL_GetHevcMaxTileNum(&MaxTileNum, pStrmInfo->Window.Width);
            if (MaxTileNum >= pStreamConfig->EncConfig.NumTile) {
                pStrmInfo->NumTile = pStreamConfig->EncConfig.NumTile;
            } else {
                AmbaLL_LogUInt5("TileNum[%d] excessed, adjust to [%d]",
                                 pStreamConfig->EncConfig.NumTile, MaxTileNum, 0U, 0U, 0U);
                pStrmInfo->NumTile = MaxTileNum;
            }
        } else {
            pStrmInfo->NumTile = 0U;
        }
        pStrmInfo->NumExtraRecon = pStreamConfig->EncConfig.NumExtraRecon;

        (void)dsp_osal_memcpy(&pStrmInfo->IntraRefreshCtrl,
                              &pStreamConfig->EncConfig.IntraRefreshCtrl,
                              sizeof(AMBA_DSP_VIDEO_INTRA_REFRESH_CTRL_s));;

        pStrmInfo->PicOrderCntType = pStreamConfig->EncConfig.PicOrderCntType;
        pStrmInfo->SPSPPSHeaderInsertMode = pStreamConfig->EncConfig.SPSPPSHeaderInsertMode;
        pStrmInfo->AUDMode = pStreamConfig->EncConfig.AUDMode;
        pStrmInfo->InsertSeiUserData = pStreamConfig->EncConfig.InsertSeiUserData;
        pStrmInfo->NonRefPIntvl = pStreamConfig->EncConfig.NonRefPIntvl;

        pStrmInfo->QuantMatrixAddr = pStreamConfig->EncConfig.QuantMatrixAddr;
        (void)dsp_osal_memcpy(&pStrmInfo->JpgBitRateCtrl,
                              &pStreamConfig->EncConfig.JpgBitRateCtrl,
                              sizeof(AMBA_DSP_JPEG_BIT_RATE_CTRL_s));

        pStrmInfo->CfgStatus = ENC_STRM_CFG_UPDATE;
    } else {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncConfig HL_EncCfgStrmInfoTimlapse Error[0x%X]",
                Rval, 0U, 0U, 0U, 0U);
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

    Rval = HL_VideoEncConfigChk(NumStream, pStreamIdx, pStreamConfig);

    /* Body */
    if (Rval == OK) {
        /* VideoEncode or Xcode*/
        UINT8 DspProfState = DSP_GetProfState();
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetResourcePtr(&Resource);
        if (DspProfState == DSP_PROF_STATUS_CAMERA) {

            for (i=0U; i<NumStream; i++) {
                HL_GetStrmInfo(HL_MTX_OPT_GET, pStreamIdx[i], &StrmInfo);

                Rval = HL_EncCfgStrmInfo(pStreamIdx[i],
                                         Resource->MaxStrmFmt[pStreamIdx[i]],
                                         &pStreamConfig[i],
                                         &StrmInfo);
                HL_SetStrmInfo(HL_MTX_OPT_SET, pStreamIdx[i], &StrmInfo);
                if (Rval != OK) {
                    ErrLine = __LINE__;
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
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    Rval = HL_VideoEncStopChk(NumStream, pStreamIdx, pStopOption, pAttachedRawSeq);

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
    UINT32 U32Value;
    UINT32 Rval, ErrLine = 0U;
    CTX_STREAM_INFO_s StrmInfo;
    ipcam_real_time_encode_param_setup_t *EncQualitySetup = HL_DefCtxCmdBufPtrEncRt;
    AMBA_DSP_FRAME_RATE_s FrmRate;

    AmbaMisra_TouchUnused(pAttachedRawSeq);

    Rval = HL_VideoEncControlFrameRateChk(NumStream, pStreamIdx, pDivisor, pAttachedRawSeq);

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
                dsp_osal_typecast(&EncQualitySetup, &CmdBufferAddrEncQt);

                EncQualitySetup->stream_id = (UINT8)pStreamIdx[i];
                EncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_FRAME_RATE;
                EncQualitySetup->multiplication_factor = 1U;

                U32Value = 0U;
                HL_VideoEnc_FrameRateDivisor((UINT16)EncQualitySetup->stream_id, &U32Value);
                if (pDivisor[i] > 1U) {
                    U32Value *= pDivisor[i];
                }
                EncQualitySetup->division_factor = (UINT8)U32Value;

                U32Value = 0U;
                (void)dsp_osal_memcpy(&FrmRate, &StrmInfo.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
                if (pDivisor[i] > 1U) {
                    FrmRate.TimeScale /= pDivisor[i];
                }
                Rval = HL_ConvertDspFrameRate(FrmRate, &U32Value);
                if (Rval != OK) {
                    ErrLine = __LINE__;
                } else {
                    EncQualitySetup->enc_frame_rate = U32Value;
                    Rval = AmbaHL_CmdEncoderRealtimeEncodeSetup(WriteMode, EncQualitySetup);
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
    UINT8 CmdBufferIdEncQt = 0U; void *CmdBufferAddrEncQt = NULL;
    UINT32 ErrLine = 0U;
    UINT16 i;
    ipcam_real_time_encode_param_setup_t *pEncQualitySetup = HL_DefCtxCmdBufPtrEncRt;

    (void)pAttachedRawSeq;

    Rval = HL_VideoEncControlRepeatDropChk(NumStream, pStreamIdx, pRepeatDropCfg, pAttachedRawSeq);

    /* Body */
    if (Rval == DSP_ERR_NONE) {
        UINT8 DspProfState = DSP_GetProfState();
        UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;

        if (DspProfState == DSP_PROF_STATUS_CAMERA) {
            for (i = 0U; i < NumStream; i++) {
                HL_AcqCmdBuffer(&CmdBufferIdEncQt, &CmdBufferAddrEncQt);
                dsp_osal_typecast(&pEncQualitySetup, &CmdBufferAddrEncQt);

                pEncQualitySetup->stream_id = (UINT8)pStreamIdx[i];
                pEncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_FRAME_SKIP;
                pEncQualitySetup->frame_skip_repeat = (UINT8)0U;
                pEncQualitySetup->frame_skip = (UINT8)pRepeatDropCfg[i].Count;

                Rval = AmbaHL_CmdEncoderRealtimeEncodeSetup(WriteMode, pEncQualitySetup);
                HL_RelCmdBuffer(CmdBufferIdEncQt);
                if (Rval != DSP_ERR_NONE) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                    break;
                }
            }
        } else {
            Rval = DSP_ERR_0004; ErrLine = __LINE__;
        }
    }

    if (Rval != DSP_ERR_NONE) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncControlRepeatDrop Error[0x%X] %d", Rval, ErrLine, 0U, 0U, 0U);
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

static inline UINT32 IsAligned16(UINT16 In)
{
    UINT32 Rval = 0U;
    if ((In & 0xFU) == 0x0U) {
        Rval = 1U;
    }

    return Rval;

}

#ifdef SUPPORT_VPROC_OSD_INSERT
static inline UINT32 HL_EncBlendBySyncOsdInsert(UINT16 StrmId,
                                                UINT16 LastViewId,
                                                UINT16 PinId,
                                                const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    static const UINT8 DspOsdDataFmtMap[OSD_FORMAT_NUM] =
    {
        [OSD_8BIT_CLUT_MODE]    = DSP_OSD_MODE_VYU565, // DONTCARE
        [OSD_16BIT_VYU_RGB_565] = DSP_OSD_MODE_VYU565, // 0 for yuv
        [OSD_16BIT_UYV_BGR_565] = DSP_OSD_MODE_UYV565,
        [OSD_16BIT_AYUV_4444]   = DSP_OSD_MODE_AYUV4444,
        [OSD_16BIT_RGBA_4444]   = DSP_OSD_MODE_RGBA4444,
        [OSD_16BIT_BGRA_4444]   = DSP_OSD_MODE_BGRA4444,
        [OSD_16BIT_ABGR_4444]   = DSP_OSD_MODE_ABGR4444,
        [OSD_16BIT_ARGB_4444]   = DSP_OSD_MODE_ARGB4444,
        [OSD_16BIT_AYUV_1555]   = DSP_OSD_MODE_AYUV1555,
        [OSD_16BIT_YUV_1555]    = DSP_OSD_MODE_YUV555,
        [OSD_16BIT_RGBA_5551]   = DSP_OSD_MODE_RGBA5551,
        [OSD_16BIT_BGRA_5551]   = DSP_OSD_MODE_BGRA5551,
        [OSD_16BIT_ABGR_1555]   = DSP_OSD_MODE_ABGR1555,
        [OSD_16BIT_ARGB_1555]   = DSP_OSD_MODE_ARGB1555,
        [OSD_32BIT_AYUV_8888]   = DSP_OSD_MODE_AYUV8888,
        [OSD_32BIT_RGBA_8888]   = DSP_OSD_MODE_RGBA8888,
        [OSD_32BIT_BGRA_8888]   = DSP_OSD_MODE_BGRA8888,
        [OSD_32BIT_ABGR_8888]   = DSP_OSD_MODE_ABGR8888,
        [OSD_32BIT_ARGB_8888]   = DSP_OSD_MODE_ARGB8888
    };

    UINT32 Rval = DSP_ERR_NONE;
    CTX_VIEWZONE_INFO_s *pViewZoneInfo = HL_CtxViewZoneInfoPtr;
    UINT16 VinId;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    ULONG CmdBufAddr = 0U;
    ULONG ULAddr;
    cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg;
    vin_fov_batch_cmd_set_t *pBatchCmdSet;
    UINT32 *pBatchQAddr = NULL, BatchCmdId = 0U;
    UINT8 IsNewCmd = 0U;
    UINT32 NewWp = 0U;
    cmd_vproc_osd_insert_t *pVprocOsdInsert = HL_DefCtxCmdBufPtrVpcOsdInsert;
    CTX_STREAM_INFO_s *pStrmInfo = HL_CtxStrmInfoPtr;
    UINT32 OsdBufId = 0U, TmpBufId;
    osd_insert_buf_t *pOsdInsrtBuf = NULL;
    UINT8 OsdMode;
    DSP_OSD_SYNC_JOB_s OsdSyncJob = {0};

    HL_GetViewZoneInfoPtr(LastViewId, &pViewZoneInfo);
    DSP_Bit2U16Idx(pViewZoneInfo->SourceVin, &VinId);
    WriteMode = HL_GetVin2CmdNormalWrite(VinId);
    AmbaLL_Log(AMBALLLOG_TYPE_HYBRID,"SyncOsdInsert  LastViewId[%d] VinId[%d] Wr[%d]", LastViewId, VinId, WriteMode);

    /* Get Bch */
    HL_CtrlBatchQBufMtx(HL_MTX_OPT_GET, LastViewId);
    (void)AmbaLL_CmdQuery(WriteMode, CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM, &CmdBufAddr, (UINT32)VinId, (UINT32)LastViewId);
    if (CmdBufAddr > 0U) {
        /* Cmd Exist */
        dsp_osal_typecast(&pVinAttachProcCfg, &CmdBufAddr);
        (void)dsp_osal_cli2virt(pVinAttachProcCfg->fov_batch_cmd_set_addr, &ULAddr);
        dsp_osal_typecast(&pBatchCmdSet, &ULAddr);
        (void)dsp_osal_cli2virt(pBatchCmdSet->batch_cmd_set_info.addr, &ULAddr);
        BatchCmdId = pBatchCmdSet->batch_cmd_set_info.id;
        AmbaLL_Log(AMBALLLOG_TYPE_HYBRID,"  [SyncOsdInsert] Cmd Exist Id[0x%X] BatchAddr[0x%X%08X]", BatchCmdId, DSP_GetU64Msb((UINT64)ULAddr), DSP_GetU64Lsb((UINT64)ULAddr));
    } else {
        /* NewCmd in CurrentCmdQ, Forward BatchCmdQ WP */
        IsNewCmd = 1U;
        HL_GetViewZoneInfoLock(LastViewId, &pViewZoneInfo);

        /* Request BatchCmdQ buffer */
        Rval = DSP_ReqBuf(&pViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
        if (Rval != DSP_ERR_NONE) {
            HL_GetViewZoneInfoUnLock(LastViewId);
        } else {
            pViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
            HL_GetViewZoneInfoUnLock(LastViewId);

            HL_GetPointerToDspBatchQ(LastViewId, (UINT16)pViewZoneInfo->BatchQPoolDesc.Wp, &pBatchQAddr, &BatchCmdId);

            /* Reset New BatchQ after Wp advanced */
            HL_ResetDspBatchQ(pBatchQAddr);
        }
        HL_GetPointerToDspBatchCmdSet(LastViewId, 0U/*Idx*/, &pBatchCmdSet);
        dsp_osal_typecast(&ULAddr, &pBatchQAddr);

        AmbaLL_Log(AMBALLLOG_TYPE_HYBRID,"  [SyncOsdInsert] NewCmd [0x%X] BchAddr[0x%X]", BatchCmdId, ULAddr, 0U);
    }
    HL_CtrlBatchQBufMtx(HL_MTX_OPT_SET, LastViewId);

    /* Get OsdInsrtCmd Buffer */
    if (IsNewCmd == 1U) {
        /* First Cmd reserved for IsoCfg */
        ULAddr += CMD_SIZE_IN_BYTE;
    } else {
        ULAddr += pBatchCmdSet->batch_cmd_set_info.size;
    }
    dsp_osal_typecast(&pVprocOsdInsert, &ULAddr);

    OsdMode = (UINT8)DSP_GetU8Bit(pBlendCfg->EnableCLUT, 0U, 5U);
//FIXME
    OsdMode = (OsdMode > 0U)? (OsdMode - 1U): 0U;

    /* Fill OsdInsrtCmd */
    pVprocOsdInsert->chan_id = (UINT8)LastViewId;
    pVprocOsdInsert->stream_id = (UINT8)(HL_CtxVprocPinDspPinMap[PinId]);

    if (pBlendCfg->Enable == 0U) {
        pVprocOsdInsert->osd_enable = (UINT8)0U;
    } else {
        if (OsdMode == OSD_8BIT_CLUT_MODE) {
            pVprocOsdInsert->osd_enable = (UINT8)1U;
        } else if (OsdMode <= OSD_16BIT_ARGB_1555) {
            pVprocOsdInsert->osd_enable = (UINT8)2U;
        } else {
            pVprocOsdInsert->osd_enable = (UINT8)3U;
        }
    }

    /* Osd Sync */
    pVprocOsdInsert->osd_insert_always = (UINT8)0U; /* only insert when encode */
    pVprocOsdInsert->force_update_flag = (UINT8)0U;
    pVprocOsdInsert->sync_timestamp = (UINT8)0U;
    pVprocOsdInsert->target_pts = 0U;

    HL_GetStrmInfoPtr(StrmId, &pStrmInfo);

    Rval = DSP_ReqBuf(&pStrmInfo->OsdInsrtBufPool, 1U, &OsdBufId, 1U/*FullnessCheck*/);
    if (Rval == DSP_ERR_NONE) {

        HL_GetStrmInfoLock(StrmId, &pStrmInfo);
        pStrmInfo->OsdInsrtBufPool.Wp = OsdBufId;
        TmpBufId = OsdBufId;
        TmpBufId += 1U;
        OsdSyncJob.BufId = (UINT8)TmpBufId;
        OsdSyncJob.StrmId = (UINT8)StrmId;
        pStrmInfo->OsdSyncBufUserData[OsdBufId] = pBlendCfg->UserData;
        HL_GetStrmInfoUnLock(StrmId);

        /* We only support one OSD in sync mode */
        pVprocOsdInsert->osd_mode = DspOsdDataFmtMap[OsdMode];

        DSP_GetDspVprocOsdInsertBuf(StrmId, (UINT16)OsdBufId, 0U/*Id*/, &pOsdInsrtBuf);
        (void)dsp_osal_virt2cli(pBlendCfg->AlphaBuf.BaseAddrY, &pOsdInsrtBuf->osd_clut_daddr);
        (void)dsp_osal_virt2cli(pBlendCfg->BlendYuvBuf.BaseAddrY, &pOsdInsrtBuf->osd_daddr);
        pOsdInsrtBuf->osd_dpitch = pBlendCfg->BlendYuvBuf.Pitch;
        pOsdInsrtBuf->osd_size.x = (INT16)pBlendCfg->BlendYuvBuf.Window.OffsetX;
        pOsdInsrtBuf->osd_size.y = (INT16)pBlendCfg->BlendYuvBuf.Window.OffsetY;
        pOsdInsrtBuf->osd_size.w = pBlendCfg->BlendYuvBuf.Window.Width;
        pOsdInsrtBuf->osd_size.h = pBlendCfg->BlendYuvBuf.Window.Height;
        (void)dsp_osal_memcpy(&pOsdInsrtBuf->osd_sync.user_id, &OsdSyncJob, sizeof(UINT32));
        pOsdInsrtBuf->osd_sync.pin_code = pBlendCfg->UserData;

        pVprocOsdInsert->osd_num_regions_minus1 = (UINT8)0U;

        DSP_GetDspVprocOsdInsertBuf(StrmId, (UINT16)OsdBufId, 0U, &pOsdInsrtBuf);
        dsp_osal_typecast(&ULAddr, &pOsdInsrtBuf);
        (void)dsp_osal_virt2cli(ULAddr, &pVprocOsdInsert->osd_region_batch_daddr);
    }

    Rval = AmbaHL_CmdVprocOsdInsertCmd(AMBA_DSP_CMD_NOP, pVprocOsdInsert);

    if (Rval == DSP_ERR_NONE) {
        /* Fill BchCmdSet when NewCmd */
        if (IsNewCmd == 1U) {
            pBatchCmdSet->vin_id = VinId;
            pBatchCmdSet->chan_id = (UINT16)HL_GetViewZoneLocalIndexOnVin(LastViewId);

/* FIXME OsdId */
            HL_SetDspBatchQInfo(BatchCmdId, IsNewCmd/*AutoReset*/, 0xFFU/*BATCHQ_INFO_ISO_CFG*/, 0U);
            pBatchCmdSet->batch_cmd_set_info.id = BatchCmdId;

            dsp_osal_typecast(&ULAddr, &pBatchQAddr);
            (void)dsp_osal_virt2cli(ULAddr, &pBatchCmdSet->batch_cmd_set_info.addr);
        }
        pBatchCmdSet->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;

        if (IsNewCmd == 1U) {
            UINT8 CmdBufId = 0U;
            void *pCmdBufAddr = NULL;

            HL_AcqCmdBuffer(&CmdBufId, &pCmdBufAddr);
            dsp_osal_typecast(&pVinAttachProcCfg, &pCmdBufAddr);
            dsp_osal_typecast(&ULAddr, &pBatchCmdSet);
            (void)dsp_osal_virt2cli(ULAddr, &pVinAttachProcCfg->fov_batch_cmd_set_addr);
            pVinAttachProcCfg->vin_id = (UINT8)VinId;
            pVinAttachProcCfg->fov_num = 1U;
            (void)AmbaHL_CmdVinAttachCfgToCapFrm(WriteMode, pVinAttachProcCfg);
            HL_RelCmdBuffer(CmdBufId);
        }
    }

    return Rval;
}
#endif

/**
* VideoEncode blending control function
* @param [in]  NumStream number of stream
* @param [in]  pStreamIdx stream index
* @param [in]  pBlendCfg blend configuration information
* @return ErrorCode
*/
UINT32 dsp_video_enc_ctrl_blend(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 Rval, i, j, ErrLine = 0U;
    CTX_STREAM_INFO_s StrmInfo;
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    const CTX_YUV_STRM_LAYOUT_s *pYuvStrmLayout;
    CTX_VPROC_INFO_s VprocInfo = {0};
    UINT16 PrevId = 0U;

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
                    HL_GetYuvStrmInfoPtr(StrmInfo.SourceYuvStrmId, &pYuvStrmInfo);
                    pYuvStrmLayout = &pYuvStrmInfo->Layout;

                    HL_GetVprocInfo(HL_MTX_OPT_ALL, pYuvStrmLayout->ChanCfg[pYuvStrmLayout->NumChan-1U].ViewZoneId, &VprocInfo);
                    for (j = 0U; j < DSP_VPROC_PIN_NUM; j++) {
                        if (DSP_GetBit(VprocInfo.PinUsage[j], StrmInfo.SourceYuvStrmId, 1U) > 0U) {
                            PrevId = (UINT16)j;
                            break;
                        }
                    }

                    /* Fetch first chan as SourceViewZone */
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    if (pBlendCfg[i].EnableCLUT == 0U) {
                        dsp_osal_typecast(&OsdBlend, &CmdBufferAddr);
                        OsdBlend->channel_id = (UINT8)pYuvStrmLayout->ChanCfg[pYuvStrmLayout->NumChan-1U].ViewZoneId;

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
                    }
#ifdef SUPPORT_VPROC_OSD_INSERT
                    else {
                        if (1U == DSP_GetU8Bit(pBlendCfg[i].EnableCLUT, 5U, 1U)) {
                            Rval = HL_EncBlendBySyncOsdInsert(pStreamIdx[i],
                                                              pYuvStrmLayout->ChanCfg[pYuvStrmLayout->NumChan-1U].ViewZoneId,
                                                              PrevId,
                                                              &pBlendCfg[i]);
                        }
                    }
#endif
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

static inline void HL_FillVidEncCtrlQualityCmd(ipcam_real_time_encode_param_setup_t *pEncQualitySetup, const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    CTX_STREAM_INFO_s StrmInfo;
    UINT16 j;

    pEncQualitySetup->stream_id = (UINT8)StreamIdx;
    if (pQCtrl->DemandIDR > 0U) {
        pEncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_INSERT_IDR;

        pEncQualitySetup->force_idr = (UINT8)1U;
    }

    if (pQCtrl->BitRateCtrl > 0U) {
        pEncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_BITRATE_CHANGE;

        pEncQualitySetup->target_bitrate = pQCtrl->BitRate;
    }

    if (pQCtrl->GOPCtrl > 0U) {
        pEncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_GOP;

        pEncQualitySetup->gop_n_new = pQCtrl->GOPCfg.N;
        pEncQualitySetup->idr_interval_new = (UINT8)pQCtrl->GOPCfg.IdrInterval;
    }

    if (pQCtrl->QPCtrl > 0U) {
        pEncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_QP_LIMIT;

        pEncQualitySetup->qp_min_on_I = pQCtrl->QpCfg.QpMinIFrame;
        pEncQualitySetup->qp_max_on_I = pQCtrl->QpCfg.QpMaxIFrame;
        pEncQualitySetup->qp_min_on_P = pQCtrl->QpCfg.QpMinPFrame;
        pEncQualitySetup->qp_max_on_P = pQCtrl->QpCfg.QpMaxPFrame;
        pEncQualitySetup->qp_min_on_B = pQCtrl->QpCfg.QpMinBFrame;
        pEncQualitySetup->qp_max_on_B = pQCtrl->QpCfg.QpMaxBFrame;
        (void)dsp_osal_memcpy(&pEncQualitySetup->i_qp_reduce, &pQCtrl->QpCfg.QpReduceIFrame, sizeof(UINT8));
        (void)dsp_osal_memcpy(&pEncQualitySetup->p_qp_reduce, &pQCtrl->QpCfg.QpReducePFrame, sizeof(UINT8));
        pEncQualitySetup->aqp = pQCtrl->QpCfg.Aqp;
    }

    if (pQCtrl->RoiCtrl > 0U) {
        pEncQualitySetup->enable_flags |= IPCAM_ENC_PARAM_MDSWCAT;

        (void)dsp_osal_virt2cli(pQCtrl->RoiCfg.IFrmMapAddr, &pEncQualitySetup->mdSwCat_daddr[0U]);
        (void)dsp_osal_virt2cli(pQCtrl->RoiCfg.PFrmMapAddr, &pEncQualitySetup->mdSwCat_daddr[1U]);
        (void)dsp_osal_virt2cli(pQCtrl->RoiCfg.BFrmMapAddr, &pEncQualitySetup->mdSwCat_daddr[2U]);

        HL_GetStrmInfo(HL_MTX_OPT_ALL, StreamIdx, &StrmInfo);
        if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
            for (j = 0U; j < 8U; j++) {
                pEncQualitySetup->md_category_lookup[j] = pQCtrl->RoiCfg.MdCatLUT[j];
            }
        }
    }

    if (pQCtrl->ActThrdCtrl > 0U) {
        HL_GetStrmInfo(HL_MTX_OPT_ALL, StreamIdx, &StrmInfo);
        if (StrmInfo.CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
            pEncQualitySetup->enable_flags |= STATS_INIT_ACTTHRESH;
            pEncQualitySetup->ActThresh[0U] = pQCtrl->ActThrdCfg.Threshold[0U];
            pEncQualitySetup->ActThresh[1U] = pQCtrl->ActThrdCfg.Threshold[1U];
            pEncQualitySetup->ActThresh[2U] = pQCtrl->ActThrdCfg.Threshold[2U];
        } else {
            // DO NOTHING
        }
    }
}

static inline void HL_VidEncCtrlQualityPostUpdate(const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    CTX_STREAM_INFO_s StrmInfo;

    if (pQCtrl->BitRateCtrl > 0U) {
        HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
        StrmInfo.BitRateCtrlStatus = ENC_RT_CTRL_ISSUED;
        StrmInfo.BitRateCfg.BitRate = pQCtrl->BitRate;
        HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
    }

    if (pQCtrl->GOPCtrl > 0U) {
        HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
        StrmInfo.GopCtrlStatus = ENC_RT_CTRL_ISSUED;
        StrmInfo.GOPCfg.N = pQCtrl->GOPCfg.N;
        StrmInfo.GOPCfg.IdrInterval = pQCtrl->GOPCfg.IdrInterval;
        HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
    }

    if (pQCtrl->QPCtrl > 0U) {
        HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
        StrmInfo.QpCtrlStatus = ENC_RT_CTRL_ISSUED;
        (void)dsp_osal_memcpy(&StrmInfo.QpCfg, &pQCtrl->QpCfg, sizeof(AMBA_DSP_VIDEO_QP_CONFIG_s));
        HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
    }

    if (pQCtrl->RoiCtrl > 0U) {
        HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
        StrmInfo.RoiCtrlStatus = ENC_RT_CTRL_ISSUED;
        (void)dsp_osal_memcpy(&StrmInfo.RoiCfg, &pQCtrl->RoiCfg, sizeof(AMBA_DSP_VIDEO_ROI_CONFIG_s));
        HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
    }

    if (pQCtrl->ActThrdCtrl > 0U) {
        HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
        StrmInfo.RoiCtrlStatus = ENC_RT_CTRL_ISSUED;
        (void)dsp_osal_memcpy(&StrmInfo.ActThrdCfg, &pQCtrl->ActThrdCfg, sizeof(AMBA_DSP_VIDEO_ACT_THRD_CONFIG_s));
        HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
    }
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
    ipcam_real_time_encode_param_setup_t *EncQualitySetup = HL_DefCtxCmdBufPtrEncRt;

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

                HL_FillVidEncCtrlQualityCmd(EncQualitySetup, pStreamIdx[i], &pQCtrl[i]);
                Rval = AmbaHL_CmdEncoderRealtimeEncodeSetup(WriteMode, EncQualitySetup);
                HL_RelCmdBuffer(CmdBufferIdEncQt);
                if (Rval != OK) {
                    ErrLine = __LINE__;
                    break;
                } else {
                    HL_VidEncCtrlQualityPostUpdate(pStreamIdx[i], &pQCtrl[i]);
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

static inline UINT32 HL_EncFeedYuvVinExtMemPrepare(const CTX_TIMELAPSE_INFO_s *pTimeLapseInfo,
                                                   const CTX_STREAM_INFO_s *pStrmInfo,
                                                   const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf,
                                                   AMBA_DSP_BUF_s *pCalcExtBuf,
                                                   UINT8 *pWriteMode)
{
    ULONG ULAddr = 0x0U;
    UINT8 IsExtMemEnc = 0U;
    UINT8 WriteMode = *pWriteMode, U8Val;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    UINT32 PhysAddr = 0x0U;
    UINT32 *pBufTblAddr = NULL;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

    if (pStrmInfo->SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        IsExtMemEnc = (UINT8)1U;
    } else {
        IsExtMemEnc = (UINT8)0U;
    }

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
    U8Val = (UINT8)(pTimeLapseInfo->VirtVinId + AMBA_DSP_MAX_VIN_NUM);
    VinExtMem->vin_id = U8Val;
    VinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_YUV422;
    VinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    VinExtMem->allocation_mode = (0U == pTimeLapseInfo->TotalIssuedMemEncodeNum) ? VIN_EXT_MEM_MODE_NEW: VIN_EXT_MEM_MODE_APPEND;
    VinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
    VinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
    if (VinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
    }
#endif
    if (IsExtMemEnc == (UINT8)1U) {
        pCalcExtBuf->Pitch = pExtYuvBuf->ExtYuvBuf.Pitch;
        pCalcExtBuf->Window.Width = pExtYuvBuf->ExtYuvBuf.Window.Width;
        pCalcExtBuf->Window.Height = pExtYuvBuf->ExtYuvBuf.Window.Height;

        WriteMode = HL_GetVin2CmdNormalWrite((UINT16)(pTimeLapseInfo->VirtVinId + AMBA_DSP_MAX_VIN_NUM));
    } else {
        UINT16 ViewZoneId, VinId;

        (void)HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId,
                                            DSP_DATACAP_CALTYPE_LUMA,
                                            pCalcExtBuf);

        /* Update WriteMode */
        HL_GetYuvStrmInfoPtr(pStrmInfo->SourceYuvStrmId, &YuvStrmInfo);
        ViewZoneId = YuvStrmInfo->Layout.ChanCfg[0U].ViewZoneId;
        HL_GetViewZoneVinId(ViewZoneId, &VinId);
        WriteMode = HL_GetVin2CmdNormalWrite(VinId);
    }
    VinExtMem->buf_pitch = pCalcExtBuf->Pitch;
    VinExtMem->buf_width = pCalcExtBuf->Window.Width;
    VinExtMem->buf_height = pCalcExtBuf->Window.Height;
    if (AMBA_DSP_YUV420 == pExtYuvBuf->ExtYuvBuf.DataFmt) {
        VinExtMem->chroma_format = DSP_YUV_420;
    } else if (AMBA_DSP_YUV400 == pExtYuvBuf->ExtYuvBuf.DataFmt) {
        VinExtMem->chroma_format = DSP_YUV_MONO;
    } else {
        VinExtMem->chroma_format = DSP_YUV_422;
    }

    /*
     * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
     * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
     * */
    VinExtMem->vin_yuv_enc_fbp_disable = 0U;

    HL_GetPointerToDspExtRawBufArray(pTimeLapseInfo->VirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                     0U/*Raw*/,
                                     &pBufTblAddr);
    (void)dsp_osal_virt2cli(pExtYuvBuf->ExtYuvBuf.BaseAddrY, &PhysAddr);
    pBufTblAddr[0U] = PhysAddr;
    dsp_osal_typecast(&ULAddr, &pBufTblAddr);
    (void)dsp_osal_virt2cli(ULAddr, &VinExtMem->buf_addr);
    Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_EncFeedYuvVinExtMemPrepare AmbaHL_CmdVinSetExtMem Error:0x%x",
                Rval, 0U, 0U, 0U, 0U);
    }
    HL_RelCmdBuffer(CmdBufferId);

    *pWriteMode = WriteMode;
    return Rval;
}

static inline UINT32 HL_EncFeedYuvVinExtMemPrepareMe(const CTX_TIMELAPSE_INFO_s *pTimeLapseInfo,
                                                     const CTX_STREAM_INFO_s *pStrmInfo,
                                                     const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf,
                                                     const UINT8 WriteMode,
                                                     AMBA_DSP_BUF_s *pCalcExtBuf)
{
    ULONG ULAddr = 0x0U;
    UINT8 IsExtMemEnc = 0U, U8Val;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    UINT32 PhysAddr = 0x0U;
    UINT32 *pBufTblAddr = NULL;
    cmd_vin_set_ext_mem_t *VinExtMem = HL_DefCtxCmdBufPtrVinExtMem;

    if (pStrmInfo->SourceYuvStrmId == ENC_STRM_EXTMEM_IDX) {
        IsExtMemEnc = (UINT8)1U;
    } else {
        IsExtMemEnc = (UINT8)0U;
    }

    /* Me1 */
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VinExtMem, &CmdBufferAddr);
    U8Val = (UINT8)(pTimeLapseInfo->VirtVinId + AMBA_DSP_MAX_VIN_NUM);
    VinExtMem->vin_id = U8Val;
    VinExtMem->memory_type = DSP_VIN_EXT_MEM_TYPE_ME;
    VinExtMem->allocation_type = VIN_EXT_MEM_ALLOC_TYPE_DISTINCT;
    VinExtMem->allocation_mode = (0U == pTimeLapseInfo->TotalIssuedMemEncodeNum)? VIN_EXT_MEM_MODE_NEW: VIN_EXT_MEM_MODE_APPEND;
    VinExtMem->overflow_cntl = VIN_EXT_MEM_CTRL_WAIT_APPEND;
    VinExtMem->num_frm_buf = MAX_EXT_MEM_BUF_NUM;
#if 0
    if (VinExtMem->num_frm_buf > EXT_MEM_VIN_MAX_NUM) {
        AmbaLL_LogUInt5("Only %d ExtMem supported", EXT_MEM_VIN_MAX_NUM, 0U, 0U, 0U, 0U);
    }
#endif
    if (IsExtMemEnc == (UINT8)1U) {
        pCalcExtBuf->Pitch = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Pitch >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
        pCalcExtBuf->Window.Width = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Width >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
        pCalcExtBuf->Window.Height = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Height >> DSP_VIDEO_ME1_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
    } else {
        Rval = HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId, DSP_DATACAP_CALTYPE_ME1, pCalcExtBuf);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("HL_EncFeedYuvVinExtMemPrepareMe HL_StillCaptureCalcExtBufImpl Me1 Error:0x%x",
                        Rval, 0U, 0U, 0U, 0U);
    } else {
        VinExtMem->buf_pitch = pCalcExtBuf->Pitch;
        VinExtMem->buf_width = pCalcExtBuf->Window.Width;
        VinExtMem->buf_height = pCalcExtBuf->Window.Height;
        VinExtMem->chroma_format = YUV_MONO;

        HL_GetPointerToDspExtRawBufArray(pTimeLapseInfo->VirtVinId + AMBA_DSP_MAX_VIN_NUM,
                                         1U/*Me*/,
                                         &pBufTblAddr);
        (void)dsp_osal_virt2cli(pExtYuvBuf->pExtME1Buf->BaseAddr, &PhysAddr);
        pBufTblAddr[0U] = PhysAddr;
        dsp_osal_typecast(&ULAddr, &pBufTblAddr);
        (void)dsp_osal_virt2cli(ULAddr, &VinExtMem->buf_addr);

        /* Fill Aux for Me0 */
        if (IsExtMemEnc == (UINT8)1U) {
            pCalcExtBuf->Pitch = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Pitch >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
            pCalcExtBuf->Window.Width = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Width >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
            pCalcExtBuf->Window.Height = ALIGN_NUM16((pExtYuvBuf->ExtYuvBuf.Window.Height >> DSP_VIDEO_ME0_DATA_SHIFT), (UINT16)DSP_BUF_ALIGNMENT);
        } else {
            Rval = HL_StillCaptureCalcExtBufImpl(pStrmInfo->SourceYuvStrmId, DSP_DATACAP_CALTYPE_ME0, pCalcExtBuf);
        }

        if (Rval != OK) {
            AmbaLL_LogUInt5("HL_EncFeedYuvVinExtMemPrepareMe HL_StillCaptureCalcExtBufImpl Me0 Error:0x%x",
                            Rval, 0U, 0U, 0U, 0U);
        } else {
            VinExtMem->aux_pitch = pCalcExtBuf->Pitch;
            VinExtMem->aux_width = pCalcExtBuf->Window.Width;
            VinExtMem->aux_height = pCalcExtBuf->Window.Height;
            /*
             * vin_yuv_enc_fbp_init will allocate resource for virtual vin yuv input from ext_mem at vin_flow_max stage
             * Need to allocate resource (vin_yuv_enc_fbp_disable=0) when send ext_mem after vproc
             * */
            VinExtMem->vin_yuv_enc_fbp_disable = 0U;

            Rval = AmbaHL_CmdVinSetExtMem(WriteMode, VinExtMem);
            if (Rval != OK) {
                AmbaLL_LogUInt5("HL_EncFeedYuvVinExtMemPrepareMe AmbaHL_CmdVinSetExtMem Error:0x%x",
                                Rval, 0U, 0U, 0U, 0U);
            }
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_EncFeedYuvVinFeedData(const CTX_STREAM_INFO_s *pStrmInfo,
                                              const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf,
                                              const UINT8 WriteMode,
                                              CTX_TIMELAPSE_INFO_s *pTimeLapseInfo)
{
    UINT8 U8Val;
    UINT32 Rval;
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
    if ((AMBA_DSP_YUV420 == pExtYuvBuf->ExtYuvBuf.DataFmt) ||
        (AMBA_DSP_YUV400 == pExtYuvBuf->ExtYuvBuf.DataFmt)) {
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
        AmbaLL_LogUInt5("HL_EncFeedYuvVinFeedData AmbaHL_CmdVinSendInputData Error:0x%x",
                Rval, 0U, 0U, 0U, 0U);
    } else {
        HL_GetTimeLapseInfo(HL_MTX_OPT_GET, (UINT16)pStrmInfo->TimeLapseId, pTimeLapseInfo);
        pTimeLapseInfo->TotalIssuedMemEncodeNum++;
        (void)dsp_osal_memcpy(&(pTimeLapseInfo->LastYuvBuf),
                              &(pExtYuvBuf->ExtYuvBuf),
                              sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        pTimeLapseInfo->LastMe1BufAddr = pExtYuvBuf->pExtME1Buf->BaseAddr;
        HL_SetTimeLapseInfo(HL_MTX_OPT_SET, (UINT16)pStrmInfo->TimeLapseId, pTimeLapseInfo);
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
    UINT32 Rval;
    CTX_STREAM_INFO_s StrmInfo = {0U};
    CTX_TIMELAPSE_INFO_s TimeLapseInfo = {0U};
    AMBA_DSP_BUF_s CalcExtBuf = {0U};
    UINT8 WriteMode = AMBA_DSP_CMD_VCAP_NORMAL_WRITE;
    UINT16 i;

    Rval = HL_VideoEncFeedYuvDataChk(NumStream, pStreamIdx, pExtYuvBuf);

    /* Body */
    if (Rval == OK) {
        for (i = 0U; i < NumStream; i++) {
            HL_GetStrmInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &StrmInfo);
            if ((StrmInfo.VideoTimeLapseActive == 1U) &&
                (StrmInfo.TimeLapseId != DSP_TIMELAPSE_IDX_INVALID)) {
                HL_GetTimeLapseInfo(HL_MTX_OPT_ALL, (UINT16)StrmInfo.TimeLapseId, &TimeLapseInfo);
                Rval = HL_EncFeedYuvVinExtMemPrepare(&TimeLapseInfo,
                                                     &StrmInfo,
                                                     &pExtYuvBuf[i],
                                                     &CalcExtBuf,
                                                     &WriteMode);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("%u HL_EncFeedYuvVinExtMemPrepare Error:0x%x",
                            pStreamIdx[i], Rval, 0U, 0U, 0U);
                } else {
                    Rval = HL_EncFeedYuvVinExtMemPrepareMe(&TimeLapseInfo,
                                                           &StrmInfo,
                                                           &pExtYuvBuf[i],
                                                           WriteMode,
                                                           &CalcExtBuf);
                }
                if (Rval != OK) {
                    AmbaLL_LogUInt5("%u HL_EncFeedYuvVinExtMemPrepareMe Error:0x%x",
                            pStreamIdx[i], Rval, 0U, 0U, 0U);
                } else {
                    Rval = HL_EncFeedYuvVinFeedData(&StrmInfo,
                                                    &pExtYuvBuf[i],
                                                    WriteMode,
                                                    &TimeLapseInfo);
                }
                if (Rval != OK) {
                    AmbaLL_LogUInt5("%u HL_EncFeedYuvVinFeedData Error:0x%x",
                            pStreamIdx[i], Rval, 0U, 0U, 0U);
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncFeedYuvData Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
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

    Rval = HL_VideoEncGrpConfig(pGrpCfg);

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

/**
* VideoEncode config MV
* @param [in]  StreamIdx encode stream index
* @param [in]  pMvCfg MV information
* @return ErrorCode
*/
UINT32 dsp_video_enc_mv_cfg(UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    UINT32 Rval;
    CTX_STREAM_INFO_s StrmInfo;

    Rval = HL_VideoEncMvConfigChk(StreamIdx, pMvCfg);

    /* Body */
    if (Rval == OK) {
        HL_GetStrmInfo(HL_MTX_OPT_GET, StreamIdx, &StrmInfo);
        if (pMvCfg->Option == AMBA_DSP_ENC_MV_FMT_AVC_ACT) {
            StrmInfo.ActBufBase = pMvCfg->BufAddr;
            StrmInfo.ActBufSize = pMvCfg->BufSize;
            StrmInfo.ActBufUnitSize = pMvCfg->UnitSize;
        } else {
            StrmInfo.MvBufBase = pMvCfg->BufAddr;
            StrmInfo.MvBufSize = pMvCfg->BufSize;
            StrmInfo.MvBufUnitSize = pMvCfg->UnitSize;
        }
        StrmInfo.MvOption = pMvCfg->Option;
        HL_SetStrmInfo(HL_MTX_OPT_SET, StreamIdx, &StrmInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VideoEncMvConfig Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
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
