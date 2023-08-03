/**
 * @file AmbaDraw_Render.c
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

#include "AmbaDraw_Render.h"
#include "AmbaDraw_Common.h"

/**
 *  Move to (X,Y)
 *
 *  @param [in]  Render             The draw render
 *  @param [out] Dst                The returned location
 *  @param [in]  X                  X coordinate (in pixel)
 *  @param [in]  Y                  Y coordinate (in pixel)
 */
static void AmbaDrawRender_MoveTo(const AMBA_DRAW_RENDER_s *Render, UINT8 **Dst, INT32 X, INT32 Y)
{
    *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * (INT32)Render->BufferPixelSize)];
}

/**
 *  Move to (X,Y)
 *
 *  @param [in]  Render             The draw render
 *  @param [out] Dst                The returned location
 *  @param [in]  X                  X coordinate (in pixel)
 *  @param [in]  Y                  Y coordinate (in pixel)
 */
static void AmbaDrawRender_MoveTo_YUV(const AMBA_DRAW_RENDER_s *Render, UINT8 **Dst, INT32 X, INT32 Y)
{
    /* Cannot support YUV format */
    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Not supported in YUV format", __func__, NULL, NULL, NULL, NULL);
    AmbaMisra_TouchUnused(&Render);
    AmbaMisra_TouchUnused(Dst);
    AmbaMisra_TouchUnused(&X);
    AmbaMisra_TouchUnused(&Y);
}

//
// Move to the next pixel
// There's no checking of whether the address is aligned to a pixel (for performance), so user need to make sure of it
//
static void AmbaDrawRender_MoveNext_U8(UINT8** Dst)
{
    (*Dst)++;
}

static void AmbaDrawRender_MoveNext_U16(UINT8** Dst)
{
    (*Dst) = &(*Dst)[2];
}

static void AmbaDrawRender_MoveNext_U32(UINT8** Dst)
{
    (*Dst) = &(*Dst)[4];
}

static void AmbaDrawRender_MoveNext_YUV(UINT8** Dst)
{
    /* Cannot support YUV format */
    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Not supported in YUV format", __func__, NULL, NULL, NULL, NULL);
    AmbaMisra_TouchUnused(Dst);
}

//
// plot pixel impl
// There's no checking of whether the address is aligned to a pixel (for performance), so user need to make sure of it
//

/**
 *  Plot a pixel in the buffer.
 *  Only for 8/16/32 bit format. Not for YUV420/YUV422 format.
 *
 *  @param [in] Dst                 The location to set color
 *  @param [in] Color               The color to plot
 *  @param [in] BufferPixelSize     The buffer pixel size (How many BYTE to store a pixel)
 *
 *  @return 0 - OK, others - Error
 */
static inline void AmbaDrawRender_PlotPixel(UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color, UINT32 BufferPixelSize)
{
    for (UINT32 i = 0U; i < BufferPixelSize; ++i) {
        Dst[i] = Color->Data[i];
    }
}

static inline void AmbaDrawRender_Plot_AXXX_4444(UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    /*
     * Data[0] = (UINT8)ColorU16
     * Data[1] = (UINT8)(ColorU16 >> 8)
     */
    UINT32 SrcA = (UINT32)Color->Data[1] >> 4U;
    UINT32 SrcX = (UINT32)Color->Data[1] & 0x0FU;
    UINT32 SrcY = (UINT32)Color->Data[0] >> 4U;
    UINT32 SrcZ = (UINT32)Color->Data[0] & 0x0FU;
    UINT32 DstA = (UINT32)Dst[1] >> 4U;
    UINT32 DstX = (UINT32)Dst[1] & 0x0FU;
    UINT32 DstY = (UINT32)Dst[0] >> 4U;
    UINT32 DstZ = (UINT32)Dst[0] & 0x0FU;
    UINT32 Temp = (DstA * (15U - SrcA)) / 15U;
    UINT32 NewA = SrcA + Temp;
    UINT32 NewX;
    UINT32 NewY;
    UINT32 NewZ;
    if (NewA != 0U) {
        NewX = ((SrcX * SrcA) + (DstX * Temp)) / NewA;
        NewY = ((SrcY * SrcA) + (DstY * Temp)) / NewA;
        NewZ = ((SrcZ * SrcA) + (DstZ * Temp)) / NewA;
        Dst[1] = ((UINT8)NewA << 4U) + (UINT8)NewX;
        Dst[0] = ((UINT8)NewY << 4U) + (UINT8)NewZ;
    } else {
        /* Do nothing. Only possible when SrcA == DstA == 0. */
    }
}

static inline void AmbaDrawRender_Plot_XXXA_4444(UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    /*
     * Data[0] = (UINT8)ColorU16
     * Data[1] = (UINT8)(ColorU16 >> 8)
     */
    UINT32 SrcX = (UINT32)Color->Data[1] >> 4U;
    UINT32 SrcY = (UINT32)Color->Data[1] & 0x0FU;
    UINT32 SrcZ = (UINT32)Color->Data[0] >> 4U;
    UINT32 SrcA = (UINT32)Color->Data[0] & 0x0FU;
    UINT32 DstX = (UINT32)Dst[1] >> 4U;
    UINT32 DstY = (UINT32)Dst[1] & 0x0FU;
    UINT32 DstZ = (UINT32)Dst[0] >> 4U;
    UINT32 DstA = (UINT32)Dst[0] & 0x0FU;
    UINT32 Temp = (DstA * (15U - SrcA)) / 15U;
    UINT32 NewA = SrcA + Temp;
    UINT32 NewX;
    UINT32 NewY;
    UINT32 NewZ;
    if (NewA != 0U) {
        NewX = ((SrcX * SrcA) + (DstX * Temp)) / NewA;
        NewY = ((SrcY * SrcA) + (DstY * Temp)) / NewA;
        NewZ = ((SrcZ * SrcA) + (DstZ * Temp)) / NewA;
        Dst[1] = ((UINT8)NewX << 4U) + (UINT8)NewY;
        Dst[0] = ((UINT8)NewZ << 4U) + (UINT8)NewA;
    } else {
        /* Do nothing. Only possible when SrcA == DstA == 0. */
    }
}

