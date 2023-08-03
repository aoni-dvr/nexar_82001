/**
*  @file SvcIsoCfg_CV2FS.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
*  @details svc iso config functions
*
*/

#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaSYS.h"
#include "AmbaNVM_Partition.h"
#include "AmbaUtility.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcNvm.h"
#include "SvcWrap.h"
#include "SvcIsoCfg.h"
#include "SvcResCfg.h"
#include "SvcVinSrc.h"
#include "SvcDSP.h"
#include "AmbaSensor.h"
#include "SvcIK.h"

#define SVC_LOG_ISOC     "ISOC"

#define SVC_ISO_CFG_DTL_LOAD_DONE   (0x1U)
static AMBA_KAL_EVENT_FLAG_t SvcIsoCfgDTLFlg GNU_SECTION_NOZEROINIT;
static AMBA_IK_COLOR_CORRECTION_s SvcIsoCfgCcThreeD GNU_SECTION_NOZEROINIT;

static void SvcIsoCfg_DTLInit(void)
{
    static UINT32 IsoCfgDTLFlgInit = 0U;
    static char   SvcIsoCfgDTLFlgName[16] = "SvcIsoCfg";

    if (IsoCfgDTLFlgInit == 0U) {
        UINT32 ErrCode;
        AmbaSvcWrap_MisraMemset(&SvcIsoCfgDTLFlg, 0, sizeof(SvcIsoCfgDTLFlg));
        ErrCode = AmbaKAL_EventFlagCreate(&SvcIsoCfgDTLFlg, SvcIsoCfgDTLFlgName);
        if (ErrCode != 0U) {
            SvcLog_NG(SVC_LOG_ISOC, "Create event flag fail. ErrCode(0x%08X)", ErrCode, 0U);
        } else {
            ErrCode = AmbaKAL_EventFlagClear(&SvcIsoCfgDTLFlg, 0xFFFFFFFFU);
            if (ErrCode != 0U) {
                SvcLog_NG(SVC_LOG_ISOC, "Clear event flag fail. ErrCode(0x%08X)", ErrCode, 0U);
            }

            IsoCfgDTLFlgInit = 1U;
        }
    }
}

static void SvcIsoCfg_DTLWaitDone(void)
{
    UINT32 ErrCode;
    UINT32 ActualFlags = 0U;

    SvcIsoCfg_DTLInit();

    ErrCode = AmbaKAL_EventFlagGet(&SvcIsoCfgDTLFlg, SVC_ISO_CFG_DTL_LOAD_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 10000U);
    if (ErrCode != 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "Wait data load done timeout. ErrCode(0x%08X)", ErrCode, 0U);
    }
}

static void SvcIsoCfg_R2R(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32  Err;

    {
        AMBA_IK_RGB_IR_s IkRgbIR;

        AmbaSvcWrap_MisraMemset(&IkRgbIR, 0, sizeof(IkRgbIR));

        Err = AmbaIK_SetRgbIr(pImgMode, &IkRgbIR);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetRgbIr failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FE_TONE_CURVE_s IkFeToneCurve;

        AmbaSvcWrap_MisraMemset(&IkFeToneCurve, 0, sizeof(IkFeToneCurve));

        Err = AmbaIK_SetFeToneCurve(pImgMode, &IkFeToneCurve);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeToneCurve failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_STATIC_BLC_LVL_s IkFeStaticBlc = {
            .BlackR  = -252,
            .BlackGr = -252,
            .BlackGb = -252,
            .BlackB  = -252,
            .BlackG  = 0,
            .BlackIr = 0,
        };

        Err = AmbaIK_SetFeStaticBlc(pImgMode, &IkFeStaticBlc, 0U);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeStaticBlc failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_DYNAMIC_BAD_PXL_COR_s IkDynamicBpc = {
            .Enable            = 4U,
            .HotPixelStrength  = 1U,
            .DarkPixelStrength = 1U,
            .CorrectionMethod  = 0U,
        };

        Err = AmbaIK_SetDynamicBadPxlCor(pImgMode, &IkDynamicBpc);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetDynamicBadPxlCor failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_CFA_LEAKAGE_FILTER_s IkCfaLeakage;

        AmbaSvcWrap_MisraMemset(&IkCfaLeakage, 0, sizeof(IkCfaLeakage));

        Err = AmbaIK_SetCfaLeakageFilter(pImgMode, &IkCfaLeakage);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCfaLeakageFilter failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_CFA_NOISE_FILTER_s IkCfaNoiseFilter = {
            .Enable = 1U,
            .ExtentFine = { 9U, 9U, 9U },
            .ExtentRegular = { 23U, 23U, 23U },
            .OriginalBlendStr = { 3U, 3U, 3U },
            .SelectivityFine = 0U,
            .SelectivityRegular = 0U,
            .StrengthFine = { 136U, 136U, 136U },
            .DirLevMax = { 255U, 255U, 255U },
            .DirLevMin = { 0U, 0U, 0U },
            .DirLevMul = { 3U, 3U, 3U },
            .DirLevOffset = { -500, -500, -500 },
            .DirLevShift = { 3U, 3U, 3U },
            .LevMul = { 3U, 3U, 3U },
            .LevOffset = { -500, -500, -500 },
            .LevShift = { 4U, 4U, 4U },
            .MaxNoiseLevel = { 100U, 100U, 100U },
            .MinNoiseLevel = { 0U, 0U, 0U },
            .ApplyToColorDiffRed = 10u,
            .ApplyToColorDiffBlue = 10U,
            .OptForRccbRed = 1U,
            .OptForRccbBlue = 1U,
            .DirectionalEnable = 1U,
            .DirectionalHorvertEdgeStrength = 24U,
            .DirectionalHorvertStrengthBias = 128U,
            .DirectionalHorvertOriginalBlendStrength = 0U,
        };

        Err = AmbaIK_SetCfaNoiseFilter(pImgMode, &IkCfaNoiseFilter);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCfaNoiseFilter failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_ANTI_ALIASING_s IkAntiAliasing = {
            .Enable = 1U,
            .Thresh = 0U,
            .LogFractionalCorrect = 0U,
        };

        Err = AmbaIK_SetAntiAliasing(pImgMode, &IkAntiAliasing);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAntiAliasing failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FE_WB_GAIN_s IkFeWbGain = {
            .Rgain  = 4096U,
            .Ggain  = 4096U,
            .Bgain  = 4096U,
            .IrGain = 4096U,
            .ShutterRatio = 1U,
        };

        Err = AmbaIK_SetFeWbGain(pImgMode, &IkFeWbGain, 0U);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_WB_GAIN_s IkAfterCeWbGain = {
            .GainR = 7137U,
            .GainG = 4096U,
            .GainB = 7476U,
        };

        Err = AmbaIK_SetAfterCeWbGain(pImgMode, &IkAfterCeWbGain);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAfterCeWbGain failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_WB_GAIN_s IkBeforeCeWbGain = {
            .GainR = 4096U,
            .GainG = 4096U,
            .GainB = 4096U,
        };

        Err = AmbaIK_SetBeforeCeWbGain(pImgMode, &IkBeforeCeWbGain);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetBeforeCeWbGain failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_DEMOSAIC_s IkDemosaic = {
            .Enable = 1U,
            .ActivityThresh = 3U,
            .ActivityDifferenceThresh = 1365U,
            .GradClipThresh = 512U,
            .GradNoiseThresh = 148U,
            .AliasInterpolationStrength = 0U,
            .AliasInterpolationThresh = 0U,
        };

        Err = AmbaIK_SetDemosaic(pImgMode, &IkDemosaic);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetDemosaic failed %u", Err, 0U);
        }

    }

    {
        AMBA_IK_GRGB_MISMATCH_s IkGbGrMismatch = {
            .NarrowEnable = 0U,
            .WideEnable = 0U,
            .WideSafety = 64U,
            .WideThresh = 64U,
        };

        Err = AmbaIK_SetGrgbMismatch(pImgMode, &IkGbGrMismatch);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetGrgbMismatch failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_RGB_TO_12Y_s IkRgbTo12Y = {
            .Enable = 1U,
            .MatrixValues = { 218, 732, 74 },
            .YOffset = 0,
        };

        Err = AmbaIK_SetRgbTo12Y(pImgMode, &IkRgbTo12Y);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetRgbTo12Y failed %u", Err, 0U);
        }
    }

    {
        SvcIsoCfg_DTLWaitDone();

        Err = AmbaIK_SetColorCorrection(pImgMode, &SvcIsoCfgCcThreeD);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetColorCorrection failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_TONE_CURVE_s IkToneCurve = {
            .ToneCurveRed = {
                0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   2,
                2,   3,   4,   4,   5,   6,   8,   9,  11,  12,  14,  15,  16,  18,  19,  21,
               22,  23,  25,  26,  28,  30,  33,  35,  37,  39,  41,  44,  46,  48,  51,  53,
               56,  59,  61,  64,  67,  69,  72,  75,  77,  80,  83,  87,  90,  93,  97, 100,
              103, 107, 110, 113, 117, 120, 123, 127, 130, 133, 135, 138, 140, 143, 145, 148,
              150, 153, 155, 158, 160, 163, 165, 168, 170, 173, 175, 178, 180, 183, 186, 189,
              192, 195, 198, 200, 203, 206, 209, 212, 215, 218, 221, 224, 227, 230, 233, 235,
              238, 241, 244, 247, 250, 254, 258, 262, 265, 269, 273, 277, 281, 285, 288, 292,
              296, 300, 304, 308, 312, 315, 319, 323, 327, 331, 335, 338, 342, 346, 350, 355,
              360, 365, 369, 374, 379, 384, 389, 394, 399, 403, 408, 413, 418, 423, 428, 433,
              437, 442, 447, 452, 458, 465, 471, 477, 484, 490, 496, 503, 509, 516, 522, 528,
              535, 541, 547, 554, 560, 565, 570, 575, 580, 585, 590, 595, 600, 605, 610, 615,
              620, 625, 630, 635, 640, 645, 650, 655, 660, 665, 670, 675, 680, 685, 690, 695,
              700, 705, 710, 715, 720, 725, 730, 735, 740, 744, 749, 753, 758, 762, 767, 771,
              776, 780, 787, 794, 801, 808, 814, 821, 828, 835, 844, 854, 863, 872, 881, 891,
              900, 909, 918, 928, 937, 946, 955, 962, 969, 976, 983, 990, 998,1007,1015,1023
            },

            .ToneCurveGreen = {
                0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   2,
                2,   3,   4,   4,   5,   6,   8,   9,  11,  12,  14,  15,  16,  18,  19,  21,
               22,  23,  25,  26,  28,  30,  33,  35,  37,  39,  41,  44,  46,  48,  51,  53,
               56,  59,  61,  64,  67,  69,  72,  75,  77,  80,  83,  87,  90,  93,  97, 100,
              103, 107, 110, 113, 117, 120, 123, 127, 130, 133, 135, 138, 140, 143, 145, 148,
              150, 153, 155, 158, 160, 163, 165, 168, 170, 173, 175, 178, 180, 183, 186, 189,
              192, 195, 198, 200, 203, 206, 209, 212, 215, 218, 221, 224, 227, 230, 233, 235,
              238, 241, 244, 247, 250, 254, 258, 262, 265, 269, 273, 277, 281, 285, 288, 292,
              296, 300, 304, 308, 312, 315, 319, 323, 327, 331, 335, 338, 342, 346, 350, 355,
              360, 365, 369, 374, 379, 384, 389, 394, 399, 403, 408, 413, 418, 423, 428, 433,
              437, 442, 447, 452, 458, 465, 471, 477, 484, 490, 496, 503, 509, 516, 522, 528,
              535, 541, 547, 554, 560, 565, 570, 575, 580, 585, 590, 595, 600, 605, 610, 615,
              620, 625, 630, 635, 640, 645, 650, 655, 660, 665, 670, 675, 680, 685, 690, 695,
              700, 705, 710, 715, 720, 725, 730, 735, 740, 744, 749, 753, 758, 762, 767, 771,
              776, 780, 787, 794, 801, 808, 814, 821, 828, 835, 844, 854, 863, 872, 881, 891,
              900, 909, 918, 928, 937, 946, 955, 962, 969, 976, 983, 990, 998,1007,1015,1023
            },

            .ToneCurveBlue = {
                0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   2,
                2,   3,   4,   4,   5,   6,   8,   9,  11,  12,  14,  15,  16,  18,  19,  21,
               22,  23,  25,  26,  28,  30,  33,  35,  37,  39,  41,  44,  46,  48,  51,  53,
               56,  59,  61,  64,  67,  69,  72,  75,  77,  80,  83,  87,  90,  93,  97, 100,
              103, 107, 110, 113, 117, 120, 123, 127, 130, 133, 135, 138, 140, 143, 145, 148,
              150, 153, 155, 158, 160, 163, 165, 168, 170, 173, 175, 178, 180, 183, 186, 189,
              192, 195, 198, 200, 203, 206, 209, 212, 215, 218, 221, 224, 227, 230, 233, 235,
              238, 241, 244, 247, 250, 254, 258, 262, 265, 269, 273, 277, 281, 285, 288, 292,
              296, 300, 304, 308, 312, 315, 319, 323, 327, 331, 335, 338, 342, 346, 350, 355,
              360, 365, 369, 374, 379, 384, 389, 394, 399, 403, 408, 413, 418, 423, 428, 433,
              437, 442, 447, 452, 458, 465, 471, 477, 484, 490, 496, 503, 509, 516, 522, 528,
              535, 541, 547, 554, 560, 565, 570, 575, 580, 585, 590, 595, 600, 605, 610, 615,
              620, 625, 630, 635, 640, 645, 650, 655, 660, 665, 670, 675, 680, 685, 690, 695,
              700, 705, 710, 715, 720, 725, 730, 735, 740, 744, 749, 753, 758, 762, 767, 771,
              776, 780, 787, 794, 801, 808, 814, 821, 828, 835, 844, 854, 863, 872, 881, 891,
              900, 909, 918, 928, 937, 946, 955, 962, 969, 976, 983, 990, 998,1007,1015,1023
            },
        };

        Err = AmbaIK_SetToneCurve(pImgMode, &IkToneCurve);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetToneCurve failed %u", Err, 0U);
        }
    }
}

