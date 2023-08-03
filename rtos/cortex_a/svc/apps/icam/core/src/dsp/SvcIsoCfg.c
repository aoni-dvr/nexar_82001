/**
*  @file SvcIsoCfg.c
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
#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcNvm.h"
#include "SvcWrap.h"
#include "SvcIsoCfg.h"
#include "SvcVinSrc.h"
#include "SvcResCfg.h"

#define SVC_LOG_ISOC     "ISOC"

#define SVC_ISO_CFG_DTL_LOAD_DONE   (0x1U)
static AMBA_KAL_EVENT_FLAG_t SvcIsoCfgDTLFlg GNU_SECTION_NOZEROINIT;
static AMBA_IK_COLOR_CORRECTION_s SvcIsoCfgCcThreeD GNU_SECTION_NOZEROINIT;
static AMBA_IK_COLOR_CORRECTION_REG_s SvcIsoCfgCcReg GNU_SECTION_NOZEROINIT;

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


static void FixedIsoConfigHdr(const AMBA_IK_MODE_CFG_s *pImgMode, UINT32 VideoPipe)
{
    UINT32 Rval;
    /* LCE relatvie */
    {
        /* CeCtrl, from vid22 */
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

            Rval = AmbaIK_SetCe(pImgMode, &CeCtrl);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCe fail, rval(%u)", Rval, 0U);
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

            Rval = AmbaIK_SetCeInputTable(pImgMode, &CeInput);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCeInputTable fail, rval(%u)", Rval, 0U);
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

            Rval = AmbaIK_SetCeOutputTable(pImgMode, &CeOutput);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetCeInputTable fail, rval(%u)", Rval, 0U);
            }
        }
    }

    /* DOL-HDR relatvie */
    if (VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) {
        /* Hdr blending, from Vid15 */
        {
            AMBA_IK_HDR_BLEND_s HdrBlend;
            AmbaSvcWrap_MisraMemset(&HdrBlend, 0, sizeof(HdrBlend));
            HdrBlend.Enable = 1U;
            HdrBlend.DeltaT0 = 11U;
            HdrBlend.DeltaT1 = 0U;
            HdrBlend.FlickerThreshold = 256U;
            HdrBlend.T0Offset = 75000U;
            HdrBlend.T1Offset = 0U;
            Rval = AmbaIK_SetHdrBlend(pImgMode, &HdrBlend);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetHdrBlend fail, rval(%u)", Rval, 0U);
            }
        }

        /* FE Static BLC */
        {
            AMBA_IK_STATIC_BLC_LVL_s Black;
            AmbaSvcWrap_MisraMemset(&Black, 0, sizeof(Black));
            Black.BlackR = -833;
            Black.BlackGr = -833;
            Black.BlackGb = -833;
            Black.BlackB = -833;
            Rval = AmbaIK_SetFeStaticBlc(pImgMode, &Black, 1U);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeStaticBlc fail, rval(%u)", Rval, 0U);
            }
        }

        /* FE WbGain */
        {
            AMBA_IK_FE_WB_GAIN_s WbGain;
            AmbaSvcWrap_MisraMemset(&WbGain, 0, sizeof(WbGain));
            WbGain.Rgain = (UINT32)1U << 12U;
            WbGain.Ggain = (UINT32)1U << 12U;
            WbGain.Bgain = (UINT32)1U << 12U;
            WbGain.ShutterRatio = 1U;
            Rval = AmbaIK_SetFeWbGain(pImgMode, &WbGain, 1U);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain fail, rval(%u)", Rval, 0U);
            }
        }
    }

    if (VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
        /* Hdr blending */
        {
            AMBA_IK_HDR_BLEND_s HdrBlend;
            AmbaSvcWrap_MisraMemset(&HdrBlend, 0, sizeof(HdrBlend));
            HdrBlend.Enable = 1U;
            HdrBlend.DeltaT0 = 11U;
            HdrBlend.DeltaT1 = 11U;
            HdrBlend.FlickerThreshold = 256U;
            HdrBlend.T0Offset = 75000U;
            HdrBlend.T1Offset = 750000U;
            Rval = AmbaIK_SetHdrBlend(pImgMode, &HdrBlend);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetHdrBlend fail, rval(%u)", Rval, 0U);
            }
        }

        /* FE Static BLC */
        {
            AMBA_IK_STATIC_BLC_LVL_s Black;
            AmbaSvcWrap_MisraMemset(&Black, 0, sizeof(Black));
            Black.BlackR = -833;
            Black.BlackGr = -833;
            Black.BlackGb = -833;
            Black.BlackB = -833;
            Rval = AmbaIK_SetFeStaticBlc(pImgMode, &Black, 2U);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeStaticBlc fail, rval(%u)", Rval, 0U);
            }
        }

        /* FE WbGain */
        {
            AMBA_IK_FE_WB_GAIN_s WbGain;
            AmbaSvcWrap_MisraMemset(&WbGain, 0, sizeof(WbGain));
            WbGain.Rgain = (UINT32)1U << 12U;
            WbGain.Ggain = (UINT32)1U << 12U;
            WbGain.Bgain = (UINT32)1U << 12U;
            WbGain.ShutterRatio = 1U;
            Rval = AmbaIK_SetFeWbGain(pImgMode, &WbGain, 2U);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ISOC, "AmbaIK_SetFeWbGain fail, rval(%u)", Rval, 0U);
            }
        }
    }
}