static inline void AmbaDrawRender_Plot_AXXX_1555(UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    if (((UINT32)Color->Data[0] & 0x80U) != 0U) {
        AmbaDrawRender_PlotPixel(Dst, Color, 2U);
    } else {
        /* Do nothing because Color is transparent */
    }
}

static inline void AmbaDrawRender_Plot_XXXA_5551(UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    if (((UINT32)Color->Data[1] & 0x01U) != 0U) {
        AmbaDrawRender_PlotPixel(Dst, Color, 2U);
    } else {
        /* Do nothing because Color is transparent */
    }
}

static inline void AmbaDrawRender_Plot_AXXX_8888(UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    /*
     * Data[0] = (UINT8)ColorU32
     * Data[1] = (UINT8)(ColorU32 >> 8)
     * Data[2] = (UINT8)(ColorU32 >> 16)
     * Data[3] = (UINT8)(ColorU32 >> 24)
     */
    UINT32 SrcA = (UINT32)Color->Data[3];
    UINT32 SrcX = (UINT32)Color->Data[2];
    UINT32 SrcY = (UINT32)Color->Data[1];
    UINT32 SrcZ = (UINT32)Color->Data[0];
    UINT32 DstA = (UINT32)Dst[3];
    UINT32 DstX = (UINT32)Dst[2];
    UINT32 DstY = (UINT32)Dst[1];
    UINT32 DstZ = (UINT32)Dst[0];
    UINT32 Temp = (DstA * (255U - SrcA)) / 255U;
    UINT32 NewA = SrcA + Temp;
    UINT32 NewX;
    UINT32 NewY;
    UINT32 NewZ;
    if (NewA != 0U) {
        NewX = ((SrcX * SrcA) + (DstX * Temp)) / NewA;
        NewY = ((SrcY * SrcA) + (DstY * Temp)) / NewA;
        NewZ = ((SrcZ * SrcA) + (DstZ * Temp)) / NewA;
        Dst[3] = (UINT8)NewA;
        Dst[2] = (UINT8)NewX;
        Dst[1] = (UINT8)NewY;
        Dst[0] = (UINT8)NewZ;
    } else {
        /* Do nothing. Only possible when SrcA == DstA == 0. */
    }
}

static inline void AmbaDrawRender_Plot_XXXA_8888(UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    /*
     * Data[0] = (UINT8)ColorU32
     * Data[1] = (UINT8)(ColorU32 >> 8)
     * Data[2] = (UINT8)(ColorU32 >> 16)
     * Data[3] = (UINT8)(ColorU32 >> 24)
     */
    UINT32 SrcX = (UINT32)Color->Data[3];
    UINT32 SrcY = (UINT32)Color->Data[2];
    UINT32 SrcZ = (UINT32)Color->Data[1];
    UINT32 SrcA = (UINT32)Color->Data[0];
    UINT32 DstX = (UINT32)Dst[3];
    UINT32 DstY = (UINT32)Dst[2];
    UINT32 DstZ = (UINT32)Dst[1];
    UINT32 DstA = (UINT32)Dst[0];
    UINT32 Temp = (DstA * (255U - SrcA)) / 255U;
    UINT32 NewA = SrcA + Temp;
    UINT32 NewX;
    UINT32 NewY;
    UINT32 NewZ;
    if (NewA != 0U) {
        NewX = ((SrcX * SrcA) + (DstX * Temp)) / NewA;
        NewY = ((SrcY * SrcA) + (DstY * Temp)) / NewA;
        NewZ = ((SrcZ * SrcA) + (DstZ * Temp)) / NewA;
        Dst[3] = (UINT8)NewX;
        Dst[2] = (UINT8)NewY;
        Dst[1] = (UINT8)NewZ;
        Dst[0] = (UINT8)NewA;
    } else {
        /* Do nothing. Only possible when SrcA == DstA == 0. */
    }
}

static inline void AmbaDrawRender_Plot_YUV420_Blend(UINT8 *DstY, UINT8 *DstUV, UINT8 *DstAlphaY, UINT8 *DstAlphaUV, UINT8 IsOddX, UINT8 IsOddY, const AMBA_DRAW_COLOR_s *Color)
{
    UINT32 SrcAValue;
    UINT32 SrcValue;
    UINT32 DstAValue;
    UINT32 DstValue;
    UINT32 Temp;
    UINT32 NewAValue;
    UINT32 NewValue;
    /* Set DstAlphaY, DstY */
    SrcAValue = (UINT32)Color->Data[4U + IsOddX];
    SrcValue = (UINT32)Color->Data[0U + IsOddX];
    DstAValue = (UINT32)(*DstAlphaY);
    DstValue = (UINT32)(*DstY);
    Temp = (DstAValue * (255U - SrcAValue)) / 255U;
    NewAValue = SrcAValue + Temp;
    if (NewAValue != 0U) {
        NewValue = ((SrcValue * SrcAValue) + (DstValue * Temp)) / NewAValue;
        *DstAlphaY = (UINT8)NewAValue;
        *DstY      = (UINT8)NewValue;
    } else {
        /* Do nothing. Only possible when SrcA == DstA == 0. */
    }
    /* Set DstAlphaUV, DstUV */
    if (IsOddY == 0U) {
        SrcAValue = (UINT32)Color->Data[6U + IsOddX];
        SrcValue = (UINT32)Color->Data[2U + IsOddX];
        DstAValue = (UINT32)(*DstAlphaUV);
        DstValue = (UINT32)(*DstUV);
        Temp = (DstAValue * (255U - SrcAValue)) / 255U;
        NewAValue = SrcAValue + Temp;
        if (NewAValue != 0U) {
            NewValue = ((SrcValue * SrcAValue) + (DstValue * Temp)) / NewAValue;
            *DstAlphaUV = (UINT8)NewAValue;
            *DstUV      = (UINT8)NewValue;
        } else {
            /* Do nothing. Only possible when SrcA == DstA == 0. */
        }
    }
}