static void SvcIsoCfg_CleanRaw(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32             Err;
    AMBA_IK_ABILITY_s  Ability = {0};

    Err = AmbaIK_GetContextAbility(pImgMode, &Ability);

    if (Err != IK_OK) {
        SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_GetContextAbility failed %u", Err, 0U);
    } else {
        if (AMBA_IK_STILL_HISO != Ability.StillPipe) {
            AMBA_IK_RGB_IR_s IkRgbIR;

            AmbaSvcWrap_MisraMemset(&IkRgbIR, 0, sizeof(IkRgbIR));

            Err = AmbaIK_SetRgbIr(pImgMode, &IkRgbIR);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetRgbIr failed %u", Err, 0U);
            }
        }
    }

    {
        AMBA_IK_FE_TONE_CURVE_s IkFeToneCurve;

        AmbaSvcWrap_MisraMemset(&IkFeToneCurve, 0, sizeof(IkFeToneCurve));

        Err = AmbaIK_SetFeToneCurve(pImgMode, &IkFeToneCurve);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeToneCurve failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_STATIC_BLC_LVL_s IkFeStaticBlc = {
            .BlackR  = 0,
            .BlackGr = 0,
            .BlackGb = 0,
            .BlackB  = 0,
            .BlackG  = 0,
            .BlackIr = 0,
        };

        Err = AmbaIK_SetFeStaticBlc(pImgMode, &IkFeStaticBlc, 0U);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeStaticBlc failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_DYNAMIC_BAD_PXL_COR_s IkDynamicBpc = {
            .Enable            = 0U,
            .HotPixelStrength  = 0U,
            .DarkPixelStrength = 0U,
            .CorrectionMethod  = 0U,
        };

        Err = AmbaIK_SetDynamicBadPxlCor(pImgMode, &IkDynamicBpc);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetDynamicBadPxlCor failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_CFA_LEAKAGE_FILTER_s IkCfaLeakage;

        AmbaSvcWrap_MisraMemset(&IkCfaLeakage, 0, sizeof(IkCfaLeakage));

        Err = AmbaIK_SetCfaLeakageFilter(pImgMode, &IkCfaLeakage);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCfaLeakageFilter failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_CFA_NOISE_FILTER_s IkCfaNoiseFilter = {
            .Enable = 0U,
            .ExtentFine = { 9U, 9U, 9U },
            .ExtentRegular = { 23U, 23U, 23U },
            .OriginalBlendStr = { 3U, 3U, 3U },
            .SelectivityFine = 0U,
            .SelectivityRegular = 0U,
            .StrengthFine = { 136U, 136U, 136U },
            .DirLevMax = { 255U, 255U, 255U },
            .DirLevMin = { 0U, 0U, 0U },
            .DirLevMul = { 3U, 3U, 3U },
            .DirLevOffset = { -500, -500, -500 },
            .DirLevShift = { 3U, 3U, 3U },
            .LevMul = { 3U, 3U, 3U },
            .LevOffset = { -500, -500, -500 },
            .LevShift = { 4U, 4U, 4U },
            .MaxNoiseLevel = { 100U, 100U, 100U },
            .MinNoiseLevel = { 0U, 0U, 0U },
            .ApplyToColorDiffRed = 10u,
            .ApplyToColorDiffBlue = 10U,
            .OptForRccbRed = 1U,
            .OptForRccbBlue = 1U,
            .DirectionalEnable = 1U,
            .DirectionalHorvertEdgeStrength = 24U,
            .DirectionalHorvertStrengthBias = 128U,
            .DirectionalHorvertOriginalBlendStrength = 0U,
        };

        Err = AmbaIK_SetCfaNoiseFilter(pImgMode, &IkCfaNoiseFilter);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCfaNoiseFilter failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_ANTI_ALIASING_s IkAntiAliasing = {
            .Enable = 0U,
            .Thresh = 0U,
            .LogFractionalCorrect = 0U,
        };

        Err = AmbaIK_SetAntiAliasing(pImgMode, &IkAntiAliasing);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAntiAliasing failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FE_WB_GAIN_s IkFeWbGain = {
            .Rgain  = 4096U,
            .Ggain  = 4096U,
            .Bgain  = 4096U,
            .IrGain = 4096U,
            .ShutterRatio = 1U,
        };

        Err = AmbaIK_SetFeWbGain(pImgMode, &IkFeWbGain, 0U);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_WB_GAIN_s IkAfterCeWbGain = {
            .GainR = 4096U,
            .GainG = 4096U,
            .GainB = 4096U,
        };

        Err = AmbaIK_SetAfterCeWbGain(pImgMode, &IkAfterCeWbGain);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAfterCeWbGain failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_WB_GAIN_s IkBeforeCeWbGain = {
            .GainR = 4096U,
            .GainG = 4096U,
            .GainB = 4096U,
        };

        Err = AmbaIK_SetBeforeCeWbGain(pImgMode, &IkBeforeCeWbGain);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetBeforeCeWbGain failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_DEMOSAIC_s IkDemosaic = {
            .Enable = 1U,
            .ActivityThresh = 3U,
            .ActivityDifferenceThresh = 1365U,
            .GradClipThresh = 512U,
            .GradNoiseThresh = 148U,
            .AliasInterpolationStrength = 0U,
            .AliasInterpolationThresh = 0U,
        };

        Err = AmbaIK_SetDemosaic(pImgMode, &IkDemosaic);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetDemosaic failed %u", Err, 0U);
        }

    }

    {
        AMBA_IK_GRGB_MISMATCH_s IkGbGrMismatch = {
            .NarrowEnable = 0U,
            .WideEnable = 0U,
            .WideSafety = 64U,
            .WideThresh = 64U,
        };

        Err = AmbaIK_SetGrgbMismatch(pImgMode, &IkGbGrMismatch);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetGrgbMismatch failed %u", Err, 0U);
        }
    }

    if ( AMBA_IK_STILL_HISO != Ability.StillPipe) {
        AMBA_IK_RGB_TO_12Y_s IkRgbTo12Y = {
            .Enable = 1U,
            .MatrixValues = { 218, 732, 74 },
            .YOffset = 0,
        };

        Err = AmbaIK_SetRgbTo12Y(pImgMode, &IkRgbTo12Y);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetRgbTo12Y failed %u", Err, 0U);
        }
    }

    {
        SvcIsoCfg_DTLWaitDone();

        Err = AmbaIK_SetColorCorrection(pImgMode, &SvcIsoCfgCcThreeD);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetColorCorrection failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_TONE_CURVE_s IkToneCurve = {
            .ToneCurveRed = {
                0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,
               64,  68,  72,  76,  80,  84,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
              128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189,
              193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253,
              257, 261, 265, 269, 273, 277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317,
              321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381,
              385, 389, 393, 397, 401, 405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445,
              449, 453, 457, 461, 465, 469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509,
              514, 518, 522, 526, 530, 534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574,
              578, 582, 586, 590, 594, 598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638,
              642, 646, 650, 654, 658, 662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702,
              706, 710, 714, 718, 722, 726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766,
              770, 774, 778, 782, 786, 790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830,
              834, 838, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895,
              899, 903, 907, 911, 915, 919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959,
              963, 967, 971, 975, 979, 983, 987, 991, 995, 999,1003,1007,1011,1015,1019,1023
            },

            .ToneCurveGreen = {
                0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,
               64,  68,  72,  76,  80,  84,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
              128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189,
              193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253,
              257, 261, 265, 269, 273, 277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317,
              321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381,
              385, 389, 393, 397, 401, 405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445,
              449, 453, 457, 461, 465, 469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509,
              514, 518, 522, 526, 530, 534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574,
              578, 582, 586, 590, 594, 598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638,
              642, 646, 650, 654, 658, 662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702,
              706, 710, 714, 718, 722, 726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766,
              770, 774, 778, 782, 786, 790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830,
              834, 838, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895,
              899, 903, 907, 911, 915, 919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959,
              963, 967, 971, 975, 979, 983, 987, 991, 995, 999,1003,1007,1011,1015,1019,1023
            },

            .ToneCurveBlue = {
                0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,
               64,  68,  72,  76,  80,  84,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
              128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189,
              193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253,
              257, 261, 265, 269, 273, 277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317,
              321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381,
              385, 389, 393, 397, 401, 405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445,
              449, 453, 457, 461, 465, 469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509,
              514, 518, 522, 526, 530, 534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574,
              578, 582, 586, 590, 594, 598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638,
              642, 646, 650, 654, 658, 662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702,
              706, 710, 714, 718, 722, 726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766,
              770, 774, 778, 782, 786, 790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830,
              834, 838, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895,
              899, 903, 907, 911, 915, 919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959,
              963, 967, 971, 975, 979, 983, 987, 991, 995, 999,1003,1007,1011,1015,1019,1023
            },
        };

        Err = AmbaIK_SetToneCurve(pImgMode, &IkToneCurve);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetToneCurve failed %u", Err, 0U);
        }
    }
}


