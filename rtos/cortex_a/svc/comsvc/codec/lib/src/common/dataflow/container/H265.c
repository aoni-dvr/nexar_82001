/**
 * @file H265.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 */
#include "H265.h"
#include "FormatAPI.h"
#include "ByteOp.h"
#include "BitOp.h"

/**
 *  HEVC Gop Header
 */
typedef struct {
    UINT32 StartCode_B16_B23_8bit:8;    /**< StartCode_B16_B23_8bit */
    UINT32 StartCode_B8_B15_8bit:8;     /**< StartCode_B8_B15_8bit */
    UINT32 StartCode_B0_B7_8bit:8;      /**< StartCode_B0_B7_8bit */
    UINT32 NalHdr_B8_B15:8;             /**< NalHdr_B8_B15 */
    UINT32 NalHdr_B0_B7:8;              /**< NalHdr_B0_B7 */
    UINT32 VerMain:8;                   /**< VerMain */
    UINT32 VerSub:8;                    /**< VerSub */
    UINT32 NumUnitInTick_B26_B31_6Bit:6;/**< NumUnitInTick_B26_B31_6Bit */
    UINT32 GopSkipLastI:1;              /**< GopSkipLastI */
    UINT32 GopSkipFirstI:1;             /**< GopSkipFirstI */
    UINT32 NumUnitInTick_B18_B25_8Bit:8;/**< NumUnitInTick_B18_B25_8Bit */
    UINT32 NumUnitInTick_B11_B15_5Bit:5;/**< NumUnitInTick_B11_B15_5Bit */
    UINT32 Rsvd0As1:1;                  /**< Rsvd0As1 */
    UINT32 NumUnitInTick_B16_B17_2Bit:2;/**< NumUnitInTick_B16_B17_2Bit */
    UINT32 NumUnitInTick_B3_B10_8Bit:8; /**< NumUnitInTick_B3_B10_8Bit */
    UINT32 TimeScale_B28_B31_4Bit:4;    /**< TimeScale_B28_B31_4Bit */
    UINT32 Rsvd1As1:1;                  /**< Rsvd1As1 */
    UINT32 NumUnitInTick_B0_B2_3Bit:3;  /**< NumUnitInTick_B0_B2_3Bit */
    UINT32 TimeScale_B20_B27_8Bit:8;    /**< TimeScale_B20_B27_8Bit */
    UINT32 TimeScale_B13_B15_3Bit:3;    /**< TimeScale_B13_B15_3Bit */
    UINT32 Rsvd2As1:1;                  /**< Rsvd2As1 */
    UINT32 TimeScale_B16_B19_4Bit:4;    /**< TimeScale_B16_B19_4Bit */
    UINT32 TimeScale_B5_B12_8Bit:8;     /**< TimeScale_B5_B12_8Bit */
    UINT32 GopFirstIPts_B30_B31_2Bit:2; /**< GopFirstIPts_B30_B31_2Bit */
    UINT32 Rsvd3As1:1;                  /**< Rsvd3As1 */
    UINT32 TimeScale_B0_B4_5Bit:5;      /**< TimeScale_B0_B4_5Bit */
    UINT32 GopFirstIPts_B22_B29_8Bit:8; /**< GopFirstIPts_B22_B29_8Bit */
    UINT32 GopFirstIPts_B15_1Bit:1;     /**< GopFirstIPts_B15_1Bit */
    UINT32 Rsvd4As1:1;                  /**< Rsvd4As1 */
    UINT32 GopFirstIPts_B16_B21_6Bit:6; /**< GopFirstIPts_B16_B21_6Bit */
    UINT32 GopFirstIPts_B7_B14_8Bit:8;  /**< GopFirstIPts_B7_B14_8Bit */
    UINT32 Rsvd5As1:1;                  /**< Rsvd5As1 */
    UINT32 GopFirstIPts_B0_B6_7Bit:7;   /**< GopFirstIPts_B0_B6_7Bit */
    UINT32 GopN:8;                      /**< GopN */
    UINT32 Rsvd6As1:4;                  /**< Rsvd6As1 */
    UINT32 GopM:4;                      /**< GopM */
    UINT32 Rsvd0:16;                    /**< Rsvd0 */
} GOP_HEADER_HEVC_s;

static inline UINT32 MAX(UINT32 a, UINT32 b) {return (a > b)?(a):(b);}
static inline UINT32 MIN(UINT32 a, UINT32 b) {return (a < b)?(a):(b);}

static inline UINT32 Ceil(FLOAT Value)
{
    UINT32 Rval = (UINT32)Value;
    if (Value != (FLOAT)Rval) {
        Rval++;
    }
    return Rval;
}