static inline void AmbaDrawRender_Plot_YUV422_Blend(UINT8 *DstY, UINT8 *DstUV, UINT8 *DstAlphaY, UINT8 *DstAlphaUV, UINT8 IsOddX, const AMBA_DRAW_COLOR_s *Color)
{
    UINT32 SrcAValue;
    UINT32 SrcValue;
    UINT32 DstAValue;
    UINT32 DstValue;
    UINT32 Temp;
    UINT32 NewAValue;
    UINT32 NewValue;
    /* Set DstAlphaY, DstY */
    SrcAValue = (UINT32)Color->Data[4U + IsOddX];
    SrcValue = (UINT32)Color->Data[0U + IsOddX];
    DstAValue = (UINT32)(*DstAlphaY);
    DstValue = (UINT32)(*DstY);
    Temp = (DstAValue * (255U - SrcAValue)) / 255U;
    NewAValue = SrcAValue + Temp;
    if (NewAValue != 0U) {
        NewValue = ((SrcValue * SrcAValue) + (DstValue * Temp)) / NewAValue;
        *DstAlphaY = (UINT8)NewAValue;
        *DstY      = (UINT8)NewValue;
    } else {
        /* Do nothing. Only possible when SrcA == DstA == 0. */
    }
    /* Set DstAlphaUV, DstUV */
    SrcAValue = (UINT32)Color->Data[6U + IsOddX];
    SrcValue = (UINT32)Color->Data[2U + IsOddX];
    DstAValue = (UINT32)(*DstAlphaUV);
    DstValue = (UINT32)(*DstUV);
    Temp = (DstAValue * (255U - SrcAValue)) / 255U;
    NewAValue = SrcAValue + Temp;
    if (NewAValue != 0U) {
        NewValue = ((SrcValue * SrcAValue) + (DstValue * Temp)) / NewAValue;
        *DstAlphaUV = (UINT8)NewAValue;
        *DstUV      = (UINT8)NewValue;
    } else {
        /* Do nothing. Only possible when SrcA == DstA == 0. */
    }
}

/**
 *  Get the point (X, Y) at the location.
 *
 *  @param [in] Render              The draw render
 *  @param [in] Dst                 The location
 *  @param [out] X                  The returned X coordinate (in pixel)
 *  @param [out] Y                  The returned Y coordinate (in pixel)
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRender_GetPoint(const AMBA_DRAW_RENDER_s *Render, const UINT8 *Dst, INT32 *X, INT32 *Y)
{
    UINT32 Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    ULONG DstUL;
    ULONG BufferStart;
    AmbaMisra_TypeCast(&DstUL, &Dst);
    AmbaMisra_TypeCast(&BufferStart, &Render->BufferInfo.RGBAddr);
    if ((DstUL >= BufferStart) && (DstUL < (BufferStart + ((ULONG)Render->BufferInfo.Pitch * (ULONG)Render->BufferInfo.Height)))) {
        ULONG XTemp = (DstUL - BufferStart) % Render->BufferInfo.Pitch;
        ULONG YTemp = (DstUL - BufferStart) / Render->BufferInfo.Pitch;
        /* TODO: XTemp should be aligned to pixel size. Deside whether to check it at the cost of performance. */
        XTemp /= Render->BufferPixelSize; /* Change to pixel size */
        *X = (INT32)XTemp;
        *Y = (INT32)YTemp;
        Rval = DRAW_OK;
    } else {
        /* Do not print error. Allow user to query a point out of range. */
        /*AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_GetPoint: [ERROR] Dst %p out of range", DstUL, 0U, 0U, 0U, 0U);*/
    }

    return Rval;
}

/**
 *  Check whether the point (X, Y) is in the range of Render->DrawArea.
 *
 *  @param [in] Render              The draw render
 *  @param [in] X                   X coordinate (in pixel)
 *  @param [in] Y                   Y coordinate (in pixel)
 *
 *  @return 0 - Point is in DrawArea, others - Point is out of range
 */
static UINT32 AmbaDrawRender_CheckRange(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y)
{
    UINT32 Rval = DRAW_OK;
    if ((X < Render->DrawArea.X) ||
        (Y < Render->DrawArea.Y) ||
        (X >= (Render->DrawArea.X + (INT32)Render->DrawArea.Width)) ||
        (Y >= (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        Rval = DRAW_ERROR_GENERAL_ERROR;
    }
    return Rval;
}

static void AmbaDrawRender_PlotPixel_U8(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_PlotPixel(Dst, Color, 1U);
        }
    }
}

static void AmbaDrawRender_PlotPixel_U16(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_PlotPixel(Dst, Color, 2U);
        }
    }
}

static void AmbaDrawRender_PlotPixel_U32(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_PlotPixel(Dst, Color, 4U);
        }
    }
}

static void AmbaDrawRender_PlotPixel_YUV(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    /* Cannot support YUV format */
    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Not supported in YUV format", __func__, NULL, NULL, NULL, NULL);
    AmbaMisra_TouchUnused(&Render);
    AmbaMisra_TouchUnused(Dst);
    AmbaMisra_TouchUnused(&Color);
}

static void AmbaDrawRender_PlotPixel_Blend(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    /* Cannot support */
    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_PlotPixel_Blend: [ERROR] Not supported with format %u", Render->BufferInfo.PixelFormat, 0U, 0U, 0U, 0U);
    AmbaMisra_TouchUnused(&Render);
    AmbaMisra_TouchUnused(Dst);
    AmbaMisra_TouchUnused(&Color);
}

static void AmbaDrawRender_PlotPixel_AXXX_4444_Blend(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_Plot_AXXX_4444(Dst, Color);
        }
    }
}

static void AmbaDrawRender_PlotPixel_XXXA_4444_Blend(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_Plot_XXXA_4444(Dst, Color);
        }
    }
}