static void SvcIosCfg_Y2Y(const AMBA_IK_MODE_CFG_s *pImgMode, const AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Err;

    if (pAbility->VideoPipe != AMBA_IK_VIDEO_Y2Y) {
        AMBA_IK_RGB_TO_YUV_MATRIX_s IkRgbToYuvMatrix = {
            .MatrixValues = { 218, 732, 74, -117, -395, 512, 512, -465, -47 },
            .YOffset = 0,
            .UOffset = 128,
            .VOffset = 128,
        };

        Err = AmbaIK_SetRgbToYuvMatrix(pImgMode, &IkRgbToYuvMatrix);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetRgbToYuvMatrix failed %u", Err, 0U);
        }
    }

    if (pAbility->VideoPipe == AMBA_IK_VIDEO_Y2Y) {
        Err = AmbaIK_SetYuvMode(pImgMode, AMBA_IK_YUV422);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetYuvMode failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_CHROMA_SCALE_s IkChromaScale = {
            .Enable = 1U,
            .GainCurve = {
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874,
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874,
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874,
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874,
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874,
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874,
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874,
                874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874, 874
            },
        };

        if (pAbility->VideoPipe == AMBA_IK_VIDEO_Y2Y) {
            IkChromaScale.Enable = 0U;
        }

        Err = AmbaIK_SetChromaScale(pImgMode, &IkChromaScale);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetChromaScale failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_CHROMA_MEDIAN_FILTER_s IkChromaMedianFilter = {
            .Enable = 1U,
            .CbAdaptiveAmount = 256U,
            .CbAdaptiveStrength = 255U,
            .CbNonAdaptiveStrength = 0U,
            .CrAdaptiveAmount = 256U,
            .CrAdaptiveStrength = 255U,
            .CrNonAdaptiveStrength = 0U,
        };

        if (pAbility->VideoPipe == AMBA_IK_VIDEO_Y2Y) {
            IkChromaMedianFilter.Enable = 0U;
        }
        Err = AmbaIK_SetChromaMedianFilter(pImgMode, &IkChromaMedianFilter);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetChromaMedianFilter failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_CHROMA_FILTER_s IkChromaFilter = {
            .Enable = 1U,
            .NoiseLevelCb = 30U,
            .NoiseLevelCr = 30U,
            .OriginalBlendStrengthCb = 0U,
            .OriginalBlendStrengthCr = 0U,
            .Radius = 32U,
        };

        if (pAbility->VideoPipe == AMBA_IK_VIDEO_Y2Y) {
            IkChromaFilter.Enable = 0U;
        }

        Err = AmbaIK_SetChromaFilter(pImgMode, &IkChromaFilter);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetChromaFilter failed %u", Err, 0U);
        }
    }

    {   //  QQQ
        AMBA_IK_WIDE_CHROMA_FILTER_s IkWideChromaFilter = {
            .Enable = 0U,
            .NoiseLevelCb = 3U,
            .NoiseLevelCr = 3U,
        };

        Err = AmbaIK_SetWideChromaFilter(pImgMode, &IkWideChromaFilter);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetWideChromaFilter failed %u", Err, 0U);
        }
    }

    {   //  QQQ
        AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s IkWideChromaFilterCombine = {
            .T0Cb = 1U,
            .T0Cr = 2U,
            .T1Cb = 3U,
            .T1Cr = 4U,
            .AlphaMaxCb = 255U,
            .AlphaMaxCr = 255U,
            .AlphaMinCb = 0U,
            .AlphaMinCr = 0U,
            .MaxChangeCb = 2U,
            .MaxChangeCr = 2U,
        };

        Err = AmbaIK_SetWideChromaFilterCombine(pImgMode, &IkWideChromaFilterCombine);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetWideChromaFilterCombine failed %u", Err, 0U);
        }
    }

    {   //  QQQ
        AMBA_IK_LUMA_NOISE_REDUCTION_s IkLumaNoiseReduction = {0};

        Err = AmbaIK_SetLumaNoiseReduction(pImgMode, &IkLumaNoiseReduction);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetLumaNoiseReduction failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FIRST_LUMA_PROC_MODE_s IkFirstLumaProcessMode;

        AmbaSvcWrap_MisraMemset(&IkFirstLumaProcessMode, 0, sizeof(IkFirstLumaProcessMode));

        IkFirstLumaProcessMode.UseSharpenNotAsf = 0U;
        Err = AmbaIK_SetFirstLumaProcMode(pImgMode, &IkFirstLumaProcessMode);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFirstLumaProcMode failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_ADV_SPAT_FLTR_s  IkAsf;

        AmbaSvcWrap_MisraMemset(&IkAsf, 0, sizeof(IkAsf));

        IkAsf.Enable              = 0U;
        IkAsf.Fir.Specify         = 2U;
        IkAsf.Adapt.T0Down        = 2U;
        IkAsf.Adapt.T0Up          = 2U;
        IkAsf.Adapt.T1Down        = 4U;
        IkAsf.Adapt.T1Up          = 3U;
        IkAsf.LevelStrAdjust.High = 1U;

        Err = AmbaIK_SetAdvSpatFltr(pImgMode, &IkAsf);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAdvSpatFltr failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FSTSHPNS_BOTH_s IkSharpenBoth;

        AmbaSvcWrap_MisraMemset(&IkSharpenBoth, 0, sizeof(IkSharpenBoth));

        IkSharpenBoth.Enable = 0U;
        Err = AmbaIK_SetFstShpNsBoth(pImgMode, &IkSharpenBoth);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsBoth failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FSTSHPNS_NOISE_s IkSharpenNoise;

        AmbaSvcWrap_MisraMemset(&IkSharpenNoise, 0, sizeof(IkSharpenNoise));

        IkSharpenNoise.LevelStrAdjust.High = 1U;
        Err = AmbaIK_SetFstShpNsNoise(pImgMode, &IkSharpenNoise);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsNoise failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FSTSHPNS_FIR_s IkFir;

        AmbaSvcWrap_MisraMemset(&IkFir, 0, sizeof(IkFir));

        Err = AmbaIK_SetFstShpNsFir(pImgMode, &IkFir);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsFir failed %u", Err, 0U);
        }
    }

    {
        UINT32 i;
        AMBA_IK_FSTSHPNS_CORING_s IkCoring;

        AmbaSvcWrap_MisraMemset(&IkCoring, 0, sizeof(IkCoring));

        for (i = 0U; i < IK_NUM_CORING_TABLE_INDEX; i++) {
            IkCoring.Coring[i] = 23U;
        }

        IkCoring.FractionalBits = 3U;
        Err = AmbaIK_SetFstShpNsCoring(pImgMode, &IkCoring);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsCoring failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FSTSHPNS_COR_IDX_SCL_s IkCoringIndexScale;

        AmbaSvcWrap_MisraMemset(&IkCoringIndexScale, 0, sizeof(IkCoringIndexScale));

        IkCoringIndexScale.High = 130U;
        IkCoringIndexScale.HighDelta = 5U;
        IkCoringIndexScale.HighStrength = 16U;
        IkCoringIndexScale.Low = 50U;
        IkCoringIndexScale.LowDelta = 5U;
        IkCoringIndexScale.LowStrength = 16U;
        IkCoringIndexScale.MidStrength = 16U;
        Err = AmbaIK_SetFstShpNsCorIdxScl(pImgMode, &IkCoringIndexScale);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsCorIdxScl failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FSTSHPNS_MIN_COR_RST_s IkMinCoringResult;

        AmbaSvcWrap_MisraMemset(&IkMinCoringResult, 0, sizeof(IkMinCoringResult));

        IkMinCoringResult.High = 1U;
        Err = AmbaIK_SetFstShpNsMinCorRst(pImgMode, &IkMinCoringResult);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsMinCorRst failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FSTSHPNS_MAX_COR_RST_s IkMaxCoringResult;

        AmbaSvcWrap_MisraMemset(&IkMaxCoringResult, 0, sizeof(IkMaxCoringResult));

        IkMaxCoringResult.High = 1U;
        Err = AmbaIK_SetFstShpNsMaxCorRst(pImgMode, &IkMaxCoringResult);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsMaxCorRst failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FSTSHPNS_SCL_COR_s IkScaleCoring;

        AmbaSvcWrap_MisraMemset(&IkScaleCoring, 0, sizeof(IkScaleCoring));

        IkScaleCoring.High = 1U;
        Err = AmbaIK_SetFstShpNsSclCor(pImgMode, &IkScaleCoring);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFstShpNsSclCor failed %u", Err, 0U);
        }
    }
}

