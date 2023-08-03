/**
*  @file SvcCvImgUtil.h
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
*  @details svc application cv image utility
*
*/

#ifndef SVC_CV_IMG_UTIL_H
#define SVC_CV_IMG_UTIL_H

typedef struct {
    UINT8   *pSrc;                          /* Source buffer */
    UINT32  SrcWidth;
    UINT32  SrcHeight;
    UINT32  SrcPitch;
    UINT32  SrcStartY;                      /* (Optional) Bypass lines before SrcStartY. Corresponding Dst lines will be bypassed also. */
    UINT8   *pDst;                          /* Destination buffer */
    UINT32  DstWidth;
    UINT32  DstHeight;
    UINT32  DstPitch;
    UINT32  HorizantalMapLen;               /* Length of the map */
    UINT8   *pHorizantalMap;                /* A map to describe repeat/skip of each pixel in horizontal direction */
                                            /* 0 - skip 1 pixel, 1 - no repeat, 2 - repeat 1 pixel, 3 - repeat 2 pixel, and so on  */
    UINT32  VerticalMapLen;                 /* Length of the map */
    UINT8   *pVerticalMap;                  /* A map to describe repeat/skip of each line in vertical direction */
                                            /* 0 - skip 1 line, 1 - no repeat, 2 - repeat 1 line, 3 - repeat 2 line, and so on  */

    /* SvcCvImgUtil_RescaleBitMap_Rept */
    UINT8   ColorOffset;                    /* (Optional) The index offset of the bitmap mapping rule (Source side) */

    /* SvcCvImgUtil_RescaleBitMap_Idx2Color */
    UINT32  *pClut;                         /* (Optional) color lookup table */
    UINT32  NumColor;                       /* (Optional) Max color index of the color lookup table */
} SVC_BIT_MAP_RESCALE_REPT_s;

typedef struct {
    DOUBLE Value;
    UINT8  *pTable;
    UINT32 TableLength;
} SVC_BIT_MAP_PIXEL_MAP_s;

UINT32 SvcCvImgUtil_RescaleBitMap_Rept(SVC_BIT_MAP_RESCALE_REPT_s *pCfg);
UINT32 SvcCvImgUtil_RescaleBitMap_Idx2Color(SVC_BIT_MAP_RESCALE_REPT_s *pCfg);
void SvcCvImgUtil_GetBitMapScaleTable(DOUBLE ScaleValue, UINT8 **ppTable, UINT32 *pTableLength);

UINT32 SvcCvImgUtil_12bCompactTo8b(UINT8 *pSrcBuf, UINT32 SrcWidth, UINT32 SrcHeight, UINT32 SrcPitch, UINT8 *pDstBuf, UINT32 DstPitch);

#endif  /* SVC_CV_IMG_UTIL_H */