static void AmbaDrawRender_PlotPixel_AXXX_1555_Blend(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_Plot_AXXX_1555(Dst, Color);
        }
    }
}

static void AmbaDrawRender_PlotPixel_XXXA_5551_Blend(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_Plot_XXXA_5551(Dst, Color);
        }
    }
}

static void AmbaDrawRender_PlotPixel_AXXX_8888_Blend(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_Plot_AXXX_8888(Dst, Color);
        }
    }
}

static void AmbaDrawRender_PlotPixel_XXXA_8888_Blend(const AMBA_DRAW_RENDER_s *Render, UINT8 *Dst, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 X;
    INT32 Y;
    if (AmbaDrawRender_GetPoint(Render, Dst, &X, &Y) == DRAW_OK) {
        if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
            AmbaDrawRender_Plot_XXXA_8888(Dst, Color);
        }
    }
}

//
// plot pixel at (x,y) impl
//

static void AmbaDrawRender_PlotPixelAt_U8(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        AmbaDrawRender_PlotPixel(Dst, Color, 1);
    }
}

static void AmbaDrawRender_PlotPixelAt_U16(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 2)];
        AmbaDrawRender_PlotPixel(Dst, Color, 2);
    }
}

static void AmbaDrawRender_PlotPixelAt_U32(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 4)];
        AmbaDrawRender_PlotPixel(Dst, Color, 4);
    }
}

static void AmbaDrawRender_YUV420_PlotPixelAt(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 IsOddX = ((X % 2) == 0) ? (0U) : (1U);
        *DstY      = Color->Data[0U + IsOddX];
        *DstAlphaY = Color->Data[4U + IsOddX];
        if ((Y % 2) == 0) {
            UINT8 *DstUV      = &Render->BufferInfo.UVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (X)];
            UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (X)];
            *DstUV      = Color->Data[2U + IsOddX];
            *DstAlphaUV = Color->Data[6U + IsOddX];
        }
    }
}

static void AmbaDrawRender_YUV422_PlotPixelAt(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstUV      = &Render->BufferInfo.UVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 IsOddX = ((X % 2) == 0) ? (0U) : (1U);
        *DstY       = Color->Data[0U + IsOddX];
        *DstUV      = Color->Data[2U + IsOddX];
        *DstAlphaY  = Color->Data[4U + IsOddX];
        *DstAlphaUV = Color->Data[6U + IsOddX];
    }
}

static void AmbaDrawRender_PlotPxlAt_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    /* Cannot support */
    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_PlotPxlAt_Blend: [ERROR] Not supported with format %u", Render->BufferInfo.PixelFormat, 0U, 0U, 0U, 0U);
    AmbaMisra_TouchUnused(&Render);
    AmbaMisra_TouchUnused(&X);
    AmbaMisra_TouchUnused(&Y);
    AmbaMisra_TouchUnused(&Color);
}

static void AmbaDrawRender_PlotPxlAt_AXXX_4444_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 2)];
        AmbaDrawRender_Plot_AXXX_4444(Dst, Color);
    }
}

static void AmbaDrawRender_PlotPxlAt_XXXA_4444_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 2)];
        AmbaDrawRender_Plot_XXXA_4444(Dst, Color);
    }
}

static void AmbaDrawRender_PlotPxlAt_AXXX_1555_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 2)];
        AmbaDrawRender_Plot_AXXX_1555(Dst, Color);
    }
}

static void AmbaDrawRender_PlotPxlAt_XXXA_5551_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 2)];
        AmbaDrawRender_Plot_XXXA_5551(Dst, Color);
    }
}

static void AmbaDrawRender_PlotPxlAt_AXXX_8888_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 4)];
        AmbaDrawRender_Plot_AXXX_8888(Dst, Color);
    }
}

static void AmbaDrawRender_PlotPxlAt_XXXA_8888_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 4)];
        AmbaDrawRender_Plot_XXXA_8888(Dst, Color);
    }
}

static void AmbaDrawRender_PlotPxlAt_YUV420_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstUV      = &Render->BufferInfo.UVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 IsOddX = ((X % 2) == 0) ? (0U) : (1U);
        UINT8 IsOddY = ((Y % 2) == 0) ? (0U) : (1U);
        AmbaDrawRender_Plot_YUV420_Blend(DstY, DstUV, DstAlphaY, DstAlphaUV, IsOddX, IsOddY, Color);
    }
}

static void AmbaDrawRender_PlotPxlAt_YUV422_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    if (AmbaDrawRender_CheckRange(Render, X, Y) == DRAW_OK) {
        UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstUV      = &Render->BufferInfo.UVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        UINT8 IsOddX = ((X % 2) == 0) ? (0U) : (1U);
        AmbaDrawRender_Plot_YUV422_Blend(DstY, DstUV, DstAlphaY, DstAlphaUV, IsOddX, Color);
    }
}