static void SvcIsoCfg_MCTF(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32  Err;

    {
        AMBA_IK_FNLSHPNS_BOTH_s FinalBothInfo;

        AmbaSvcWrap_MisraMemset(&FinalBothInfo, 0, sizeof(FinalBothInfo));

        Err = AmbaIK_SetFnlShpNsBoth(pImgMode, &FinalBothInfo);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsBoth failed %u", Err, 0U);
        }
    }

    {   //  QQQ
        AMBA_IK_FNLSHPNS_BOTH_TDT_s FinalThreedTable;

        AmbaSvcWrap_MisraMemset(&FinalThreedTable, 0, sizeof(FinalThreedTable));

        Err = AmbaIK_SetFnlShpNsBothTdt(pImgMode, &FinalThreedTable);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsBothTdt failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FNLSHPNS_NOISE_s FinalSharpenNoise;

        AmbaSvcWrap_MisraMemset(&FinalSharpenNoise, 0, sizeof(FinalSharpenNoise));

        FinalSharpenNoise.LevelStrAdjust.High = 1U;
        Err = AmbaIK_SetFnlShpNsNoise(pImgMode, &FinalSharpenNoise);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsNoise failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FNLSHPNS_FIR_s FinalFir;

        AmbaSvcWrap_MisraMemset(&FinalFir, 0, sizeof(FinalFir));

        Err = AmbaIK_SetFnlShpNsFir(pImgMode, &FinalFir);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsFir failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FNLSHPNS_CORING_s FinalCoring;

        AmbaSvcWrap_MisraMemset(&FinalCoring, 0, sizeof(FinalCoring));

        FinalCoring.FractionalBits = 1U;
        Err = AmbaIK_SetFnlShpNsCoring(pImgMode, &FinalCoring);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsCoring failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FNLSHPNS_COR_IDX_SCL_s FinalCoringIndexScale;

        AmbaSvcWrap_MisraMemset(&FinalCoringIndexScale, 0, sizeof(FinalCoringIndexScale));

        FinalCoringIndexScale.High = 1U;
        Err = AmbaIK_SetFnlShpNsCorIdxScl(pImgMode, &FinalCoringIndexScale);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsCorIdxScl failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FNLSHPNS_MIN_COR_RST_s FinalMinCoringResult;

        AmbaSvcWrap_MisraMemset(&FinalMinCoringResult, 0, sizeof(FinalMinCoringResult));

        FinalMinCoringResult.High = 1U;
        Err = AmbaIK_SetFnlShpNsMinCorRst(pImgMode, &FinalMinCoringResult);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsMinCorRst failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FNLSHPNS_MAX_COR_RST_s FinalMaxCoringResult;

        AmbaSvcWrap_MisraMemset(&FinalMaxCoringResult, 0, sizeof(FinalMaxCoringResult));

        FinalMaxCoringResult.High = 1U;
        Err = AmbaIK_SetFnlShpNsMaxCorRst(pImgMode, &FinalMaxCoringResult);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsMaxCorRst failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_FNLSHPNS_SCL_COR_s FinalScaleCoring;

        AmbaSvcWrap_MisraMemset(&FinalScaleCoring, 0, sizeof(FinalScaleCoring));

        FinalScaleCoring.High = 1U;
        Err = AmbaIK_SetFnlShpNsSclCor(pImgMode, &FinalScaleCoring);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFnlShpNsSclCor failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_VIDEO_MCTF_s VideoMctf;
        AmbaSvcWrap_MisraMemset(&VideoMctf, 0, sizeof(VideoMctf));
        VideoMctf.Enable = 0U;
        VideoMctf.YSpatSmthIso.High                       = 1U;
        VideoMctf.YSpatSmthDir.High                       = 1U;
        VideoMctf.CbSpatSmthIso.High                      = 1U;
        VideoMctf.CbSpatSmthDir.High                      = 1U;
        VideoMctf.YOverallMaxChange.High                  = 1U;
        VideoMctf.CbOverallMaxChange.High                 = 1U;
        VideoMctf.CrOverallMaxChange.High                 = 1U;
        VideoMctf.YTemporalEitherMaxChangeOrT0T1Add.High  = 1U;
        VideoMctf.CbTemporalEitherMaxChangeOrT0T1Add.High = 1U;
        VideoMctf.CrTemporalEitherMaxChangeOrT0T1Add.High = 1U;

        Err = AmbaIK_SetVideoMctf(pImgMode, &VideoMctf);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetVideoMctf failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_VIDEO_MCTF_TA_s VideoMctfTemporalAdjust;

        AmbaSvcWrap_MisraMemset(&VideoMctfTemporalAdjust, 0, sizeof(VideoMctfTemporalAdjust));
        VideoMctfTemporalAdjust.LevAdjustHigh = 1U;
        Err = AmbaIK_SetVideoMctfTa(pImgMode, &VideoMctfTemporalAdjust);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetVideoMctfTa failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_POS_DSP33_t VideoMctfAndFinalSharpen;

        AmbaSvcWrap_MisraMemset(&VideoMctfAndFinalSharpen, 0, sizeof(VideoMctfAndFinalSharpen));
        Err = AmbaIK_SetVideoMctfAndFnlshp(pImgMode, &VideoMctfAndFinalSharpen);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetVideoMctfAndFnlshp failed %u", Err, 0U);
        }
    }
}

