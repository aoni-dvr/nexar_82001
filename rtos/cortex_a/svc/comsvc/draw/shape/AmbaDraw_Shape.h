/**
 * @file AmbaDraw_Shape.h
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

#ifndef AMBADRAW_SHAPE_H
#define AMBADRAW_SHAPE_H

#include <AmbaDraw.h>

#include "AmbaDraw_Render.h"
#include "AmbaDraw_CubicSpline.h"

/*************************************************************************
 * Shape struct
 ************************************************************************/

/**
 * Line object description
 */
typedef struct {
    AMBA_DRAW_LINE_CFG_s ObjCfg;            /**< Object config                          */
    INT32 OffsetX;                          /**< X offset                               */
    INT32 OffsetY;                          /**< Y offset                               */
} AMBA_DRAW_LINE_DESC_s;

/**
 * Rectangle object description
 */
typedef struct {
    AMBA_DRAW_RECT_CFG_s ObjCfg;            /**< Object config                          */
    INT32 OffsetX;                          /**< X offset                               */
    INT32 OffsetY;                          /**< Y offset                               */
} AMBA_DRAW_RECT_DESC_s;

/**
 * Circle object description
 */
typedef struct {
    AMBA_DRAW_CIRCLE_CFG_s ObjCfg;          /**< Object config                          */
    INT32 OffsetX;                          /**< X offset                               */
    INT32 OffsetY;                          /**< Y offset                               */
} AMBA_DRAW_CIRCLE_DESC_s;


/**
 * Polygon object description
 */
typedef struct {
    AMBA_DRAW_POLY_CFG_s ObjCfg;            /**< Object config                          */
    INT32 OffsetX;                          /**< X offset                               */
    INT32 OffsetY;                          /**< Y offset                               */
} AMBA_DRAW_POLY_DESC_s;

/**
 * Curve object description
 */
typedef struct {
    AMBA_DRAW_CURVE_CFG_s ObjCfg;           /**< Object config                          */
    INT32 OffsetX;                          /**< X offset                               */
    INT32 OffsetY;                          /**< Y offset                               */
    AMBA_DRAW_CUBIC_SPLINE_s SplineX;       /**< Cubic spline of X coordinates          */
    AMBA_DRAW_CUBIC_SPLINE_s SplineY;       /**< Cubic spline of Y coordinates          */
} AMBA_DRAW_CURVE_DESC_s;

/*************************************************************************
 * Shape Function
 ************************************************************************/

/**
 *  Check postion
 *
 *  @param [in] Postion             Postion
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_IsValidPos(UINT32 Postion);

/**
 *  Check shadow config
 *
 *  @param [in] Shadow              Shadow config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_IsValidShadow(const AMBA_DRAW_SHADOW_s *Shadow);

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawLine_IsValidConfig(const AMBA_DRAW_LINE_CFG_s *ObjCfg);

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRect_IsValidConfig(const AMBA_DRAW_RECT_CFG_s *ObjCfg);

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCirc_IsValidConfig(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg);

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawPoly_IsValidConfig(const AMBA_DRAW_POLY_CFG_s *ObjCfg);

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCurve_IsValidConfig(const AMBA_DRAW_CURVE_CFG_s *ObjCfg);

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawLine_GetObjArea(const AMBA_DRAW_LINE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea);

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRect_GetObjArea(const AMBA_DRAW_RECT_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea);

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCirc_GetObjArea(const AMBA_DRAW_CIRCLE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea);

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawPoly_GetObjArea(const AMBA_DRAW_POLY_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea);

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCurve_GetObjArea(AMBA_DRAW_CURVE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea);

/**
 *  Draw the Line object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawLine_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_LINE_DESC_s *ObjDesc,  const AMBA_DRAW_AREA_s *Area);

/**
 *  Draw the Rectangle object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRect_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_RECT_DESC_s *ObjDesc,  const AMBA_DRAW_AREA_s *Area);

/**
 *  Draw the Circle object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCirc_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_CIRCLE_DESC_s *ObjDesc,  const AMBA_DRAW_AREA_s *Area);

/**
 *  Draw the Polygon object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawPoly_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_POLY_DESC_s *ObjDesc,  const AMBA_DRAW_AREA_s *Area);

/**
 *  Draw the Curve object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCurve_Draw(const AMBA_DRAW_RENDER_s *Render, AMBA_DRAW_CURVE_DESC_s *ObjDesc,  const AMBA_DRAW_AREA_s *Area);

#endif /* AMBADRAW_SHAPE_H */