static void AmbaDrawRender_PlotLineAt_U8(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + (UINT32)XStart);
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + (UINT32)XEnd);
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_PlotPixel(&Render->BufferInfo.RGBAddr[Dst], Color, Render->BufferPixelSize);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLineAt_U16(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_PlotPixel(&Render->BufferInfo.RGBAddr[Dst], Color, Render->BufferPixelSize);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLineAt_U32(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_PlotPixel(&Render->BufferInfo.RGBAddr[Dst], Color, Render->BufferPixelSize);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_YUV420_PlotLineAt(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XLimit = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        INT32 Len = XLimit - XStart;
        if (Len > 0) {
            UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 IsOddX = ((XStart % 2) == 0) ? (0U) : (1U);

            for (INT32 i = 0; i < Len; ++i) {
                DstY[i]      = Color->Data[0U + IsOddX];
                DstAlphaY[i] = Color->Data[4U + IsOddX];

                if ((Y % 2) == 0) {
                    UINT8 *DstUV      = &Render->BufferInfo.UVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (XStart)];
                    UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (XStart)];
                    DstUV[i]      = Color->Data[2U + IsOddX];
                    DstAlphaUV[i] = Color->Data[6U + IsOddX];
                }
                if (IsOddX == 0U) {
                    IsOddX = 1U;
                } else {
                    IsOddX = 0U;
                }
            }
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_YUV422_PlotLineAt(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XLimit = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        INT32 Len = XLimit - XStart;
        if (Len > 0) {
            UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstUV      = &Render->BufferInfo.UVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 IsOddX = ((XStart % 2) == 0) ? (0U) : (1U);

            for (INT32 i = 0; i < Len; ++i) {
                DstY[i]       = Color->Data[0U + IsOddX];
                DstUV[i]      = Color->Data[2U + IsOddX];
                DstAlphaY[i]  = Color->Data[4U + IsOddX];
                DstAlphaUV[i] = Color->Data[6U + IsOddX];
                if (IsOddX == 0U) {
                    IsOddX = 1U;
                } else {
                    IsOddX = 0U;
                }
            }
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    /* Cannot support */
    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_PlotLnAt_Blend: [ERROR] Not supported with format %u", Render->BufferInfo.PixelFormat, 0U, 0U, 0U, 0U);
    AmbaMisra_TouchUnused(&Render);
    AmbaMisra_TouchUnused(&X);
    AmbaMisra_TouchUnused(&Y);
    AmbaMisra_TouchUnused(&W);
    AmbaMisra_TouchUnused(&Color);
}

static void AmbaDrawRender_PlotLnAt_AXXX_4444_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_Plot_AXXX_4444(&Render->BufferInfo.RGBAddr[Dst], Color);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_XXXA_4444_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_Plot_XXXA_4444(&Render->BufferInfo.RGBAddr[Dst], Color);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_AXXX_1555_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_Plot_AXXX_1555(&Render->BufferInfo.RGBAddr[Dst], Color);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_XXXA_5551_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_Plot_XXXA_5551(&Render->BufferInfo.RGBAddr[Dst], Color);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_AXXX_8888_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_Plot_AXXX_8888(&Render->BufferInfo.RGBAddr[Dst], Color);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_XXXA_8888_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XEnd = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        /* Render->DrawArea should be in buffer area, so XStart, XEnd, and Y are also in the buffer area after the check. */
        UINT32 DstStart = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XStart * Render->BufferPixelSize));
        UINT32 DstEnd = (((UINT32)Y * Render->BufferInfo.Pitch) + ((UINT32)XEnd * Render->BufferPixelSize));
        /* Plot line from XStart to (XEnd - 1) */
        for (UINT32 Dst = DstStart; Dst < DstEnd; Dst += Render->BufferPixelSize) {
            AmbaDrawRender_Plot_XXXA_8888(&Render->BufferInfo.RGBAddr[Dst], Color);
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_YUV420_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XLimit = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        INT32 Len = XLimit - XStart;
        if (Len > 0) {
            UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstUV      = &Render->BufferInfo.UVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 IsOddX = ((XStart % 2) == 0) ? (0U) : (1U);
            UINT8 IsOddY = ((Y % 2) == 0) ? (0U) : (1U);
            for (INT32 i = 0; i < Len; ++i) {
                AmbaDrawRender_Plot_YUV420_Blend(&DstY[i], &DstUV[i], &DstAlphaY[i], &DstAlphaUV[i], IsOddX, IsOddY, Color);
                if (IsOddX == 0U) {
                    IsOddX = 1U;
                } else {
                    IsOddX = 0U;
                }
            }
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

static void AmbaDrawRender_PlotLnAt_YUV422_Blend(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, UINT32 W, const AMBA_DRAW_COLOR_s *Color)
{
    if ((Render->DrawArea.Y <= Y) && (Y < (Render->DrawArea.Y + (INT32)Render->DrawArea.Height))) {
        /* Consider draw area. */
        INT32 XStart = AmbaDraw_Max(X, Render->DrawArea.X);
        INT32 XLimit = AmbaDraw_Min(X + (INT32)W, Render->DrawArea.X + (INT32)Render->DrawArea.Width);
        INT32 Len = XLimit - XStart;
        if (Len > 0) {
            UINT8 *DstY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstUV      = &Render->BufferInfo.UVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 *DstAlphaUV = &Render->BufferInfo.AlphaUVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (XStart)];
            UINT8 IsOddX = ((XStart % 2) == 0) ? (0U) : (1U);
            for (INT32 i = 0; i < Len; ++i) {
                AmbaDrawRender_Plot_YUV422_Blend(&DstY[i], &DstUV[i], &DstAlphaY[i], &DstAlphaUV[i], IsOddX, Color);
                if (IsOddX == 0U) {
                    IsOddX = 1U;
                } else {
                    IsOddX = 0U;
                }
            }
        }
    } else {
        /* Line out of range. Do nothing. */
    }
}

//
// get pixel impl
// TODO: Check the alignment of Dst to pixel size
//

/**
 *  Get a pixel in the buffer.
 *
 *  @param [in]  Dst                The location to set color
 *  @param [in]  BufferPixelSize    The buffer pixel size (How many BYTE to store a pixel)
 *  @param [out] Color              The returned color
 *
 *  @return 0 - OK, others - Error
 */
static inline void AmbaDrawRender_GetPixel(const UINT8 *Dst, UINT32 BufferPixelSize, AMBA_DRAW_COLOR_s *Color)
{
    for (UINT32 i = 0U; i < BufferPixelSize; ++i) {
        Color->Data[i] = Dst[i];
    }
#if 0 /* Ignore the dummy data for better performance. User should not access the dummy data. */
    for (UINT32 i = BufferPixelSize; i < AMBA_DRAW_COLOR_LEN; ++i) {
        Color->Data[i] = 0U;
    }
#endif
}

static void AmbaDrawRender_GetPixel_U8(const UINT8 *Dst, AMBA_DRAW_COLOR_s *Color)
{
    AmbaDrawRender_GetPixel(Dst, 1U, Color);
}

static void AmbaDrawRender_GetPixel_U16(const UINT8 *Dst, AMBA_DRAW_COLOR_s *Color)
{
    AmbaDrawRender_GetPixel(Dst, 2U, Color);
}

static void AmbaDrawRender_GetPixel_U32(const UINT8 *Dst, AMBA_DRAW_COLOR_s *Color)
{
    AmbaDrawRender_GetPixel(Dst, 4U, Color);
}

static void AmbaDrawRender_GetPixel_YUV(const UINT8 *Dst, AMBA_DRAW_COLOR_s *Color)
{
    /* Cannot support YUV format */
    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Not supported in YUV format", __func__, NULL, NULL, NULL, NULL);
    AmbaMisra_TouchUnused(&Dst);
    AmbaMisra_TouchUnused(Color);
}

//
// Get pixel at (x,y) impl
//

static UINT32 AmbaDrawRender_GetPixelAt_U8(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval;
    Rval = AmbaDrawRender_CheckRange(Render, X, Y);
    if (Rval == DRAW_OK) {
        const UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X)];
        AmbaDrawRender_GetPixel(Dst, 1, Color);
    }
    return Rval;
}

