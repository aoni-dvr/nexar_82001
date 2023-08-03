/**
 * @file H264.c
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
#include "H264.h"
#include "FormatAPI.h"
#include "ByteOp.h"
#include "BitOp.h"

/**
 *  GOP header of AVC sub version
 */
typedef struct {
    UINT32 StartCode_B24_B31_8bit:8;    /**< StartCode_B24_B31_8bit */
    UINT32 StartCode_B16_B23_8bit:8;    /**< StartCode_B16_B23_8bit */
    UINT32 StartCode_B8_B15_8bit:8;     /**< StartCode_B8_B15_8bit */
    UINT32 StartCode_B0_B7_8bit:8;      /**< StartCode_B0_B7_8bit */
    UINT32 NalHdr:8;                    /**< NalHdr */
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
    //SubVer2
    UINT32 GopFirstIPts_B56_B63_8Bit:8; /**< GopFirstIPts_B56_B63_8Bit */
    UINT32 GopFirstIPts_B48_B55_8Bit:8; /**< GopFirstIPts_B48_B55_8Bit */
    UINT32 GopFirstIPts_B41_B47_7Bit:7; /**< GopFirstIPts_B41_B47_7Bit */
    UINT32 Rsvd6As1:1;                  /**< Rsvd6As1 */
    UINT32 GopFirstIPts_B33_B40_8Bit:8; /**< GopFirstIPts_B33_B40_8Bit */
    UINT32 GopN_B2_B7_6Bit:6;           /**< GopN_B2_B7_6Bit */
    UINT32 Rsvd7As1:1;                  /**< Rsvd7As1 */
    UINT32 GopFirstIPts_B32_1Bit:1;     /**< GopFirstIPts_B32_1Bit */
    UINT32 Rsvd8As1:2;                  /**< Rsvd8As1 */
    UINT32 GopM_B0_B3_4Bit:4;           /**< GopM_B0_B3_4Bit */
    UINT32 GopN_B0_B1_2Bit:2;           /**< GopN_B0_B1_2Bit */
    UINT32 RsvdAs0:16;                  /**< RsvdAs0 */
} GOP_HEADER_AVC_SUB_VER_s;

/**
 * get scaling list
 */
#ifdef H264_SIMPLE_PARSE
static UINT32 GetScalingList(SVC_BIT_OP_s *Ctx, INT32 Size, UINT8 *Matrix)
{
    static const UINT8 ZigZagScan16[16]  = {0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15};
    static const UINT8 ZigZagScan8[64] = {0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
               12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
               35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
               58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};
    UINT32 Rval = FORMAT_OK;
    UINT32 Last, Next;
    INT32 Delta, i;
    UINT8 Scan;
    Last = 8;
    Next = 8;

    for (i = 0; i < Size; i++) {
        Scan = (Size == 16) ? ZigZagScan16[i] : ZigZagScan8[i];
        if (Next != 0U) {
            Rval = SvcFormat_GetSe(Ctx, &Delta);
            if (Rval != FORMAT_OK) {
                break;
            }
            Next = (Last + (UINT32)Delta + 256U) % 256U;
            *Matrix = ((Scan == 0U) && (Next == 0U)) ? 1U : 0U;
        }
        Last = (Next == 0U) ? Last : Next;
    }
    return Rval;
}
#else
static UINT32 GetScalingList(SVC_BIT_OP_s *Ctx, INT32 *Buffer, INT32 Size, UINT8 *Matrix)
{
    static UINT8 ZigZagScan16[16]  = {0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15};
    static UINT8 ZigZagScan8[64] = {0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
               12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
               35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
               58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};
    UINT32 Rval = FORMAT_OK;
    UINT32 Last, Next;
    INT32 Delta, i;
    UINT8 Scan;
    Last = 8;
    Next = 8;
    for (i = 0; i < Size; i++) {
        Scan = (Size == 16) ? ZigZagScan16[i] : ZigZagScan8[i];
        if (Next != 0U) {
            Rval = SvcFormat_GetSe(Ctx, &Delta);
            if (Rval != FORMAT_OK) {
                break;
            }
            Next = (Last + Delta + 256U) % 256U;
            *Matrix =((Scan == 0U) && (Next == 0U)) ? 1U : 0U;
        }
        Buffer[Scan] = (Next == 0) ? Last: Next;
        Last = Buffer[Scan];
    }
    return Rval;
}
#endif
/**
 * Parse HRD_parameters_set
 *
 */
