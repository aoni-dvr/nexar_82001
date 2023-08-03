/**
*  @file SvcOsd.h
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
*  @details svc application cv handler
*
*/

#ifndef SVC_OSD_H
#define SVC_OSD_H

#define SVC_OSD_CHAN_NUM            (2U)

#define SVC_OSD_MEM_TYPE_NONCACHE   (0U)
#define SVC_OSD_MEM_TYPE_CACHE      (1U)

#ifndef CONFIG_ICAM_32BITS_OSD_USED
#define SVC_OSD_FORMAT              OSD_8BIT_CLUT_MODE
#define SVC_OSD_PIXEL_SIZE_SFT      (0U)   /* 0: 8 bit */
#else
#define SVC_OSD_FORMAT              OSD_32BIT_ABGR_8888
#define SVC_OSD_PIXEL_SIZE_SFT      (2U)   /* 2: 32 bit */
#endif


#ifndef CONFIG_SVC_LVGL_USED
#define SVC_OSD_CHAR_WIDTH          (5U)
#define SVC_OSD_CHAR_HEIGHT         (10U)
#define SVC_OSD_CHAR_GAP            (2U)
#else
#define SVC_OSD_CHAR_WIDTH          (8U)
#define SVC_OSD_CHAR_HEIGHT         (10U)
#define SVC_OSD_CHAR_GAP            (0U)
#endif


typedef void (*DrawPixelFunc_t)(const UINT8 *Addr, UINT32 Color);

typedef struct {
    UINT8 PixelFormat; /**< osd buffer color format */
    UINT32 BufferPitch; /**< osd buffer pitch */
    UINT32 BufferWidth; /**< osd buffer width */
    UINT32 BufferHeight; /**< osd buffer height */
    UINT32 BufferSize;
    UINT16 WindowOffsetX;
    UINT16 WindowOffsetY;
    UINT16 WindowWidth;
    UINT16 WindowHeight;
    UINT8 Interlace;
    UINT8 Reserve8;
    UINT16 Reserve16;
} SvcOSD_ExtBuffer_CFG_s;

typedef struct {
    UINT16  Width;
    UINT16  Height;
} SVC_OSD_SIZE_s;

typedef struct {
    UINT16  OffsetX;
    UINT16  OffsetY;
    UINT16  Width;
    UINT16  Height;
} SVC_OSD_WINDOW_s;

typedef struct {
    SVC_OSD_SIZE_s OsdSize;
    SVC_OSD_SIZE_s VoutSize;
    UINT8          VoutInterlace;
    UINT8          MemType;     /* 0-Noncache, 1-Cache */
    UINT8          *pOsdMem;

    UINT8          *pClut;      /* NULL-Use internal color lookup table */
    UINT32         ClutSize;
} SVC_OSD_CONFIG_s;

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
} SVC_OSD_SW_RESCALE_CFG_s;



typedef struct {
    UINT32 cf : 5;          /* Color format: See `lv_img_color_format_t`*/
    UINT32 always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/

    UINT32 reserved : 2; /*Reserved to be used later*/

    UINT32 w : 11; /*Width of the image map*/
    UINT32 h : 11; /*Height of     the image map*/
} SVC_OSD_BMP_HEADER_s;


typedef struct {
    SVC_OSD_BMP_HEADER_s header;
    UINT32 data_size;
    UINT8 * data;
} SVC_OSD_BMP_s;


UINT32 SvcOsd_Init(UINT32 Chan, const SVC_OSD_CONFIG_s *pConfig);
UINT32 SvcOsd_DrawRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color, UINT32 Thickness);
UINT32 SvcOsd_DrawSolidRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color);
UINT32 SvcOsd_DrawString(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 FontSize, UINT32 Color, const char *pString);
UINT32 SvcOsd_DrawCircle(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Radius, UINT32 LineWidth, UINT32 Color);
UINT32 SvcOsd_DrawLine(UINT32 Chan, UINT32 X1, UINT32 Y1, UINT32 X2, UINT32 Y2, UINT32 LineWidth, UINT32 Color);
#ifndef CONFIG_ICAM_32BITS_OSD_USED
UINT32 SvcOsd_DrawSeg(UINT32 Chan, const UINT8 *pArgMaxOut, UINT32 RoiWidth, UINT32 RoiHeight, UINT32 RoiStartX, UINT32 RoiStartY,
  UINT32 NetworkWidth, UINT32 NetworkHeight);
#endif
UINT32 SvcOsd_Clear(UINT32 Chan);
UINT32 SvcOsd_ClearBlock(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend);
UINT32 SvcOsd_LoadBmp(void *pDestBuf, UINT32 BufSize, const char *pPath);
UINT32 SvcOsd_LoadBmpReverse(void *pDestBuf, UINT32 Pitch, UINT32 Width, UINT32 Height, const char *pPath);
UINT32 SvcOsd_DrawBmp(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, const void *pBmpInfo);
UINT32 SvcOsd_DrawBin(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, const void *pBinBuf);
UINT32 SvcOsd_DrawBin_Rescale(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, SVC_OSD_SW_RESCALE_CFG_s *pRescaleCfg);
UINT32 SvcOsd_Flush(UINT32 Chan);
UINT32 SvcOsd_SetExtCLUT(UINT32 Chan, const UINT32 *pCLUT);
UINT32 SvcOsd_SetExtBufInfo(UINT32 Chan, const SvcOSD_ExtBuffer_CFG_s *pBufConfig);
UINT32 SvcOsd_UpdateExtBuf(UINT32 Chan, const UINT8 *pOSDBufAddr);
UINT32 SvcOsd_ReleaseExtOSD(UINT32 Chan);
UINT32 SvcOsd_SetWorkBuf(UINT8 *pOsdWorkMem);

UINT32 SvcOsd_GetOsdBufSize(UINT32 Chan, UINT32 *pWidth, UINT32 *pHeight);
void   SvcOsd_SetClut(UINT32 Idx, UINT32 Color);
UINT32 SvcOsd_GetOsdCLUT(UINT32 Chan, UINT32 **pCLUT, UINT32 *pNumColor);
UINT32 SvcOsd_GetOsdColor(UINT32 Chan, UINT32 Idx, UINT32 *pColor);
void   SvcOsd_ColorIdxToColorSetting(UINT32 Chan, UINT32 Idx, UINT32 *pColor);

UINT32 SvcOsd_TransferLivWinToOsdWin(UINT32 Chan, SVC_OSD_WINDOW_s *pVideoWin, SVC_OSD_WINDOW_s *pOsdWin);
UINT32 SvcOsd_MemSizeQuery(UINT32 Chan, UINT32 *pMemSize);
UINT32 SvcOsd_WorkMemSizeQuery(UINT32 *pMemSize);

#ifdef CONFIG_SVC_LVGL_USED
UINT32 SvcOsd_DrawMultiplePointsLine(UINT32 Chan, void *Points, UINT32 PointNum,UINT32 LineWidth, UINT32 Color);
//UINT32 SvcOsd_DrawBmp_Refine(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Width, UINT32 Height, void *pInfo);

void SvcOsd_LvglDraw(UINT32 VoutID, UINT32 Level);
void SvcOsd_SetLvglBuffer(UINT32 Chan);
void SvcOsd_DrawDiagBoxRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Thickness, UINT32 Bg_Color, UINT32 Border_Color);
#endif
UINT32 SvcOsd_GetFontRes(UINT32 font_size, UINT32 *Width, UINT32 *Height);

#endif  /* SVC_SVC_OSD_H */