static UINT32 AmbaDrawRender_GetPixelAt_U16(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval;
    Rval = AmbaDrawRender_CheckRange(Render, X, Y);
    if (Rval == DRAW_OK) {
        const UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 2)];
        AmbaDrawRender_GetPixel(Dst, 2, Color);
    }
    return Rval;
}

static UINT32 AmbaDrawRender_GetPixelAt_U32(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval;
    Rval = AmbaDrawRender_CheckRange(Render, X, Y);
    if (Rval == DRAW_OK) {
        const UINT8 *Dst = &Render->BufferInfo.RGBAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (X * 4)];
        AmbaDrawRender_GetPixel(Dst, 4, Color);
    }
    return Rval;
}

static UINT32 AmbaDrawRender_YUV420_GetPixelAt(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval;
    INT32 AlignX;
    if ((X % 2) == 0) {
        /* X is Even */
        AlignX = X;
    } else {
        /* X is Odd */
        AlignX = X - 1;
    }
    Rval = AmbaDrawRender_CheckRange(Render, AlignX, Y);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawRender_CheckRange(Render, AlignX + 1, Y);
        if (Rval == DRAW_OK) {
            const UINT8 *SrcY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            const UINT8 *SrcUV      = &Render->BufferInfo.UVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            const UINT8 *SrcAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            const UINT8 *SrcAlphaUV = &Render->BufferInfo.AlphaUVAddr[((Y/2) * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            Color->Data[0] = SrcY[0];
            Color->Data[1] = SrcY[1];
            Color->Data[2] = SrcUV[0];
            Color->Data[3] = SrcUV[1];
            Color->Data[4] = SrcAlphaY[0];
            Color->Data[5] = SrcAlphaY[1];
            Color->Data[6] = SrcAlphaUV[0];
            Color->Data[7] = SrcAlphaUV[1];
        }
    }
    return Rval;
}

static UINT32 AmbaDrawRender_YUV422_GetPixelAt(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y, AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval;
    INT32 AlignX;
    if ((X % 2) == 0) {
        /* X is Even */
        AlignX = X;
    } else {
        /* X is Odd */
        AlignX = X - 1;
    }
    Rval = AmbaDrawRender_CheckRange(Render, AlignX, Y);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawRender_CheckRange(Render, AlignX + 1, Y);
        if (Rval == DRAW_OK) {
            const UINT8 *SrcY       = &Render->BufferInfo.YAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            const UINT8 *SrcUV      = &Render->BufferInfo.UVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            const UINT8 *SrcAlphaY  = &Render->BufferInfo.AlphaYAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            const UINT8 *SrcAlphaUV = &Render->BufferInfo.AlphaUVAddr[(Y * (INT32)Render->BufferInfo.Pitch) + (AlignX)];
            Color->Data[0] = SrcY[0];
            Color->Data[1] = SrcY[1];
            Color->Data[2] = SrcUV[0];
            Color->Data[3] = SrcUV[1];
            Color->Data[4] = SrcAlphaY[0];
            Color->Data[5] = SrcAlphaY[1];
            Color->Data[6] = SrcAlphaUV[0];
            Color->Data[7] = SrcAlphaUV[1];
        }
    }
    return Rval;
}


