/**
*  @file SvcWinCalc.h
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
*  @details svc playback command functions
*
*/

#ifndef SVC_WIN_CALC_H
#define SVC_WIN_CALC_H

#define AR_1X1      (((UINT32)1U  << (UINT32)16U) / (UINT32)1U)
#define AR_2X1      (((UINT32)2U  << (UINT32)16U) / (UINT32)1U)
#define AR_4X3      (((UINT32)4U  << (UINT32)16U) / (UINT32)3U)
#define AR_8X3      (((UINT32)8U  << (UINT32)16U) / (UINT32)3U)
#define AR_8X9      (((UINT32)8U  << (UINT32)16U) / (UINT32)9U)
#define AR_3X2      (((UINT32)3U  << (UINT32)16U) / (UINT32)2U)
#define AR_16X9     (((UINT32)16U << (UINT32)16U) / (UINT32)9U)
#define AR_17X9     (((UINT32)17U << (UINT32)16U) / (UINT32)9U)

typedef struct {
    UINT32      Width;
    UINT32      Height;
    UINT32      AR;          /* AspectRatio */
} SVC_WIN_CALC_WIN_SIZE_s;

typedef struct {
    UINT32 ZoomRatio;        /* ZoomRatio: zoom ratio. 100 = 1X, 1000 = 10X */
    UINT32 ImgWidth;
    UINT32 ImgHeight;
    UINT32 InputCX;          /* center x of cropping */
    UINT32 InputCY;
    UINT32 DispW1X;          /* width of 1X on Vout */
    UINT32 DispH1X;
    UINT32 ActiveWidth;
    UINT32 ActiveHeight;
    UINT32 ActiveWinOffsetX;
    UINT32 ActiveWinOffsetY;
} SVC_WIN_CALC_ZOOM_INPUT_s;

typedef struct {
    UINT32 ImgWidth;
    UINT32 ImgHeight;
    UINT32 CropCX;
    UINT32 CropCY;
    UINT32 CropWidth;
    UINT32 CropHeight;
    UINT32 DispOffsetX;
    UINT32 DispOffsetY;
    UINT32 DispWidth;
    UINT32 DispHeight;
    UINT32 ZoomfactorX;      /* Zoomfactor << 16 */
    UINT32 ZoomfactorY;
} SVC_WIN_CALC_ZOOM_OUTPUT_s;

void SvcWinCalc_CalcBaseDispSize(const SVC_WIN_CALC_WIN_SIZE_s *pActiveWindow, const SVC_WIN_CALC_WIN_SIZE_s *pInputImage, SVC_WIN_CALC_WIN_SIZE_s *pBaseDisp);
void SvcWinCalc_CalcZoomFactor(const SVC_WIN_CALC_ZOOM_INPUT_s *pInput, SVC_WIN_CALC_ZOOM_OUTPUT_s *pOutput);
void SvcWinCalc_CalcPyramidHierWin(const UINT32 Half, const AMBA_DSP_WINDOW_DIMENSION_s *pMainWindow, AMBA_DSP_WINDOW_DIMENSION_s *pHierWindow);


#endif  /* SVC_WIN_CALC_H */
