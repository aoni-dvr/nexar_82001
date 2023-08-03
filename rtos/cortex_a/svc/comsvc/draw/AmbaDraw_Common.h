/**
 * @file AmbaDraw_Common.h
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
#ifndef AMBADRAW_COMMON_H
#define AMBADRAW_COMMON_H

#include <AmbaTypes.h>
#include <AmbaWrap.h>
#include <AmbaPrint.h>
#include <AmbaUtility.h>
#include <AmbaDef.h>
#include <AmbaMisraFix.h>
#include "AmbaDraw_Render.h"
#include "shape/AmbaDraw_Shape.h"
#include "bmp/AmbaDraw_BMP.h"
#include "string/AmbaDraw_String.h"

/*************************************************************************
 * Definition
 ************************************************************************/

/**
 * Error code
 */
#define DRAW_OK                                (OK)             /**< Execution OK               */
#define DRAW_ERROR_ARG                         DRAW_ERR_0000    /**< Invalid argument           */
#define DRAW_ERROR_GENERAL_ERROR               DRAW_ERR_0001    /**< General error              */
#define DRAW_ERROR_PARAM_STRUCTURE             DRAW_ERR_0000    /**< Incorrect structure used   */
#define DRAW_ERROR_PARAM_VALUE_RANGE           DRAW_ERR_0000    /**< Incorrect value range      */
#define DRAW_ERROR_OUT_OF_MEMORY               DRAW_ERR_0001    /**< Out of memory              */
#define DRAW_ERROR_OS                          DRAW_ERR_0001    /**< OS error                   */


/* For backward compatibility */
#define AMBA_DRAW_OSDFMT_16BIT_VYU_RGB_565   AMBA_DRAW_OSDFMT_16BIT_VYU_565 /**< VYU_RGB_565    */
#define AMBA_DRAW_OSDFMT_16BIT_UYV_BGR_565   AMBA_DRAW_OSDFMT_16BIT_UYV_565 /**< UYV_BGR_565    */

/** The osd formate, pixel format, check with AmbaDSP_VOUT.h "OSD Data Format*/
/**< 8-bit color look-up table mode */
#define AMBA_DRAW_OSDFMT_8BIT_CLUT_MODE   (0U)     /**< AYUV_8888             */
/* 16-bit direct mode */
#define AMBA_DRAW_OSDFMT_16BIT_VYU_565    (1U)     /**< VYU_565               */
#define AMBA_DRAW_OSDFMT_16BIT_UYV_565    (2U)     /**< UYV_565               */
#define AMBA_DRAW_OSDFMT_16BIT_AYUV_4444  (3U)     /**< AYUV_4444             */
#define AMBA_DRAW_OSDFMT_16BIT_RGBA_4444  (4U)     /**< RGBA_4444             */
#define AMBA_DRAW_OSDFMT_16BIT_BGRA_4444  (5U)     /**< BGRA_4444             */
#define AMBA_DRAW_OSDFMT_16BIT_ABGR_4444  (6U)     /**< ABGR_4444             */
#define AMBA_DRAW_OSDFMT_16BIT_ARGB_4444  (7U)     /**< ARGB_4444             */
#define AMBA_DRAW_OSDFMT_16BIT_AYUV_1555  (8U)     /**< AYUV_1555             */
#define AMBA_DRAW_OSDFMT_16BIT_YUV_1555   (9U)     /**< YUV_1555, MSB ignored */
#define AMBA_DRAW_OSDFMT_16BIT_RGBA_5551  (10U)    /**< RGBA_5551             */
#define AMBA_DRAW_OSDFMT_16BIT_BGRA_5551  (11U)    /**< BGRA_5551             */
#define AMBA_DRAW_OSDFMT_16BIT_ABGR_1555  (12U)    /**< ABGR_1555             */
#define AMBA_DRAW_OSDFMT_16BIT_ARGB_1555  (13U)    /**< ARGB_1555             */
#define AMBA_DRAW_OSDFMT_16BIT_RGB_565    (14U)    /**< RGB_565               */
#define AMBA_DRAW_OSDFMT_16BIT_BGR_565    (15U)    /**< BGR_565               */
/* 32-bit direct mode */
#define AMBA_DRAW_OSDFMT_32BIT_AYUV_8888  (27U)    /**< AYUV_8888             */
#define AMBA_DRAW_OSDFMT_32BIT_RGBA_8888  (28U)    /**< RGBA_8888             */
#define AMBA_DRAW_OSDFMT_32BIT_BGRA_8888  (29U)    /**< BGRA_8888             */
#define AMBA_DRAW_OSDFMT_32BIT_ABGR_8888  (30U)    /**< ABGR_8888             */
#define AMBA_DRAW_OSDFMT_32BIT_ARGB_8888  (31U)    /**< ARGB_8888             */

#define AMBA_DRAW_PRINT_MODULE_ID           ((UINT16)(DRAW_ERR_BASE >> 16U))    /**< Module ID for AmbaPrint */

#define AMBA_DRAW_LINE_WIDTH_MAX            (1024U) /**< Max line width        */

/**
 * Object type
 */