/**
 *  Check parameters.
 *
 *  @param [in,out] Render          The draw render
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRender_Init_ParamCheck(const AMBA_DRAW_RENDER_s *Render)
{
    UINT32 Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    if (Render != NULL) {
        if ((Render->BufferInfo.Pitch > 0U) && (Render->BufferInfo.Width > 0U) && (Render->BufferInfo.Height > 0U)) {
            if ((Render->BufferPixelSize == 1U) || (Render->BufferPixelSize == 2U) || (Render->BufferPixelSize == 4U)) {
                if ((Render->DrawArea.Width > 0U) && (Render->DrawArea.Height > 0U)) {
                    if ((Render->DrawArea.X >= 0) &&
                        (Render->DrawArea.Y >= 0) &&
                        (((UINT32)Render->DrawArea.X + Render->DrawArea.Width) <= Render->BufferInfo.Width) &&
                        (((UINT32)Render->DrawArea.Y + Render->DrawArea.Height) <= Render->BufferInfo.Height)) {
                        if (Render->BufferInfo.PixelFormat < AMBA_DRAW_BUFFER_YUV420_WITH_ALPHA) {
                            if (Render->BufferInfo.RGBAddr == NULL) {
                                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] RGBAddr is NULL", __func__, NULL, NULL, NULL, NULL);
                            } else {
                                Rval = DRAW_OK;
                            }
                        } else {
                            if (Render->BufferInfo.YAddr == NULL) {
                                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] YAddr is NULL", __func__, NULL, NULL, NULL, NULL);
                            } else if (Render->BufferInfo.UVAddr == NULL) {
                                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] UVAddr is NULL", __func__, NULL, NULL, NULL, NULL);
                            } else if (Render->BufferInfo.AlphaYAddr == NULL) {
                                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AlphaYAddr is NULL", __func__, NULL, NULL, NULL, NULL);
                            } else if (Render->BufferInfo.AlphaUVAddr == NULL) {
                                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AlphaUVAddr is NULL", __func__, NULL, NULL, NULL, NULL);
                            } else {
                                Rval = DRAW_OK;
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_Init_ParamCheck: [ERROR] Draw area (%d %d %d %d) out of range", Render->DrawArea.X, Render->DrawArea.Y, (INT32)Render->DrawArea.Width, (INT32)Render->DrawArea.Height, 0);
                        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_Init_ParamCheck: [ERROR] BufferPitch %u BufferWidth %u BufferHeight %u", Render->BufferInfo.Pitch, Render->BufferInfo.Width, Render->BufferInfo.Height, 0U, 0U);
                    }
                } else {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_Init_ParamCheck: [ERROR] Draw area (%d %d %d %d)", Render->DrawArea.X, Render->DrawArea.Y, (INT32)Render->DrawArea.Width, (INT32)Render->DrawArea.Height, 0);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_Init_ParamCheck: [ERROR] Invalid BufferPixelSize %u", Render->BufferPixelSize, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_Init_ParamCheck: [ERROR] BufferPitch %u BufferWidth %u BufferHeight %u", Render->BufferInfo.Pitch, Render->BufferInfo.Width, Render->BufferInfo.Height, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid Render", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Initialize Amba Render functions for parsing image
 *  Assign Render function (MoveTo_f, MoveNext_f, GetPixel_f, GetPixelAt_f)
 *
 *  @param [in,out] Render          The draw render
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRender_Init_ParseFunc(AMBA_DRAW_RENDER_s *Render)
{
    UINT32 Rval = DRAW_OK;

    /* TODO: Convert PixelFormat to OSD data format (Definition might be different in the future) */
    switch ((UINT32)Render->BufferInfo.PixelFormat) {
    case AMBA_DRAW_BUFFER_YUV420_WITH_ALPHA:
        Render->MoveTo_f = AmbaDrawRender_MoveTo_YUV;
        Render->MoveNext_f = AmbaDrawRender_MoveNext_YUV;
        Render->GetPixel_f = AmbaDrawRender_GetPixel_YUV;
        Render->GetPixelAt_f = AmbaDrawRender_YUV420_GetPixelAt;
        break;
    case AMBA_DRAW_BUFFER_YUV422_WITH_ALPHA:
        Render->MoveTo_f = AmbaDrawRender_MoveTo_YUV;
        Render->MoveNext_f = AmbaDrawRender_MoveNext_YUV;
        Render->GetPixel_f = AmbaDrawRender_GetPixel_YUV;
        Render->GetPixelAt_f = AmbaDrawRender_YUV422_GetPixelAt;
        break;
    case OSD_8BIT_CLUT_MODE:
        /* U8 */
        Render->MoveTo_f = AmbaDrawRender_MoveTo;
        Render->MoveNext_f = AmbaDrawRender_MoveNext_U8;
        Render->GetPixel_f = AmbaDrawRender_GetPixel_U8;
        Render->GetPixelAt_f = AmbaDrawRender_GetPixelAt_U8;
        break;
    case OSD_16BIT_VYU_RGB_565:
    case OSD_16BIT_UYV_BGR_565:
    case OSD_16BIT_AYUV_4444:
    case OSD_16BIT_RGBA_4444:
    case OSD_16BIT_BGRA_4444:
    case OSD_16BIT_ABGR_4444:
    case OSD_16BIT_ARGB_4444:
    case OSD_16BIT_AYUV_1555:
    case OSD_16BIT_YUV_1555:
    case OSD_16BIT_RGBA_5551:
    case OSD_16BIT_BGRA_5551:
    case OSD_16BIT_ABGR_1555:
    case OSD_16BIT_ARGB_1555:
        /* U16 */
        Render->MoveTo_f = AmbaDrawRender_MoveTo;
        Render->MoveNext_f = AmbaDrawRender_MoveNext_U16;
        Render->GetPixel_f = AmbaDrawRender_GetPixel_U16;
        Render->GetPixelAt_f = AmbaDrawRender_GetPixelAt_U16;
        break;
    case OSD_32BIT_AYUV_8888:
    case OSD_32BIT_RGBA_8888:
    case OSD_32BIT_BGRA_8888:
    case OSD_32BIT_ABGR_8888:
    case OSD_32BIT_ARGB_8888:
        /* U32 */
        Render->MoveTo_f = AmbaDrawRender_MoveTo;
        Render->MoveNext_f = AmbaDrawRender_MoveNext_U32;
        Render->GetPixel_f = AmbaDrawRender_GetPixel_U32;
        Render->GetPixelAt_f = AmbaDrawRender_GetPixelAt_U32;
        break;
    default:
        /* Should not happen when all the formats are considered. PixelFormat is valid after AmbaDrawRender_Init_ParamCheck() passed. */
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_ParseFunc: [ERROR] Unexpected format %u", Render->BufferInfo.PixelFormat, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
        break;
    }

    return Rval;
}

