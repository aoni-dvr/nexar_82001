/**
 *  @file Util_DrawFunction.h
 *
 * Copyright (c) 2021 Ambarella International LP
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
 *  @details Header file of Util DrawFunction
 *
 */


#ifndef UTIL_DRAW_FUNCTION_H
#define UTIL_DRAW_FUNCTION_H

#include "AmbaTypes.h"

#define UTIL_DRAW_MAX_CHANNEL_NUM (3U)

#define UTIL_DRAW_RECTANGLE_POINT_NUM (2U)
#define UTIL_DRAW_LINE_POINT_NUM (2U)
#define UTIL_DRAW_TEXT_SIZE (512)
#define UTIL_DRAW_TEXT_HEIGHT (12U)

/**< Define error code*/
#define UTIL_DRAW_ERR_OK (0U)
#define UTIL_DRAW_ERR_NG (1U)



typedef struct {
    UINT8 *pBuf[UTIL_DRAW_MAX_CHANNEL_NUM];
    INT32 Width;
    INT32 Height;
    INT32 Stride;
    UINT32 ChannelNum;
} UTIL_DRAW_BUFFER_INFO_s;



/**
* Draw a rectangle.
* @param [in/out] pBufInfo    Drawing buffer information
* @param [in] Point1          Top-left point of a rectangle.
* @param [in] Point2          Down-right point of a rectangle.
* @param [in] LineWidth       Linewidth of the rectangle.
* @param [in] color           Uint8 color index.
* @return success "0" or fail "1".
*/
UINT32 UtilDRAW_Rectangle(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const INT32 Point1[UTIL_DRAW_RECTANGLE_POINT_NUM], const INT32 Point2[UTIL_DRAW_RECTANGLE_POINT_NUM], UINT32 LineWidth, const UINT8 color[UTIL_DRAW_MAX_CHANNEL_NUM]);

/**
* Draw an arrow.
* @param [in] Initial point of an arrow (x,y).
* @param [in] Degree: angle based on x+ axis (0~360).
* @param [in] LineLenght of the arrow.
* @param [in] Linewidth of the arrow.
* @param [in] Uint8 color index.
* @param [in/out] U8 buffer.
* @return errorcode.
*/
UINT32 UtilVSL_DrawArrow(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, INT32 x, INT32 y, DOUBLE Degree, INT32 LineLenght, UINT32 LineWidth, const UINT8 Color[UTIL_DRAW_MAX_CHANNEL_NUM]);

/**
* Draw a line.
* @param [in] First point of a line.
* @param [in] Second point of a line.
* @param [in] Linewidth of the line.
* @param [in] Uint8 color index.
* @param [in/out] U8 buffer.
* @return success "0" or fail "1".
*/
UINT32 UtilDRAW_Line(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const INT32 Point1[UTIL_DRAW_LINE_POINT_NUM], const INT32 Point2[UTIL_DRAW_LINE_POINT_NUM], UINT32 LineWidth, const UINT8 color[UTIL_DRAW_MAX_CHANNEL_NUM]);

/**
* Draw a polyline.
* @param [in] xList: x array of point.
* @param [in] yList: y array of point.
* @param [in] listNum: # of point.
* @param [in] encloseFlag: connection of head and tail (0 or 1).
* @param [in] Uint8 color index.
* @param [in/out] U8 buffer.
* @return errorcode.
*/
UINT32 UtilDRAW_PolyLine(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const INT32 *xList, const INT32 *yList, INT32 listNum, INT32 encloseFlag, UINT32 LineWidth, const UINT8 color[UTIL_DRAW_MAX_CHANNEL_NUM]);


/**
* Draw a Cross.
* @param [in/out] pBufInfo     Information of buffer
* @param [in]     x            x point.
* @param [in]     y            y point.
* @param [in]     CrossSize    Size of cross
* @param [in]     LineWidth    Line Width (at least 1)
* @param [in]     color        Color index.
* @return errorcode.
*/
UINT32 UtilDRAW_Cross(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, INT32 x, INT32 y, UINT32 CrossSize, UINT32 LineWidth, const UINT8 color[UTIL_DRAW_MAX_CHANNEL_NUM]);


/**
* Draw a text.
* @param [in] OfsX, OfsY: left-top position of word.
* @param [in] Word: text content.
* @param [in] WordSz: height of text size (current supprot 6~64).
* @param [in] WColor: text color (WColor[3] = {255,0,0} as red).
* @param [in] IsUsingBackground: 0 or 1. 0: using buffer color as background, 1: using BGColor as background.
* @param [in] BGColor: background color if IsUsingBackground is 1, ex:BGColor[3] = {255,0,0} as red.
* @param [in/out] U8 buffer.
* @return success "0" or fail "1".
*/
UINT32 UtilDRAW_PutTextBW(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, UINT32 OfsX, UINT32 OfsY, const char Word[UTIL_DRAW_TEXT_SIZE], UINT32 WordSz, const UINT8 WColor[UTIL_DRAW_MAX_CHANNEL_NUM], UINT8 IsUsingBackground, const UINT8 BGColor[UTIL_DRAW_MAX_CHANNEL_NUM]);

/**
* Draw a circle.
* @param [in, out] pBufInfo     Information of buffer
* @param [in]      Centre       Centre point, [0] for x, [1] for y.
* @param [in]      Radius       y point of centre.
* @param [in]      LineWidth    Line Width (at least 1), invalid at this version
* @param [in]      color        Color index.
* @return errorcode.
*/
UINT32 UtilDRAW_Circle(const UTIL_DRAW_BUFFER_INFO_s *pBufInfo, const INT32 Centre[UTIL_DRAW_LINE_POINT_NUM], UINT32 Radius, UINT32 LineWidth, const UINT8 color[UTIL_DRAW_MAX_CHANNEL_NUM]);


#endif //UTIL_DRAW_FUNCTION_H
