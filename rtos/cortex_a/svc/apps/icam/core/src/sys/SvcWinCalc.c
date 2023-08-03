/**
 *  @file SvcWinCalc.c
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
 *  @details svc window calculation
 *
 */
#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaDSP_EventInfo.h"
#include "SvcWinCalc.h"

/**
* calculate size of 1.0X with respect to user specified active window
* @param [in] pActiveWindow active window size
* @param [in] pInputImage input image size
* @param [out] pBaseDisp 1X disp size
* @return none
*/
void SvcWinCalc_CalcBaseDispSize(const SVC_WIN_CALC_WIN_SIZE_s *pActiveWindow, const SVC_WIN_CALC_WIN_SIZE_s *pInputImage, SVC_WIN_CALC_WIN_SIZE_s *pBaseDisp)
{
    UINT32 DispW1X, DispH1X;

    if (pInputImage->AR < pActiveWindow->AR) {
        DispW1X = (pActiveWindow->Width * pInputImage->AR) / pActiveWindow->AR;
        DispH1X = pActiveWindow->Height;
    } else if (pInputImage->AR > pActiveWindow->AR) {
        DispW1X = pActiveWindow->Width;
        DispH1X = (pActiveWindow->Height * pActiveWindow->AR) / pInputImage->AR;
    } else {
        DispW1X = pActiveWindow->Width;
        DispH1X = pActiveWindow->Height;
    }
    pBaseDisp->Width  = (DispW1X + 1U) & 0xFFFFFFE0U;
    pBaseDisp->Height = (DispH1X + 1U) & 0xFFFFFFFEU;
    pBaseDisp->AR     = (pBaseDisp->Width << (UINT32)16U) / (pBaseDisp->Height);

}

/**
* calculate cropping region and displaying information
* @param [in] pInput input parameters
* @param [out] pOutput cropping region and displaying information
* @return none
*/
void SvcWinCalc_CalcZoomFactor(const SVC_WIN_CALC_ZOOM_INPUT_s *pInput, SVC_WIN_CALC_ZOOM_OUTPUT_s *pOutput)
{
    DOUBLE ZoomFactorX, ZoomFactorY, CropWidth, CropHeight, Temp;
    UINT32 DispWidth, DispHeight, CropWidthHalf, CropHeightHalf;
    UINT32 InputCX = pInput->InputCX, InputCY = pInput->InputCY;

    ZoomFactorX = (((DOUBLE)pInput->DispW1X * (DOUBLE)pInput->ZoomRatio) / 100.0) / (DOUBLE)pInput->ImgWidth;
    ZoomFactorY = (((DOUBLE)pInput->DispH1X * (DOUBLE)pInput->ZoomRatio) / 100.0) / (DOUBLE)pInput->ImgHeight;

    /* correct crop window */
    CropWidth  = (DOUBLE)pInput->ActiveWidth / ZoomFactorX;
    CropHeight = (DOUBLE)pInput->ActiveHeight / ZoomFactorY;

    if (CropWidth >= (DOUBLE)pInput->ImgWidth) {
        CropWidth = (DOUBLE)pInput->ImgWidth;
    }

    if (CropHeight >= (DOUBLE)pInput->ImgHeight) {
        CropHeight = (DOUBLE)pInput->ImgHeight;
    }

    Temp           = (CropWidth / 2.0) + 0.999999;
    CropWidthHalf  = (UINT32)Temp;
    Temp           = (CropHeight / 2.0) + 0.999999;
    CropHeightHalf = (UINT32)Temp;
    /* correct center_x */
    if (((pInput->ImgWidth - 1U) - pInput->InputCX) <= CropWidthHalf) {    /* Right */
        InputCX = (pInput->ImgWidth - 1U) - CropWidthHalf;
    }

    if (pInput->InputCX <= CropWidthHalf) {                                /* Left */
        InputCX = CropWidthHalf;
    }

    /* correct center_y */
    if (((pInput->ImgHeight - 1U) - pInput->InputCY) <= CropHeightHalf) {  /* Bottom */
        InputCY = (pInput->ImgHeight - 1U) - CropHeightHalf;
    }

    if (InputCY <= CropHeightHalf) {                                       /* Top */
        InputCY = CropHeightHalf;
    }

    Temp       = (CropWidth * ZoomFactorX) + 0.999999;
    DispWidth  = (UINT32)Temp;
    Temp       = (CropHeight * ZoomFactorY) + 0.999999;
    DispHeight = (UINT32)Temp;

    if (DispWidth >= pInput->ActiveWidth) {
        DispWidth = pInput->ActiveWidth;
    }

    if (DispHeight >= pInput->ActiveHeight) {
        DispHeight = pInput->ActiveHeight;
    }

    pOutput->ImgWidth    = pInput->ImgWidth;
    pOutput->ImgHeight   = pInput->ImgHeight;
    pOutput->CropCX      = InputCX;
    pOutput->CropCY      = InputCY;
    pOutput->CropWidth   = (UINT32)CropWidth;
    pOutput->CropHeight  = (UINT32)CropHeight;
    pOutput->DispWidth   = DispWidth;
    pOutput->DispHeight  = DispHeight;
    pOutput->DispOffsetX = pInput->ActiveWinOffsetX + ((pInput->ActiveWidth - DispWidth) >> 1U);
    pOutput->DispOffsetY = pInput->ActiveWinOffsetY + ((pInput->ActiveHeight - DispHeight) >> 1U);
    Temp                 = ZoomFactorX * 65536.0;
    pOutput->ZoomfactorX = (UINT32)Temp;
    Temp                 = ZoomFactorY * 65536.0;
    pOutput->ZoomfactorY = (UINT32)Temp;

    /* align height to 4 for interlace mode */
    pOutput->DispHeight  = ClearBits(pOutput->DispHeight, 0x03U);
    pOutput->DispOffsetY = ClearBits(pOutput->DispOffsetY, 0x03U);
}

/**
* calculate Hier window
* @param [in] pMainWindow main window size
* @param [out] pHierWindow hier window size
* @return none
*/
void SvcWinCalc_CalcPyramidHierWin(const UINT32 Half, const AMBA_DSP_WINDOW_DIMENSION_s *pMainWindow, AMBA_DSP_WINDOW_DIMENSION_s *pHierWindow)
{
    #define SQRT2 (1.414213562)
    FLOAT  Width, Height;

    Width = (FLOAT)pMainWindow->Width;
    Height = (FLOAT)pMainWindow->Height;
    if (Half == 0U) {
        Width = Width / (FLOAT)SQRT2;
        Height = Height / (FLOAT)SQRT2;
    } else {
        Width = Width / (FLOAT) 2.0;
        Height = Height / (FLOAT) 2.0;
    }
    pHierWindow->Width = (UINT16)GetAlignedValU32((UINT32)Width, (UINT32)16U);
    pHierWindow->Height = (UINT16)GetAlignedValU32((UINT32)Height, (UINT32)8U);
}
