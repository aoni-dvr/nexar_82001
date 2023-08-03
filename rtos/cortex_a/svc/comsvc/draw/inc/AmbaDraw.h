/**
 * @file AmbaDraw.h
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

#ifndef AMBADRAW_H
#define AMBADRAW_H

#include <AmbaTypes.h>
#include <AmbaDSP_VOUT.h>

/*************************************************************************
 * Draw Definitions
 ************************************************************************/
#define DRAW_ERR_0000                   (DRAW_ERR_BASE       )  /**< Invalid argument */
#define DRAW_ERR_0001                   (DRAW_ERR_BASE | 0x1U)  /**< Fatal error */

#define AMBA_DRAW_MAX_POLY_CORNER_NUM   (6U)                    /**< Maximum number of polygon corners */
#define AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM   (32U)           /**< Maximum number of control points of a curve */
#define AMBA_DRAW_MAX_STRING_MSG_LEN    (256U)                  /**< Maximum of string message length */
#define AMBA_DRAW_COLOR_LEN             (8U)                    /**< Maximum of color length */

/**
 *  The definition of Alignment type
 */
#define AMBA_DRAW_ALIGN_V_TOP           (0x01U)                 /**< Top position */
#define AMBA_DRAW_ALIGN_V_MIDDLE        (0x02U)                 /**< Middle position */
#define AMBA_DRAW_ALIGN_V_BOTTOM        (0x04U)                 /**< Bottom postion */
#define AMBA_DRAW_ALIGN_H_LEFT          (0x08U)                 /**< Left position */
#define AMBA_DRAW_ALIGN_H_CENTER        (0x10U)                 /**< Center postion */
#define AMBA_DRAW_ALIGN_H_RIGHT         (0x20U)                 /**< Right position */

/**
 *  The Draw buffer PixelFormat
 */
#define AMBA_DRAW_BUFFER_8BIT_CLUT_MODE     OSD_8BIT_CLUT_MODE                          /**< 8 bit CLUT mode. Index (0 ~ 255) of an 8-bit CLUT (Color Look-Up Table). */
#define AMBA_DRAW_BUFFER_32BIT_ARGB_8888    OSD_32BIT_ARGB_8888                         /**< 32 bit ARGB 8888. (8-bit A + 8-bit R + 8-bit G + 8-bit B) */
/**
 *  YUV420
 *  For a 1920X1080 BMP, store the image in an array arranged as below:
 *  1. 1920X1080 Y data (ex. Y1 Y2 Y3 Y4 ...)
 *  2. 1920X540  UV data (ex. U1 V1 U3 V3 ...)
 *  3. 1920X1080 Alpha data of Y (ex. AY1 AY2 AY3 AY4 ...)
 *  4. 1920X540  Alpha data of UV (ex. AU1 AV1 AU3 AV3 ...)
 *  For a 32 bit color (such as LineColor), consider it as a buffer storing 2X1 BMP
 *  1. 8 bit Y data
 *  2. 8 bit UV data
 *  3. 8 bit Alpha data of Y
 *  4. 8 bit Alpha data of UV
 *    (ex. Y1 Y2 U1 V1 AY1 AY2 AU1 AV1)
 */
#define AMBA_DRAW_BUFFER_YUV420_WITH_ALPHA  (AMBA_DRAW_BUFFER_32BIT_ARGB_8888 + 1U)     /**< YUV420 */
/**
 *  YUV422
 *  For a 1920X1080 BMP, store the image in an array arranged as below:
 *  1. 1920X1080 Y data (ex. Y1 Y2 Y3 Y4 ...)
 *  2. 1920X1080 UV data (ex. U1 V1 U3 V3 ...)
 *  3. 1920X1080 Alpha data of Y (ex. AY1 AY2 AY3 AY4 ...)
 *  4. 1920X1080 Alpha data of UV (ex. AU1 AV1 AU3 AV3 ...)
 *  For a 32 bit color (such as LineColor), consider it as a buffer storing 2X1 BMP
 *  1. 8 bit Y data
 *  2. 8 bit UV data
 *  3. 8 bit Alpha data of Y
 *  4. 8 bit Alpha data of UV
 *    (ex. Y1 Y2 U1 V1 AY1 AY2 AU1 AV1)
 */
