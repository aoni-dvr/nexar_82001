/**
 * @file AmbaDraw_BMPFont.h
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
#ifndef AMBADRAW_BMPFONT_H
#define AMBADRAW_BMPFONT_H

#include <AmbaTypes.h>
#include "AmbaDraw_Render.h"

/*************************************************************************
 * BMP Font Enums
 ************************************************************************/

/*************************************************************************
 * BMP Font Structures
 ************************************************************************/

/**
* Font draw string config
 */
typedef struct {
    UINT32 FontId;                                                      /**< Font index in Font Bin buffer                      */
    const char *String;                                                 /**< String message                                     */
    AMBA_DRAW_COLOR_s ColorFore;                                        /**< Fore color                                         */
    AMBA_DRAW_COLOR_s ColorBack;                                        /**< Back color                                         */
    INT32 StringX;                                                      /**< X of String (same as AMBA_DRAW_STR_DESC_s.StringX) */
    INT32 StringY;                                                      /**< Y of String (same as AMBA_DRAW_STR_DESC_s.StringY) */
} AMBADRAW_FONT_DRAW_STRING_CONFIG_s;

/**
 *  The information corresponding to each AMBA_DRAW_BMPFONT_BIN_FONT_s
 */
typedef struct {
    UINT16  *Offset;                                                    /**< bit-offsets for variable-width font            */
    UINT8   *Data;                                                      /**< character bitmap data array                    */
} AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s;

/**
 * Font functions
 */
typedef struct {
    UINT32 (*Init_f)(void);                                             /**< Init Font                                          */
    UINT32 (*Load_f)(UINT8 *FontBuffer,
                     UINT32 FontBufferSize,
                     AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s *FontPageInfo,
                     UINT32 MaxFontPageNum);                                     /**< Load FontBuffer                                    */
    UINT32 (*Draw_f)(const AMBA_DRAW_RENDER_s *Render,
                     const AMBADRAW_FONT_DRAW_STRING_CONFIG_s *DrawConfig);      /**< Draw function                                      */
    UINT32 (*GetStrWidth_f)(UINT32 FontId, const char *String, UINT32 *Width);   /**< Get String Width function                          */
    UINT32 (*GetStrHeight_f)(UINT32 FontId, const char *String, UINT32 *Height); /**< Get String Height function                         */
} AMBADRAW_FONT_FUNCTION_s;

/**
 * Font config
 */
typedef struct {
    const char *FileName;                                               /**< the binary filename of font                        */
    UINT32 Size;                                                        /**< Total size of the font module                      */
    UINT8 *Buffer;                                                      /**< Buffer address to a static buffer to load font     */
    AMBADRAW_FONT_FUNCTION_s Func;                                      /**< Functions of BMP font                              */
    AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s *FontPageInfo;                    /**< Font page info array                               */
    UINT32 MaxFontPageNum;                                              /**< The max number of data in FontPageInfo             */
} AMBADRAW_FONT_s;

/*************************************************************************
 * BMP Font Defination
 ************************************************************************/

/*************************************************************************
 * BMP Font APIs
 ************************************************************************/

/**
 *  Get Font functions
 *
 *  @param [out] Func               Functions of BMP font
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawBMPFont_GetFunc(AMBADRAW_FONT_FUNCTION_s *Func);

/**
 *  Check font
 *
 *  @param [in] FontCfg             Font config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawBMPFont_IsValidFont(const AMBADRAW_FONT_s *FontCfg);

#endif /* AMBADRAW_BMPFONT_H */