static void SvcIsoCfg_CE(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 Err = SVC_OK;
    {
        AMBA_IK_CE_s CeCtrl = {
            .Enable = 1U,
            .LumaAvgMethod = 0U,
            .LumaAvgWeightR = 32,
            .LumaAvgWeightGr = 32,
            .LumaAvgWeightGb = 32,
            .LumaAvgWeightB = 32,
            .FirEnable = 1U,
            .FirCoeff = {4U, 4U, 32U},
            .CoringIndexScaleShift = 3U,
            .CoringIndexScaleLow = 1U,
            .CoringIndexScaleLowDelta = 1U,
            .CoringIndexScaleLowStrength = 8U,
            .CoringIndexScaleMidStrength = 8U,
            .CoringIndexScaleHigh = 3U,
            .CoringIndexScaleHighDelta = 1U,
            .CoringIndexScaleHighStrength = 8U,

            .CoringGainShift = 5U,
            .CoringGainLow = 1U,
            .CoringGainLowDelta = 1U,
            .CoringGainLowStrength = 1U,
            .CoringGainMidStrength = 1U,
            .CoringGainHigh = 3U,
            .CoringGainHighDelta = 1U,
            .CoringGainHighStrength = 1U,
            .CoringTable = {
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},

            .BoostGainShift = 4U,
            .BoostTableSizeExp = 4U,
            .BoostTable = {
                28, 22, 22, 18, 18, 14, 12, 10, 8, 6, 4, 3, 1, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

            .Radius = 3U,
            .Epsilon = 50U,
        };

        Err = AmbaIK_SetCe(pImgMode, &CeCtrl);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCe failed %u", Err, 0U);
        }
    }

    /* CeInput */
    {
        AMBA_IK_CE_INPUT_TABLE_s CeInput = {
            .InputEnable = 1U,
            .InputTable = {
                0, 6550, 12727, 18569, 24111, 29383, 34410, 39213, 43812, 48223,
                52461, 56539, 60469, 64260, 67924, 71467, 74897, 78222, 81448, 84580,
                87624, 90585, 93467, 96273, 99009, 101677, 104281, 106824, 109308, 111736,
                114111, 116435, 118710, 120938, 123121, 125261, 127359, 129417, 131437, 133419,
                135366, 137279, 139158, 141005, 142821, 144608, 146365, 148093, 149795, 151470,
                153120, 154745, 156346, 157923, 159478, 161011, 162522, 164013, 165483, 166933,
                168364, 169777, 171171, 172548, 173907, 176575, 179179, 181721, 184206, 186634,
                189009, 191333, 193608, 195836, 198019, 200158, 202257, 204315, 206335, 208317,
                210264, 212177, 214056, 215903, 217719, 219505, 221262, 222991, 224693, 226368,
                228018, 229643, 231244, 232821, 234376, 235909, 237420, 240381, 243262, 246069,
                248805, 251473, 254077, 256619, 259103, 261532, 263907, 266230, 268505, 270733,
                272916, 275056, 277154, 279213, 281232, 283215, 285162, 287075, 288954, 290801,
                292617, 294403, 296160, 297889, 299591, 301266, 302916, 304541, 306142, 309274,
                312318, 315278, 318160, 320967, 323702, 326371, 328974, 331517, 334001, 336429,
                338804, 341128, 343403, 345631, 347814, 349954, 352052, 354110, 356130, 358113,
                360060, 361972, 363852, 365699, 367515, 369301, 371058, 372787, 374489, 376164,
                377814, 381039, 384172, 387216, 390176, 393058, 395865, 398600, 401268, 403872,
                406415, 408899, 411327, 413702, 416026, 418301, 420529, 422712, 424852, 426950,
                429008, 431028, 433011, 434958, 436870, 438749, 440597, 442413, 444199, 445956,
                447685, 449386, 451062, 454336, 457515, 460602, 463604, 466524, 469368, 472139,
                474840, 477476, 480049, 482562, 485018, 487419, 489768, 492067, 494319, 496524,
                498685, 500804, 502882, 504921, 506922, 508886, 510816, 512712, 514575, 516406,
                518207, 519979, 521722, 523437, 525125, 528425, 531627, 534736, 537759, 540699,
                543562, 546351, 549069, 551721, 554309, 556837, 559307, 561722, 564084, 566395,
                568658, 570875, 573047, 575176, 577264, 579312, 581323, 583296, 585235, 587139,
                589010, 590849, 592658, 594436, 596186, 597908, 599603, 602916, 606129, 609250,
                612284, 615234, 618106, 620904, 623631, 626291, 628887, 631422, 633899, 636320,
                638689, 641007, 643275, 645498, 647675, 649810, 651903, 653956, 655971, 657949,
                659892, 661800, 663676, 665519, 667331, 669113, 670867, 672592, 674291, 677609,
                680829, 683956, 686994, 689950, 692827, 695629, 698360, 701024, 703624, 706163,
                708644, 711069, 713440, 715761, 718033, 720258, 722438, 724575, 726671, 728727,
                730744, 732725, 734669, 736580, 738457, 740302, 742116, 743901, 745656, 747383,
                749083, 752405, 755628, 758757, 761798, 764756, 767636, 770440, 773174, 775840,
                778442, 780982, 783465, 785891, 788265, 790587, 792860, 795087, 797269, 799407,
                801504, 803561, 805580, 807561, 809507, 811419, 813297, 815143, 816958, 818743,
                820499, 822227, 823928, 827252, 830476, 833607, 836649, 839609, 842489, 845295,
                848029, 850696, 853299, 855841, 858324, 860752, 863126, 865449, 867723, 869950,
                872133, 874272, 876369, 878427, 880446, 882428, 884375, 886287, 888165, 890012,
                891828, 893613, 895370, 897098, 898800, 902124, 905349, 908480, 911524, 914484,
                917365, 920171, 922906, 925574, 928177, 930719, 933203, 935631, 938005, 940329,
                942603, 944831, 947014, 949153, 951251, 953309, 955328, 957311, 959257, 961170,
                963049, 964896, 966711, 968497, 970254, 971983, 973684, 977009, 980234, 983366,
                986410, 989370, 992251, 995058, 997793, 1000461, 1003065, 1005607, 1008091,
                1010519, 1012894, 1015218, 1017492, 1019720, 1021903, 1024043, 1026141, 1028199,
                1030218, 1032201, 1034148, 1036060, 1037939, 1039786, 1041602, 1043388, 1045145,
                1046874, 1046874},
        };

        Err = AmbaIK_SetCeInputTable(pImgMode, &CeInput);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCeInputTable failed %u", Err, 0U);
        }
    }

    /* CeOutput */
    {
        AMBA_IK_CE_OUT_TABLE_s CeOutput = {
            .OutputEnable = 1U,
            .OutputShift = 12U,
            .OutputTable = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
                4, 4, 4, 4, 4, 5, 5, 5, 5, 5,
                6, 6, 6, 6, 7, 7, 7, 8, 8, 8,
                9, 9, 9, 10, 10, 11, 11, 12, 12, 13,
                13, 14, 14, 15, 16, 16, 17, 18, 18, 19,
                20, 21, 21, 22, 23, 24, 25, 26, 27, 28,
                29, 30, 32, 33, 34, 36, 37, 39, 40, 42,
                43, 45, 47, 49, 51, 53, 55, 57, 59, 61,
                64, 66, 69, 71, 74, 77, 80, 83, 87, 90,
                94, 97, 101, 105, 109, 113, 118, 122, 127, 132,
                137, 142, 148, 154, 160, 166, 172, 179, 186, 193,
                201, 209, 217, 225, 234, 243, 252, 262, 272, 283,
                294, 305, 317, 329, 342, 355, 369, 383, 398, 414,
                430, 446, 463, 481, 500, 519, 540, 560, 582, 605,
                628, 652, 678, 704, 731, 759, 789, 819, 851, 884,
                918, 953, 990, 1029, 1068, 1110, 1153, 1197, 1243, 1292,
                1341, 1393, 1447, 1503, 1561, 1622, 1684, 1749, 1817, 1887,
                1960, 2036, 2115, 2196, 2281, 2370, 2461, 2556, 2655, 2758,
                2864, 2975, 3090, 3209, 3333, 3462, 3596, 3735, 3879, 4029,
                4185, 4347, 4515, 4689, 4870, 5058, 5254, 5457, 5668, 5887,
                6114, 6351, 6596, 6851, 7116, 7390, 7676, 7973, 8281, 8601,
                8933, 9278, 9637, 10009, 10396, 10797, 11215, 11648, 12098, 12565,
                13051, 13555, 14079, 14623, 15188, 15775},
        };

        Err = AmbaIK_SetCeOutputTable(pImgMode, &CeOutput);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCeOutputTable failed %u", Err, 0U);
        }
    }
}