static UINT32 ParseHRDImpl(SVC_BIT_OP_s *Ctx, SVC_H264_HRD_s *HRD)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TmpU32;
    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
    if (Rval == FORMAT_OK) {
        HRD->CpbCntMinus1 = (UINT8)TmpU32;
        Rval = SvcFormat_GetBit(Ctx, 4, &HRD->BitRateScale);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBit(Ctx, 4, &HRD->CpbSizeScale);
            if (Rval == FORMAT_OK) {
                if (HRD->CpbCntMinus1 < 32U) { /* range 0-31 */
                    UINT32 i;
                    for (i = 0; i <= HRD->CpbCntMinus1; i++) {
                        Rval = SvcFormat_GetUe(Ctx, &HRD->BitRateValueMinus1[i]);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetUe(Ctx, &HRD->CpbSizeValueMinus1[i]);
                        }
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_Get1Bit(Ctx, &HRD->CbrFlag[i]);
                        }
                        if (Rval != FORMAT_OK) {
                            break;
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetBit(Ctx, 5, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            HRD->InitialCpbRemovalDelayLengthMinus1 = (UINT16)TmpU32;
                            Rval = SvcFormat_GetBit(Ctx, 5, &TmpU32);
                            if (Rval == FORMAT_OK) {
                                HRD->CpbRemovalDelayLengthMinus1 = (UINT16)TmpU32;
                                Rval = SvcFormat_GetBit(Ctx, 5, &TmpU32);
                                if (Rval == FORMAT_OK) {
                                    HRD->DpbOutputDelayLengthMinus1 = (UINT16)TmpU32;
                                    Rval = SvcFormat_GetBit(Ctx, 5, &TmpU32);
                                    if (Rval == FORMAT_OK) {
                                        HRD->TimeOffsetLength = (UINT16)TmpU32;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect cpb_cnt_minus1 %u", HRD->CpbCntMinus1, 0U, 0U, 0U, 0U);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseVideoSignalType(SVC_BIT_OP_s *Ctx, SVC_H264_VUI_s *VUI)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &VUI->VideoSignalTypePresentFlag);
    if ((Rval == FORMAT_OK) && (VUI->VideoSignalTypePresentFlag != 0U)) {
        UINT32 TmpU32;
        Rval = SvcFormat_GetBit(Ctx, 3, &TmpU32);
        if (Rval == FORMAT_OK) {
            VUI->VideoFormat = (UINT8)TmpU32;
            Rval = SvcFormat_Get1Bit(Ctx, &VUI->VideoFullRangeFlag);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_Get1Bit(Ctx, &VUI->ColourDescriptionPresentFlag);
                if (Rval == FORMAT_OK) {
                    if (VUI->ColourDescriptionPresentFlag != 0U) {
                        Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            VUI->ColourPrimaries = (UINT8)TmpU32;
                            Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                            if (Rval == FORMAT_OK) {
                                VUI->TransferCharacteristics = (UINT8)TmpU32;
                                Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                                if (Rval == FORMAT_OK) {
                                    VUI->MatrixCoefficients = (UINT8)TmpU32;
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

static UINT32 ParseTimingInfo(SVC_BIT_OP_s *Ctx, SVC_H264_VUI_s *VUI)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &VUI->TimingInfoPresentFlag);
    if ((Rval == FORMAT_OK) && (VUI->TimingInfoPresentFlag != 0U)) {
        UINT32 A, B;
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
                        Rval = SvcFormat_Get1Bit(Ctx, &VUI->FixedFrameRateFlag);
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseBitstreamRestriction(SVC_BIT_OP_s *Ctx, SVC_H264_VUI_s *VUI)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &VUI->BitstreamRestrictionFlag);
    if ((Rval == FORMAT_OK) && (VUI->BitstreamRestrictionFlag != 0U)) {
        Rval = SvcFormat_Get1Bit(Ctx, &VUI->MotionVectorsOverPicBoundariesFlag);
        if (Rval == FORMAT_OK) {
            UINT32 TmpU32;
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                VUI->MaxBytesPerPicDenom = (UINT8)TmpU32;
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    VUI->MaxBitsPerMbDenom = (UINT8)TmpU32;
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        VUI->Log2MaxMvLengthHorizontal = (UINT16)TmpU32;
                        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            VUI->Log2MaxMvLengthVertical = (UINT16)TmpU32;
                            Rval = SvcFormat_GetUe(Ctx, &VUI->NumReorderFrames);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetUe(Ctx, &VUI->MaxDecFrameBuffering);
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseHRD(SVC_BIT_OP_s *Ctx, SVC_H264_VUI_s *VUI)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &VUI->NalHrdParametersPresentFlag);
    if (Rval == FORMAT_OK) {
        if (VUI->NalHrdParametersPresentFlag != 0U) {
            Rval = ParseHRDImpl(Ctx, &VUI->NalHrdParam);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_Get1Bit(Ctx, &VUI->VclHrdParametersPresentFlag);
            if (Rval == FORMAT_OK) {
                if (VUI->VclHrdParametersPresentFlag != 0U) {
                    Rval = ParseHRDImpl(Ctx, &VUI->VclHrdParam);
                }
                if (Rval == FORMAT_OK) {
                    if ((VUI->NalHrdParametersPresentFlag != 0U) || (VUI->VclHrdParametersPresentFlag != 0U)) {
                        Rval = SvcFormat_Get1Bit(Ctx, &VUI->LowDelayHrdFlag);
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
static UINT32 ParseVUI(SVC_BIT_OP_s *Ctx, SVC_H264_VUI_s *VUI)
{
    UINT32 Rval;
    Rval = SvcFormat_Get1Bit(Ctx, &VUI->AspectRatioInfoPresentFlag);
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
            Rval = SvcFormat_Get1Bit(Ctx, &VUI->OverscanInfoPresentFlag);
            if (Rval == FORMAT_OK) {
                if (VUI->OverscanInfoPresentFlag != 0U) {
                    Rval = SvcFormat_Get1Bit(Ctx, &VUI->OverscanAppropriateFlag);
                }
                if (Rval == FORMAT_OK) {
                    Rval = ParseVideoSignalType(Ctx, VUI);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_Get1Bit(Ctx, &VUI->ChromaLocationInfoPresentFlag);
                        if (Rval == FORMAT_OK) {
                            if (VUI->ChromaLocationInfoPresentFlag != 0U) {
                                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                                if (Rval == FORMAT_OK) {
                                    VUI->ChromaSampleLocTypeTopField = (UINT8)TmpU32;
                                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                                    if (Rval == FORMAT_OK) {
                                        VUI->ChromaSampleLocTypeBottomField = (UINT8)TmpU32;
                                    }
                                }
                            }
                            if (Rval == FORMAT_OK) {
                                Rval = ParseTimingInfo(Ctx, VUI);
                                if (Rval == FORMAT_OK) {
                                #if 0
                                    LOG_VERB("h264 :VUI->AspectRatioInfoPresentFlag = %u", VUI->AspectRatioInfoPresentFlag);
                                    LOG_VERB("h264 :VUI->AspectRatioIdc = %u", VUI->AspectRatioIdc);
                                    LOG_VERB("h264 :VUI->SarWidth = %u", VUI->SarWidth);
                                    LOG_VERB("h264 :VUI->SarHeight = %u", VUI->SarHeight);
                                    LOG_VERB("h264 :VUI->NumUnitsInTick = %u", VUI->NumUnitsInTick);
                                    LOG_VERB("h264 :VUI->TimeScale = %u", VUI->TimeScale);
                                #endif
                                    Rval = ParseHRD(Ctx, VUI);
                                    if (Rval == FORMAT_OK) {
                                        Rval = SvcFormat_Get1Bit(Ctx, &VUI->PicStructPresentFlag);
                                        if (Rval == FORMAT_OK) {
                                            Rval = ParseBitstreamRestriction(Ctx, VUI);
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

static UINT32 ParseSliceGroups_Type6(SVC_BIT_OP_s *Ctx, SVC_H264_PPS_s *PPS)
{
    UINT32 Rval;
    UINT8 NumBits;
    if ((PPS->NumSliceGroupsMinus1 + 1U) > 4U) {
        NumBits = 3;
    } else if ((PPS->NumSliceGroupsMinus1 + 1U) > 2U) {
        NumBits = 2;
    } else {
        NumBits = 1;
    }
    Rval = SvcFormat_GetUe(Ctx, &PPS->NumSliceGroupMapUnitsMinus1);
    if (Rval == FORMAT_OK) {
        if (PPS->NumSliceGroupMapUnitsMinus1 < 8U) {    /* AMBA PPS set do not store such data now, default size = 8; */
            UINT32 TmpU32, i;
            for (i = 0; i <= PPS->NumSliceGroupMapUnitsMinus1; i++) {
                Rval = SvcFormat_GetBit(Ctx, NumBits, &TmpU32);
                if (Rval != FORMAT_OK) {
                    break;
                }
                PPS->SliceGroupId[i] = (UINT8)TmpU32;
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "num_slice_group_map_units_minus1 %u too large", PPS->NumSliceGroupMapUnitsMinus1, 0U, 0U, 0U, 0U);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 ParseSliceGroups(SVC_BIT_OP_s *Ctx, SVC_H264_PPS_s *PPS)
{
    UINT32 Rval = SvcFormat_GetUe(Ctx, &PPS->NumSliceGroupsMinus1);
    if (Rval == FORMAT_OK) {
        if (PPS->NumSliceGroupsMinus1 < 8U) { /* range 0-7 */
            if (PPS->NumSliceGroupsMinus1 > 0U) {
                Rval = SvcFormat_GetUe(Ctx, &PPS->SliceGroupMapType);
                if (Rval == FORMAT_OK) {
                    UINT32 i;
                    if (PPS->SliceGroupMapType == 0U) {
                        for (i = 0; i <= PPS->NumSliceGroupsMinus1; i++) {
                            Rval = SvcFormat_GetUe(Ctx, &PPS->RunLengthMinus1[i]);
                            if (Rval != FORMAT_OK) {
                                break;
                            }
                        }
                    } else if (PPS->SliceGroupMapType == 2U) {
                        for (i = 0; i < PPS->NumSliceGroupsMinus1; i++) {
                            Rval = SvcFormat_GetUe(Ctx, &PPS->TopLeft[i]);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetUe(Ctx, &PPS->BottomRight[i]);
                            }
                            if (Rval != FORMAT_OK) {
                                break;
                            }
                        }
                    } else if ((PPS->SliceGroupMapType == 3U) || (PPS->SliceGroupMapType == 4U) || (PPS->SliceGroupMapType == 5U)) {
                        Rval = SvcFormat_Get1Bit(Ctx, &PPS->SliceGroupChangeDirectionFlag);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetUe(Ctx, &PPS->SliceGroupChangeRateMinus1);
                        }
                    } else if (PPS->SliceGroupMapType == 6U) {
                        Rval = ParseSliceGroups_Type6(Ctx, PPS);
                    } else {
                        /**/
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "num_slice_groups_minus1 %u too large", PPS->NumSliceGroupsMinus1, 0U, 0U, 0U, 0U);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 ParseMoreRbspData(SVC_BIT_OP_s *Ctx, SVC_H264_PPS_s *PPS, UINT8 ChromaFormatIDC)
{
    //Fidelity Range Extensions Stuff
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &PPS->Transform8x8ModeFlag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_Get1Bit(Ctx, &PPS->PicScalingMatrixPresentFlag);
        if (Rval == FORMAT_OK) {
            if (PPS->PicScalingMatrixPresentFlag != 0U) {
                UINT32 ScalingList = (UINT32)6 + (((ChromaFormatIDC != YUV444) ? (UINT32)2 : (UINT32)6) * (UINT32)PPS->Transform8x8ModeFlag);
                UINT32 i;
                for (i = 0U; i < ScalingList; i++) {
                    Rval = SvcFormat_Get1Bit(Ctx, &PPS->PicScalingListPresentFlag[i]);
                    if ((PPS->PicScalingListPresentFlag[i] != 0U) && (Rval == FORMAT_OK)) {
#ifdef H264_SIMPLE_PARSE
                        if (i < 6U) {
                            Rval = GetScalingList(Ctx, 16, &PPS->UseDefaultScalingMatrix4x4Flag[i]);
                        } else {
                            Rval = GetScalingList(Ctx, 64, &PPS->UseDefaultScalingMatrix8x8Flag[i-6U]);
                        }
#else
                        if (i < 6U) {
                            Rval = GetScalingList(Ctx, PPS->ScalingList4x4[i], 16, &PPS->UseDefaultScalingMatrix4x4Flag[i]);
                        } else {
                            Rval = GetScalingList(Ctx, PPS->ScalingList8x8[i-6], 64, &PPS->UseDefaultScalingMatrix8x8Flag[i-6]);
                        }
#endif
                        if (Rval != FORMAT_OK) {
                            break;
                        }
                    }
                }
            }
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetSe(Ctx, &PPS->SecondCchromaQpIndexOffset);
            }
        }
    }
    return Rval;
}

/**
 * Parse pic parameters set
 *
 */
static UINT32 ParsePPS(SVC_BIT_OP_s *Ctx, UINT8 *Buffer, UINT32 BufferSize, SVC_H264_PPS_s *PPS, UINT8 ChromaFormatIDC)
{
    UINT32 Rval;
    UINT32 TmpU32;
    SvcFormat_InitBitBuf(Ctx, Buffer, BufferSize);
    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
    if (Rval == FORMAT_OK) {
        PPS->PicParameterSetId = (UINT8)TmpU32;
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            PPS->SeqParameterSetId = (UINT8)TmpU32;
            Rval = SvcFormat_Get1Bit(Ctx, &PPS->EntropyCodingModeFlag);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_Get1Bit(Ctx, &PPS->PicOrderPresentFlag);
                if (Rval == FORMAT_OK) {
                    Rval = ParseSliceGroups(Ctx, PPS);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            PPS->NumRefIdxL0ActiveMinus1 = (UINT16)TmpU32;
                            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                            if (Rval == FORMAT_OK) {
                                PPS->NumRefIdxL1ActiveMinus1 = (UINT16)TmpU32;
                                Rval = SvcFormat_Get1Bit(Ctx, &PPS->WeightedPredFlag);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_GetBit(Ctx, 2, &TmpU32);
                                    if (Rval == FORMAT_OK) {
                                        PPS->WeightedBipredIdc = (UINT8)TmpU32;
                                        Rval = SvcFormat_GetSe(Ctx, &PPS->PicInitQpMinus26);
                                        if (Rval == FORMAT_OK) {
                                            Rval = SvcFormat_GetSe(Ctx, &PPS->PicInitQsMinus26);
                                            if (Rval == FORMAT_OK) {
                                                Rval = SvcFormat_GetSe(Ctx, &PPS->ChromaQpIndexOffset);
                                                if (Rval == FORMAT_OK) {
                                                    Rval = SvcFormat_Get1Bit(Ctx, &PPS->DeblockingFilterControlPresentFlag);
                                                    if (Rval == FORMAT_OK) {
                                                        Rval = SvcFormat_Get1Bit(Ctx, &PPS->ConstrainedIntraPredFlag);
                                                        if (Rval == FORMAT_OK) {
                                                            Rval = SvcFormat_Get1Bit(Ctx, &PPS->RedundantPicCntPresentFlag);
                                                            if (Rval == FORMAT_OK) {
                                                                if (SvcFormat_MoreRbspSata(Ctx, BufferSize) != 0) { /* more_data_in_rbsp() */
                                                                    Rval = ParseMoreRbspData(Ctx, PPS, ChromaFormatIDC);
                                                                } else {
                                                                    PPS->SecondCchromaQpIndexOffset = PPS->ChromaQpIndexOffset;
                                                                }
                                                                if (Rval == FORMAT_OK) {
                                                                    PPS->Valid = 1;
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
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SkipConstrainedSet(SVC_BIT_OP_s *Ctx, SVC_H264_SPS_s *SPS)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &SPS->ConstrainedSet0Flag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_Get1Bit(Ctx, &SPS->ConstrainedSet1Flag);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_Get1Bit(Ctx, &SPS->ConstrainedSet2Flag);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_Get1Bit(Ctx, &SPS->ConstrainedSet3Flag);
                if (Rval == FORMAT_OK) {
                    UINT32 Reserved;
                    Rval = SvcFormat_GetBit(Ctx, 4, &Reserved);
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseSeqScalingMatrix(SVC_BIT_OP_s *Ctx, SVC_H264_SPS_s *SPS)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &SPS->SeqScalingMatrixPresentFlag);
    if (Rval == FORMAT_OK) {
        if (SPS->SeqScalingMatrixPresentFlag != 0U) {
            UINT32 ScalingList = (SPS->ChromaFormatIdc != YUV444) ? (UINT32)8 : (UINT32)12;
            UINT32 i;
            for (i = 0U; i < ScalingList; i++) {
                Rval = SvcFormat_Get1Bit(Ctx, &SPS->SeqScalingListPresentFlag[i]);
                if ((SPS->SeqScalingListPresentFlag[i] != 0U) && (Rval == FORMAT_OK)) {
#ifdef H264_SIMPLE_PARSE
                    if (i < 6U) {
                        Rval = GetScalingList(Ctx, 16, &SPS->UseDefaultScalingMatrix4x4Flag[i]);
                    } else {
                        Rval = GetScalingList(Ctx, 64, &SPS->UseDefaultScalingMatrix8x8Flag[i - 6U]);
                    }
#else
                    if (i < 6U) {
                        Rval = GetScalingList(Ctx, SPS->ScalingList4x4[i], 16, &SPS->UseDefaultScalingMatrix4x4Flag[i]);
                    } else {
                        Rval = GetScalingList(Ctx, SPS->ScalingList8x8[i - 6], 64, &SPS->UseDefaultScalingMatrix8x8Flag[i - 6]);
                    }
#endif
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
            }
        }
    }
    return Rval;
}

#ifdef H264_SIMPLE_PARSE
#define MAX_REF_FRAMES  32U     /**< Maimum number of reference frames */
#else
#define MAX_REF_FRAMES  256U    /**< Maimum number of reference frames */
#endif
static UINT32 ParsePicOrderCnt(SVC_BIT_OP_s *Ctx, SVC_H264_SPS_s *SPS)
{
    UINT32 Rval;
    UINT32 TmpU32;
    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
    if (Rval == FORMAT_OK) {
        SPS->PicOrderCntType = (UINT8)TmpU32;
        if (SPS->PicOrderCntType == 0U) {
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                SPS->Log2MaxPicOrderCntLsbMinus4 = (UINT8)TmpU32;
            }
        } else if (SPS->PicOrderCntType == 1U) {
            Rval = SvcFormat_Get1Bit(Ctx, &SPS->DeltaPicOrderAlwaysZeroFlag);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetSe(Ctx, &SPS->OffsetForNonRefPic);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetSe(Ctx, &SPS->OffsetForTopToBottomField);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            SPS->NumRefFramesInPicOrderCntCycle = (UINT8)TmpU32;
                            if (SPS->NumRefFramesInPicOrderCntCycle < MAX_REF_FRAMES) {
                                INT32 TmpS32;
                                UINT32 i;
                                for (i = 0; i < SPS->NumRefFramesInPicOrderCntCycle; i++) {
                                    Rval = SvcFormat_GetSe(Ctx, &TmpS32);
                                    if (Rval != FORMAT_OK) {
                                        break;
                                    }
                                    SPS->OffsetForRefFrame[i] = (UINT8)TmpS32;
                                }
                            } else {
                                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect num_ref_frames_in_pic_order_cnt_cycle %u", SPS->NumRefFramesInPicOrderCntCycle, 0U, 0U, 0U, 0U);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            }
                        }
                    }
                }
            }
        } else {
            /**/
        }
    }
    return Rval;
}

static UINT32 ParseFrameCropping(SVC_BIT_OP_s *Ctx, SVC_H264_SPS_s *SPS)
{
    UINT32 Rval = SvcFormat_Get1Bit(Ctx, &SPS->FrameCroppingFlag);
    if ((Rval == FORMAT_OK) && (SPS->FrameCroppingFlag != 0U)) {
        UINT32 TmpU32;
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            SPS->FrameCroppingRectLeftOffset = (UINT16)TmpU32;
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                SPS->FrameCroppingRectRightOffset = (UINT16)TmpU32;
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    SPS->FrameCroppingRectTopOffset = (UINT16)TmpU32;
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        SPS->FrameCroppingRectBottomOffset = (UINT16)TmpU32;
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseSPS_SpecificProfileIdc(SVC_BIT_OP_s *Ctx, SVC_H264_SPS_s *SPS)
{
    UINT32 Rval = FORMAT_OK;
    if ((SPS->ProfileIdc == 100U) || (SPS->ProfileIdc == 110U) ||
        (SPS->ProfileIdc == 122U) || (SPS->ProfileIdc == 244U) ||
        (SPS->ProfileIdc == 44U)) {
        UINT32 TmpU32;
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            SPS->ChromaFormatIdc = (UINT8)TmpU32;
            if (SPS->ChromaFormatIdc == YUV444) {
                Rval = SvcFormat_Get1Bit(Ctx, &SPS->SeparateColourPlaneFlag);
            }
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                if (Rval == FORMAT_OK) {
                    SPS->BitDepthLumaMinus8 = (UINT8)TmpU32;
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        SPS->BitDepthChromaMinus8 = (UINT8)TmpU32;
                        Rval = SvcFormat_Get1Bit(Ctx, &SPS->LosslessQpprimeFlag);
                        if (Rval == FORMAT_OK) {
                            Rval = ParseSeqScalingMatrix(Ctx, SPS);
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 ParseSPS_CheckProfileIdc(UINT8 ProfileIdc)
{
    UINT32 Rval;
    if ((ProfileIdc == 66U) || (ProfileIdc == 77U) || (ProfileIdc == 88U) ||
        (ProfileIdc == 100U) || (ProfileIdc == 110U) || (ProfileIdc == 122U) ||
        (ProfileIdc == 244U) || (ProfileIdc == 44U)) {
        Rval = FORMAT_OK;
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "ParseSPS_CheckProfileIdc Unsupport profile %u!", ProfileIdc, 0U, 0U, 0U, 0U); /* Unsupported Profile */
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

//#define DEBUG_MSG
/**
 * Parse seq parameters set
 *
 */
static UINT32 ParseSPS(SVC_BIT_OP_s *Ctx, UINT8 *Buffer, UINT32 BufferSize, SVC_H264_SPS_s *SPS)
{
    UINT32 Rval;
    UINT32 TmpU32;
    SvcFormat_InitBitBuf(Ctx, Buffer, BufferSize);
    Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
    if (Rval == FORMAT_OK) {
        SPS->ProfileIdc = (UINT8)TmpU32;
        Rval = ParseSPS_CheckProfileIdc(SPS->ProfileIdc);
        if (Rval == FORMAT_OK) {
            Rval = SkipConstrainedSet(Ctx, SPS);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBit(Ctx, 8, &TmpU32);
                if (Rval == FORMAT_OK) {
                    SPS->LevelIdc = (UINT8)TmpU32;
                    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        SPS->SeqParameterSetId = (UINT8)TmpU32;
                        SPS->ChromaFormatIdc = YUV420;
                        SPS->BitDepthLumaMinus8   = 0U;
                        SPS->BitDepthChromaMinus8 = 0U;
                        SPS->LosslessQpprimeFlag   = 0U;
                        SPS->SeparateColourPlaneFlag = 0U;
                        Rval = ParseSPS_SpecificProfileIdc(Ctx, SPS);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                            if (Rval == FORMAT_OK) {
                                SPS->Log2MaxFrameNumMinus4 = (UINT8)TmpU32;
                                Rval = ParsePicOrderCnt(Ctx, SPS);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_GetUe(Ctx, &SPS->NumRefFrames);
                                    if (Rval == FORMAT_OK) {
                                        Rval = SvcFormat_Get1Bit(Ctx, &SPS->GapsInFrameNumValueAllowedFlag);
                                        if (Rval == FORMAT_OK) {
                                            Rval = SvcFormat_GetUe(Ctx, &SPS->PicWidthInMbsMinus1);
                                            if (Rval == FORMAT_OK) {
                                                Rval = SvcFormat_GetUe(Ctx, &SPS->PicHeightInMapUnitsMinus1);
                                                if (Rval == FORMAT_OK) {
                                                    Rval = SvcFormat_Get1Bit(Ctx, &SPS->FrameMbsOnlyFlag);
                                                    if ((Rval == FORMAT_OK) && (SPS->FrameMbsOnlyFlag == 0U)) {
                                                        Rval = SvcFormat_Get1Bit(Ctx, &SPS->MbAdaptiveFrameFieldFlag);
                                                    }
                                                    if (Rval == FORMAT_OK) {
                                                        Rval = SvcFormat_Get1Bit(Ctx, &SPS->Direct8x8InferenceFlag);
                                                        if (Rval == FORMAT_OK) {
                                                            Rval = ParseFrameCropping(Ctx, SPS);
                                                            if (Rval == FORMAT_OK) {
                                                                #if 0
                                                                LOG_VERB("h264 :SPS->ProfileIdc = %u", SPS->ProfileIdc);
                                                                LOG_VERB("h264 :SPS->LevelIdc = %u", SPS->LevelIdc);
                                                                LOG_VERB("h264 :SPS->SeqParameterSetId = %u", SPS->SeqParameterSetId);
                                                                LOG_VERB("h264 :SPS->Log2MaxFrameNumMinus4 = %u", SPS->Log2MaxFrameNumMinus4);
                                                                LOG_VERB("h264 :SPS->NumRefFrames = %u", SPS->NumRefFrames);
                                                                LOG_VERB("h264 :SPS->PicWidthInMbsMinus1 = %u", SPS->PicWidthInMbsMinus1);
                                                                LOG_VERB("h264 :SPS->PicHeightInMapUnitsMinus1 = %u", SPS->PicHeightInMapUnitsMinus1);
                                                                LOG_VERB("h264 :SPS->FrameCroppingRectLeftOffset = %u", SPS->FrameCroppingRectLeftOffset);
                                                                LOG_VERB("h264 :SPS->FrameCroppingRectRightOffset = %u", SPS->FrameCroppingRectRightOffset);
                                                                LOG_VERB("h264 :SPS->FrameCroppingRectTopOffset = %u", SPS->FrameCroppingRectTopOffset);
                                                                LOG_VERB("h264 :SPS->FrameCroppingRectBottomOffset = %u", SPS->FrameCroppingRectBottomOffset);
                                                                #endif
                                                                Rval = SvcFormat_Get1Bit(Ctx, &SPS->VuiParametersPresentFlag);
                                                                if (Rval == FORMAT_OK) {
                                                                    if (SPS->VuiParametersPresentFlag != 0U) {
                                                                        Rval = ParseVUI(Ctx, &SPS->VuiParam);
                                                                    }
                                                                    SPS->Valid = 1;
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
                        }
                    }
                }
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
static UINT32 ParseNALU(const UINT8 *Buffer, UINT32 Size, SVC_H264_NALU_s *NAL, UINT32 *ParseOffset)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 Pos = 0;
    *ParseOffset = 0;
    while ((Pos < Size) && (Buffer[Pos] == 0U)) {
        Pos++;
    }
    Pos++;
    if (Pos == Size) {
        *ParseOffset = Pos;
    } else {
        /* no Start Code at the begin of the NALU */
        if (Buffer[Pos - 1U] == 1U) {
            /* no Start Code at the begin of the NALU */
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
                            while (Buffer[Pos - 2U - tz8b] == 0U) {
                                tz8b++;
                            }
                            NAL->Len = (Pos - 1U) - (UINT32)NAL->PrefixLen - lz8bcnt - tz8b;
                            NAL->ForbiddenBit = (((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] >> 7) & 0x01U);
                            NAL->NalReferenceIdc = (((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] >> 5) & 0x03U);
                            NAL->NaluType = (UINT8)((Buffer[lz8bcnt + (UINT32)NAL->PrefixLen]) & 0x1fU);
                            *ParseOffset = Pos;
                            Done = 1U;
                            break;
                        }
                        Pos++;
                        Info3 = StartCodeSeek(&Buffer[Pos - 4U], 3);
                        if (Info3 == 0U) {
                            Info2 = StartCodeSeek(&Buffer[Pos - 3U], 2);
                        }
                        Found = ((Info2 == 1U) || (Info3 == 1U)) ? 1U : 0U;
                    }
                    if (Done == 0U) {
                        INT32 Rewind = 0;
                        UINT32 tz8b = 0;
                        if (Info3 != 0U) {
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
                        NAL->NalReferenceIdc = (((UINT32)Buffer[lz8bcnt + (UINT32)NAL->PrefixLen] >> 5) & 0x03U);
                        NAL->NaluType = (Buffer[lz8bcnt + (UINT32)NAL->PrefixLen]) & 0x1fU;
#if 0
                        (void) AmbaPrint("%s: type %d Pos %d len %d, NAL->reference_idc %d", __func__, NAL->NaluType, Pos, NAL->Len, NAL->NalReferenceIdc);
#endif
                        *ParseOffset = (Pos + (UINT32)Rewind);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 ParseSlice(SVC_BIT_OP_s *Ctx, UINT8 *Buffer, UINT32 BufferSize, SVC_H264_SliceInfo_s *SliceInfo, const SVC_H264_SPS_s *SPS, const SVC_H264_NALU_s *NALU)
{
    UINT32 Rval;
    SvcFormat_InitBitBuf(Ctx, Buffer, BufferSize);
    Rval = SvcFormat_GetUe(Ctx, &SliceInfo->FirstMbInSlice);
    if (Rval == FORMAT_OK) {
        UINT32 TmpU32;
        Rval = SvcFormat_GetUe(Ctx, &TmpU32);
        if (Rval == FORMAT_OK) {
            if (TmpU32 > 4U) {
                TmpU32 -= 5U;
            }
            SliceInfo->SliceType = (UINT8)TmpU32;
            Rval = SvcFormat_GetUe(Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                SliceInfo->PicParameterSetId = (UINT8)TmpU32;
                if (SPS->SeparateColourPlaneFlag != 0U) {
                    Rval = SvcFormat_GetBit(Ctx, 2, &TmpU32);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetBit(Ctx, SPS->Log2MaxFrameNumMinus4 + 4U, &SliceInfo->FrameNum);
                    if (Rval == FORMAT_OK) {
                        if (SPS->FrameMbsOnlyFlag == 0U) {
                            Rval = SvcFormat_Get1Bit(Ctx, &SliceInfo->FieldPicFlag);
                            if ((Rval == FORMAT_OK) && (SliceInfo->FieldPicFlag != 0U)) {
                                UINT8 TmpU8;
                                Rval = SvcFormat_Get1Bit(Ctx, &TmpU8);
                                if (Rval == FORMAT_OK) {
                                    SliceInfo->BottomFieldFlag = (UINT16)TmpU8;
                                }
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            if (NALU->NaluType == SVC_H264_NALU_TYPE_IDR) {
                                Rval = SvcFormat_GetUe(Ctx, &TmpU32);
                                if (Rval == FORMAT_OK) {
                                    SliceInfo->IdrPicId = (UINT16)TmpU32;
                                }
                            } else {
                                SliceInfo->IdrPicId = 0xFFFFU;
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

/**
 * Preprocessing step before parsing H.264 NAL/SPS/PPS/SLICE
 *
 * @param [in,out] Buffer in: Source buffer, out: Source buffer without 'emulation prevention three byte'.
 * @param [in] Size Original Size before trimming 'emulation prevention three byte'.
 * @param [out] RbspSize Actual Size after trimming 'emulation prevention three byte'.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
static UINT32 EliminateEmulationPrevention3byte(UINT8 *Buffer, UINT32 Size, UINT32 *RbspSize)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 RbsBuffer[64] = {0};
    UINT32 i = 0;
    UINT32 CurSize = 0;
    while (i < Size){
        if ( ((i + 2U) < Size) && ((Buffer[i] == 0U) && (Buffer[i + 1U] == 0U) && (Buffer[i + 2U]== 3U)) ){
            RbsBuffer[CurSize] = Buffer[i];
            CurSize++;
            RbsBuffer[CurSize] = Buffer[i + 1U];
            CurSize++;
            i += 2U;
        }
        else{
            RbsBuffer[CurSize] = Buffer[i];
            CurSize++;
        }
        i++;
    }
    if (CurSize <= sizeof(RbsBuffer)) {
        Rval = W2F(AmbaWrap_memcpy(Buffer, RbsBuffer, CurSize));
        *RbspSize = CurSize;
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect CurSize %u", CurSize, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Parse H.264 NAL/SPS/PPS/SLICE
 *
 * @param [in] Buffer Source buffer
 * @param [in] BufferSize Buffer length
 * @param [out] SPS Space to store SPS
 * @param [out] PPS Space to store PPS
 * @param [out] SliceInfo Space to store slice information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH264_HeaderParse(UINT8 *Buffer, UINT32 BufferSize, SVC_H264_SPS_s *SPS, SVC_H264_PPS_s *PPS, SVC_H264_SliceInfo_s *SliceInfo)
{
    SVC_BIT_OP_s BitInfo;
    UINT32 Rval = W2F(AmbaWrap_memset(&BitInfo, 0, sizeof(SVC_BIT_OP_s)));
    if (Rval == FORMAT_OK) {
        UINT32 ParseSize = 0;
        SVC_H264_NALU_s NALU = {0};
        NALU.FirstNALU = 1U;
        do {
            UINT32 ParseOffset = 0;
            UINT32 BufferOffset = BufferSize - ParseSize;
            if (BufferOffset == 0U) {
                break;
            }
            Rval = ParseNALU(&Buffer[ParseSize], BufferOffset, &NALU, &ParseOffset);
            if (Rval == FORMAT_OK) {
                if (ParseOffset == 0U) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong value!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            }
            if (Rval == FORMAT_OK) {
                UINT8 *Pos = &Buffer[ParseSize + (UINT32)NALU.PrefixLen + 1U];
                if ((NALU.NaluType == SVC_H264_NALU_TYPE_SLICE) || (NALU.NaluType == SVC_H264_NALU_TYPE_IDR)) {
                    Rval = ParseSlice(&BitInfo, Pos, NALU.Len - 1U, SliceInfo, SPS, &NALU);
                } else if (NALU.NaluType == SVC_H264_NALU_TYPE_SPS) {
                    UINT32 RbspSize = 0;
                    Rval = EliminateEmulationPrevention3byte(Pos , NALU.Len - 1U, &RbspSize);
                    if (Rval == FORMAT_OK) {
                        Rval = ParseSPS(&BitInfo, Pos, RbspSize, SPS);
                    }
                } else if (NALU.NaluType == SVC_H264_NALU_TYPE_PPS) {
                    UINT32 RbspSize = 0;
                    Rval = EliminateEmulationPrevention3byte(Pos , NALU.Len - 1U, &RbspSize);
                    if (Rval == FORMAT_OK) {
                        Rval = ParsePPS(&BitInfo, Pos, RbspSize, PPS, SPS->ChromaFormatIdc);
                    }
                } else {
                    /**/
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

static UINT32 FindStartCode(UINT32 Offset, const UINT8 *Base, UINT32 Size, UINT32 *NewOffset)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if (Size >= 4UL) {
        if (Offset < Size) {
            UINT32 OffsetTmp = Offset;
            UINT32 Code = 0;
            INT32 Shift = 24;
            do {
                Code |= ((UINT32)Base[OffsetTmp] << (UINT32)Shift);
                Rval = SvcFormat_PlusAddr(1, OffsetTmp, Size, &OffsetTmp);
                if (Rval != FORMAT_OK) {
                    break;
                }
                Shift -= 8;
            } while (Shift >= 0);
            if (Rval == FORMAT_OK) {
                UINT32 StartCodePattern = 0x00000001;
                while (Code != StartCodePattern) {
                    Code = (Code << 8) | Base[OffsetTmp];
                    Rval = SvcFormat_PlusAddr(1, OffsetTmp, Size, &OffsetTmp);
                    if ((Rval != FORMAT_OK) || (OffsetTmp == Offset)) {
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                        break;  /* start Code miss... */
                    }
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_MinusAddr(4, OffsetTmp, Size, &OffsetTmp);
                    if (Rval == FORMAT_OK) {
                        *NewOffset = OffsetTmp;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong Offset!", __func__, NULL, NULL, NULL, NULL);
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
UINT32 SvcH264_ReadStream(SVC_STREAM_HDLR_s *Stream, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 FrameSize, UINT32 *DataSize)
{
    UINT32 Rval = FORMAT_OK;
    const UINT8 Start[SVC_H264_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    UINT32 Tmp32;
    UINT32 TmpFrameSize;
    UINT32 OffsetTmp = BufferOffset;
    *DataSize = 0;
    TmpFrameSize = FrameSize;
    while (TmpFrameSize != 0U) {
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_ReadStreamMem(Start, SVC_H264_STRAT_CODE_SIZE, OffsetTmp, BufferBase, BufferSize, &OffsetTmp);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_ReadStream(Stream, Tmp32, OffsetTmp, BufferBase, BufferSize, &OffsetTmp);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
        TmpFrameSize -= (Tmp32 + 4U);
        *DataSize += (Tmp32 + SVC_H264_STRAT_CODE_SIZE);
    }
    return Rval;
}

static UINT32 SvcH264_GetAvcConfig_SPS(UINT32 NALUSize, UINT32 NALUUnitPos, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H264_DEC_CONFIG_s *H264)
{
    UINT32 Rval;
    if (NALUSize > SVC_FORMAT_MAX_SPS_LENGTH) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect NALUSize %u", NALUSize, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        UINT32 OffsetTmp = BufferSize;
        Rval = SvcFormat_WriteStreamMem(H264->SPS, NALUSize, NALUUnitPos, BufferBase, BufferSize, &OffsetTmp);
        if (Rval == FORMAT_OK) {
            UINT8 H264SPSBuffer[SVC_FORMAT_MAX_SPS_LENGTH] = {0};
            if (((NALUSize - 4U) & 0x03U) != 0U) { /* 4 = start nCode; align with 4 bytes boundary */
                /* TODO: workaround is needed */
                H264->SPSLen = ((((UINT16)NALUSize - 4U) >> 2U) + 1U) << 2U;
            } else {
                H264->SPSLen = (UINT16)NALUSize - 4U;
            }
            Rval = W2F(AmbaWrap_memcpy(H264SPSBuffer, H264->SPS, H264->SPSLen));
            if (Rval == FORMAT_OK) {
                UINT32 RbspSize = 0;
                /* first byte is the NAL header*/
                Rval = (UINT32)EliminateEmulationPrevention3byte(&H264SPSBuffer[1] , (UINT32)H264->SPSLen - 1U, &RbspSize);
                if (Rval == FORMAT_OK) {
                    SVC_BIT_OP_s BitInfo;
                    Rval = W2F(AmbaWrap_memset(&BitInfo, 0, sizeof(SVC_BIT_OP_s)));
                    if (Rval == FORMAT_OK) {
                        SVC_H264_SPS_s H264SPS = {0};
                        Rval = ParseSPS(&BitInfo, &H264SPSBuffer[1], RbspSize, &H264SPS);
                        if (Rval == FORMAT_OK) {
                            H264->SarWidth = H264SPS.VuiParam.SarWidth;
                            H264->SarHeight = H264SPS.VuiParam.SarHeight;
                            H264->NumUnitsInTick = H264SPS.VuiParam.NumUnitsInTick;
                            H264->TimeScale = H264SPS.VuiParam.TimeScale;
                            //AmbaPrint("%s: %u %u", __func__, H264->SarWidth, H264->SarHeight);
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcH264_GetAvcConfig_PPS(UINT32 NALUSize, UINT32 NALUUnitPos, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H264_DEC_CONFIG_s *H264)
{
    UINT32 Rval;
    if (NALUSize > SVC_FORMAT_MAX_PPS_LENGTH) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect NALUSize %u", NALUSize, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        UINT32 OffsetTmp = BufferSize;
        Rval = SvcFormat_WriteStreamMem(H264->PPS, NALUSize, NALUUnitPos, BufferBase, BufferSize, &OffsetTmp);
        if (Rval == FORMAT_OK) {
            if (((NALUSize - 4U) & 0x03U) != 0U) { /* 4 = start nCode; align with 4 bytes boundary. */
                /* TODO: workaround is needed */
                H264->PPSLen = ((((UINT16)NALUSize - 4U) >> 2U) + 1U) << 2U;
            } else {
                H264->PPSLen = (UINT16)NALUSize - 4U;
            }
        }
    }
    return Rval;
}

/**
 * Get Avc configuration
 *
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [out] H264 The returned configuration
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH264_GetAvcConfig(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H264_DEC_CONFIG_s *H264)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 NextNALU = BufferSize;
    Rval = FindStartCode(BufferOffset, BufferBase, BufferSize, &NextNALU);
    if (Rval == FORMAT_OK) {
        UINT32 FrameSize = 0;
        UINT32 SliceSize = Size;
        do {
            UINT8 NALUUnitType = 0;
            UINT32 NALUUnitPos = BufferSize;
            UINT32 CurrentNALU = NextNALU;
            Rval = SvcFormat_PlusAddr(SVC_H264_STRAT_CODE_SIZE, CurrentNALU, BufferSize, &NALUUnitPos);
            if (Rval == FORMAT_OK) {
                NALUUnitType = SVC_H264_NALU_TYPE(BufferBase[NALUUnitPos]);
                /* To avoid time-consuming start nCode search procedure, we put the remaining bitstream once we have reached slice NALU. */
                if ((NALUUnitType == SVC_H264_NALU_TYPE_SLICE) || (NALUUnitType == SVC_H264_NALU_TYPE_IDR)) {
                    if (FrameSize >= (0xFFFFFFFFU - SliceSize)) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect FrameSize %u SliceSize %u", FrameSize, SliceSize, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        FrameSize += SliceSize;
                    }
                    break;
                }
                Rval = FindStartCode(NALUUnitPos, BufferBase, BufferSize, &NextNALU);
            }
            if (Rval == FORMAT_OK) {
                UINT32 NALUSize = (NextNALU >= CurrentNALU) ? (NextNALU - CurrentNALU) : ((BufferSize - CurrentNALU) + NextNALU);
                switch (NALUUnitType) {
                case SVC_H264_NALU_TYPE_AUD:
                case SVC_H264_NALU_TYPE_DPA:
                case SVC_H264_NALU_TYPE_DPB:
                case SVC_H264_NALU_TYPE_DPC:
                case SVC_H264_NALU_TYPE_SEI:
                    if (FrameSize >= (0xFFFFFFFFU - NALUSize)) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect FrameSize %u NALUSize %u", FrameSize, NALUSize, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        FrameSize += NALUSize;
                    }
                    break;
                case SVC_H264_NALU_TYPE_SPS:
                    if (H264->SPSLen == 0U) {
                        Rval = SvcH264_GetAvcConfig_SPS(NALUSize, NALUUnitPos, BufferBase, BufferSize, H264);
                    }
                    break;
                case SVC_H264_NALU_TYPE_PPS:
                    if (H264->PPSLen == 0U) {
                        Rval = SvcH264_GetAvcConfig_PPS(NALUSize, NALUUnitPos, BufferBase, BufferSize, H264);
                    }
                    break;
                default:
                    /**/
                    break;
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
        if (FrameSize > Size) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong frame Size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
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
UINT32 SvcH264_GetFrameSize(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, UINT32 Align, UINT32 *FrameSize)
{
    UINT32 NextNALU = BufferSize;
    UINT32 Rval = FindStartCode(BufferOffset, BufferBase, BufferSize, &NextNALU);
    if (Rval == FORMAT_OK) {
        UINT32 CurrFrameSize = 0;
        UINT32 SliceSize = Size;
        UINT32 Padding = 0;
        do {
            UINT8 NALUUnitType = SVC_H264_NALU_TYPE_UNKNOWN;
            UINT32 CurrentNALU = NextNALU;
            UINT32 NALUUnitPos = BufferSize;
            Rval = SvcFormat_PlusAddr(SVC_H264_STRAT_CODE_SIZE, CurrentNALU, BufferSize, &NALUUnitPos);
            if (Rval == FORMAT_OK) {
                NALUUnitType = SVC_H264_NALU_TYPE(BufferBase[NALUUnitPos]);
                /* To avoid time-consuming start nCode search procedure, we put the remaining bitstream once we have reached slice NALU. */
                if ((NALUUnitType == SVC_H264_NALU_TYPE_SLICE) || (NALUUnitType == SVC_H264_NALU_TYPE_IDR)) {
                    if (CurrFrameSize >= (0xFFFFFFFFU - SliceSize)) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect FrameSize %u, SliceSize = %u", CurrFrameSize, SliceSize, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        CurrFrameSize += SliceSize;
                        if ((CurrFrameSize & (Align - 1U)) != 0U) {
                            Padding = Align - (CurrFrameSize & (Align - 1U));
                            CurrFrameSize += Padding;
                        }
                        *FrameSize = CurrFrameSize;
                    }
                    break;
                }
                Rval = FindStartCode(NALUUnitPos, BufferBase, BufferSize, &NextNALU);
            }
            if (Rval == FORMAT_OK) {
                UINT32 NALUSize = (NextNALU >= CurrentNALU) ? (NextNALU - CurrentNALU) : ((BufferSize - CurrentNALU) + NextNALU);
                if ((NALUUnitType == SVC_H264_NALU_TYPE_AUD) || (NALUUnitType == SVC_H264_NALU_TYPE_DPA) || (NALUUnitType == SVC_H264_NALU_TYPE_DPB) || (NALUUnitType == SVC_H264_NALU_TYPE_DPC) || (NALUUnitType == SVC_H264_NALU_TYPE_SEI)) {
                    if (CurrFrameSize >= (0xFFFFFFFFU - NALUSize)) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong frame Size!", __func__, NULL, NULL, NULL, NULL);
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
            if (CurrFrameSize > (Size + Padding)) {
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
UINT32 SvcH264_WriteStream(SVC_STREAM_HDLR_s *Stream, UINT32 Size, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *WriteSize, UINT32 Align)
{
    UINT32 NextNALU = BufferSize;
    UINT32 Rval = FindStartCode(BufferOffset, BufferBase, BufferSize, &NextNALU);
    if (Rval == FORMAT_OK) {
        UINT32 SliceSize = Size;
        UINT32 WrittenBytes = 0;
        UINT32 Padding = 0;
        do {
            UINT8 NALUUnitType = SVC_H264_NALU_TYPE_UNKNOWN;
            UINT32 NALUUnitPos = BufferSize;
            UINT32 CurrentNALU = NextNALU;
            Rval = SvcFormat_PlusAddr(SVC_H264_STRAT_CODE_SIZE, CurrentNALU, BufferSize, &NALUUnitPos);
            if (Rval == FORMAT_OK) {
                NALUUnitType = SVC_H264_NALU_TYPE(BufferBase[NALUUnitPos]);
                /* To avoid time-consuming start nCode search procedure, we put the remaining bitstream once we have reached slice NALU. */
                if ((NALUUnitType == SVC_H264_NALU_TYPE_SLICE) || (NALUUnitType == SVC_H264_NALU_TYPE_IDR)) {
                    if (WrittenBytes >= (0xFFFFFFFFU - SliceSize)) {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect WrittenBytes %u, SliceSize = %u", WrittenBytes, SliceSize, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    } else {
                        WrittenBytes += SliceSize;
                        if ((WrittenBytes & (Align - 1U)) != 0U) {
                            Padding = Align - (WrittenBytes & (Align - 1U));
                            WrittenBytes += Padding;
                        }
                        Rval = SvcFormat_PutBe32(Stream, SliceSize - SVC_H264_STRAT_CODE_SIZE + Padding);
                    }
                    if (Rval == FORMAT_OK) {
                        /* Start Code should be excluded. */
                        Rval = SvcFormat_WriteStream(Stream, SliceSize - SVC_H264_STRAT_CODE_SIZE, NALUUnitPos, BufferBase, BufferSize);
                    }
                    if (Rval == FORMAT_OK) {
                        /* put padding zero */
                        Rval = SvcFormat_PutPadding(Stream, Padding);
                    }
                    break;
                }
                Rval = FindStartCode(NALUUnitPos, BufferBase, BufferSize, &NextNALU);
            }
            if (Rval == FORMAT_OK) {
                UINT32 NALUSize = (NextNALU >= CurrentNALU) ? (NextNALU - CurrentNALU) : ((BufferSize - CurrentNALU) + NextNALU);
                switch (NALUUnitType) {
                case SVC_H264_NALU_TYPE_AUD:
                case SVC_H264_NALU_TYPE_DPA:
                case SVC_H264_NALU_TYPE_DPB:
                case SVC_H264_NALU_TYPE_DPC:
                case SVC_H264_NALU_TYPE_SEI:
                    Rval = SvcFormat_PutBe32(Stream, NALUSize - SVC_H264_STRAT_CODE_SIZE);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_WriteStream(Stream, NALUSize - SVC_H264_STRAT_CODE_SIZE, NALUUnitPos, BufferBase, BufferSize);
                        if (Rval == FORMAT_OK) {
                            if (WrittenBytes >= (0xFFFFFFFFU - NALUSize)) {
                                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect WrittenBytes %u, NALUSize=%u", WrittenBytes, NALUSize, 0U, 0U, 0U);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            } else {
                                WrittenBytes += NALUSize;
                            }
                        }
                    }
                    break;
                default:
                    /**/
                    break;
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
            if (WrittenBytes <= (Size + Padding)) {
                *WriteSize = WrittenBytes;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong WrittenBytes!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        }
    }
    return Rval;
}

static inline UINT16 HIGH_WORD(UINT32 x) {return (UINT16)((x >> 16) & 0x0000FFFFU);}
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
UINT32 SvcH264_PutGOPHeader(UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, const SVC_VIDEO_TRACK_INFO_s *Video, UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 PTS, UINT32 TimePerFrame, UINT32 *NewBufferOffset)
{
    UINT32 Rval;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Video->Info;
    UINT16 High, Low;
    UINT64 TmpPts = PTS;
    UINT32 TmpPtsMsb;
    GOP_HEADER_AVC_SUB_VER_s GopHeaderVer2 = {0};
    UINT8 GopHeaderBuffer[SVC_H264_GOP_NALU_SIZE] = {0};
    UINT8 Reserved = 1U;
    TmpPts = NORMALIZE_CLOCK(TmpPts, Track->TimeScale);
    TmpPtsMsb = (UINT32)((TmpPts >> 32U) & 0xFFFFFFFFU);
    GopHeaderVer2.StartCode_B24_B31_8bit = 0U;
    GopHeaderVer2.StartCode_B16_B23_8bit = 0U;
    GopHeaderVer2.StartCode_B8_B15_8bit  = 0U;
    GopHeaderVer2.StartCode_B0_B7_8bit   = 0x01U;
    GopHeaderVer2.NalHdr = 0x7AU;
    GopHeaderVer2.VerMain = 0x01U;
    GopHeaderVer2.VerSub = 0x2U;
    GopHeaderVer2.GopSkipFirstI = (UINT8)SkipFirstI;
    GopHeaderVer2.GopSkipLastI = (UINT8)SkipLastI;
    High = HIGH_WORD(TimePerFrame);
    Low = LOW_WORD(TimePerFrame);
    GopHeaderVer2.NumUnitInTick_B26_B31_6Bit = (UINT8)(High >> (16U - 6U));
    GopHeaderVer2.NumUnitInTick_B18_B25_8Bit = (UINT8)(High >> (10U - 8U));
    GopHeaderVer2.NumUnitInTick_B16_B17_2Bit = (UINT8)High;
    GopHeaderVer2.Rsvd0As1 = Reserved;
    GopHeaderVer2.NumUnitInTick_B11_B15_5Bit = (UINT8)(Low >> (16U - 5U));
    GopHeaderVer2.NumUnitInTick_B3_B10_8Bit = (UINT8)(Low >> (11U - 8U));
    High = HIGH_WORD(Track->TimeScale);
    GopHeaderVer2.TimeScale_B28_B31_4Bit = (UINT8)(High >> (16U - 4U));
    GopHeaderVer2.Rsvd1As1 = Reserved;
    GopHeaderVer2.NumUnitInTick_B0_B2_3Bit = (UINT8)Low;
    GopHeaderVer2.TimeScale_B20_B27_8Bit = (UINT8)(High >> (12U - 8U));
    Low = LOW_WORD(Track->TimeScale);
    GopHeaderVer2.TimeScale_B13_B15_3Bit = (UINT8)(Low >> (16U - 3U));
    GopHeaderVer2.Rsvd2As1 = Reserved;
    GopHeaderVer2.TimeScale_B16_B19_4Bit = (UINT8)High;
    GopHeaderVer2.TimeScale_B5_B12_8Bit = (UINT8)(Low >> (13U - 8U));
    High = HIGH_WORD((UINT32)TmpPts);
    GopHeaderVer2.GopFirstIPts_B30_B31_2Bit = (UINT8)(High >> (16U - 2U));
    GopHeaderVer2.Rsvd3As1 = Reserved;
    GopHeaderVer2.TimeScale_B0_B4_5Bit = (UINT8)Low;
    GopHeaderVer2.GopFirstIPts_B22_B29_8Bit = (UINT8)(High >> (14U - 8U));
    Low = LOW_WORD((UINT32)TmpPts);
    GopHeaderVer2.GopFirstIPts_B15_1Bit = (UINT8)(Low >> (16U - 1U));
    GopHeaderVer2.Rsvd4As1 = Reserved;
    GopHeaderVer2.GopFirstIPts_B16_B21_6Bit = (UINT8)High;
    GopHeaderVer2.GopFirstIPts_B7_B14_8Bit = (UINT8)(Low >> (15U - 8U));
    GopHeaderVer2.Rsvd5As1 = Reserved;
    GopHeaderVer2.GopFirstIPts_B0_B6_7Bit = (UINT8)Low;
    High = HIGH_WORD(TmpPtsMsb);
    Low = LOW_WORD(TmpPtsMsb);
    GopHeaderVer2.GopFirstIPts_B56_B63_8Bit = (UINT8)(High >> 8U);
    GopHeaderVer2.GopFirstIPts_B48_B55_8Bit = (UINT8)High;
    GopHeaderVer2.GopFirstIPts_B41_B47_7Bit = (UINT8)(Low >> 9U);
    GopHeaderVer2.Rsvd6As1 = Reserved;
    GopHeaderVer2.GopFirstIPts_B33_B40_8Bit = (UINT8)(Low >> 1U);
    GopHeaderVer2.GopN_B2_B7_6Bit = (UINT8)(Video->N >> 2U);
    GopHeaderVer2.Rsvd7As1 = Reserved;
    GopHeaderVer2.GopFirstIPts_B32_1Bit = (UINT8)Low;
    GopHeaderVer2.Rsvd8As1 = 0U;
    GopHeaderVer2.GopM_B0_B3_4Bit = (UINT8)Video->M;
    GopHeaderVer2.GopN_B0_B1_2Bit = (UINT8)Video->N;
    Rval = W2F(AmbaWrap_memcpy(GopHeaderBuffer, &GopHeaderVer2, SVC_H264_GOP_NALU_SIZE));
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_ReadStreamMem(GopHeaderBuffer, SVC_H264_GOP_NALU_SIZE, BufferOffset, BufferBase, BufferSize, NewBufferOffset);
    }
    return Rval;
}

#if 0
UINT8 *SvcH264_PutGOPHeader(UINT8 *Addr, UINT8 *Base, const UINT8 *Limit, const SVC_VIDEO_TRACK_INFO_s *Video, UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 PTS)
{
    UINT8 *Rval = NULL;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Video->Info;

    UINT16 High, Low;
    UINT8 Header[SVC_H264_GOP_NALU_SIZE];
    const UINT32 TimePerFrame = Track->TimePerFrame;
    UINT64 TmpPts = PTS;
    TmpPts = NORMALIZE_CLOCK(TmpPts, Track->TimeScale);
//    LOG_VERB("PTS = %llu %u", PTS, SkipLastI);
    Header[0] = (UINT8)0x00;
    Header[1] = (UINT8)0x00;
    Header[2] = (UINT8)0x00;
    Header[3] = (UINT8)0x01;
    /* NAL Header */
    Header[4] = (UINT8)0x7a;
    /* Main version */
    Header[5] = (UINT8)0x01;
    /* Sub version */
    Header[6] = (UINT8)0x01;
    High = HIGH_WORD(TimePerFrame);
    Low = LOW_WORD(TimePerFrame);
    Header[7] = ((UINT8)SkipFirstI << (8U - 1U)) | ((UINT8)SkipLastI << (7U - 1U)) | ((UINT8)(High >> (16U - 6U)));
    Header[8] = ((UINT8)(High >> (10U - 8U)));
    Header[9] = ((UINT8)(High << (8U - 2U))) | (1U << (6U - 1U)) | ((UINT8)(Low >> (16U - 5U)));
    Header[10] = ((UINT8)(Low >> (11U - 8U)));
    High = HIGH_WORD(Track->TimeScale);
    Header[11] = ((UINT8)(Low << (8U - 3U))) | (1U << (5U - 1U)) | ((UINT8)(High >> (16U - 4U)));
    Header[12] = ((UINT8)(High >> (12U - 8U)));
    Low = LOW_WORD(Track->TimeScale);
    Header[13] = ((UINT8)(High << (8U - 4U))) | (1U << (4U - 1U)) | ((UINT8)(Low >> (16U - 3U)));
    Header[14] = ((UINT8)(Low >> (13U - 8U)));
    High = HIGH_WORD((UINT32)TmpPts);
    Header[15] = ((UINT8)(Low << (8U - 5U))) | (1U << (3U - 1U)) | ((UINT8)(High >> (16U - 2U)));
    Header[16] = ((UINT8)(High >> (14U - 8U)));
    Low = LOW_WORD((UINT32)TmpPts);
    Header[17] = ((UINT8)(High << (8U - 6U))) | (1U << (2U - 1U)) | ((UINT8)(Low >> (16U - 1U)));
    Header[18] = ((UINT8)(Low >> (15U - 8U)));
    Header[19] = ((UINT8)(Low << (8U - 7U))) | (1U >> (1U - 1U));
    Header[20] = ((UINT8)Video->N << (8U - 8U));
    Header[21] = ((UINT8)Video->M << (8U - 4U)) & 0xf0U;
    Rval = SvcFormat_ReadStreamMem(Header, SVC_H264_GOP_NALU_SIZE, Addr, Base, Limit);

    return Rval;
}

/**
 * Put EOS header
 */
UINT8 AmbaH264_PutEOS(SVC_FIFO_HDLR_s *Fifo, UINT8 *BufferBase, UINT8 *BufferLimit)
{
    SVC_FIFO_BITS_DESC_s Frame = {0};
#ifdef USING_NORMAL_EOS
    UINT8 eos_nalu[SVC_H264_EOS_NALU_SIZE] = {0, 0, 0, 1, 0x0b};
#else
    UINT8 eos_nalu[SVC_H264_EOS_NALU_SIZE] = {0, 0, 0, 1, 0x0a};
#endif
    if (SvcFIFO_PrepareEntry(Fifo, &Frame) == FORMAT_OK) {
        Frame.size = sizeof(eos_nalu);
        if (SvcFormat_ReadStreamMem(eos_nalu, sizeof(eos_nalu), Frame.startAddr, BufferBase, BufferLimit) != NULL) {
            if (SvcFIFO_WriteEntry(Fifo, &Frame) == FORMAT_OK) {
#if 0
                if (!p_movie->entropy_mode)
                    h264_cavlc_decode_write_desc_eos((UINT32)(ptr-1));
#endif
                return FORMAT_OK;
            }
            Format_Perror(0, 0);
        } else {
            Format_Perror(0, 0);
        }
    } else {
        Format_Perror(0, 0);
    }
    return FORMAT_ERR_GENERAL_ERROR;
}

UINT8 *AmbaH264_PutEOS(UINT8 *Addr, UINT8 *BufferBase, UINT8 *BufferLimit)
{
#ifdef USING_NORMAL_EOS
    UINT8 eos_nalu[SVC_H264_EOS_NALU_SIZE] = {0, 0, 0, 1, 0x0b};
#else
    UINT8 eos_nalu[SVC_H264_EOS_NALU_SIZE] = {0, 0, 0, 1, 0x0a};
#endif
    return SvcFormat_ReadStreamMem(eos_nalu, sizeof(eos_nalu), Addr, BufferBase, BufferLimit);
}
#endif