#define AMBA_DRAW_BUFFER_YUV422_WITH_ALPHA  (AMBA_DRAW_BUFFER_YUV420_WITH_ALPHA + 1U)   /**< YUV422 */

/**
 *  The definition of DrawOption
 */
#define AMBA_DRAW_OPTION_ALPHA_BLENDING (0x01U)                 /**< Enable alpha blending of objects with background   */

/**
 *  Font definitions MUST sync with GUI gen
 */
#define AMBA_DRAW_BMPFONT_MAGIC         (0x1234U)               /**< AMBA_DRAW_BMPFONT_MAGIC      0x1234    */
#define AMBA_DRAW_BMPFONT_VERSION       (0x0100U)               /**< AMBA_DRAW_BMPFONT_VERSION    0x0100    */
#define AMBA_DRAW_BMPFONT_PFT_VARIABLE  (0x01U)                 /**< Variable-Width font (1-bpp, standard)  */
#define AMBA_DRAW_BMPFONT_PFT_OUTLINE   (0x02U)                 /**< Outline font (2-bpp)                   */
#define AMBA_DRAW_BMPFONT_PFT_ALIASED   (0x04U)                 /**< Anti-aliased (4-bpp) font              */

/**
 *  BMP give transparency color
 *  This definitions MUST sync with GUI gen
 */
#define AMBA_DRAW_BMP_BMF2_HAS_TRANS    (0x80U)

/**
 *  BMP Encode Type
 *  These definitions MUST sync with GUI gen
 */
#define AMBA_DRAW_ENCODE_BMP_BMF2_RAW       (0x00U)             /**< Bitmap is not encoded.                 */
#define AMBA_DRAW_ENCODE_BMP_BMF2_AMBARLE   (0x01U)             /**< Bitmap is RLE encoded.                 */
#define AMBA_DRAW_ENCODE_BMP_BMF2_AVCHDRLE  (0x02U)             /**< Bitmap is AVCHD RLE encoded.           */
#define AMBA_DRAW_ENCODE_BMP_BMF2_STDRLE    (0x03U)             /**< Bitmap is STD RLE encoded.             */
#define AMBA_DRAW_ENCODE_BMP_BMF2_ANM       (0x04U)             /**< Bitmap is animation.                   */
#define AMBA_DRAW_ENCODE_BMP_END            (0x05U)             /**< Bitmap encode end.                     */

/**
 *  BMP Loading Status
 *  These definitions MUST sync with GUI gen
 */
#define BMP_STATUS_INIT                 (0x00U)                 /**< Bitmap is inited                       */
#define BMP_STATUS_LOADED               (0x01U)                 /**< Bitmap is loaded                       */
#define BMP_INIT_SATAUS_END             (0x02U)                 /**< Bitmap status end                      */

/*************************************************************************
 * Draw Structures
 ************************************************************************/
/**
 * Initial struct for draw
 */
typedef struct {
    UINT8  *FontAddr;                       /**< The buffer address of font                     */
    UINT32 FontBufSize;                     /**< The buffer size of font (in Byte)              */
    UINT8  *BufferAddr;                     /**< The buffer address                             */
    UINT32 BufferSize;                      /**< The size (in Byte)                             */
    UINT16 MaxLineNum;                      /**< The max number of Line                         */
    UINT16 MaxRectNum;                      /**< The max number of Rect                         */
    UINT16 MaxCircleNum;                    /**< The max number of Circle                       */
    UINT16 MaxBmpNum;                       /**< The max number of BMP                          */
    UINT16 MaxStringNum;                    /**< The max number of String                       */
    UINT16 MaxPolyNum;                      /**< The max number of Polygon                      */
    UINT16 MaxCurveNum;                     /**< The max number of Curve                        */
    UINT16 MaxFontPageNum;                  /**< The max number of Font page                    */
} AMBA_DRAW_INIT_CONFIG_s;