static void FixedIsoConfigExceptY2y(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32  Rval;

    /* static-blc */
    {
        AMBA_IK_STATIC_BLC_LVL_s Black;
        AmbaSvcWrap_MisraMemset(&Black, 0, sizeof(Black));
        Black.BlackR  = -833;
        Black.BlackGr = -833;
        Black.BlackGb = -833;
        Black.BlackB  = -833;
        Rval = AmbaIK_SetFeStaticBlc(pImgMode, &Black, 0U);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set static-blc, rval(%u)", Rval, 0U);
        }
    }

    /* wb-gain */
    {
        AMBA_IK_FE_WB_GAIN_s  WbGain;
        AmbaSvcWrap_MisraMemset(&WbGain, 0, sizeof(WbGain));
        WbGain.Rgain        = 0x1000U;
        WbGain.Ggain        = 0x1000U;
        WbGain.Bgain        = 0x1000U;
        WbGain.ShutterRatio = 1U;
        Rval = AmbaIK_SetFeWbGain(pImgMode, &WbGain, 0U);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set wb-gain, rval(%u)", Rval, 0U);
        }
    }

    /* bpc */
    {
        AMBA_IK_DYNAMIC_BAD_PXL_COR_s  Bpc;
        AmbaSvcWrap_MisraMemset(&Bpc, 0, sizeof(Bpc));
        Bpc.Enable            = 3U;
        Bpc.HotPixelStrength  = 9U;
        Bpc.DarkPixelStrength = 8U;
        Bpc.CorrectionMethod  = 0U;
        Rval = AmbaIK_SetDynamicBadPxlCor(pImgMode, &Bpc);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set bpc, rval(%u)", Rval, 0U);
        }
    }

    /* cfa-leakage */
    {
        AMBA_IK_CFA_LEAKAGE_FILTER_s  Leakage;
        AmbaSvcWrap_MisraMemset(&Leakage, 0, sizeof(Leakage));
        Rval = AmbaIK_SetCfaLeakageFilter(pImgMode, &Leakage);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set cfa-leakage, rval(%u)", Rval, 0U);
        }
    }

    /* anit-aliasing */
    {
        AMBA_IK_ANTI_ALIASING_s  AntiAliase;
        AmbaSvcWrap_MisraMemset(&AntiAliase, 0, sizeof(AntiAliase));
        AntiAliase.Enable               = 1;
        AntiAliase.Thresh               = 0;
        AntiAliase.LogFractionalCorrect = 0;
        Rval = AmbaIK_SetAntiAliasing(pImgMode, &AntiAliase);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set anit-aliasing, rval(%u)", Rval, 0U);
        }
    }

    /* grgb-mismatch */
    {
        AMBA_IK_GRGB_MISMATCH_s  GrGb;
        AmbaSvcWrap_MisraMemset(&GrGb, 0, sizeof(GrGb));
        GrGb.NarrowEnable = 1;
        GrGb.WideEnable   = 0;
        GrGb.WideSafety   = 64;
        GrGb.WideThresh   = 64;
        Rval = AmbaIK_SetGrgbMismatch(pImgMode, &GrGb);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set grgb-mismatch, rval(%u)", Rval, 0U);
        }
    }

    /* bce-wbgain */
    {
        AMBA_IK_WB_GAIN_s  WbGain;
        AmbaSvcWrap_MisraMemset(&WbGain, 0, sizeof(WbGain));
        WbGain.GainR = (UINT32)1U << 12U;
        WbGain.GainG = (UINT32)1U << 12U;
        WbGain.GainB = (UINT32)1U << 12U;
        Rval = AmbaIK_SetBeforeCeWbGain(pImgMode, &WbGain);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set bce-wbgain, rval(%u)", Rval, 0U);
        }
    }

    /* ace-wbgain */
    {
        AMBA_IK_WB_GAIN_s  WbGain;
        AmbaSvcWrap_MisraMemset(&WbGain, 0, sizeof(WbGain));
        WbGain.GainR = 5989;
        WbGain.GainG = 4089;
        WbGain.GainB = 6325;
        Rval = AmbaIK_SetAfterCeWbGain(pImgMode, &WbGain);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set ace-wbgain, rval(%u)", Rval, 0U);
        }
    }

    /* cfa-noise */
    {
        AMBA_IK_CFA_NOISE_FILTER_s  CfaNoise;
        AmbaSvcWrap_MisraMemset(&CfaNoise, 0, sizeof(CfaNoise));
        CfaNoise.Enable              = 1;
        CfaNoise.ExtentFine[0]       = 7;
        CfaNoise.ExtentFine[1]       = 7;
        CfaNoise.ExtentFine[2]       = 7;
        CfaNoise.ExtentRegular[0]    = 68;
        CfaNoise.ExtentRegular[1]    = 68;
        CfaNoise.ExtentRegular[2]    = 68;
        CfaNoise.NoiseLevel[0]       = 100;
        CfaNoise.NoiseLevel[1]       = 100;
        CfaNoise.NoiseLevel[2]       = 100;
        CfaNoise.OriginalBlendStr[0] = 6;
        CfaNoise.OriginalBlendStr[1] = 6;
        CfaNoise.OriginalBlendStr[2] = 6;
        CfaNoise.StrengthFine[0]     = 73;
        CfaNoise.StrengthFine[1]     = 73;
        CfaNoise.StrengthFine[2]     = 73;
        Rval = AmbaIK_SetCfaNoiseFilter(pImgMode, &CfaNoise);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set cfa-noise, rval(%u)", Rval, 0U);
        }
    }

    /* demosaic */
    {
        AMBA_IK_DEMOSAIC_s  Demosaic;
        AmbaSvcWrap_MisraMemset(&Demosaic, 0, sizeof(Demosaic));
        Demosaic.Enable                   = 1U;
        Demosaic.ActivityDifferenceThresh = 1365;
        Demosaic.ActivityThresh           = 3;
        Demosaic.GradClipThresh           = 512;
        Demosaic.GradNoiseThresh          = 160;
        Rval = AmbaIK_SetDemosaic(pImgMode, &Demosaic);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set demosaic, rval(%u)", Rval, 0U);
        }
    }

    /* CC */
    {
        SvcIsoCfg_DTLWaitDone();

        /* CC-reg */
        {
            Rval = AmbaIK_SetColorCorrectionReg(pImgMode, &SvcIsoCfgCcReg);
            if (Rval == OK) {
                SvcLog_OK(SVC_LOG_ISOC, "set CC-reg, IkCtxId(%d)", pImgMode->ContextId, 0U);
            } else {
                SvcLog_NG(SVC_LOG_ISOC, "## fail to set CC-reg, rval(%u)", Rval, 0U);
            }
        }

        /* CC-3d */
        {
            Rval = AmbaIK_SetColorCorrection(pImgMode, &SvcIsoCfgCcThreeD);
            if (Rval == OK) {
                SvcLog_OK(SVC_LOG_ISOC, "set CC-3d, IkCtxId(%d)", pImgMode->ContextId, 0U);
            } else {
                SvcLog_NG(SVC_LOG_ISOC, "## fail to set CC-3d, rval(%u)", Rval, 0U);
            }
        }
    }

    /* tone-curve */
    {
        AMBA_IK_TONE_CURVE_s  Tone = {
            .ToneCurveRed = {
                0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76,
                80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,
                148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209,
                213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253, 257, 261, 265, 269, 273,
                277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317, 321, 325, 329, 333, 337,
                341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381, 385, 389, 393, 397, 401,
                405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445, 449, 453, 457, 461, 465,
                469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509, 514, 518, 522, 526, 530,
                534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574, 578, 582, 586, 590, 594,
                598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638, 642, 646, 650, 654, 658,
                662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702, 706, 710, 714, 718, 722,
                726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766, 770, 774, 778, 782, 786,
                790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830, 834, 838, 842, 846, 850,
                855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895, 899, 903, 907, 911, 915,
                919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959, 963, 967, 971, 975, 979,
                983, 987, 991, 995, 999, 1003, 1007, 1011, 1015, 1019, 1023,
            },
            .ToneCurveGreen = {
                0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76,
                80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,
                148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209,
                213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253, 257, 261, 265, 269, 273,
                277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317, 321, 325, 329, 333, 337,
                341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381, 385, 389, 393, 397, 401,
                405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445, 449, 453, 457, 461, 465,
                469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509, 514, 518, 522, 526, 530,
                534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574, 578, 582, 586, 590, 594,
                598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638, 642, 646, 650, 654, 658,
                662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702, 706, 710, 714, 718, 722,
                726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766, 770, 774, 778, 782, 786,
                790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830, 834, 838, 842, 846, 850,
                855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895, 899, 903, 907, 911, 915,
                919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959, 963, 967, 971, 975, 979,
                983, 987, 991, 995, 999, 1003, 1007, 1011, 1015, 1019, 1023,
            },
            .ToneCurveBlue = {
                0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76,
                80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,
                148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209,
                213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253, 257, 261, 265, 269, 273,
                277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317, 321, 325, 329, 333, 337,
                341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381, 385, 389, 393, 397, 401,
                405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445, 449, 453, 457, 461, 465,
                469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509, 514, 518, 522, 526, 530,
                534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574, 578, 582, 586, 590, 594,
                598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638, 642, 646, 650, 654, 658,
                662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702, 706, 710, 714, 718, 722,
                726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766, 770, 774, 778, 782, 786,
                790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830, 834, 838, 842, 846, 850,
                855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895, 899, 903, 907, 911, 915,
                919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959, 963, 967, 971, 975, 979,
                983, 987, 991, 995, 999, 1003, 1007, 1011, 1015, 1019, 1023,
            },
        };

        Rval = AmbaIK_SetToneCurve(pImgMode, &Tone);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set tone-curve, rval(%u)", Rval, 0U);
        }
    }

    /* rgb2yuv */
    {
        AMBA_IK_RGB_TO_YUV_MATRIX_s  Rgb2Yuv;
        AmbaSvcWrap_MisraMemset(&Rgb2Yuv, 0, sizeof(Rgb2Yuv));
        Rgb2Yuv.MatrixValues[0] = 306;
        Rgb2Yuv.MatrixValues[1] = 601;
        Rgb2Yuv.MatrixValues[2] = 117;
        Rgb2Yuv.MatrixValues[3] = -173;
        Rgb2Yuv.MatrixValues[4] = -339;
        Rgb2Yuv.MatrixValues[5] = 512;
        Rgb2Yuv.MatrixValues[6] = 512;
        Rgb2Yuv.MatrixValues[7] = -429;
        Rgb2Yuv.MatrixValues[8] = -83;
        Rgb2Yuv.YOffset         = 0;
        Rgb2Yuv.UOffset         = 128;
        Rgb2Yuv.VOffset         = 128;
        Rval = AmbaIK_SetRgbToYuvMatrix(pImgMode, &Rgb2Yuv);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ISOC, "## fail to set rgb2yuv, rval(%u)", Rval, 0U);
        }
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
    UINT8 *pAddrU8;
    UINT32 CurBinSize = 0U;
    const UINT32 *pAddrU32;

    SvcIsoCfg_DTLInit();

    AmbaSvcWrap_MisraMemset(&SvcIsoCfgCcReg, 0, sizeof(SvcIsoCfgCcReg));
    AmbaSvcWrap_MisraMemset(&SvcIsoCfgCcThreeD, 0, sizeof(SvcIsoCfgCcThreeD));

    RetVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, "capture1_CC_3d.bin", &CurBinSize);
    if (RetVal != 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "Get CC 3d table size fail! ErrCode(0x%08x)", RetVal, 0U);
    } else if (CurBinSize == 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "CC 3d table file size should not zero!", 0U, 0U);
    } else if (IK_CC_3D_SIZE > CurBinSize) {
        SvcLog_NG(SVC_LOG_ISOC, "Request CC 3D size(0x%x) > file size(0x%x)", IK_CC_3D_SIZE, CurBinSize);
    } else {

        AmbaMisra_TypeCast(&(pAddrU8), &(BaseThreeD));

        RetVal = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA,
                                       "capture1_CC_3d.bin",
                                       0U,
                                       IK_CC_3D_SIZE,
                                       pAddrU8,
                                       5000U);
        if (RetVal != 0U) {
            SvcLog_NG(SVC_LOG_ISOC, "Read CC 3D table fail! ErrCode(0x%08x)", RetVal, 0U);
        } else {
            AmbaMisra_TypeCast(&(pAddrU32), &(BaseThreeD));
            AmbaSvcWrap_MisraMemcpy(SvcIsoCfgCcThreeD.MatrixThreeDTable, pAddrU32, SizeThreeD);
            SvcLog_OK(SVC_LOG_ISOC, "load CC-3D, size(%u)", CurBinSize, 0U);
        }
    }
    CurBinSize = 0U;

    RetVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, "capture1_CC_Reg.bin", &CurBinSize);
    if (RetVal != 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "Get CC reg table size fail! ErrCode(0x%08x)", RetVal, 0U);
    } else if (CurBinSize == 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "CC 3d table file size should not zero!", 0U, 0U);
    } else if (IK_CC_3D_SIZE > CurBinSize) {
        SvcLog_NG(SVC_LOG_ISOC, "Request CC reg size(0x%x) > file size(0x%x)", IK_CC_REG_SIZE, CurBinSize);
    } else {

        AmbaMisra_TypeCast(&(pAddrU8), &(BaseReg));

        RetVal = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA,
                                       "capture1_CC_Reg.bin",
                                       0U,
                                       IK_CC_REG_SIZE,
                                       pAddrU8,
                                       5000U);
        if (RetVal != 0U) {
            SvcLog_NG(SVC_LOG_ISOC, "Read CC reg table fail! ErrCode(0x%08x)", RetVal, 0U);
        } else {
            AmbaMisra_TypeCast(&(pAddrU32), &(BaseReg));
            AmbaSvcWrap_MisraMemcpy(SvcIsoCfgCcReg.RegSettingTable, pAddrU32, SizeReg);
            SvcLog_OK(SVC_LOG_ISOC, "load CC-reg, size(%u)", CurBinSize, 0U);
        }
    }

    AmbaMisra_TouchUnused(&RetVal);

    RetVal = AmbaKAL_EventFlagSet(&SvcIsoCfgDTLFlg, SVC_ISO_CFG_DTL_LOAD_DONE);
    if (RetVal != 0U) {
        SvcLog_NG(SVC_LOG_ISOC, "Set CC table load flag fail! ErrCode(0x%08x)", RetVal, 0U);
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

static void SvcIsoCfg_Duplex(const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 Err;

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
        static AMBA_IK_GRID_POINT_s   GridPoint[WARP_GRID_WIDTH * WARP_GRID_HEIGHT];
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
        UINT64                           AttachedRawSeq = 0U;
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
* first iso configuration of image kernel
* @param [in] pImgMode block of image kernel context
* @param [in] ForStill 0-video mode 1-still mode
* @return none
*/
void SvcIsoCfg_Fixed(const AMBA_IK_MODE_CFG_s *pImgMode, UINT8 ForStill)
{
    UINT32             Rval;
    AMBA_IK_ABILITY_s  Ability = {0};

    Rval = AmbaIK_GetContextAbility(pImgMode, &Ability);
    SvcLog_DBG(SVC_LOG_ISOC, "[SvcIsoCfg_Fixed] Pipe %u", Ability.Pipe, 0U);
    SvcLog_DBG(SVC_LOG_ISOC, "[SvcIsoCfg_Fixed] VideoPipe %u StillPipe %u", Ability.VideoPipe, Ability.StillPipe);

    if (Rval == OK) {
#if defined(AMBA_IK_VIDEO_Y2Y_MIPI)
        if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI)
#endif
        {
            /* set warp */
            {
                Rval = AmbaIK_SetWarpEnb(pImgMode, 0U);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set warp, rval(%u)", Rval, 0U);
                }
            }

            /* set dzoom */
            {
                AMBA_IK_DZOOM_INFO_s  DzoomInfo;
                AmbaSvcWrap_MisraMemset(&DzoomInfo, 0, sizeof(DzoomInfo));
                DzoomInfo.ZoomX = 0x10000U;
                DzoomInfo.ZoomY = 0x10000U;
                Rval = AmbaIK_SetDzoomInfo(pImgMode, &DzoomInfo);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set dzoom, rval(%u)", Rval, 0U);
                }
            }

            if (Ability.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
                FixedIsoConfigExceptY2y(pImgMode);
            }

            /* chroma-scale */
            {
                AMBA_IK_CHROMA_SCALE_s  ChromaScale = {
                    .Enable = 1U,
                    .GainCurve = {
                        160, 186, 213, 240, 267, 294, 321, 348, 375, 401, 428, 455, 482, 509, 536, 563,
                        585, 606, 618, 632, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640,
                        640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640,
                        640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640,
                        640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640,
                        640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640,
                        640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640, 640,
                        640, 632, 622, 597, 572, 535, 497, 460, 422, 385, 347, 310, 272, 235, 197, 160
                    },
                };

                if (Ability.VideoPipe == AMBA_IK_VIDEO_Y2Y) {
                    ChromaScale.Enable = 0U;
                }

                Rval = AmbaIK_SetChromaScale(pImgMode, &ChromaScale);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set chroma-scale, rval(%u)", Rval, 0U);
                }
            }

            /* chroma-filter */
            {
                AMBA_IK_CHROMA_FILTER_s  ChromaFilter;
                AmbaSvcWrap_MisraMemset(&ChromaFilter, 0, sizeof(ChromaFilter));
                if (Ability.VideoPipe == AMBA_IK_VIDEO_Y2Y) {
                    ChromaFilter.Enable = 0U;
                } else {
                    ChromaFilter.Enable = 1U;
                }
                ChromaFilter.Radius       = 32U;
                ChromaFilter.NoiseLevelCb = 30U;
                ChromaFilter.NoiseLevelCr = 30U;
                Rval = AmbaIK_SetChromaFilter(pImgMode, &ChromaFilter);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set chroma-filter, rval(%u)", Rval, 0U);
                }
            }

            /* chroma-median-filter */
            {
                AMBA_IK_CHROMA_MEDIAN_FILTER_s  ChromaMedian;
                AmbaSvcWrap_MisraMemset(&ChromaMedian, 0, sizeof(ChromaMedian));
                if (Ability.VideoPipe == AMBA_IK_VIDEO_Y2Y) {
                    ChromaMedian.Enable = 0U;
                } else {
                    ChromaMedian.Enable = 1;
                }
                ChromaMedian.CbAdaptiveAmount      = 256;
                ChromaMedian.CbAdaptiveStrength    = 255;
                ChromaMedian.CbNonAdaptiveStrength = 0;
                ChromaMedian.CrAdaptiveAmount      = 256;
                ChromaMedian.CrAdaptiveStrength    = 255;
                ChromaMedian.CrNonAdaptiveStrength = 0;
                Rval = AmbaIK_SetChromaMedianFilter(pImgMode, &ChromaMedian);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set chroma-median-filter, rval(%u)", Rval, 0U);
                }
            }

            /* first-luma */
            {
                AMBA_IK_FIRST_LUMA_PROC_MODE_s  FirstLuma;
                AmbaSvcWrap_MisraMemset(&FirstLuma, 0, sizeof(FirstLuma));
                FirstLuma.UseSharpenNotAsf = 0U;
                Rval = AmbaIK_SetFirstLumaProcMode(pImgMode, &FirstLuma);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set first-luma, rval(%u)", Rval, 0U);
                }
            }

            /* asf */
            {
                AMBA_IK_ADV_SPAT_FLTR_s  Asf;
                AmbaSvcWrap_MisraMemset(&Asf, 0, sizeof(Asf));
                Asf.Enable              = 0U;
                Asf.Fir.Specify         = 2U;
                Asf.Adapt.T0Down        = 2U;
                Asf.Adapt.T0Up          = 2U;
                Asf.Adapt.T1Down        = 4U;
                Asf.Adapt.T1Up          = 3U;
                Asf.LevelStrAdjust.High = 1U;
                Rval = AmbaIK_SetAdvSpatFltr(pImgMode, &Asf);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_ISOC, "## fail to set asf, rval(%u)", Rval, 0U);
                }
            }

            /* first-sharpen */
            {
                /* both */
                {
                    AMBA_IK_FSTSHPNS_BOTH_s  FstShpBoth;
                    AmbaSvcWrap_MisraMemset(&FstShpBoth, 0, sizeof(FstShpBoth));
                    FstShpBoth.Enable = 0U;
                    Rval = AmbaIK_SetFstShpNsBoth(pImgMode, &FstShpBoth);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-both, rval(%u)", Rval, 0U);
                    }
                }

                /* fir */
                {
                    AMBA_IK_FSTSHPNS_FIR_s  FstShpFir;
                    AmbaSvcWrap_MisraMemset(&FstShpFir, 0, sizeof(FstShpFir));
                    Rval = AmbaIK_SetFstShpNsFir(pImgMode, &FstShpFir);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-fir, rval(%u)", Rval, 0U);
                    }
                }

                /* noise */
                {
                    AMBA_IK_FSTSHPNS_NOISE_s  FstShpNoise;
                    AmbaSvcWrap_MisraMemset(&FstShpNoise, 0, sizeof(FstShpNoise));
                    FstShpNoise.LevelStrAdjust.High = 1U;
                    Rval = AmbaIK_SetFstShpNsNoise(pImgMode, &FstShpNoise);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-noise, rval(%u)", Rval, 0U);
                    }
                }

                /* coring */
                {
                    AMBA_IK_FSTSHPNS_CORING_s  FstShpCoring;
                    AmbaSvcWrap_MisraMemset(&FstShpCoring, 0, sizeof(FstShpCoring));
                    FstShpCoring.FractionalBits = 1U;
                    Rval = AmbaIK_SetFstShpNsCoring(pImgMode, &FstShpCoring);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-coring, rval(%u)", Rval, 0U);
                    }
                }

                /* coring-idx */
                {
                    AMBA_IK_FSTSHPNS_COR_IDX_SCL_s  FstShpCoringIdx;
                    AmbaSvcWrap_MisraMemset(&FstShpCoringIdx, 0, sizeof(FstShpCoringIdx));
                    FstShpCoringIdx.High = 1U;
                    Rval = AmbaIK_SetFstShpNsCorIdxScl(pImgMode, &FstShpCoringIdx);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-coring-idx, rval(%u)", Rval, 0U);
                    }
                }

                /* min-coring */
                {
                    AMBA_IK_FSTSHPNS_MIN_COR_RST_s  FstShpMinCoring;
                    AmbaSvcWrap_MisraMemset(&FstShpMinCoring, 0, sizeof(FstShpMinCoring));
                    FstShpMinCoring.High = 1U;
                    Rval = AmbaIK_SetFstShpNsMinCorRst(pImgMode, &FstShpMinCoring);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-min-coring, rval(%u)", Rval, 0U);
                    }
                }

                /* max-coring */
                {
                    AMBA_IK_FSTSHPNS_MAX_COR_RST_s  FstShpMaxCoring;
                    AmbaSvcWrap_MisraMemset(&FstShpMaxCoring, 0, sizeof(FstShpMaxCoring));
                    FstShpMaxCoring.High = 1U;
                    Rval = AmbaIK_SetFstShpNsMaxCorRst(pImgMode, &FstShpMaxCoring);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-max-coring, rval(%u)", Rval, 0U);
                    }
                }

                /* scale */
                {
                    AMBA_IK_FSTSHPNS_SCL_COR_s  FstShpScale;
                    AmbaSvcWrap_MisraMemset(&FstShpScale, 0, sizeof(FstShpScale));
                    FstShpScale.High = 1U;
                    Rval = AmbaIK_SetFstShpNsSclCor(pImgMode, &FstShpScale);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fst-shp-scale, rval(%u)", Rval, 0U);
                    }
                }
            }

            /* video-mctf */
            {
                /* mctf */
                {
                    AMBA_IK_VIDEO_MCTF_s  Mctf = {0};

                    Mctf.YSpatSmthIso.High                       = 1U;
                    Mctf.YSpatSmthDir.High                       = 1U;
                    Mctf.CbSpatSmthIso.High                      = 1U;
                    Mctf.CbSpatSmthDir.High                      = 1U;
                    Mctf.YOverallMaxChange.High                  = 1U;
                    Mctf.CbOverallMaxChange.High                 = 1U;
                    Mctf.CrOverallMaxChange.High                 = 1U;
                    Mctf.YTemporalEitherMaxChangeOrT0T1Add.High  = 1U;
                    Mctf.CbTemporalEitherMaxChangeOrT0T1Add.High = 1U;
                    Mctf.CrTemporalEitherMaxChangeOrT0T1Add.High = 1U;
                    Rval = AmbaIK_SetVideoMctf(pImgMode, &Mctf);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set video-mctf, rval(%u)", Rval, 0U);
                    }
                }

                /* ta */
                {
                    AMBA_IK_VIDEO_MCTF_TA_s  MctfTa;
                    AmbaSvcWrap_MisraMemset(&MctfTa, 0, sizeof(MctfTa));
                    MctfTa.LevAdjustHigh = 1U;
                    Rval = AmbaIK_SetVideoMctfTa(pImgMode, &MctfTa);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set video-mctf-ta, rval(%u)", Rval, 0U);
                    }
                }

                /* sharpen */
                {
                    AMBA_IK_POS_DSP33_t  MctfShp;
                    AmbaSvcWrap_MisraMemset(&MctfShp, 0, sizeof(MctfShp));
                    Rval = AmbaIK_SetVideoMctfAndFnlshp(pImgMode, &MctfShp);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set video-mctf-sharpen, rval(%u)", Rval, 0U);
                    }
                }
            }

            /* final-sharpen */
            {
                /* both */
                {
                    AMBA_IK_FNLSHPNS_BOTH_s  FinShpBoth;
                    AmbaSvcWrap_MisraMemset(&FinShpBoth, 0, sizeof(FinShpBoth));
                    FinShpBoth.Method6Lev.High = 1U;
                    Rval = AmbaIK_SetFnlShpNsBoth(pImgMode, &FinShpBoth);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-both, rval(%u)", Rval, 0U);
                    }
                }

                /* fir */
                {
                    AMBA_IK_FNLSHPNS_FIR_s  FinShpFir;
                    AmbaSvcWrap_MisraMemset(&FinShpFir, 0, sizeof(FinShpFir));
                    Rval = AmbaIK_SetFnlShpNsFir(pImgMode, &FinShpFir);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-fir, rval(%u)", Rval, 0U);
                    }
                }

                /* noise */
                {
                    AMBA_IK_FNLSHPNS_NOISE_s  FinShpNoise;
                    AmbaSvcWrap_MisraMemset(&FinShpNoise, 0, sizeof(FinShpNoise));
                    FinShpNoise.LevelStrAdjust.High = 1U;
                    Rval = AmbaIK_SetFnlShpNsNoise(pImgMode, &FinShpNoise);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-noise, rval(%u)", Rval, 0U);
                    }
                }

                /* coring */
                {
                    AMBA_IK_FNLSHPNS_CORING_s  FinShpCoring;
                    AmbaSvcWrap_MisraMemset(&FinShpCoring, 0, sizeof(FinShpCoring));
                    FinShpCoring.FractionalBits = 1U;
                    Rval = AmbaIK_SetFnlShpNsCoring(pImgMode, &FinShpCoring);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-coring, rval(%u)", Rval, 0U);
                    }
                }

                /* coring-idx */
                {
                    AMBA_IK_FNLSHPNS_COR_IDX_SCL_s  FinShpCoringIdx;
                    AmbaSvcWrap_MisraMemset(&FinShpCoringIdx, 0, sizeof(FinShpCoringIdx));
                    FinShpCoringIdx.High = 1U;
                    Rval = AmbaIK_SetFnlShpNsCorIdxScl(pImgMode, &FinShpCoringIdx);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-coring-idx, rval(%u)", Rval, 0U);
                    }
                }

                /* min-coring */
                {
                    AMBA_IK_FNLSHPNS_MIN_COR_RST_s  FinShpMinCoring;
                    AmbaSvcWrap_MisraMemset(&FinShpMinCoring, 0, sizeof(FinShpMinCoring));
                    FinShpMinCoring.High = 1U;
                    Rval = AmbaIK_SetFnlShpNsMinCorRst(pImgMode, &FinShpMinCoring);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-min-coring, rval(%u)", Rval, 0U);
                    }
                }

                /* max-coring */
                {
                    AMBA_IK_FNLSHPNS_MAX_COR_RST_s  FinShpMaxCoring;
                    AmbaSvcWrap_MisraMemset(&FinShpMaxCoring, 0, sizeof(FinShpMaxCoring));
                    FinShpMaxCoring.High = 1U;
                    Rval = AmbaIK_SetFnlShpNsMaxCorRst(pImgMode, &FinShpMaxCoring);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-max-coring, rval(%u)", Rval, 0U);
                    }
                }

                /* scale-coring */
                {
                    AMBA_IK_FNLSHPNS_SCL_COR_s  FinShpScaleCoring;
                    AmbaSvcWrap_MisraMemset(&FinShpScaleCoring, 0, sizeof(FinShpScaleCoring));
                    FinShpScaleCoring.High = 1U;
                    Rval = AmbaIK_SetFnlShpNsSclCor(pImgMode, &FinShpScaleCoring);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to set fin-shp-scale-coring, rval(%u)", Rval, 0U);
                    }
                }

                if (ForStill == 0U) {
                    UINT64                           AttachedRawSeq = 0ULL;
                    AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s  IsoCfgCtrl;
                    AmbaSvcWrap_MisraMemset(&IsoCfgCtrl, 0, sizeof(IsoCfgCtrl));
                    IsoCfgCtrl.ViewZoneId = (UINT16)pImgMode->ContextId;
                    IsoCfgCtrl.CtxIndex   = pImgMode->ContextId;
                    Rval = AmbaDSP_LiveviewUpdateIsoCfg(1, &IsoCfgCtrl, &AttachedRawSeq);
                    if (OK != Rval) {
                        SvcLog_NG(SVC_LOG_ISOC, "## fail to update iso config, rval(%u)", Rval, 0U);
                    }
                }
            }

            /* for Duplex mode */
            {
                UINT32 Err, VinSrc;
                Err = SvcResCfg_GetFovSrc(pImgMode->ContextId, &VinSrc);
                if ((ForStill == 0U) && (Err == SVC_OK) &&
                    ((VinSrc == SVC_VIN_SRC_MEM_DEC) || (VinSrc == SVC_VIN_SRC_MEM_YUV420) || (VinSrc == SVC_VIN_SRC_MEM_YUV422))) {
                    SvcIsoCfg_Duplex(pImgMode);
                }
            }

            if ((Ability.VideoPipe == AMBA_IK_VIDEO_LINEAR_CE) ||
                (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                (Ability.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
                FixedIsoConfigHdr(pImgMode, Ability.VideoPipe);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_ISOC, "## fail to get IK ability, rval(%u)", Rval, 0U);
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
    *pSize = IK_CC_REG_SIZE;
    return SVC_OK;
}