/**
 *  Initialize Amba Render functions for plotting when blending is enabled
 *  Assign Render function (PlotPixel_f, PlotPixelAt_f, PlotHLineAt_f)
 *
 *  @param [in,out] Render          The draw render
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRender_Init_PlotFunc_Blend(AMBA_DRAW_RENDER_s *Render)
{
    UINT32 Rval = DRAW_OK;

    /* TODO: Convert PixelFormat to OSD data format (Definition might be different in the future) */
    switch ((UINT32)Render->BufferInfo.PixelFormat) {
    case AMBA_DRAW_BUFFER_YUV420_WITH_ALPHA:
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_YUV420_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_YUV420_Blend;
        break;
    case AMBA_DRAW_BUFFER_YUV422_WITH_ALPHA:
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_YUV422_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_YUV422_Blend;
        break;
    case OSD_8BIT_CLUT_MODE:
        /* U8 */
        /* Not supported */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_Blend;
        break;
    case OSD_16BIT_VYU_RGB_565:
    case OSD_16BIT_UYV_BGR_565:
    case OSD_16BIT_YUV_1555:
        /* U16 */
        /* Not supported */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_Blend;
        break;
    case OSD_16BIT_AYUV_4444:
    case OSD_16BIT_ABGR_4444:
    case OSD_16BIT_ARGB_4444:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_AXXX_4444_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_AXXX_4444_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_AXXX_4444_Blend;
        break;
    case OSD_16BIT_RGBA_4444:
    case OSD_16BIT_BGRA_4444:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_XXXA_4444_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_XXXA_4444_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_XXXA_4444_Blend;
        break;
    case OSD_16BIT_AYUV_1555:
    case OSD_16BIT_ABGR_1555:
    case OSD_16BIT_ARGB_1555:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_AXXX_1555_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_AXXX_1555_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_AXXX_1555_Blend;
        break;
    case OSD_16BIT_RGBA_5551:
    case OSD_16BIT_BGRA_5551:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_XXXA_5551_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_XXXA_5551_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_XXXA_5551_Blend;
        break;
    case OSD_32BIT_AYUV_8888:
    case OSD_32BIT_ABGR_8888:
    case OSD_32BIT_ARGB_8888:
        /* U32 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_AXXX_8888_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_AXXX_8888_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_AXXX_8888_Blend;
        break;
    case OSD_32BIT_RGBA_8888:
    case OSD_32BIT_BGRA_8888:
        /* U32 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_XXXA_8888_Blend;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPxlAt_XXXA_8888_Blend;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLnAt_XXXA_8888_Blend;
        break;
    default:
        /* Should not happen when all the formats are considered. PixelFormat is valid after AmbaDrawRender_Init_ParamCheck() passed. */
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_Init: [ERROR] Unexpected format %u", Render->BufferInfo.PixelFormat, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
        break;
    }

    return Rval;
}

/**
 *  Initialize Amba Render functions for plotting when blending is disabled
 *  Assign Render function (PlotPixel_f, PlotPixelAt_f, PlotHLineAt_f)
 *
 *  @param [in,out] Render          The draw render
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRender_Init_PlotFunc(AMBA_DRAW_RENDER_s *Render)
{
    UINT32 Rval = DRAW_OK;

    /* TODO: Convert PixelFormat to OSD data format (Definition might be different in the future) */
    switch ((UINT32)Render->BufferInfo.PixelFormat) {
    case AMBA_DRAW_BUFFER_YUV420_WITH_ALPHA:
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_YUV;
        Render->PlotPixelAt_f = AmbaDrawRender_YUV420_PlotPixelAt;
        Render->PlotHLineAt_f = AmbaDrawRender_YUV420_PlotLineAt;
        break;
    case AMBA_DRAW_BUFFER_YUV422_WITH_ALPHA:
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_YUV;
        Render->PlotPixelAt_f = AmbaDrawRender_YUV422_PlotPixelAt;
        Render->PlotHLineAt_f = AmbaDrawRender_YUV422_PlotLineAt;
        break;
    case OSD_8BIT_CLUT_MODE:
        /* U8 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U8;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U8;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U8;
        break;
    case OSD_16BIT_VYU_RGB_565:
    case OSD_16BIT_UYV_BGR_565:
    case OSD_16BIT_YUV_1555:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U16;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U16;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U16;
        break;
    case OSD_16BIT_AYUV_4444:
    case OSD_16BIT_ABGR_4444:
    case OSD_16BIT_ARGB_4444:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U16;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U16;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U16;
        break;
    case OSD_16BIT_RGBA_4444:
    case OSD_16BIT_BGRA_4444:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U16;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U16;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U16;
        break;
    case OSD_16BIT_AYUV_1555:
    case OSD_16BIT_ABGR_1555:
    case OSD_16BIT_ARGB_1555:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U16;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U16;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U16;
        break;
    case OSD_16BIT_RGBA_5551:
    case OSD_16BIT_BGRA_5551:
        /* U16 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U16;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U16;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U16;
        break;
    case OSD_32BIT_AYUV_8888:
    case OSD_32BIT_ABGR_8888:
    case OSD_32BIT_ARGB_8888:
        /* U32 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U32;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U32;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U32;
        break;
    case OSD_32BIT_RGBA_8888:
    case OSD_32BIT_BGRA_8888:
        /* U32 */
        Render->PlotPixel_f = AmbaDrawRender_PlotPixel_U32;
        Render->PlotPixelAt_f = AmbaDrawRender_PlotPixelAt_U32;
        Render->PlotHLineAt_f = AmbaDrawRender_PlotLineAt_U32;
        break;
    default:
        /* Should not happen when all the formats are considered. PixelFormat is valid after AmbaDrawRender_Init_ParamCheck() passed. */
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRender_Init: [ERROR] Unexpected format %u", Render->BufferInfo.PixelFormat, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
        break;
    }

    return Rval;
}

/**
 *  Initialize Amba Render
 *
 *  @param [in,out] Render          The draw render
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRender_Init(AMBA_DRAW_RENDER_s *Render)
{
    UINT32 Rval;

    Rval = AmbaDrawRender_Init_ParamCheck(Render);
    if (Rval == DRAW_OK) {
        /* Assign Render function (MoveTo_f, MoveNext_f, GetPixel_f, GetPixelAt_f) */
        Rval = AmbaDrawRender_Init_ParseFunc(Render);
        if (Rval == DRAW_OK) {
            /* Assign Render function (PlotPixel_f, PlotPixelAt_f, PlotHLineAt_f) */
            if (Render->IsBlending == 1U) {
                Rval = AmbaDrawRender_Init_PlotFunc_Blend(Render);
            } else {
                Rval = AmbaDrawRender_Init_PlotFunc(Render);
            }
        }
    }

    return Rval;
}
