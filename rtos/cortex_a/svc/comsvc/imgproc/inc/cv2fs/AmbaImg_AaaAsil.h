/**
 *  @file AmbaImg_Proc.h
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
 *  @Description    :: Definitions & Constants for Sample AE(Auto Exposure) and AWB(Auto White Balance) algorithms
 *
 */

#ifndef AMBA_IMG_AAA_ASIL
#define AMBA_IMG_AAA_ASIL

#include "AmbaKAL.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_Adjustment.h"

typedef struct  {
    INT32    min_gr;
    INT32    max_gr;
    INT32    min_gb;
    INT32    max_gb;
    UINT8    patch_no[32][32];
} AWB_PATCH_INFO_s;//for external use

typedef struct  {
    INT32    min_gr;
    INT32    max_gr;
    INT32    min_gb;
    INT32    max_gb;
    UINT8    patch_no[14641];
    UINT8    padding[3];
} AWB_PATCH_INFO_FULL_s;//for external use


typedef struct  {
    AMBA_IK_CE_s    CEInfo;
}ADJ_FLEXDIAG_GROUP0_s;

typedef struct  {
    AMBA_IK_CE_s                    CEInfo;
    AMBA_IK_COLOR_CORRECTION_s      CCTable;
    // AMBA_IK_CFA_LEAKAGE_FILTER_s    CLeakage;
    // AMBA_IK_DEMOSAIC_s              Demosaic;
    // AMBA_IK_CFA_NOISE_FILTER_s      CFANoise;
    // AMBA_IK_RGB_IR_s                RGBIR;

}ADJ_FLEXDIAG_GROUP1_s;

typedef struct {
    UINT32  FrameIndex;
    UINT16  SumRGB[12288];
    UINT8   ROIWeight[4096];
    UINT8   LumaWeight[64];
    UINT8   TableWeight[20];
    AMBA_IK_WB_GAIN_s GoldenCalibration[2];
    AMBA_IK_WB_GAIN_s CurrentCalibration [2];
    UINT32  HighLightEvIndex; // Specifies the lowest Luma value (LV) of a high light environment.
    UINT32  OutdoorEvIndex;   // Specifies the lowest LV of an outdoor environment.
    UINT32  EvIndex;
    UINT32  DoubleInc;
    AMBA_IK_WB_GAIN_s FineRatio;
    UINT16  AwbRgbShift;
    UINT16  AwbTileWidth;
    UINT16  AwbTileHeight;
    UINT8   Padding0[2];
    // AWB_CONTROL_s AwbCtrl;
    UINT32 NumOfTiles;
    UINT32 RGainToCmp;
    UINT32 BGainToCmp;
    UINT32 RGainDefault;
    UINT32 BGainDefault;
    UINT32 RGRatio[2];    //for fine tune and def param
    UINT32 BGRatio[2];    //for fine tune and def param
    AMBA_IK_WB_GAIN_s CurGain;
    INT16  Speed;
    UINT8  Padding1[2];
    UINT32 Cal_Set_GainR[2];
    INT32  Cal_DeltaR[2];
    UINT32 Cal_Set_GainB[2];
    INT32  Cal_DeltaB[2];
    AWB_PATCH_INFO_FULL_s IndoorWRFULL;
    AWB_PATCH_INFO_FULL_s OutdoorWRFULL;
    AWB_PATCH_INFO_FULL_s HighlightWRFULL;
    UINT8  DbgPrt;
    UINT8  Padding2[3];
    UINT32 CfaAWBRatio;
//AE
    FLOAT     CurShutterTime;
    FLOAT     CurAgcGain;
    INT32     CurDgain;
    UINT16    CurLumaStat;
    INT16     CurLimitStatus;
    UINT16    CurTarget;
    UINT8     Padding3[2];
    FLOAT     PreShutterTime;
    FLOAT     PreAgcGain;
    INT32     PreDgain;

//ADJ
    PIPELINE_CONTROL_s *pPipe0;
    PIPELINE_CONTROL_s *pPipe1;
    PIPELINE_CONTROL_s *pPipe2;
    AMBA_AAA_ASIL_OP_INFO_s AAAOP;
}AMBA_AAA_FLEXIDAG_INFO_s;

typedef struct {
    INT32 Result;
} AMBA_AE_FlEXIDAG_RESULT_t;

typedef struct {
    INT32 Result;
} AMBA_AWB_FlEXIDAG_RESULT_t;

typedef struct {
    INT32 Result;
} AMBA_ADJ_FlEXIDAG_RESULT_t;

typedef struct {
    AMBA_AE_FlEXIDAG_RESULT_t AeResult;
    AMBA_AWB_FlEXIDAG_RESULT_t AwbResult;
    AMBA_ADJ_FlEXIDAG_RESULT_t AdjResult;
} AMBA_IMGPROC_FlEXIDAG_RESULT_t;

#endif  /* _AMBA_AE_AWB_ */