static void SvcIsoCfg_HDR(const AMBA_IK_MODE_CFG_s *pImgMode, const AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 Err = SVC_OK;

    if (pAbility->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) {
        {
            AMBA_IK_HDR_BLEND_s HdrBlend = {0};

            HdrBlend.Enable = 1U;
            HdrBlend.DeltaT0[0] = 11U;
            HdrBlend.DeltaT1[0] = 0U;
            HdrBlend.FlickerThreshold = 256U;
            HdrBlend.T0Offset[0] = 75000U;
            HdrBlend.T1Offset[0] = 0U;
            Err = AmbaIK_SetHdrBlend(pImgMode, &HdrBlend);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetHdrBlend failed %u", Err, 0U);
            }
        }

        /* FE Static BLC */
        {
            AMBA_IK_STATIC_BLC_LVL_s Black = {0};

            Black.BlackR = -833;
            Black.BlackGr = -833;
            Black.BlackGb = -833;
            Black.BlackB = -833;
            Black.BlackG = 0;
            Black.BlackIr = 0;
            Err = AmbaIK_SetFeStaticBlc(pImgMode, &Black, 1U);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeStaticBlc failed %u", Err, 0U);
            }
        }

        /* FE WbGain */
        {
            AMBA_IK_FE_WB_GAIN_s WbGain = {0U};

            WbGain.Rgain = (UINT32)1U << 12U;
            WbGain.Ggain = (UINT32)1U << 12U;
            WbGain.Bgain = (UINT32)1U << 12U;
            WbGain.IrGain = (UINT32)1U << 12U;
            WbGain.ShutterRatio = 1U;
            Err = AmbaIK_SetFeWbGain(pImgMode, &WbGain, 1U);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain failed %u", Err, 0U);
            }
        }
    }

    if (pAbility->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
        /* Hdr blending */
        {
            AMBA_IK_HDR_BLEND_s HdrBlend = {0};

            HdrBlend.Enable = 1U;
            HdrBlend.DeltaT0[0] = 11U;
            HdrBlend.DeltaT1[0] = 11U;
            HdrBlend.FlickerThreshold = 256U;
            HdrBlend.T0Offset[0] = 75000U;
            HdrBlend.T1Offset[0] = 750000U;
            Err = AmbaIK_SetHdrBlend(pImgMode, &HdrBlend);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain failed %u", Err, 0U);
            }
        }

        /* FE Static BLC */
        {
            AMBA_IK_STATIC_BLC_LVL_s Black = {0};

            Black.BlackR = -833;
            Black.BlackGr = -833;
            Black.BlackGb = -833;
            Black.BlackB = -833;
            Black.BlackG = 0;
            Black.BlackIr = 0;
            Err = AmbaIK_SetFeStaticBlc(pImgMode, &Black, 2U);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain failed %u", Err, 0U);
            }
        }

        /* FE WbGain */
        {
            AMBA_IK_FE_WB_GAIN_s WbGain = {0U};

            WbGain.Rgain = (UINT32)1U << 12U;
            WbGain.Ggain = (UINT32)1U << 12U;
            WbGain.Bgain = (UINT32)1U << 12U;
            WbGain.IrGain = (UINT32)1U << 12U;
            WbGain.ShutterRatio = 1U;
            Err = AmbaIK_SetFeWbGain(pImgMode, &WbGain, 2U);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain failed %u", Err, 0U);
            }
        }

    }
}

static void SvcIsoCfg_AaaStat(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 Err;
    AMBA_IK_AWB_STAT_INFO_s AwbStat = {0};
    AMBA_IK_AE_STAT_INFO_s AeStat = {0};
    AMBA_IK_AF_STAT_INFO_s AfStat = {0};

    AwbStat.AwbTileNumCol = AMBA_IK_3A_AWB_TILE_COL_COUNT;
    AwbStat.AwbTileNumRow = AMBA_IK_3A_AWB_TILE_ROW_COUNT;
    AwbStat.AwbTileColStart = 0U;
    AwbStat.AwbTileRowStart = 0U;
    AwbStat.AwbTileWidth = 4096U/AwbStat.AwbTileNumCol;
    AwbStat.AwbTileHeight = 4096U/AwbStat.AwbTileNumRow;
    AwbStat.AwbTileActiveWidth = AwbStat.AwbTileWidth;
    AwbStat.AwbTileActiveHeight = AwbStat.AwbTileHeight;
    AwbStat.AwbPixMinValue = 0U;
    AwbStat.AwbPixMaxValue = 16383U;
    Err = AmbaIK_SetAwbStatInfo(pImgMode, &AwbStat);
    if (Err != IK_OK) {
        SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAwbStatInfo failed %u", Err, 0U);
    }

    AeStat.AeTileNumCol = AMBA_IK_3A_AE_TILE_COL_COUNT;
    AeStat.AeTileNumRow = AMBA_IK_3A_AE_TILE_ROW_COUNT;
    AeStat.AeTileColStart = 0U;
    AeStat.AeTileRowStart = 0U;
    AeStat.AeTileWidth = 4096U/AeStat.AeTileNumCol;
    AeStat.AeTileHeight = 4096U/AeStat.AeTileNumRow;
    AeStat.AePixMinValue = 0U;
    AeStat.AePixMaxValue = 16383U;
    Err = AmbaIK_SetAeStatInfo(pImgMode, &AeStat);
    if (Err != IK_OK) {
        SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAeStatInfo failed %u", Err, 0U);
    }

    AfStat.AfTileNumCol = AMBA_IK_3A_CFA_AF_TILE_COL_COUNT;
    AfStat.AfTileNumRow = AMBA_IK_3A_AF_TILE_ROW_COUNT;
    AfStat.AfTileColStart = 0U;
    AfStat.AfTileRowStart = 0U;
    AfStat.AfTileWidth = 4096U/AfStat.AfTileNumCol;
    AfStat.AfTileHeight = 4096U/AfStat.AfTileNumRow;
    AfStat.AfTileActiveWidth = AfStat.AfTileWidth;
    AfStat.AfTileActiveHeight = AfStat.AfTileHeight;
    Err = AmbaIK_SetAfStatInfo(pImgMode, &AfStat);
    if (Err != IK_OK) {
        SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetAfStatInfo failed %u", Err, 0U);
    }

}