/**
 * Draw buffer information
 * The definition of Width/Height/Pitch is consistent with InputWidth/InputHeight/Pitch of AMBA_DSP_VOUT_OSD_BUF_CONFIG_s
 *
 * For YUV420 format:
 *     RGBAddr is dummy
 *     Size of YAddr buffer = Pitch * Height
 *     Size of UVAddr buffer = Pitch * Height
 *     Size of AlphaYAddr buffer = Pitch * Height
 *     Size of AlphaUVAddr buffer = Pitch * Height
 * For YUV422 format:
 *     RGBAddr is dummy
 *     Size of YAddr buffer = Pitch * Height
 *     Size of UVAddr buffer = Pitch * CEIL(Height/2)
 *     Size of AlphaYAddr buffer = Pitch * Height
 *     Size of AlphaUVAddr buffer = Pitch * CEIL(Height/2)
 * For 8/16/32Bit format:
 *     Size of RGBAddr buffer = Pitch * Height
 *     YAddr/UVAddr/AlphaYAddr/AlphaUVAddr are dummy
 */
typedef struct {
    UINT8  PixelFormat;                     /**< Pixel Format (ex: AMBA_DRAW_BUFFER_8BIT_CLUT_MODE)     */
    UINT32 Width;                           /**< Buffer width (in Pixel)                                */
    UINT32 Height;                          /**< Buffer height (in Pixel)                               */
    UINT32 Pitch;                           /**< Buffer pitch (in Byte)                                 */
    UINT8  *RGBAddr;                        /**< Raw buffer address                                     */
    UINT8  *YAddr;                          /**< Y buffer address                                       */
    UINT8  *UVAddr;                         /**< UV buffer address                                      */
    UINT8  *AlphaYAddr;                     /**< Y alpha buffer address                                 */
    UINT8  *AlphaUVAddr;                    /**< UV alpha buffer address                                */
} AMBA_DRAW_BUFFER_INFO_s;

/**
 * Draw area description
 */
typedef struct {
    INT32  X;                               /**< X offset of the area (in Pixel)        */
    INT32  Y;                               /**< Y offset of the area (in Pixel)        */
    UINT32 Width;                           /**< Width of the area (in Pixel)           */
    UINT32 Height;                          /**< Height of the area (in Pixel)          */
} AMBA_DRAW_AREA_s;

/**
 * Draw color description
 *
 * For 8 bit format:
 *     Assume the color is "UINT8 Color", then the Data is saved as:
 *     Data[0] = Color
 *     Data[1~7] = 0
 *
 * For 16 bit format:
 *     Assume the color is "UINT16 Color", then the Data is saved as:
 *     Data[0] = (UINT8)Color
 *     Data[1] = (UINT8)(Color >> 8)
 *     Data[2~7] = 0
 *
 * For 32 bit format:
 *     Assume the color is "UINT32 Color", then the Data is saved as:
 *     Data[0] = (UINT8)Color
 *     Data[1] = (UINT8)(Color >> 8)
 *     Data[2] = (UINT8)(Color >> 16)
 *     Data[3] = (UINT8)(Color >> 24)
 *     Data[4~7] = 0
 *
 * For YUV420/YUV422 format:
 *     Assume the color is "UINT8 Y, U, V, AlphaY, AlphaUV", then the Data is saved as:
 *     Data[0] = Y
 *     Data[1] = Y
 *     Data[2] = U
 *     Data[3] = V
 *     Data[4] = AlphaY
 *     Data[5] = AlphaY
 *     Data[6] = AlphaUV
 *     Data[7] = AlphaUV
 */
typedef struct {
    UINT8 Data[AMBA_DRAW_COLOR_LEN];        /**< Color data                             */
} AMBA_DRAW_COLOR_s;

/**
 * Draw shadow description
 */