#define AMBA_DRAW_OBJ_LINE      (0x0U)          /**< String object type      */
#define AMBA_DRAW_OBJ_RECT      (0x1U)          /**< Rectangle object type   */
#define AMBA_DRAW_OBJ_CIRCLE    (0x2U)          /**< Circle object type      */
#define AMBA_DRAW_OBJ_BMP       (0x3U)          /**< BMP object type         */
#define AMBA_DRAW_OBJ_STRING    (0x4U)          /**< String object type      */
#define AMBA_DRAW_OBJ_POLY      (0x5U)          /**< Polygon object type     */
#define AMBA_DRAW_OBJ_CURVE     (0x6U)          /**< Curve object type       */
#define AMBA_DRAW_OBJ_MAX       (0x7U)          /**< Max object type         */

/**
 * Position description
 */
#define AMBA_DRAW_POSITION_TL   (AMBA_DRAW_ALIGN_V_TOP    | AMBA_DRAW_ALIGN_H_LEFT)     /**< Top-left position                          */
#define AMBA_DRAW_POSITION_TM   (AMBA_DRAW_ALIGN_V_TOP    | AMBA_DRAW_ALIGN_H_CENTER)   /**< Top-middle position                        */
#define AMBA_DRAW_POSITION_TR   (AMBA_DRAW_ALIGN_V_TOP    | AMBA_DRAW_ALIGN_H_RIGHT)    /**< Top-right postion                          */
#define AMBA_DRAW_POSITION_ML   (AMBA_DRAW_ALIGN_V_MIDDLE | AMBA_DRAW_ALIGN_H_LEFT)     /**< Middle-right position                      */
#define AMBA_DRAW_POSITION_MM   (AMBA_DRAW_ALIGN_V_MIDDLE | AMBA_DRAW_ALIGN_H_CENTER)   /**< Middle-middle postion                      */
#define AMBA_DRAW_POSITION_MR   (AMBA_DRAW_ALIGN_V_MIDDLE | AMBA_DRAW_ALIGN_H_RIGHT)    /**< Middle-right position                      */
#define AMBA_DRAW_POSITION_BL   (AMBA_DRAW_ALIGN_V_BOTTOM | AMBA_DRAW_ALIGN_H_LEFT)     /**< Bottom-left position                       */
#define AMBA_DRAW_POSITION_BM   (AMBA_DRAW_ALIGN_V_BOTTOM | AMBA_DRAW_ALIGN_H_CENTER)   /**< Bottom-middle position                     */
#define AMBA_DRAW_POSITION_BR   (AMBA_DRAW_ALIGN_V_BOTTOM | AMBA_DRAW_ALIGN_H_RIGHT)    /**< Bottom-right position                      */

/*************************************************************************
 * Struct
 ************************************************************************/


/*************************************************************************
 * Variable
 ************************************************************************/


/*************************************************************************
 * Function
 ************************************************************************/

/**
 *  Convert AmbaWrap error to Draw error
 *  @param [in]  Ret                AmbaWrap error
 *  @return 0 - OK, others - Error
 */
static inline UINT32 DRAW_WRAP2D(UINT32 Ret)
{
    UINT32 Rval;
    switch (Ret) {
        case OK: /* OK */
            Rval = DRAW_OK;
            break;
        default:
            Rval = DRAW_ERROR_GENERAL_ERROR; /* Fatal error */
            break;
    }
    return Rval;
}

/**
 *  Get the absolute value of A
 *
 *  @param [in] A                   Input value
 *
 *  @return The absolute value
 */
static inline INT32 AmbaDraw_Abs(INT32 A)
{
    INT32 PositiveA;
    if (A < 0) {
        PositiveA = -A;
    } else {
        PositiveA = A;
    }
    return PositiveA;
}

/**
 *  Get the max value of A and B
 *
 *  @param [in] A                   Input value
 *  @param [in] B                   Input value
 *
 *  @return The max value
 */
static inline INT32 AmbaDraw_Max(INT32 A, INT32 B)
{
    return (A > B) ? (A) : (B);
}

/**
 *  Get the min value of A and B
 *
 *  @param [in] A                   Input value
 *  @param [in] B                   Input value
 *
 *  @return The min value
 */
static inline INT32 AmbaDraw_Min(INT32 A, INT32 B)
{
    return (A < B) ? (A) : (B);
}

/**
 *  Round A and return the result
 *
 *  @param [in] A                   Input value
 *
 *  @return The rounded value
 */
static inline INT32 AmbaDraw_Round(DOUBLE A)
{
    DOUBLE TempA = A;
    if (A >= 0.0) {
        TempA += 0.5;
    } else {
        TempA -= 0.5;
    }
    return (INT32)TempA;
}

/**
 *  Get aligned value
 *
 *  @param [in] Val                 Input value
 *  @param [in] AlignBase           Align base
 *
 *  @return The aligned value
 */
static inline ULONG AmbaDraw_GetAlignedValUL(ULONG Val, ULONG AlignBase)
{
    return (((Val) + (AlignBase - 1U)) & ~(AlignBase - 1U));
}

/**
 *  Get object size
 *
 *  @param [in] PixelFormat         Pixel format
 *  @param [out] PixelSize          Pixel size (byte)
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_GetPixelSize(UINT8 PixelFormat, UINT32 *PixelSize);

/**
 *  Check the object area.
 *
 *  @param [in] Render              The draw render
 *  @param [in] ObjArea             The object area
 *  @param [in] Area                The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_CheckObjArea(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_AREA_s *ObjArea, const AMBA_DRAW_AREA_s *Area);


#endif // AMBADRAW_COMMON_H
