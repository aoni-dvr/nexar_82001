/**
*  @file SvcBufCalc.h
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
*  @details svc buffer calculation
*
*/

#ifndef SVC_BUFCALC_H
#define SVC_BUFCALC_H

#include "AmbaMisraFix.h"

#define PYRAMID_BITS_MASK 0x3F00U

/**
* size calculation of pyramid buffer
* @param [in] pMainWin main window size
* @param [in] pHierWin hier window size
* @param [in] HierBit hier bit
* @param [out] pBufSize buffer size
* @param [out] pBufWidth buffer width
* @param [out] pBufHeight buffer height
* @return none
*/
static inline void SvcBufCalc_Pyramid420(const AMBA_DSP_WINDOW_DIMENSION_s *pMainWin,
                                         const AMBA_DSP_WINDOW_DIMENSION_s *pHierWin,
                                         UINT16 HierBit,
                                         UINT32 *pBufSize,
                                         UINT16 *pBufWidth,
                                         UINT16 *pBufHeight)
{
    UINT32  i;
    UINT16  OctaveMode = 0U;
    UINT8   EnableBit = 0U;
    UINT16  RevHierBit = 0U;

    for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i++){
        if (((HierBit >> i) & 0x1U) == 0U){
            EnableBit = 1U << i;
            RevHierBit |= EnableBit;
        }
    }
    OctaveMode = (const UINT16)((RevHierBit << 8U) & PYRAMID_BITS_MASK);

    if (AmbaDSP_CalHierBufferSize(&(pMainWin->Width),
                                  &(pMainWin->Height),
                                  &(pHierWin->Width),
                                  &(pHierWin->Height),
                                  OctaveMode,
                                  pBufSize,
                                  pBufWidth,
                                  pBufHeight) == OK) {
        *pBufSize = ((*pBufSize) * 3U) >> 1U;
    } else {
        *pBufSize = 0U;
    }
}

/**
* size calculation of 12 bits Y buffer
* @param [in] pMainWin main window size
* @param [out] pBufSize buffer size
* @param [out] pBufWidth buffer width
* @param [out] pBufHeight buffer height
* @return none
*/
static inline void SvcBufCalc_MainY12b(const AMBA_DSP_WINDOW_DIMENSION_s *pMainWin,
                                      UINT32 *pBufSize,
                                      UINT16 *pBufWidth,
                                      UINT16 *pBufHeight)
{
    UINT32  Pitch;

    Pitch = GetAlignedValU32(((UINT32)pMainWin->Width * 3U) >> 1U, 64U);
    *pBufWidth  = (UINT16)Pitch;
    *pBufHeight = pMainWin->Height;
    *pBufSize = ((UINT32)*pBufWidth) * ((UINT32)*pBufHeight);
}

/**
* size calculation of YUV420 buffer
* @param [in] pWin image size
* @param [out] pBufSize buffer size
* @param [out] pBufWidth buffer width
* @param [out] pBufHeight buffer height
* @return none
*/
static inline void SvcBufCalc_YUV420Buffer(const AMBA_DSP_WINDOW_DIMENSION_s *pWin,
                                           UINT32 *pBufSize,
                                           UINT16 *pBufWidth,
                                           UINT16 *pBufHeight)
{
    UINT32  Pitch;

    Pitch = GetAlignedValU32((UINT32)pWin->Width, 64U);
    *pBufWidth  = (UINT16)Pitch;
    //*pBufHeight = GetAlignedValU32((pWin->Height * 3U) >> 1U, 2U);
    *pBufHeight = pWin->Height;
    *pBufSize = ((UINT32)*pBufWidth) * ((UINT32)*pBufHeight);
    *pBufSize = ((*pBufSize * 3U) >> 1U);
}

#endif  /* SVC_BUFCALC_H */
