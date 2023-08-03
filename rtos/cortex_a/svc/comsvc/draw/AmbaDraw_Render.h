/**
 * @file AmbaDraw_Render.h
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

#ifndef AMBA_DRAW_RENDER_H
#define AMBA_DRAW_RENDER_H

#include <AmbaTypes.h>
#include <AmbaDraw.h>

/*************************************************************************
 * Render Enums
 ************************************************************************/

/*************************************************************************
 * Render Structures
 ************************************************************************/
/**
 * Render Format
 */
typedef struct AMBA_DRAW_RENDER_s_ {
    AMBA_DRAW_BUFFER_INFO_s BufferInfo;     /**< Buffer info                    */
    UINT32 BufferPixelSize;                 /**< Buffer pixel size (in Byte)    */
    AMBA_DRAW_AREA_s DrawArea;              /**< Buffer area (in Pixel) to draw */
    UINT8 IsBlending;                       /**< Enable alpha blending          */
    /**
     *  Move to (X,Y)
     *
     *  @param [in]  Render             The draw render
     *  @param [out] Dst                The returned location
     *  @param [in]  X                  X coordinate (in Pixel)
     *  @param [in]  Y                  Y coordinate (in Pixel)
     */
    void (*MoveTo_f)     ( const struct AMBA_DRAW_RENDER_s_ *Render,
                           UINT8 **Dst,
                           INT32 X,
                           INT32 Y );
    /**
     *  Move to the next pixel
     *
     *  @param [out] Dst                The returned location
     */
    void (*MoveNext_f)   ( UINT8** Dst );
    /**
     *  Get the color of the pixel at the location
     *
     *  @param [in]  Dst                The location to get color
     *  @param [out] Color              The returned color
     */
    void (*GetPixel_f)   ( const UINT8 *Dst,
                           AMBA_DRAW_COLOR_s *Color );
    /**
     *  Get the color of the pixel at (X,Y)
     *
     *  @param [in]  Render             The draw render
     *  @param [in]  X                  X coordinate (in Pixel)
     *  @param [in]  Y                  Y coordinate (in Pixel)
     *  @param [out] Color              The returned color
     *
     *  @return 0 - OK, others - Error
     */
    UINT32 (*GetPixelAt_f)(const struct AMBA_DRAW_RENDER_s_ *Render,
                           INT32 X,
                           INT32 Y,
                           AMBA_DRAW_COLOR_s *Color );
    /**
     *  Plot pixel at the location
     *
     *  @param [in]  Render             The draw render
     *  @param [in]  Dst                The location to set color
     *  @param [in]  Color              The color to plot
     */
    void (*PlotPixel_f)  ( const struct AMBA_DRAW_RENDER_s_ *Render,
                           UINT8 *Dst,
                           const AMBA_DRAW_COLOR_s *Color );
    /**
     *  Plot pixel at (X,Y)
     *
     *  @param [in]  Render             The draw render
     *  @param [in]  X                  X coordinate (in Pixel)
     *  @param [in]  Y                  Y coordinate (in Pixel)
     *  @param [in]  Color              The color to plot
     */
    void (*PlotPixelAt_f)( const struct AMBA_DRAW_RENDER_s_ *Render,
                           INT32 X,
                           INT32 Y,
                           const AMBA_DRAW_COLOR_s *Color );
    /**
     *  Plot line from (X,Y) to (X+W-1,Y)
     *
     *  @param [in]  Render             The draw render
     *  @param [in]  X                  X coordinate (in Pixel)
     *  @param [in]  Y                  Y coordinate (in Pixel)
     *  @param [in]  W                  The width of the line (in Pixel)
     *  @param [in]  Color              The color to plot
     */
    void (*PlotHLineAt_f)( const struct AMBA_DRAW_RENDER_s_ *Render,
                           INT32 X,
                           INT32 Y,
                           UINT32 W,
                           const AMBA_DRAW_COLOR_s *Color );
} AMBA_DRAW_RENDER_s;

/**
 *  Initialize Amba Render
 *  Init Render, filled Render->RowStartAddr for speedy(suggest)
 *
 *  @param [in,out] Render          The draw render
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRender_Init(AMBA_DRAW_RENDER_s *Render);

#endif /* AMBA_DRAW_RENDER_H */