typedef struct {
    UINT8  Enable;                          /**< Enable shadow                          */
    UINT32 ShadowPostion;                   /**< Postion (ex: AMBA_DRAW_ALIGN_V_TOP)    */
    UINT32 Distance;                        /**< Distance (in Pixel)                    */
    AMBA_DRAW_COLOR_s Color;                /**< Color                                  */
} AMBA_DRAW_SHADOW_s;

/**
 * AmbaDraw Line config
 */
typedef struct {
    INT32  X1;                              /**< X coordinate (in Pixel) of one end-point       */
    INT32  Y1;                              /**< Y coordinate (in Pixel) of one end-point       */
    INT32  X2;                              /**< X coordinate (in Pixel) of another end-point   */
    INT32  Y2;                              /**< Y coordinate (in Pixel) of another end-point   */
    UINT32 LineWidth;                       /**< Line width (in Pixel)                          */
    UINT32 DashInterval;                    /**< Dash interval (in Pixel)                       */
    AMBA_DRAW_COLOR_s LineColor;            /**< Line color                                     */
} AMBA_DRAW_LINE_CFG_s;

/**
 * AmbaDraw Rectangle config
 */
typedef struct {
    UINT32 Width;                           /**< Rectangle Width (in Pixel)             */
    UINT32 Height;                          /**< Rectangle Height (in Pixel)            */
    UINT32 LineWidth;                       /**< Line width (in Pixel)                  */
    AMBA_DRAW_COLOR_s LineColor;            /**< Line color                             */
    AMBA_DRAW_COLOR_s FillColor;            /**< Fill color                             */
    AMBA_DRAW_SHADOW_s Shadow;              /**< Shadow settings                        */
} AMBA_DRAW_RECT_CFG_s;

/**
 * AmbaDraw Circle config
 */
typedef struct {
    UINT32 Radius;                          /**< Radius length (in Pixel)               */
    UINT32 LineWidth;                       /**< Line width (in Pixel)                  */
    AMBA_DRAW_COLOR_s LineColor;            /**< Line color                             */
    AMBA_DRAW_COLOR_s FillColor;            /**< Fill color                             */
} AMBA_DRAW_CIRCLE_CFG_s;

/**
 * AmbaDraw Polygon config
 */
typedef struct {
    UINT32 Corner;                          /**< Corner of the polygon. Limited by AMBA_DRAW_MAX_POLY_CORNER_NUM    */
    INT32  X[AMBA_DRAW_MAX_POLY_CORNER_NUM];/**< X coordinate (in Pixel) in clockwise order                         */
    INT32  Y[AMBA_DRAW_MAX_POLY_CORNER_NUM];/**< Y coordinate (in Pixel) in clockwise order                         */
    AMBA_DRAW_COLOR_s FillColor;            /**< Fill color                                                         */
    AMBA_DRAW_SHADOW_s Shadow;              /**< Shadow settings                                                    */
} AMBA_DRAW_POLY_CFG_s;

/**
 * AmbaDraw Curve config
 */
typedef struct {
    UINT32 ControlPointNum;                                     /**< Number of control points. Limited by AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM */
    INT32  X[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];            /**< X coordinate (in Pixel) of each control point                              */
    INT32  Y[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];            /**< Y coordinate (in Pixel) of each control point                              */
    UINT32 LineWidth[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];    /**< Line width (in Pixel) at each control point                                */
    AMBA_DRAW_COLOR_s LineColor[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM-1U]; /**< Line color between each control point                           */
} AMBA_DRAW_CURVE_CFG_s;

/**
 * AmbaDraw BMP config
 *
 * For YUV420 format:
 *     Data in 'Data' buffer: Y, UV, Alpha Y, Alpha UV consecutively
 *     Size of 'Data' buffer = Width * Height * (2 + 2*CEIL(Height/2))
 * For YUV422 format:
 *     Data in 'Data' buffer: Y, UV, Alpha Y, Alpha UV consecutively
 *     Size of 'Data' buffer = Width * Height * 4
 * For 8/16/32Bit format:
 *     Size of 'Data' buffer = Width * Height
 */