static UINT32 GenWarpForCrop(const AMBA_IK_WINDOW_DIMENSION_s *pFullView,
                             const AMBA_IK_WINDOW_GEOMETRY_s *pCropViewQ4,
                             AMBA_IK_WARP_INFO_s *pWarpInfo,
                             AMBA_IK_GRID_POINT_s *pWarp,
                             UINT8 UnityWarp)
{
#define WARP_GRID_WIDTH  (128U)
#define WARP_GRID_HEIGHT (96U)
    UINT32 Return = SVC_OK;
    UINT32 HorGridNum;     /* Horizontal grid number. */
    UINT32 VerGridNum;     /* Vertical grid number. */
    UINT32 TileWidthExp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    UINT32 TileHeightExp;  /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    UINT32 MaxHorGrid = WARP_GRID_WIDTH, MaxVerGrid = WARP_GRID_HEIGHT;
    UINT32 i, j, SFT=4U;

    TileWidthExp = 4U;
    while (((MaxHorGrid - 1U) << TileWidthExp) < pFullView->Width) {
        TileWidthExp++;
    }
    HorGridNum = 1U + ((pFullView->Width + ((UINT32)1U<<TileWidthExp) - 1U) >> TileWidthExp);

    TileHeightExp = 4U;
    while (((MaxVerGrid - 1U) << TileHeightExp) < pFullView->Height) {
        TileHeightExp++;
    }
    VerGridNum = 1U + ((pFullView->Height + ((UINT32)1U<<TileHeightExp) - 1U) >> TileHeightExp);

    for (j=0U; j<VerGridNum; j++) {
        for (i=0U; i<HorGridNum; i++) {
            if (UnityWarp == 1U) {
                pWarp[(j*HorGridNum)+i].X = 0;
                pWarp[(j*HorGridNum)+i].Y = 0;
            } else {
                UINT32 MisraU32_1, MisraU32_2;
                INT32 MisraI32;
                MisraU32_1 = pCropViewQ4->StartX;
                MisraU32_2 = ((((pFullView->Width<<SFT) - pCropViewQ4->Width) * i)<<(TileWidthExp))/pFullView->Width;
                MisraI32 = (INT32)MisraU32_1 - (INT32)MisraU32_2;
                pWarp[(j*HorGridNum)+i].X = (INT16)MisraI32;
                MisraU32_1 = pCropViewQ4->StartY;
                MisraU32_2 = ((((pFullView->Height<<SFT) - pCropViewQ4->Height) * j)<<(TileHeightExp))/pFullView->Height;
                MisraI32 = (INT32)MisraU32_1 - (INT32)MisraU32_2;
                pWarp[(j*HorGridNum)+i].Y = (INT16)MisraI32;
            }
        }
    }

    pWarpInfo->Version                           = 0x20180401;
    pWarpInfo->HorGridNum                        = HorGridNum;
    pWarpInfo->VerGridNum                        = VerGridNum;
    pWarpInfo->TileWidthExp                      = TileWidthExp;
    pWarpInfo->TileHeightExp                     = TileHeightExp;
    pWarpInfo->VinSensorGeo.StartX               = 0U;
    pWarpInfo->VinSensorGeo.StartY               = 0U;
    pWarpInfo->VinSensorGeo.Width                = pFullView->Width;
    pWarpInfo->VinSensorGeo.Height               = pFullView->Height;
    pWarpInfo->VinSensorGeo.HSubSample.FactorNum = 1U;
    pWarpInfo->VinSensorGeo.HSubSample.FactorDen = 1U;
    pWarpInfo->VinSensorGeo.VSubSample.FactorNum = 1U;
    pWarpInfo->VinSensorGeo.VSubSample.FactorDen = 1U;
    pWarpInfo->Enb_2StageCompensation            = 0U;
    pWarpInfo->pWarp                             = pWarp;

    return Return;
}

static void SvcIsoCfg_Duplex(const AMBA_IK_MODE_CFG_s *pImgMode, UINT32 YuvMode)
{
    UINT32  Err;

    {
        AMBA_IK_WINDOW_SIZE_INFO_s  WindowSizeInfo;
        const SVC_RES_CFG_s         *pResCfg = SvcResCfg_Get();

        AmbaSvcWrap_MisraMemset(&WindowSizeInfo, 0, sizeof(WindowSizeInfo));
        WindowSizeInfo.VinSensor.StartX = pResCfg->FovCfg[pImgMode->ContextId].RawWin.OffsetX;
        WindowSizeInfo.VinSensor.StartY = pResCfg->FovCfg[pImgMode->ContextId].RawWin.OffsetY;
        WindowSizeInfo.VinSensor.Width  = pResCfg->FovCfg[pImgMode->ContextId].RawWin.Width;
        WindowSizeInfo.VinSensor.Height = pResCfg->FovCfg[pImgMode->ContextId].RawWin.Height;
        WindowSizeInfo.VinSensor.HSubSample.FactorDen = 1U;
        WindowSizeInfo.VinSensor.HSubSample.FactorNum = 1U;
        WindowSizeInfo.VinSensor.VSubSample.FactorDen = 1U;
        WindowSizeInfo.VinSensor.VSubSample.FactorNum = 1U;
        WindowSizeInfo.Main.Width       = pResCfg->FovCfg[pImgMode->ContextId].MainWin.Width;
        WindowSizeInfo.Main.Height      = pResCfg->FovCfg[pImgMode->ContextId].MainWin.Height;
        Err = AmbaIK_SetWindowSizeInfo(pImgMode, &WindowSizeInfo);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetWindowSizeInfo failed %u", Err, 0U);
        }
    }

    {
        static AMBA_IK_GRID_POINT_s   GridPoint[WARP_GRID_WIDTH * WARP_GRID_HEIGHT] GNU_SECTION_NOZEROINIT;
        AMBA_IK_WINDOW_DIMENSION_s    FullView;
        AMBA_IK_WARP_INFO_s           WarpInfo;
        const SVC_RES_CFG_s           *pResCfg = SvcResCfg_Get();

        AmbaSvcWrap_MisraMemset(&GridPoint, 0, sizeof(GridPoint));
        AmbaSvcWrap_MisraMemset(&FullView, 0, sizeof(FullView));
        AmbaSvcWrap_MisraMemset(&WarpInfo, 0, sizeof(WarpInfo));

        Err = AmbaIK_SetWarpEnb(pImgMode, 1U);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetWarpEnb failed %u", Err, 0U);
        }

        FullView.Width  = pResCfg->FovCfg[pImgMode->ContextId].RawWin.Width;
        FullView.Height = pResCfg->FovCfg[pImgMode->ContextId].RawWin.Height;

        Err = GenWarpForCrop(&FullView, NULL, &WarpInfo, GridPoint, 1U);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "GenWarpForCrop failed %u", Err, 0U);
        }
        Err = AmbaIK_SetWarpInfo(pImgMode, &WarpInfo);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetWarpInfo failed %u", Err, 0U);
        }
    }

    {
        Err = AmbaIK_SetYuvMode(pImgMode, YuvMode);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetYuvMode failed %u", Err, 0U);
        }
    }

    {
        AMBA_IK_STITCH_INFO_s  StitchInfo;
        const SVC_RES_CFG_s    *pResCfg = SvcResCfg_Get();

        AmbaSvcWrap_MisraMemset(&StitchInfo, 0, sizeof(StitchInfo));

        if (pResCfg->FovCfg[pImgMode->ContextId].RawWin.Width > 1920U) {
            StitchInfo.Enable = 1U;
            StitchInfo.TileNum_x = ((UINT32)pResCfg->FovCfg[pImgMode->ContextId].RawWin.Width + 1919U)/1920U;
        } else {
            StitchInfo.Enable = 0U;
            StitchInfo.TileNum_x = 1U;
        }
        StitchInfo.TileNum_y = 1U;
        Err = AmbaIK_SetStitchingInfo(pImgMode, &StitchInfo);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetStitchingInfo failed %u", Err, 0U);
        }
    }

    {
        const SVC_RES_CFG_s  *pCfg         = SvcResCfg_Get();
        UINT32               RawWidth      = pCfg->FovCfg[pImgMode->ContextId].RawWin.Width;
        UINT32               RawHeight     = pCfg->FovCfg[pImgMode->ContextId].RawWin.Height;
        INT32                ActiveOffsetX = (INT32) pCfg->FovCfg[pImgMode->ContextId].ActWin.OffsetX;
        INT32                ActiveOffsetY = (INT32) pCfg->FovCfg[pImgMode->ContextId].ActWin.OffsetY;
        UINT32               ActiveWidth   = pCfg->FovCfg[pImgMode->ContextId].ActWin.Width;
        UINT32               ActiveHeight  = pCfg->FovCfg[pImgMode->ContextId].ActWin.Height;
        AMBA_IK_DZOOM_INFO_s Dzoom         = {0};

        /* IK dzoom control, ShiftX and ShiftY 0 will be the center of the preview after Dzoom */
        if ((ActiveWidth == 0U) || (ActiveHeight == 0U)) {
            Dzoom.Enable = 0U;
            Dzoom.ShiftX = 0;
            Dzoom.ShiftY = 0;
            Dzoom.ZoomX  = 65536U;
            Dzoom.ZoomY  = 65536U;
        } else {
            Dzoom.Enable = 1U;
            Dzoom.ShiftX = (INT32) ((ActiveOffsetX - (((INT32) RawWidth - (INT32) ActiveWidth) / 2)) * 65536);
            Dzoom.ShiftY = (INT32) ((ActiveOffsetY - (((INT32) RawHeight - (INT32) ActiveHeight) / 2)) * 65536);
            Dzoom.ZoomX  = (UINT32) (65536U * RawWidth / ActiveWidth);
            Dzoom.ZoomY  = (UINT32) (65536U * RawHeight / ActiveHeight);
        }
        Err = AmbaIK_SetDzoomInfo(pImgMode, &Dzoom);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetDzoomInfo failed %u", Err, 0U);
        }
    }

    {
        UINT64                           AttachedRawSeq = 0ULL;
        AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s  IsoCfgCtrl;
        AMBA_IK_EXECUTE_CONTAINER_s      CfgExecInfo;

        AmbaSvcWrap_MisraMemset(&IsoCfgCtrl, 0, sizeof(IsoCfgCtrl));
        AmbaSvcWrap_MisraMemset(&CfgExecInfo, 0, sizeof(CfgExecInfo));
        (void)AmbaIK_ExecuteConfig(pImgMode, &CfgExecInfo);

        IsoCfgCtrl.ViewZoneId = (UINT16)pImgMode->ContextId;
        IsoCfgCtrl.CfgIndex   = CfgExecInfo.IkId;
        IsoCfgCtrl.CtxIndex   = pImgMode->ContextId;
        AmbaMisra_TypeCast(&IsoCfgCtrl.CfgAddress, &CfgExecInfo.pIsoCfg);
        (void)AmbaDSP_LiveviewUpdateIsoCfg(1, &IsoCfgCtrl, &AttachedRawSeq);
    }
}