static UINT32 SkipPTL(SVC_BIT_OP_s *Ctx, UINT8 MaxSubLayersMinus1)
{
    UINT32 Rval;
    UINT32 TmpU32;
    /* skipping useless fields of the PTL (12bytes)*/
    Rval = SvcFormat_GetBit(Ctx, 96, &TmpU32);  /* could overflow, but don't care */
    if (Rval == FORMAT_OK) {
        UINT32 i;
        UINT8 SubLayerProfilePresentFlag[8] = {0};
        UINT8 SubLayerLevelPresentFlag[8] = {0};
        for (i = 0; i < MaxSubLayersMinus1; i++) {
            Rval = SvcFormat_Get1Bit(Ctx, &SubLayerProfilePresentFlag[i]);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_Get1Bit(Ctx, &SubLayerLevelPresentFlag[i]);
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            if (MaxSubLayersMinus1 > 0U) {
                Rval = SvcFormat_GetBit(Ctx, (8U - MaxSubLayersMinus1) * 2U, &TmpU32);
            }
            if (Rval == FORMAT_OK) {
                for (i = 0; i < MaxSubLayersMinus1; i++) {
                    if (SubLayerProfilePresentFlag[i] != 0U) {
                        Rval = SvcFormat_GetBit(Ctx, 88, &TmpU32);  /* could overflow, but don't care */
                    }
                    if (Rval == FORMAT_OK) {
                        if (SubLayerLevelPresentFlag[i] != 0U) {
                            Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                        }
                    }
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseVPS(SVC_BIT_OP_s *Ctx, UINT8 *Buffer, UINT32 BufferSize, SVC_H265_VPS_s *VPS)
{
    UINT32 Rval;
    UINT32 TmpU32;
    SvcFormat_InitBitBuf(Ctx, Buffer, BufferSize);
    /*
     * vps_video_parameter_set_id  u(4)
     * vps_reserved_3_2bis         u(2)
     * vps_max_layers_minus1       u(6)
     * vps_max_sub_layers_minus1   u(3)
     */
    Rval = SvcFormat_GetBit(Ctx, 12, &TmpU32);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBit(Ctx, 3, &TmpU32);
        if (Rval == FORMAT_OK) {
            VPS->VpsMaxSubLayersMinus1 = (UINT8)TmpU32;
            /*
             * vps_temporal_id_nesting_flag u(1)
             * vps_reserved_0xffff_16bits   u(16)
             */
            Rval = SvcFormat_GetBit(Ctx, 17, &TmpU32);
            if (Rval == FORMAT_OK) {
                Rval = W2F(AmbaWrap_memcpy(VPS->Generalconfiguration, &Buffer[2], SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH));
                if (Rval == FORMAT_OK) {
                    Rval = SkipPTL(Ctx, VPS->VpsMaxSubLayersMinus1);
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseSubLayerHRD(SVC_BIT_OP_s *Ctx, UINT32 CpbCntMinus1, UINT8 SubPicHrdParamsPresentFlag)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    UINT32 TmpU32;
    UINT8 TmpU8;
    for (i = 0; i <= CpbCntMinus1; i++) {
        /* bit_rate_value_minus1 */
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        /* cpb_size_value_minus1 */
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        }
        if ((SubPicHrdParamsPresentFlag != 0U) && (Rval == FORMAT_OK)) {
            /* cpb_size_du_value_minus1 */
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            /* bit_rate_du_value_minus1 */
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            }
        }
        /* cbr_flag */
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 ParseNalVclHRD(SVC_BIT_OP_s *Ctx, UINT8 *SubPicHrdParamsPresentFlag)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, SubPicHrdParamsPresentFlag);
    if (Rval == FORMAT_OK) {
        UINT32 TmpU32;
        if (*SubPicHrdParamsPresentFlag != 0U) {
            /*
            * tick_divisor_minus2   u(8)
            * du_cpb_removal_delay_increment_length_minus1 u(5)
            * sub_pic_cpb_params_in_pic_timing_sei_flag    u(1)
            * dpb_output_delay_du_length_minus1            u(5)
            */
            Rval = SvcFormat_GetBit(Ctx, 19, &TmpU32);
        }
        if (Rval == FORMAT_OK) {
            /*
             * bit_rate_scale u(4)
             * cpb_size_scale u(4)
             */
            Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
            if (Rval == FORMAT_OK) {
                if (*SubPicHrdParamsPresentFlag != 0U) {
                    /* cpb_size_du_scale */
                    Rval = SvcFormat_GetBit(Ctx, 4, &TmpU32);
                }
                if (Rval == FORMAT_OK) {
                    /*
                     * initial_cpb_removal_delay_length_minus1 u(5)
                     * au_cpb_removal_delay_length_minus1      u(5)
                     * dpb_output_delay_length_minus1          u(5)
                     */
                    Rval = SvcFormat_GetBit(Ctx, 15, &TmpU32);
                }
            }
        }
    }
    return Rval;
}

/**
 * Parse HRD_parameters_set
 *
 */
static UINT32 ParseHRD(SVC_BIT_OP_s *Ctx, UINT8 SpsMaxSubLayersMinus1)
{
    UINT32 Rval;
    UINT8 NalHrdParamsPresentFlag;
    UINT8 VclHrdParamsPresentFlag;
    UINT8 SubPicHrdParamsPresentFlag = 0U;
    Rval = SvcFormat_Get1Bit(Ctx, &NalHrdParamsPresentFlag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_Get1Bit(Ctx, &VclHrdParamsPresentFlag);
        if (Rval == FORMAT_OK) {
            UINT32 TmpU32;
            if ((NalHrdParamsPresentFlag != 0U) || (VclHrdParamsPresentFlag != 0U)) {
                Rval = ParseNalVclHRD(Ctx, &SubPicHrdParamsPresentFlag);
            }
            if (Rval == FORMAT_OK) {
                UINT32 i;
                for (i = 0; i <= SpsMaxSubLayersMinus1; i++) {
                    UINT32 CpbCntMinus1 = 0;
                    UINT8 LowDelayHrdFlag = 0;
                    UINT8 FixedPicRateWithinCVSFlag = 0;
                    UINT8 FixedPicRateGeneralFlag;
                    Rval = SvcFormat_Get1Bit(Ctx, &FixedPicRateGeneralFlag);

                    if (Rval == FORMAT_OK) {
                        if (FixedPicRateGeneralFlag == 0U) {
                            Rval = SvcFormat_Get1Bit(Ctx, &FixedPicRateWithinCVSFlag);
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        if (FixedPicRateWithinCVSFlag != 0U) {
                            /* elemental_duration_in_tc_minus1 */
                            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        } else {
                            Rval = SvcFormat_Get1Bit(Ctx, &LowDelayHrdFlag);
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        if (LowDelayHrdFlag == 0U) {
                            Rval = SvcFormat_GetUe(Ctx, &CpbCntMinus1);
                            if (Rval == FORMAT_OK) {
                                if (CpbCntMinus1 >= 32U) {   /* range 0-31 */
                                //    LOG_ERR("incorrect CpbCntMinus1=%d", CpbCntMinus1);
                                    Rval = FORMAT_ERR_GENERAL_ERROR;
                                }
                            }
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        if (NalHrdParamsPresentFlag != 0U) {
                            Rval = ParseSubLayerHRD(Ctx, CpbCntMinus1, SubPicHrdParamsPresentFlag);
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        if (VclHrdParamsPresentFlag != 0U) {
                            Rval = ParseSubLayerHRD(Ctx, CpbCntMinus1, SubPicHrdParamsPresentFlag);
                        }
                    }
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SkipDefaultDisplayWindow(SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval;
    UINT8 TmpU8;
    /* default_display_window_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        UINT32 TmpU32;
        /* def_disp_win_left_offset */
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            /* def_disp_win_right_offset */
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                /* def_disp_win_top_offset */
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    /* def_disp_win_bottom_offset */
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                }
            }
        }
    }
    return Rval;
}

static UINT32 SkipVideoSignalType(SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval;
    UINT8 TmpU8;
    /* video_signal_type_present_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        UINT32 TmpU32;
        /* video_format */
        Rval = SvcFormat_GetBit(Ctx, 3, &TmpU32);
        if (Rval == FORMAT_OK) {
            /* video_full_range_flag */
            Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
            if (Rval == FORMAT_OK) {
                /* colour_description_present_flag */
                Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
                    /* colour_primaries */
                    Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        /* transfer_characteristics */
                        Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            /* matrix_coefficients */
                            Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseTimingInfo(SVC_BIT_OP_s *Ctx, SVC_H265_VUI_s *VUI, UINT8 SpsMaxSubLayersMinus1)
{
    UINT32 Rval;
    UINT32 A, B;
    UINT8 TmpU8;
    /* vui_timing_info_present_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        Rval = SvcFormat_GetBit(Ctx, 16, &A);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBit(Ctx, 16, &B);
            if (Rval == FORMAT_OK) {
                VUI->NumUnitsInTick = ((A << 16) | B);
                Rval = SvcFormat_GetBit(Ctx, 16, &A);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetBit(Ctx, 16, &B);
                    if (Rval == FORMAT_OK) {
                        VUI->TimeScale = ((A << 16) | B);
                        /* poc_proportional_to_timing_flag */
                        Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                        if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
                            UINT32 TmpU32;
                            /* num_ticks_poc_diff_one_minus1 */
                            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        }
                        if (Rval == FORMAT_OK) {
                            /* vui_hrd_parameters_present_flag */
                            Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                            if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
                                Rval = ParseHRD(Ctx, SpsMaxSubLayersMinus1);
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SkipBitstreamRestriction(SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval;
    UINT8 TmpU8;
    /* bitstream_restriction_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        UINT32 TmpU32;
        /*
         * tiles_fixed_structure_flag   u(1)
         * motion_vectors_over_pic_boundaries_flag   u(1)
         * restricted_ref_pic_lists_flag  u(1)
         */
        Rval = SvcFormat_GetBit(Ctx, 3, &TmpU32);
        if (Rval == FORMAT_OK) {
            /* min_spatial_segmentation_idc */
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                /* max_bytes_per_pic_denom */
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    /* max_bits_per_min_cu_denom */
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        /* log2_max_mv_length_horizontal */
                        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            /* log2_max_mv_length_vertical */
                            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

/**
 * Parse VUI parameters set
 *
 */
static UINT32 ParseVUI(SVC_BIT_OP_s *Ctx, SVC_H265_VUI_s *VUI, UINT8 SpsMaxSubLayersMinus1)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &VUI->AspectRatioInfoPresentFlag);
    if (Rval == FORMAT_OK) {
        UINT32 TmpU32;
        if (VUI->AspectRatioInfoPresentFlag != 0U) {
            Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
            if (Rval == FORMAT_OK) {
                VUI->AspectRatioIdc = (UINT8)TmpU32;
                if (VUI->AspectRatioIdc == 255U) {
                    Rval = SvcFormat_GetBit(Ctx, 16, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        VUI->SarWidth = (UINT16)TmpU32;
                        Rval = SvcFormat_GetBit(Ctx, 16, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            VUI->SarHeight = (UINT16)TmpU32;
                        }
                    }
                }
            }
        }
        if (Rval == FORMAT_OK) {
            UINT8 TmpU8;
            /* overscan_info_present_flag */
            Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
            if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
                /* overscan_appropriate_flag */
                Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
            }
            if (Rval == FORMAT_OK) {
                Rval = SkipVideoSignalType(Ctx);
                if (Rval == FORMAT_OK) {
                    /* chroma_loc_info_present_flag */
                    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
                        /* chroma_sample_loc_type_top_field */
                        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            /* chroma_sample_loc_type_bottom_field */
                            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        /*
                         * neutral_chroma_indication_flag u(1)
                         * field_seq_flag                 u(1)
                         * frame_field_info_present_flag  u(1)
                         */
                        Rval = SvcFormat_GetBit(Ctx, 3, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            Rval = SkipDefaultDisplayWindow(Ctx);
                            if (Rval == FORMAT_OK) {
                                Rval = ParseTimingInfo(Ctx, VUI, SpsMaxSubLayersMinus1);
                                if (Rval == FORMAT_OK) {
                                    #if 0
                                    LOG_VERB("h265 :VUI->AspectRatioInfoPresentFlag = %u", VUI->AspectRatioInfoPresentFlag);
                                    LOG_VERB("h265 :VUI->AspectRatioIdc = %u", VUI->AspectRatioIdc);
                                    LOG_VERB("h265 :VUI->SarWidth = %u", VUI->SarWidth);
                                    LOG_VERB("h265 :VUI->SarHeight = %u", VUI->SarHeight);
                                    LOG_VERB("h265 :VUI->NumUnitsInTick = %u", VUI->NumUnitsInTick);
                                    LOG_VERB("h265 :VUI->TimeScale = %u", VUI->TimeScale);
                                    #endif
                                    /* bitstream_restriction_flag */
                                    Rval = SkipBitstreamRestriction(Ctx);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseShortTermRPS_Prediction1(SVC_BIT_OP_s *Ctx, UINT32 *NumDeltaPocs, UINT32 RpsIdx)
{
    /* delta_rps_sign */
    UINT8 TmpU8;
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if (Rval == FORMAT_OK) {
        UINT32 TmpU32;
        /* abs_delta_rps_minus1 */
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            NumDeltaPocs[RpsIdx] = 0;
            if (RpsIdx > 1U) {
                UINT32 i;
                for (i = 0; i < NumDeltaPocs[RpsIdx - 1U]; i++) {
                    UINT8 UseDeltaFlag = 0;
                    UINT8 UsedByCurrPicFlag;
                    Rval = SvcFormat_Get1Bit(Ctx, &UsedByCurrPicFlag);
                    if (Rval == FORMAT_OK) {
                        if (UsedByCurrPicFlag == 0U) {
                            Rval = SvcFormat_Get1Bit(Ctx, &UseDeltaFlag);
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        if ((UsedByCurrPicFlag == 1U) || (UseDeltaFlag == 1U)) {
                            NumDeltaPocs[RpsIdx]++;
                        }
                    }
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseShortTermRPS_Prediction0(SVC_BIT_OP_s *Ctx, UINT32 *NumDeltaPocs, UINT32 RpsIdx)
{
    UINT32 NumNegativePics = 0U, NumPositivePics = 0U;
    UINT32 Rval = SvcFormat_GetUe(Ctx, &NumNegativePics);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetUe(Ctx, &NumPositivePics);
    }
    if (Rval == FORMAT_OK) {
        UINT32 i, TmpU32;
        UINT8 TmpU8;
        NumDeltaPocs[RpsIdx] = NumNegativePics + NumPositivePics;
        for (i = 0; i < NumNegativePics; i++) {
            /* delta_poc_s0_minus1[rps_idx] */
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            /* used_by_curr_pic_s0_flag[rps_idx] */
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            for (i = 0; i < NumPositivePics; i++) {
                /* delta_poc_s1_minus1[rps_idx] */
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                /* used_by_curr_pic_s1_flag[rps_idx] */
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseShortTermRefPicSet(SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval;
    UINT32 NumShortTermRefPicSets;
    /* num_short_term_ref_pic_sets */
    Rval = SvcFormat_GetUe(Ctx, &NumShortTermRefPicSets);
    if (Rval == FORMAT_OK) {
        /* range 0-64 */
        if (NumShortTermRefPicSets <= 64U) {
            UINT32 RpsIdx;
            UINT32 NumDeltaPocs[SVC_H265_MAX_SHORT_TERM_RPS_COUNT];
            for (RpsIdx = 0; RpsIdx < NumShortTermRefPicSets; RpsIdx++) {
                UINT8 InterRefPicSetPredictionFlag;
                if (RpsIdx != 0U) {
                    /* inter_ref_pic_set_prediction_flag */
                    Rval = SvcFormat_Get1Bit(Ctx, &InterRefPicSetPredictionFlag);
                } else {
                    InterRefPicSetPredictionFlag = 0;
                }
                if (Rval == FORMAT_OK) {
                    if (InterRefPicSetPredictionFlag != 0U) {
                        Rval = ParseShortTermRPS_Prediction1(Ctx, NumDeltaPocs, RpsIdx);
                    } else {
                        Rval = ParseShortTermRPS_Prediction0(Ctx, NumDeltaPocs, RpsIdx);
                    }
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "invalid num_short_term_ref_pic_sets %u", NumShortTermRefPicSets, 0U, 0U, 0U, 0U);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 SkipLongTermRefPicsSps(SVC_BIT_OP_s *Ctx, UINT8 Log2MaxPicOrderCntLsbMinus4)
{
    UINT32 Rval;
    UINT8 TmpU8;
    /* long_term_ref_pics_present_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        UINT32 NumLongTermRefPicsSps;
        /* num_long_term_ref_pics_sps */
        Rval = SvcFormat_GetUe(Ctx, &NumLongTermRefPicsSps);
        if (Rval == FORMAT_OK) {
            UINT32 i;
            UINT32 TmpU32;
            for (i = 0; i < NumLongTermRefPicsSps; i++) {
                UINT32 Len = MIN((UINT32)Log2MaxPicOrderCntLsbMinus4 + 4U, 16U);
                /* lt_ref_pic_poc_lsb_sps[i] */
                Rval = SvcFormat_GetBit(Ctx, (UINT8)Len, &TmpU32);
                /* used_by_curr_pic_lt_sps_flag[i] */
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        }
    }
    return Rval;
}

static UINT32 SkipConformanceWindow(SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval;
    UINT8 TmpU8;
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        UINT32 TmpU32;
        /* skip window offsets*/
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                }
            }
        }
    }
    return Rval;
}

static UINT32 SkipSpsSubLayerOrderingInfo(SVC_BIT_OP_s *Ctx, const SVC_H265_SPS_s *SPS)
{
    UINT32 Rval;
    UINT8 TmpU8;
    /* sps_sub_layer_ordering_info_present_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if (Rval == FORMAT_OK) {
        UINT32 TmpU32;
        UINT32 i = (TmpU8 == 1U) ? 0U : (UINT32)SPS->SpsMaxSubLayersMinus1;
        for (; i <= SPS->SpsMaxSubLayersMinus1; i++) {
            /* max_dec_pic_buffering_minus1 */
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            /* max_num_reorder_pics */
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            }
            /* max_latency_increase_plus1 */
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}

static UINT32 SkipTransform(SVC_BIT_OP_s *Ctx)
{
    /* log2_min_transform_block_size_minus2 */
    UINT32 Rval;
    UINT32 TmpU32;
    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
    if (Rval == FORMAT_OK) {
        /* log2_diff_max_min_transform_block_size */
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            /* max_transform_hierarchy_depth_inter */
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                /* max_transform_hierarchy_depth_intra */
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            }
        }
    }
    return Rval;
}

static UINT32 ParseScalingList(SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 TmpU8;
    /* scaling_list_enabled_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        /* sps_scaling_list_data_present_flag */
        Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
        if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
            UINT32 TmpU32;
            INT32 TmpS32;
            INT32 i, j, k, num_coeffs, JLimit;
            for (i = 0; i < 4; i++) {
                JLimit = (i == 3) ? (INT32)2 : (INT32)6;
                for (j = 0; j < JLimit; j++) {
                    /* scaling_list_pred_mode_flag[i][j] */
                    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                    if ((TmpU8 != 0U) && (Rval == FORMAT_OK)) {
                        /* scaling_list_pred_matrix_id_delta[i][j] */
                        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    } else {
                        if (Rval == FORMAT_OK) {
                            num_coeffs = (INT32)MIN(64U, (UINT32)1U << (4U + ((UINT32)i << 1U)));
                            if (i > 1) {
                                /* scaling_list_dc_coef_minus8[i-2][j] */
                                Rval = SvcFormat_GetSe(Ctx, &TmpS32);
                            }
                            if (Rval == FORMAT_OK) {
                                for (k = 0; k < num_coeffs; k++) {
                                    /* scaling_list_delta_coef */
                                    Rval = SvcFormat_GetSe(Ctx, &TmpS32);
                                    if (Rval != FORMAT_OK) {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        }
    }
    return Rval;
}

static UINT32 SkipPcm(SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval;
    UINT8 TmpU8;
    /* pcm_enabled_flag */
    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
    if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
        UINT32 TmpU32;
        /* pcm_sample_bit_depth_luma_minus1 */
        Rval = SvcFormat_GetBit(Ctx, 4, &TmpU32);
        if (Rval == FORMAT_OK) {
            /* pcm_sample_bit_depth_chroma_minus1 */
            Rval = SvcFormat_GetBit(Ctx, 4, &TmpU32);
            if (Rval == FORMAT_OK) {
                /* log2_min_pcm_luma_coding_block_size_minus3 */
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    /* log2_diff_max_min_pcm_luma_coding_block_size */
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        /* pcm_loop_filter_disabled_flag */
                        Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseSPS_SkipFields(SVC_BIT_OP_s *Ctx, UINT8 Log2MaxPicOrderCntLsbMinus4, UINT32 *Log2MinLumaCodingBlockSizeMinus3, UINT32 *Log2DiffMaxMinLumaCodingBlockSize)
{
    /* log2_min_luma_coding_block_size_minus3 */
    UINT32 Rval = SvcFormat_GetUe(Ctx, Log2MinLumaCodingBlockSizeMinus3);
    if (Rval == FORMAT_OK) {
        /* log2_diff_max_min_luma_coding_block_size */
        Rval = SvcFormat_GetUe(Ctx, Log2DiffMaxMinLumaCodingBlockSize);
        if (Rval == FORMAT_OK) {
            Rval = SkipTransform(Ctx);
            if (Rval == FORMAT_OK) {
                Rval = ParseScalingList(Ctx);
                if (Rval == FORMAT_OK) {
                    UINT8 TmpU8;
                    /* amp_enabled_flag */
                    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                    if (Rval == FORMAT_OK) {
                        /* sample_adaptive_offset_enabled_flag */
                        Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                        if (Rval == FORMAT_OK) {
                            Rval = SkipPcm(Ctx);
                            if (Rval == FORMAT_OK) {
                                Rval = ParseShortTermRefPicSet(Ctx);
                                if (Rval == FORMAT_OK) {
                                    Rval = SkipLongTermRefPicsSps(Ctx, Log2MaxPicOrderCntLsbMinus4);
                                    if (Rval == FORMAT_OK) {
                                        /* sps_temporal_mvp_enabled_flag */
                                        Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                                        if (Rval == FORMAT_OK) {
                                            /* strong_intra_smoothing_enabled_flag */
                                            Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseSPS(SVC_BIT_OP_s *Ctx, UINT8 *Buffer, UINT32 BufferSize, SVC_H265_SPS_s *SPS)
{
    UINT32 Rval, TmpU32;
    SvcFormat_InitBitBuf(Ctx, Buffer, BufferSize);
    /* 3th
       sps_video_parameter_set_id(4)
       sps_max_sub_layers_minus1(3)
       sps_temporal_id_nesting_flags(1)
    */
    Rval = SvcFormat_GetBit(Ctx, 4, &TmpU32);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBit(Ctx, 3, &TmpU32);
    }
    if (Rval == FORMAT_OK) {
        SPS->SpsMaxSubLayersMinus1 = (UINT8)TmpU32;
        Rval = SvcFormat_Get1Bit(Ctx, &SPS->TemporalIdNested);
    }
    if (Rval == FORMAT_OK) {
        Rval = SkipPTL(Ctx, SPS->SpsMaxSubLayersMinus1);
    }
    if (Rval == FORMAT_OK) {
        /* skip seq_parameter_set_id */
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                SPS->ChromaFormat = (UINT8)TmpU32;
                if (SPS->ChromaFormat == 3U) {
                    UINT8 TmpU8;
                    Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        UINT8 Log2MaxPicOrderCntLsbMinus4 = 0U;
        UINT32 PicWidth = 0U, PicHeight = 0U;
        UINT32 Log2MinLumaCodingBlockSizeMinus3 = 0U, Log2DiffMaxMinLumaCodingBlockSize = 0U;
        /* skip pic width & height */
        Rval = SvcFormat_GetUe(Ctx, &PicWidth);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetUe(Ctx, &PicHeight);
        }
        if (Rval == FORMAT_OK) {
            Rval = SkipConformanceWindow(Ctx);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                SPS->BitDepthLumaMinus8 = (UINT8)TmpU32;
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    SPS->BitDepthChromaMinus8 = (UINT8)TmpU32;
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        Log2MaxPicOrderCntLsbMinus4 = (UINT8)TmpU32;
                        Rval = SkipSpsSubLayerOrderingInfo(Ctx, SPS);
                    }
                }
            }
        }
        if (Rval == FORMAT_OK) {
            Rval = ParseSPS_SkipFields(Ctx, Log2MaxPicOrderCntLsbMinus4, &Log2MinLumaCodingBlockSizeMinus3, &Log2DiffMaxMinLumaCodingBlockSize);
        }
        if (Rval == FORMAT_OK) {
            UINT8 TmpU8;
            UINT32 MinCbLog2SizeY = (Log2MinLumaCodingBlockSizeMinus3 + 3U);
            UINT32 CtbLog2SizeY = (MinCbLog2SizeY + Log2DiffMaxMinLumaCodingBlockSize);
            UINT32 CtbSizeY = ((UINT32)1U << CtbLog2SizeY);
            UINT32 PicHeightInCtbsY = (Ceil((FLOAT)PicHeight/(FLOAT)CtbSizeY));
            UINT32 PicWidthInCtbsY = (Ceil((FLOAT)PicWidth/(FLOAT)CtbSizeY));
            SPS->PicSizeInCtbsY = (UINT8)(PicWidthInCtbsY * PicHeightInCtbsY);
            #if 0
            LOG_VERB("h265 :SPS->SpsMaxSubLayersMinus1 = %u", SPS->SpsMaxSubLayersMinus1);
            LOG_VERB("h265 :SPS->TemporalIdNested = %u", SPS->TemporalIdNested);
            LOG_VERB("h265 :SPS->chromaFormat = %u", SPS->ChromaFormat);
            LOG_VERB("h265 :SPS->PicWidth = %u", PicWidth);
            LOG_VERB("h265 :SPS->PicHeight = %u", PicHeight);
            LOG_VERB("h265 :SPS->bitDepthLumaMinus8 = %u", SPS->BitDepthLumaMinus8);
            LOG_VERB("h265 :SPS->bitDepthChromaMinus8 = %u", SPS->BitDepthChromaMinus8);
            LOG_VERB("h265 :SPS->NumShortTermRefPicSets = %u", NumShortTermRefPicSets);
            #endif
            /* vui_parameters_present_flag */
            Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
            if ((Rval == FORMAT_OK) && (TmpU8 != 0U)) {
                Rval = ParseVUI(Ctx, &SPS->VuiParam, SPS->SpsMaxSubLayersMinus1);
            }
        }
    }
    return Rval;
}

static UINT8 StartCodeSeek(const UINT8 *Buffer, UINT8 ZeroNum)
{
    UINT8 Inf = 1U;
    UINT8 i;
    for (i = 0; i < ZeroNum; i++) {
        if (Buffer[i] != 0U) {
            Inf = 0U;
        }
    }
    if (Buffer[i] != 1U) {
        Inf = 0U;
    }
    return Inf;
}

/**
 * Parse NAL
 *
 */
static UINT32 ParseNALU(const UINT8 *Buffer, UINT32 Size, SVC_H265_NALU_s *NAL, UINT32 *ParseOffset)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 Pos = 0;
    *ParseOffset = 0;
    while ((Pos < Size) && (Buffer[Pos] == 0U)) {
        Pos++;
    }
    Pos++;
    if (Pos == Size) {
        *ParseOffset = Pos; // all 0
    } else {
        if (Buffer[Pos - 1U] == 1U) {
            if (Pos >= 3U) {
                UINT32 lz8bcnt = 0;
                if (Pos == 3U) {
                    NAL->PrefixLen = 3;
                    lz8bcnt = 0;
                } else {
                    NAL->PrefixLen = 4;
                    lz8bcnt = Pos - 4U;
                }
                /* The leading_zero_8bits syntax can only be present in the first byte stream NAL unit */
                if ((NAL->FirstNALU == 1U) || (lz8bcnt == 0U)) {
                    UINT8 Found = 0U;
                    UINT8 Done = 0U;
                    UINT8 Info2 = 0U;
                    UINT8 Info3 = 0U;
                    NAL->FirstNALU = 0U;
                    while (Found == 0U) {
                        if (Pos == Size) {
                            UINT32 tz8b = 0;
                            // reach Buffer tail (no other start code)
                            while (Buffer[Pos - 2U - tz8b] == 0U) {   // why 2?
                                tz8b++;
                            }
                            NAL->Len = (Pos - 1U) - (UINT32)NAL->PrefixLen - lz8bcnt - tz8b;
                            NAL->ForbiddenBit = (((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] >> 7) & 0x01U);
                            NAL->NaluType = (UINT8)(((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] >> 1) & 0x3fU);
                            NAL->NuhLayerId = (((((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] << 5) & 0x20U) | (((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen + 1U] >> 3) & 0x1FU)));
                            NAL->TemporalId = ((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen + 1U] & 0x07U);
                            *ParseOffset = Pos; // or Pos - 1?
                            Done = 1U;
                            break;
                        }
                        Pos++;
                        Info3 = StartCodeSeek(&Buffer[Pos - 4U], 3);
                        if (Info3 == 0U) {
                            Info2 = StartCodeSeek(&Buffer[Pos - 3U], 2);
                        }
                        Found = ((Info2 != 0U) || (Info3 != 0U)) ? 1U : 0U;
                    }
                    if (Done == 0U) {
                        INT32 Rewind = 0;
                        UINT32 tz8b = 0;
                        if (Info3 != 0U) {   // why no info 2?
                            while (Buffer[Pos - 5U - tz8b] == 0U) {
                                tz8b++;
                            }
                        }
                        if (Info3 != 0U) {
                            Rewind = -4;
                        } else if (Info2 != 0U) {
                            Rewind = -3;
                        } else {
                                /* Do nothing */
                        }
                        NAL->Len = (Pos + (UINT32)Rewind) - (UINT32)NAL->PrefixLen - lz8bcnt - tz8b;
                        NAL->ForbiddenBit = (((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] >> 7) & 0x01U);
                        NAL->NaluType = (UINT8)(((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] >> 1) & 0x3fU);
                        NAL->NuhLayerId = (((((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] << 5U) & 0x20U) | (((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen + 1U] >> 3U) & 0x1FU)));
                        NAL->TemporalId = ((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen + 1U] & 0x07U);
#if 0
                        (void) AmbaPrint("%s: type %d Pos %d len %d, NAL->reference_idc %d", __func__, NAL->NaluType, Pos, NAL->Len, NAL->nal_reference_idc);
#endif
                        *ParseOffset = (Pos + (UINT32)Rewind);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            } else {
                /* no Start Code at the begin of the NALU */
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        } else {
            /* no Start Code at the begin of the NALU */
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 EliminateEmulationPrevention3byte(UINT8 *Buffer, UINT32 Size, UINT32 *RbspSize)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 RbsBuffer[96] = {0};
    UINT32 i = 0;
    UINT32 CurSize = 0;
    while (i < Size){
        if ( ((i + 2U) < Size) && ((Buffer[i] == 0U) && (Buffer[i + 1U] == 0U) && (Buffer[i + 2U]== 3U)) ){
            RbsBuffer[CurSize] = Buffer[i];
            CurSize++;
            RbsBuffer[CurSize] = Buffer[i + 1U];
            CurSize++;
            i+=2U;
        } else {
            RbsBuffer[CurSize] = Buffer[i];
            CurSize++;
        }
        i++;
    }
    if (CurSize <= sizeof(RbsBuffer)) {
        Rval = W2F(AmbaWrap_memcpy(Buffer, RbsBuffer, CurSize));
        *RbspSize = CurSize;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect CurSize!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Parse H.265 VPS/SPS
 *
 * @param [in] Buffer Source buffer
 * @param [in] BufferSize Buffer length
 * @param [out] VPS Space to store VPS
 * @param [out] SPS Space to store SPS
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_HeaderParse(UINT8 *Buffer, UINT32 BufferSize, SVC_H265_VPS_s *VPS, SVC_H265_SPS_s *SPS)
{
    SVC_BIT_OP_s BitInfo;
    UINT32 Rval = W2F(AmbaWrap_memset(&BitInfo, 0, sizeof(SVC_BIT_OP_s)));
    if (Rval == FORMAT_OK) {
        UINT32 ParseSize = 0;
        SVC_H265_NALU_s NALU = {0};
        NALU.FirstNALU = 1U;
        do {
            UINT32 ParseOffset = 0U;
            UINT32 BufferOffset = BufferSize - ParseSize;
            if (BufferOffset == 0U) {
                break;
            }
            /* Rval = ParseNALU(Buffer + ParseSize, BufferOffset, &NALU, &ParseOffset); */
            Rval = ParseNALU(&Buffer[ParseSize], BufferOffset, &NALU, &ParseOffset);
            if (Rval == FORMAT_OK){
                if (ParseOffset == 0U) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            }
            if (Rval == FORMAT_OK) {
                /* Pos = Buffer + ParseSize + NALU.prefixlen + 2U; */
                UINT8 *Pos = &Buffer[ParseSize + (UINT32)NALU.PrefixLen + 2U];
                if (NALU.NaluType == SVC_H265_NALU_TYPE_VPS) {
                    Rval = ParseVPS(&BitInfo, Pos, NALU.Len - 2U, VPS);
                } else if (NALU.NaluType == SVC_H265_NALU_TYPE_SPS) {
                    UINT32 RbspSize = 0;
                    Rval = EliminateEmulationPrevention3byte(Pos , NALU.Len - 2U, &RbspSize);
                    if (Rval == FORMAT_OK) {
                        Rval = ParseSPS(&BitInfo, Pos, RbspSize, SPS);
                    }
                } else {
                    /* Do nothing */
                }
                if (Rval == FORMAT_OK) {
                    ParseSize += ParseOffset;
                    if (ParseSize > BufferSize) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect ParseSize %u, BufferSize = %u, ParseOffset = %u", ParseSize, BufferSize, ParseOffset, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                }
            }
        } while (Rval == FORMAT_OK);
    }
    return Rval;
}

static UINT32 FindStartCode(UINT32 Offset, const UINT8 *Base, UINT32 Size, UINT32 *NewOffset, UINT16 *StartcodeLen)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if (Size >= 4U) {
        if (Offset < Size) {
            UINT32 OffsetTmp = Offset;
            UINT32 Code = 0;
            INT32 Shift = 24;
            do {
                Code |= ((UINT32)Base[OffsetTmp]  << (UINT32)Shift);
                Rval = SvcFormat_PlusAddr(1, OffsetTmp, Size, &OffsetTmp);
                if (Rval != FORMAT_OK) {
                    break;
                }
                Shift -= 8;
            } while (Shift >= 0);
            if (Rval == FORMAT_OK) {
                UINT32 StartCodePattern = 0x00000001;   /* start code may be 0x00000001 or 0x000001 */
                while ((Code != StartCodePattern) && ((Code & 0xFFFFFF00U) != (StartCodePattern << 8U))) {
                    Code = (Code << 8U) | Base[OffsetTmp];
                    Rval = SvcFormat_PlusAddr(1, OffsetTmp, Size, &OffsetTmp);
                    if ((Rval != FORMAT_OK) || (OffsetTmp == Offset)) {
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                        break;  /* start Code miss... */
                    }
                }
                if (Rval == FORMAT_OK) {
                    if (Code == StartCodePattern) {
                        *StartcodeLen = 4;
                    } else {
                        *StartcodeLen = 3;
                    }
                    Rval = SvcFormat_MinusAddr(4, OffsetTmp, Size, &OffsetTmp);
                    if (Rval == FORMAT_OK) {
                        *NewOffset = OffsetTmp;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Buffer too small!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Read from stream
 *
 * @param [in] Stream Stream handler
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [in] FrameSize Frame size
 * @param [out] DataSize The returned data size
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_ReadStream(SVC_STREAM_HDLR_s *Stream, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 FrameSize, UINT32 *DataSize)
{
    UINT32 Rval = FORMAT_OK;
    const UINT8 Start[SVC_H265_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    UINT32 TmpU32;
    UINT32 TmpFrameSize;
    UINT32 OffsetTmp = BufferOffset;
    *DataSize = 0;
    TmpFrameSize = FrameSize;
    while (TmpFrameSize != 0U) {
        Rval = SvcFormat_GetBe32(Stream, &TmpU32);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_ReadStreamMem(Start, SVC_H265_STRAT_CODE_SIZE, OffsetTmp, BufferBase, BufferSize, &OffsetTmp);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_ReadStream(Stream, TmpU32, OffsetTmp, BufferBase, BufferSize, &OffsetTmp);
        }
        if (Rval == FORMAT_OK) {
            TmpFrameSize -= (TmpU32 + 4U);
            *DataSize += (TmpU32 + SVC_H265_STRAT_CODE_SIZE);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 SvcH265_GetHvcConfig_VPS(UINT32 NALUSize, UINT32 NALUUnitPos, UINT16 NalStartCodeLen, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H265_DEC_CONFIG_s *H265)
{
    UINT32 Rval;
    if (NALUSize > SVC_FORMAT_MAX_VPS_LENGTH) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s incorrect NALUSize!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        UINT32 OffsetTmp = BufferSize;
        Rval = SvcFormat_WriteStreamMem(H265->VPS, NALUSize, NALUUnitPos, BufferBase, BufferSize, &OffsetTmp);
        if (Rval == FORMAT_OK) {
            SVC_BIT_OP_s BitInfo;
            Rval = W2F(AmbaWrap_memset(&BitInfo, 0, sizeof(SVC_BIT_OP_s)));
            if (Rval == FORMAT_OK) {
                SVC_H265_VPS_s H265VPS = {0};
                H265->VPSLen = (UINT16)NALUSize - NalStartCodeLen;
                /* first two bytes are the NAL header */
                Rval = ParseVPS(&BitInfo, &H265->VPS[2], (UINT32)H265->VPSLen - 2U, &H265VPS);
                if (Rval == FORMAT_OK) {
                    H265->NumTemporalLayers = (UINT8)MAX((UINT32)H265->NumTemporalLayers, (UINT32)H265VPS.VpsMaxSubLayersMinus1 + 1U);
                    Rval = W2F(AmbaWrap_memcpy(H265->Generalconfiguration, H265->Generalconfiguration, SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH));
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcH265_GetHvcConfig_SPS(UINT32 NALUSize, UINT32 NALUUnitPos, UINT16 NalStartCodeLen, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H265_DEC_CONFIG_s *H265)
{
    UINT32 Rval;
    if (NALUSize > SVC_FORMAT_MAX_SPS_LENGTH) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s incorrect NALUSize!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        UINT32 OffsetTmp = BufferSize;
        Rval = SvcFormat_WriteStreamMem(H265->SPS, NALUSize, NALUUnitPos, BufferBase, BufferSize, &OffsetTmp);
        if (Rval == FORMAT_OK) {
            UINT8 H265SPSBuffer[SVC_FORMAT_MAX_SPS_LENGTH] = {0};
            H265->SPSLen = (UINT16)NALUSize - NalStartCodeLen;
            /* first two bytes are the NAL header */
            Rval = W2F(AmbaWrap_memcpy(H265SPSBuffer, H265->SPS, H265->SPSLen));
            if (Rval == FORMAT_OK) {
                UINT32 RbspSize = 0;
                Rval = EliminateEmulationPrevention3byte(&H265SPSBuffer[2] , (UINT32)H265->SPSLen - 2U, &RbspSize);
                if (Rval == FORMAT_OK) {
                    SVC_BIT_OP_s BitInfo;
                    Rval = W2F(AmbaWrap_memset(&BitInfo, 0, sizeof(SVC_BIT_OP_s)));
                    if (Rval == FORMAT_OK) {
                        SVC_H265_SPS_s H265SPS = {0};
                        Rval = ParseSPS(&BitInfo, &H265SPSBuffer[2], RbspSize, &H265SPS);
                        if (Rval == FORMAT_OK) {
                            H265->NumTemporalLayers = (UINT8)MAX((UINT32)H265->NumTemporalLayers, (UINT32)H265SPS.SpsMaxSubLayersMinus1 + 1U);
                            H265->BitDepthChromaMinus8 = H265SPS.BitDepthChromaMinus8;
                            H265->BitDepthLumaMinus8 = H265SPS.BitDepthLumaMinus8;
                            H265->ChromaFormat = H265SPS.ChromaFormat;
                            H265->TemporalIdNested = H265SPS.TemporalIdNested;
                            H265->SarWidth = H265SPS.VuiParam.SarWidth;
                            H265->SarHeight = H265SPS.VuiParam.SarHeight;
                            H265->NumUnitsInTick = H265SPS.VuiParam.NumUnitsInTick;
                            H265->TimeScale = H265SPS.VuiParam.TimeScale;
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcH265_GetHvcConfig_PPS(UINT32 NALUSize, UINT32 NALUUnitPos, UINT16 NalStartCodeLen, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H265_DEC_CONFIG_s *H265)
{
    UINT32 Rval;
    if (NALUSize > SVC_FORMAT_MAX_PPS_LENGTH) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s incorrect NALUSize!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        UINT32 OffsetTmp = BufferSize;
        Rval = SvcFormat_WriteStreamMem(H265->PPS, NALUSize, NALUUnitPos, BufferBase, BufferSize, &OffsetTmp);
        if (Rval == FORMAT_OK) {
            H265->PPSLen = (UINT16)NALUSize - NalStartCodeLen;
        }
    }
    return Rval;
}

/**
 * Get Hvc configuration
 *
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [out] H265 The returned configuration
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_GetHvcConfig(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H265_DEC_CONFIG_s *H265)
{
    UINT16 StartCodeLen = 0;
    UINT32 NextNALU = BufferSize;
    UINT32 Rval = FindStartCode(BufferOffset, BufferBase, BufferSize, &NextNALU, &StartCodeLen);
    if (Rval == FORMAT_OK) {
        UINT32 FrameSize = Size;
        do {
            UINT8 NALUUnitType = 0;
            UINT16 NalStartCodeLen = StartCodeLen;
            UINT32 CurrentNALU = NextNALU;
            UINT32 NALUUnitPos = BufferSize;
            Rval = SvcFormat_PlusAddr(StartCodeLen, CurrentNALU, BufferSize, &NALUUnitPos);
            if (Rval == FORMAT_OK) {
                NALUUnitType = SVC_H265_NALU_TYPE(BufferBase[NALUUnitPos]);
                if (NALUUnitType < SVC_H265_NALU_TYPE_IDR_N_LP) {
                    break;
                }
                Rval = FindStartCode(NALUUnitPos, BufferBase, BufferSize, &NextNALU, &StartCodeLen);
            }
            if (Rval == FORMAT_OK) {
                UINT32 NALUSize = (NextNALU >= CurrentNALU) ? (NextNALU - CurrentNALU) : ((BufferSize - CurrentNALU) + NextNALU);
                switch (NALUUnitType) {
                case SVC_H265_NALU_TYPE_VPS:
                    if (H265->VPSLen == 0U) {
                        Rval = SvcH265_GetHvcConfig_VPS(NALUSize, NALUUnitPos, NalStartCodeLen, BufferBase, BufferSize, H265);
                    }
                    break;
                case SVC_H265_NALU_TYPE_SPS:
                    if (H265->SPSLen == 0U) {
                        Rval = SvcH265_GetHvcConfig_SPS(NALUSize, NALUUnitPos, NalStartCodeLen, BufferBase, BufferSize, H265);
                    }
                    break;
                case SVC_H265_NALU_TYPE_PPS:
                    if (H265->PPSLen == 0U) {
                        Rval = SvcH265_GetHvcConfig_PPS(NALUSize, NALUUnitPos, NalStartCodeLen, BufferBase, BufferSize, H265);
                    }
                    break;
                default:
                    /**/
                    break;
                }
                if (Rval == FORMAT_OK) {
                    if (FrameSize <= NALUSize) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s wrong frame Size!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        FrameSize -= NALUSize;
                    }
                }
            }
        } while (Rval == FORMAT_OK);
    }
    return Rval;
}

/**
 * Get frame size
 *
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [in] Align Alignment
 * @param [out] FrameSize Frame size
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_GetFrameSize(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, UINT32 Align, UINT32 *FrameSize)
{
    UINT16 StartCodeLen = 0;
    UINT32 NextNALU = BufferSize;
    UINT32 Rval = FindStartCode(BufferOffset, BufferBase, BufferSize, &NextNALU, &StartCodeLen);
    if (Rval == FORMAT_OK) {
        UINT32 CurrFrameSize = 0;
        UINT32 SliceSize = Size;
        UINT32 Padding = 0;
        do {
            UINT8 NALUUnitType = 0;
            UINT32 CurrentNALU = NextNALU;
            UINT32 NALUUnitPos = BufferSize;
            Rval = SvcFormat_PlusAddr(StartCodeLen, CurrentNALU, BufferSize, &NALUUnitPos);
            if (Rval == FORMAT_OK) {
                NALUUnitType = SVC_H265_NALU_TYPE(BufferBase[NALUUnitPos]);
                /* To avoid time-consuming start nCode search procedure, we put the remaining bitstream once we have reached slice NALU. */
                if (NALUUnitType < SVC_H265_NALU_TYPE_VPS) {
                    if (CurrFrameSize >= (0xFFFFFFFFU - SliceSize)) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect FrameSize %u, SliceSize = %u", CurrFrameSize, SliceSize, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        CurrFrameSize += (SliceSize - StartCodeLen + 4U);
                        if ((CurrFrameSize & (Align - 1U)) != 0U) {
                            Padding = Align - (CurrFrameSize & (Align - 1U));
                            CurrFrameSize += Padding;
                        }
                        *FrameSize = CurrFrameSize;
                    }
                    break;
                }
                Rval = FindStartCode(NALUUnitPos, BufferBase, BufferSize, &NextNALU, &StartCodeLen);
            }
            if (Rval == FORMAT_OK) {
                UINT32 NALUSize = (NextNALU >= CurrentNALU) ? (NextNALU - CurrentNALU) : ((BufferSize - CurrentNALU) + NextNALU);
                if (NALUUnitType >= SVC_H265_NALU_TYPE_AUD) {
                    if (CurrFrameSize >= (0xFFFFFFFFU - NALUSize)) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect FrameSize %u, NALUSize=%u", CurrFrameSize, NALUSize, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        CurrFrameSize += NALUSize;
                    }
                }
                if (Rval == FORMAT_OK) {
                    if (SliceSize <= NALUSize) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong slice Size!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        SliceSize -= NALUSize;
                    }
                }
            }
        } while (Rval == FORMAT_OK);
        if (Rval == FORMAT_OK) {
            if (CurrFrameSize > (Size + Padding + 1U)) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong frame Size!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        }
    }
    return Rval;
}

/**
 * Write to stream
 *
 * @param [in] Stream Stream handler
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [out] WriteSize The return written size
 * @param [in] Align Alignment
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_WriteStream(SVC_STREAM_HDLR_s *Stream, UINT32 Size, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *WriteSize, UINT32 Align)
{
    UINT16 StartCodeLen = 0;
    UINT32 NextNALU = BufferSize;
    UINT32 Rval = FindStartCode(BufferOffset, BufferBase, BufferSize, &NextNALU, &StartCodeLen);
    if (Rval == FORMAT_OK) {
        UINT32 WrittenBytes = 0;
        UINT32 SliceSize = Size;
        UINT32 Padding = 0;
        do {
            UINT8 NALUUnitType = 0;
            UINT32 CurrentNALU = NextNALU;
            UINT32 NALUUnitPos = BufferSize;
            Rval = SvcFormat_PlusAddr(StartCodeLen, CurrentNALU, BufferSize, &NALUUnitPos);
            if (Rval == FORMAT_OK) {
                NALUUnitType = SVC_H265_NALU_TYPE(BufferBase[NALUUnitPos]);
                /* To avoid time-consuming start nCode search procedure, we put the remaining bitstream once we have reached slice NALU. */
                if (NALUUnitType < SVC_H265_NALU_TYPE_VPS) {
                    if (WrittenBytes >= (0xFFFFFFFFU - SliceSize)) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect WrittenBytes %u, SliceSize = %u", WrittenBytes, SliceSize, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        WrittenBytes += (SliceSize - StartCodeLen + 4U);
                        if ((WrittenBytes & (Align - 1U)) != 0U) {
                            Padding = Align - (WrittenBytes & (Align - 1U));
                            WrittenBytes += Padding;
                        }
                        Rval = SvcFormat_PutBe32(Stream, SliceSize - StartCodeLen + Padding);
                    }
                    /* Start Code should be excluded. */
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_WriteStream(Stream, SliceSize - StartCodeLen, NALUUnitPos, BufferBase, BufferSize);
                    }
                    /* put padding zero */
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutPadding(Stream, Padding);
                    }
                    break;
                }
                Rval = FindStartCode(NALUUnitPos, BufferBase, BufferSize, &NextNALU, &StartCodeLen);
            }
            if (Rval == FORMAT_OK) {
                UINT32 NALUSize = (NextNALU >= CurrentNALU) ? (NextNALU - CurrentNALU) : ((BufferSize - CurrentNALU) + NextNALU);
                if (NALUUnitType >= SVC_H265_NALU_TYPE_AUD) {
                    Rval = SvcFormat_PutBe32(Stream, NALUSize - StartCodeLen);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_WriteStream(Stream, NALUSize - StartCodeLen, NALUUnitPos, BufferBase, BufferSize);
                        if (Rval == FORMAT_OK) {
                            if (WrittenBytes >= (0xFFFFFFFFU - NALUSize)) {
                                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect WrittenBytes %u, NALUSize=%u", WrittenBytes, NALUSize, 0U, 0U, 0U);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            } else {
                                WrittenBytes += NALUSize;
                            }
                        }
                    }
                }
                if (Rval == FORMAT_OK) {
                    if (SliceSize <= NALUSize) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong slice Size!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        SliceSize -= NALUSize;
                    }
                }
            }
        } while (Rval == FORMAT_OK);
        if (Rval == FORMAT_OK) {
            if (WrittenBytes <= (Size + Padding + 1U)) {
                *WriteSize = WrittenBytes;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong WrittenBytes!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        }
    }
    return Rval;
}

static inline UINT16 HIGH_WORD(UINT32 x) {return (UINT16)((x >> 16U) & 0x0000FFFFU);}
static inline UINT16 LOW_WORD(UINT32 x) {return (UINT16)(x & 0x0000FFFFU);}

/**
 * Put gop header to buffer
 *
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [in] Video Video track information
 * @param [in] SkipFirstI Skip first I frame
 * @param [in] SkipLastI Skip last I frame
 * @param [in] PTS Pts
 * @param [out] NewBufferOffset The resulted buffer offset
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_PutGOPHeader(UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, const SVC_VIDEO_TRACK_INFO_s *Video, UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 PTS, UINT32 TimePerFrame, UINT32 *NewBufferOffset)
{
    UINT32 Rval;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Video->Info;
    UINT16 High, Low;
    UINT64 PtsTmp = PTS;
    GOP_HEADER_HEVC_s GopHeader = {0};
    UINT8 GopHeaderBuffer[SVC_H265_GOP_NALU_SIZE] = {0};
    UINT8 Reserved = 1U;
    PtsTmp = NORMALIZE_CLOCK(PtsTmp, Track->TimeScale);
    GopHeader.StartCode_B16_B23_8bit = 0U;
    GopHeader.StartCode_B8_B15_8bit  = 0U;
    GopHeader.StartCode_B0_B7_8bit   = 0x01U;
    GopHeader.NalHdr_B8_B15 = 0x34U;
    GopHeader.NalHdr_B0_B7 = 0x00U;
    GopHeader.VerMain = 0x01U;
    GopHeader.VerSub = 0x01U;
    GopHeader.GopSkipFirstI = (UINT8)SkipFirstI;
    GopHeader.GopSkipLastI = (UINT8)SkipLastI;
    High = HIGH_WORD(TimePerFrame);
    Low = LOW_WORD(TimePerFrame);
    GopHeader.NumUnitInTick_B26_B31_6Bit = (UINT8)(High >> (16U - 6U));
    GopHeader.NumUnitInTick_B18_B25_8Bit = (UINT8)(High >> (10U - 8U));
    GopHeader.NumUnitInTick_B16_B17_2Bit = (UINT8)High;
    GopHeader.Rsvd0As1 = Reserved;
    GopHeader.NumUnitInTick_B11_B15_5Bit = (UINT8)(Low >> (16U - 5U));
    GopHeader.NumUnitInTick_B3_B10_8Bit = (UINT8)(Low >> (11U - 8U));
    High = HIGH_WORD(Track->TimeScale);
    GopHeader.TimeScale_B28_B31_4Bit = (UINT8)(High >> (16U - 4U));
    GopHeader.Rsvd1As1 = Reserved;
    GopHeader.NumUnitInTick_B0_B2_3Bit = (UINT8)Low;
    GopHeader.TimeScale_B20_B27_8Bit = (UINT8)(High >> (12U - 8U));
    Low = LOW_WORD(Track->TimeScale);
    GopHeader.TimeScale_B13_B15_3Bit = (UINT8)(Low >> (16U - 3U));
    GopHeader.Rsvd2As1 = Reserved;
    GopHeader.TimeScale_B16_B19_4Bit = (UINT8)High;
    GopHeader.TimeScale_B5_B12_8Bit = (UINT8)(Low >> (13U - 8U));
    High = HIGH_WORD((UINT32)PtsTmp);
    GopHeader.GopFirstIPts_B30_B31_2Bit = (UINT8)(High >> (16U - 2U));
    GopHeader.Rsvd3As1 = Reserved;
    GopHeader.TimeScale_B0_B4_5Bit = (UINT8)Low;
    GopHeader.GopFirstIPts_B22_B29_8Bit = (UINT8)(High >> (14U - 8U));
    Low = LOW_WORD((UINT32)PtsTmp);
    GopHeader.GopFirstIPts_B15_1Bit = (UINT8)(Low >> (16U - 1U));
    GopHeader.Rsvd4As1 = Reserved;
    GopHeader.GopFirstIPts_B16_B21_6Bit = (UINT8)High;
    GopHeader.GopFirstIPts_B7_B14_8Bit = (UINT8)(Low >> (15U - 8U));
    GopHeader.Rsvd5As1 = Reserved;
    GopHeader.GopFirstIPts_B0_B6_7Bit = (UINT8)Low;
    GopHeader.GopN = (UINT8)Video->N;
    GopHeader.Rsvd6As1 = 0U;
    GopHeader.GopM = (UINT8)Video->M;
    Rval = W2F(AmbaWrap_memcpy(GopHeaderBuffer, &GopHeader, SVC_H265_GOP_NALU_SIZE));
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_ReadStreamMem(GopHeaderBuffer, SVC_H265_GOP_NALU_SIZE, BufferOffset, BufferBase, BufferSize, NewBufferOffset);
    }
    return Rval;
}