typedef struct {
    UINT8  Transparent;                     /**< Bitmap give transparency color or not  */
    UINT16 Width;                           /**< Bitmap width (in Pixel)                */
    UINT16 Height;                          /**< Bitmap height (in Pixel)               */
    AMBA_DRAW_COLOR_s TransparentColor;     /**< Transparent color for > 8bit bitmaps   */
    UINT8  *Data;                           /**< Bitmap data pointer                    */
} AMBA_DRAW_BMP_CFG_s;

/**
 * AmbaDraw String config
 */
typedef struct {
    UINT32 Width;                           /**< String box width (in Pixel)                                */
    UINT32 Height;                          /**< String box height (in Pixel)                               */
    AMBA_DRAW_COLOR_s Color;                /**< String color                                               */
    UINT8  Alignment;                       /**< String alignment in string box (ex: AMBA_DRAW_ALIGN_V_TOP) */
    UINT32 FontId;                          /**< Font index in Font Bin buffer                              */
    char Msg[AMBA_DRAW_MAX_STRING_MSG_LEN]; /**< The string message                                         */
    AMBA_DRAW_SHADOW_s Shadow;              /**< Shadow settings                                            */
} AMBA_DRAW_STRING_CFG_s;

/**
 *  The definition of BMP font header in BMP font BIN.
 *  This structure MUST sync with GUI gen
 */
#pragma pack(4)
typedef struct {
    UINT16  Magic;                          /**< Magic number                                   */
    UINT16  Version;                        /**< Version                                        */
    UINT16  HeaderSize;                     /**< sizeof (AMBA_DRAW_BMPFONT_BIN_HEADER_s)        */
    UINT16  SizeOfFont;                     /**< sizeof (AMBA_DRAW_BMPFONT_BIN_FONT_s)          */
    char FontName[120];                     /**< Font logical name                              */
    UINT16  Pages;                          /**< number of pages (>= 1)                         */
    UINT16  SizeOfLang;                     /**< sizeof (AMBA_DRAW_BMPFONT_BIN_LANG_s)          */
    UINT16  LangNum;                        /**< number of languages (>= 1)                     */
    UINT16  Reversed[6];                    /**< future use                                     */
} AMBA_DRAW_BMPFONT_BIN_HEADER_s;
#pragma pack()

/**
 *  The definition of BMP font language in BMP font BIN.
 *  This structure MUST sync with GUI gen
 */
#pragma pack(4)
typedef struct {
    UINT16 Height;                          /**< total height of character                      */
    UINT16 Start;                           /**< start page of the language                     */
    UINT16 End;                             /**< end page of the language                       */
    UINT16 First;                           /**< first unicode of the language                  */
    UINT16 Last;                            /**< last of the language                           */
    char Name[24];                          /**< name of the language                           */
    UINT16 Flags;                           /**< reversed for loader's flags                    */
    UINT16 Reversed[6];                     /**< reversed for future use                        */
} AMBA_DRAW_BMPFONT_BIN_LANG_s;
#pragma pack()

/**
 *  The definition of BMP font BIN.
 *  This structure MUST sync with GUI gen
 */
#pragma pack(4)
typedef struct {
    UINT8   Type;                           /**< bit-flags defined below                        */
    UINT8   Ascent;                         /**< Ascent above baseline                          */
    UINT8   Descent;                        /**< Descent below baseline                         */
    UINT8   Height;                         /**< total height of character                      */
    UINT16  LineBytes;                      /**< total bytes (width) of one scanline            */
    UINT16  FirstChar;                      /**< first character present in font (page)         */
    UINT16  LastChar;                       /**< last character present in font (page)          */
    UINT32  Offset;                         /**< bit-offsets for variable-width font            */
    UINT32  Next;                           /**< NULL unless multi-page Unicode font            */
    UINT32  Data;                           /**< character bitmap data array                    */
} AMBA_DRAW_BMPFONT_BIN_FONT_s;
#pragma pack()

/**
 *  BMP Header Format
 *  This structure MUST sync with GUI gen
 */