/**
* iso configuration cc table loading function
* @param [in] BaseThreeD address of cc 3d buffer
* @param [in] SizeThreeD size of cc 3d buffer
* @param [in] BaseReg address of cc reg buffer
* @param [in] SizeReg size of cc reg buffer
* @return none
*/
void SvcIsoCfg_CcLoad(ULONG BaseThreeD, UINT32 SizeThreeD, ULONG BaseReg, UINT32 SizeReg)
{
    UINT32 RetVal;
    UINT8 *pThreeDTable;
    UINT32 Cc3dBinSize = 0U;
    UINT32 *pAddrU32 = NULL;

    AmbaMisra_TouchUnused(&BaseReg);
    AmbaMisra_TouchUnused(&SizeReg);

    SvcIsoCfg_DTLInit();

    AmbaSvcWrap_MisraMemset(&SvcIsoCfgCcThreeD, 0, sizeof(SvcIsoCfgCcThreeD));
    SvcIsoCfgCcThreeD.MatrixEn = 1U;
    SvcIsoCfgCcThreeD.MatrixShiftMinus8 = 2U;
    SvcIsoCfgCcThreeD.Matrix[0] = 1050;
    SvcIsoCfgCcThreeD.Matrix[1] = -20;
    SvcIsoCfgCcThreeD.Matrix[2] = -40;
    SvcIsoCfgCcThreeD.Matrix[3] = 1100;
    SvcIsoCfgCcThreeD.Matrix[4] = 30;
    SvcIsoCfgCcThreeD.Matrix[5] = 1080;

    RetVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, "capture1_CC_3d.bin", &Cc3dBinSize);
    if (RetVal != 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "Get CC 3d table size fail! ErrCode(0x%08x)", RetVal, 0U);
    } else if (Cc3dBinSize == 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "CC 3d table file size should not zero!", 0U, 0U);
    } else if (IK_CC_3D_SIZE > Cc3dBinSize) {
        SvcLog_NG(SVC_LOG_ISOC, "Request CC 3D size(0x%x) > file size(0x%x)", IK_CC_3D_SIZE, Cc3dBinSize);
    } else {

        AmbaMisra_TypeCast(&(pThreeDTable), &(BaseThreeD));

        RetVal = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA,
                                       "capture1_CC_3d.bin",
                                       0U,
                                       IK_CC_3D_SIZE,
                                       pThreeDTable,
                                       5000U);
        if (RetVal != 0U) {
            SvcLog_NG(SVC_LOG_ISOC, "Read CC 3D table fail! ErrCode(0x%08x)", RetVal, 0U);
        } else {
            AmbaMisra_TypeCast(&(pAddrU32), &(BaseThreeD));
            AmbaSvcWrap_MisraMemcpy(SvcIsoCfgCcThreeD.MatrixThreeDTable, pAddrU32, SizeThreeD);
            SvcLog_DBG(SVC_LOG_ISOC, "Read CC 3D table done", RetVal, 0U);
        }
    }

    AmbaMisra_TouchUnused(pAddrU32);
    AmbaMisra_TouchUnused(&RetVal);

    RetVal = AmbaKAL_EventFlagSet(&SvcIsoCfgDTLFlg, SVC_ISO_CFG_DTL_LOAD_DONE);
    if (RetVal != 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "Set CC table load flag fail! ErrCode(0x%08x)", RetVal, 0U);
    }
}

/**
* first iso configuration of image kernel
* @param [in] pImgMode block of image kernel context
* @param [in] ForStill 0-video mode 1-still mode
* @return none
*/
void SvcIsoCfg_Fixed(const AMBA_IK_MODE_CFG_s *pImgMode, UINT8 ForStill)
{
    UINT32             Rval, VinSrc, Err;
    AMBA_IK_ABILITY_s  Ability = {0};

    AmbaMisra_TouchUnused(&ForStill);

     if (0U < SvcDSP_IsCleanBoot()) {
        Rval = AmbaIK_GetContextAbility(pImgMode, &Ability);
        if (Rval == OK) {

            SvcIsoCfg_AaaStat(pImgMode);

            if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
                if (3U == ForStill) {
                    /* clean Raw */
                    SvcIsoCfg_CleanRaw(pImgMode);
                } else {
                    /* R2R */
                    SvcIsoCfg_R2R(pImgMode);
                }
            }

            /* Y2Y */
            SvcIosCfg_Y2Y(pImgMode, &Ability);

            /* MCTF */
            if ( AMBA_IK_STILL_HISO != Ability.StillPipe) { /* not for high iso */
                SvcIsoCfg_MCTF(pImgMode);
            }

            if ((Ability.VideoPipe == AMBA_IK_VIDEO_LINEAR_CE) ||
                (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
                SvcIsoCfg_CE(pImgMode);
                SvcIsoCfg_HDR(pImgMode, &Ability);
            }

            {
                /* set warp buffer info for sensor/yuv input */
                AMBA_IK_WARP_BUFFER_INFO_s  WarpBufInfo;
                const SVC_RES_CFG_s         *pCfg = SvcResCfg_Get();

                if (pCfg->FovCfg[pImgMode->ContextId].PipeCfg.MaxWarpDma == 0U) {
                    WarpBufInfo.LumaDmaSize = 24U;
                } else {
                    WarpBufInfo.LumaDmaSize = pCfg->FovCfg[pImgMode->ContextId].PipeCfg.MaxWarpDma;
                }
                if (pCfg->FovCfg[pImgMode->ContextId].PipeCfg.PipeMode == SVC_VDOPIPE_DRAMEFCY) {
                    WarpBufInfo.DramEfficiency = 2U;
                    WarpBufInfo.LumaWaitLines  = pCfg->FovCfg[pImgMode->ContextId].PipeCfg.WarpLumaWaitLine;
                } else {
                    WarpBufInfo.DramEfficiency = 0U;
                    WarpBufInfo.LumaWaitLines  = 8U;
                }

                if (AmbaIK_SetWarpBufferInfo(pImgMode, &WarpBufInfo) != SVC_OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set warp_buf_info", 0U, 0U);
                }
            }

            /* for Duplex mode */
            Err = SvcResCfg_GetFovSrc(pImgMode->ContextId, &VinSrc);
            if ((ForStill == 0U) && (Err == SVC_OK) &&
                ((VinSrc == SVC_VIN_SRC_MEM_DEC) || (VinSrc == SVC_VIN_SRC_MEM_YUV420) || (VinSrc == SVC_VIN_SRC_MEM_YUV422))) {

                UINT32 YuvMode = AMBA_IK_YUV420;

                if (VinSrc == SVC_VIN_SRC_MEM_YUV422) {
                    YuvMode = AMBA_IK_YUV422;
                }

                SvcIsoCfg_Duplex(pImgMode, YuvMode);
            }
        } else {
            SvcLog_NG(SVC_LOG_ISOC, "Fail to GetContextAbility(...)", Rval, 0U);
        }
    }
}

/**
 * iso configuration query cc 3d table size
 * @param [out] the working buffer size
 * return 0-OK, 1-NG
 */
UINT32 SvcIsoCfg_CcThreeDMemQuery(UINT32 *pSize)
{
    *pSize = IK_CC_3D_SIZE;
    return SVC_OK;
}

/**
 * iso configuration query cc reg table size
 * @param [out] the working buffer size
 * return 0-OK, 1-NG
 */
UINT32 SvcIsoCfg_CcRegMemQuery(UINT32 *pSize)
{
    *pSize = 64U; // dummy size
    return SVC_OK;
}