typedef struct {
    UINT16  Magic;                          /**< Magic number                                   */
    UINT16  Version;                        /**< Version                                        */
    UINT16  HeaderSize;                     /**< sizeof (AMBA_DRAW_BMP_BIN_HEADER_s)            */
    UINT16  DescSize;                       /**< sizeof (AMBA_DRAW_BMP_BIN_DESC_s)              */
    UINT16  ResNum;                         /**< number of resolution (>= 1)                    */
    UINT16  BmpNum;                         /**< number of bmp (>= 1)                           */
    UINT16  PixelFormat;                    /**< pixel format                                   */
    UINT16  Reversed[9];                    /**< future use                                     */
} AMBA_DRAW_BMP_BIN_HEADER_s;

/**
 *  BMP Format
 *  This structure MUST sync with GUI gen
 */
typedef struct {
    UINT8 Flags;                            /**< combination of flags (ex. AMBA_DRAW_ENCODE_BMP_BMF2_RAW) */
    UINT8  Bits;                            /**< 1, 2, 4, 8, 16, or 24                          */
    UINT16 Width;                           /**< in pixels                                      */
    UINT16 Height;                          /**< in pixels                                      */
    UINT8 PixelFormat;                      /**< bitmap pixel format                            */
    UINT8  Reserve;                         /**< reserve byte                                   */
    UINT32 TransColor;                      /**< transparent color for > 8bpp bitmaps           */
    UINT32  Data;                           /**< bitmap data pointer                            */
} AMBA_DRAW_BMP_BIN_s;

/**
 *  BMP Descritor Format
 *  This structure MUST sync with GUI gen
 */
typedef struct {
    UINT32 Offset;                          /**< file position                                  */
    UINT32 Size;                            /**< bmp size                                       */
    UINT8 Flags;                            /**< internal flags for loader (ex. BMP_STATUS_INIT)*/
    UINT16 Count;                           /**< internal count for loader                      */
    UINT32 BmpPtr;                          /**< internal pointer for loader                    */
} AMBA_DRAW_BMP_BIN_DESC_s;

/*************************************************************************
 * Draw APIs
 ************************************************************************/
UINT32 AmbaDraw_Init(const AMBA_DRAW_INIT_CONFIG_s *InitConfig);

UINT32 AmbaDraw_GetInitDefaultCfg(AMBA_DRAW_INIT_CONFIG_s *InitConfig);

UINT32 AmbaDraw_GetInitBufferSize(const AMBA_DRAW_INIT_CONFIG_s *InitConfig, UINT32 *BufferSize);

UINT32 AmbaDraw_DrawBuffer(UINT16 ObjId, INT32 X, INT32 Y, const AMBA_DRAW_BUFFER_INFO_s *BufferInfo, const AMBA_DRAW_AREA_s *Area, UINT32 DrawOption);

UINT32 AmbaDraw_GetObjArea(UINT16 ObjId, INT32 X, INT32 Y, AMBA_DRAW_AREA_s *Area);

UINT32 AmbaDraw_CreateLine(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 *ObjId);

UINT32 AmbaDraw_CreateRect(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 *ObjId);

UINT32 AmbaDraw_CreateCircle(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 *ObjId);

UINT32 AmbaDraw_CreateBMP(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 *ObjId);

UINT32 AmbaDraw_CreateString(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 *ObjId);

UINT32 AmbaDraw_CreatePoly(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 *ObjId);

UINT32 AmbaDraw_CreateCurve(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 *ObjId);

UINT32 AmbaDraw_UpdateLine(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 ObjId);

UINT32 AmbaDraw_UpdateRect(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 ObjId);

UINT32 AmbaDraw_UpdateCircle(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 ObjId);

UINT32 AmbaDraw_UpdateBMP(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 ObjId);

UINT32 AmbaDraw_UpdateString(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 ObjId);

UINT32 AmbaDraw_UpdatePoly(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 ObjId);

UINT32 AmbaDraw_UpdateCurve(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 ObjId);

#endif /* AMBADRAW_H */

