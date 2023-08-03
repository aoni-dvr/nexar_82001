/**
 * @file AmbaDraw_CubicSpline.h
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

#ifndef AMBADRAW_CUBIC_SPLINE_H
#define AMBADRAW_CUBIC_SPLINE_H

#include <AmbaTypes.h>
#include <AmbaDraw.h>

/************************************************************************\
 * Definitions
\************************************************************************/

/**
 * Cubic Spline Interpolation config
 * Given (n) control points X0~Xn-1 and T0~Tn-1, find a formula X(t) so that:
 *     X(Ti) = Xi, 0 <= i <= n-1
 */
typedef struct {
    UINT32 ControlPointNum;                             /**< Number of control points. Range: 2 ~ AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM */
    DOUBLE X[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];    /**< X value of each control points                                             */
    DOUBLE T[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];    /**< T value of each control points. T should be strictly ascending.            */
} AMBA_DRAW_CUBIC_SPLINE_CFG_s;

/**
 * Cubic Spline Interpolation information
 * It's a peicewise polynomial (degree = 3) passing through each control points.
 * Given (n) control points X0~Xn-1 and T0~Tn-1, find a formula X(t) so that:
 *     X(Ti) = Xi, 0 <= i <= n-1
 * Define the formula as:
 *     Xi(t) = Ai + Bi*(t-Ti) + Ci*(t-Ti)^2 + Di*(t-Ti)^3, Ti <= t <= Ti+1, 0 <= i <= n-2
 */
typedef struct {
    AMBA_DRAW_CUBIC_SPLINE_CFG_s Cfg;                   /**< Cubic Spline Interpolation config                                      */
    DOUBLE A[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];    /**< Parameter A of polynomial                                              */
    DOUBLE B[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];    /**< Parameter B of polynomial                                              */
    DOUBLE C[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];    /**< Parameter C of polynomial                                              */
    DOUBLE D[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM];    /**< Parameter D of polynomial                                              */
    DOUBLE Min;                                         /**< Min value of X(t)                                                      */
    DOUBLE Max;                                         /**< Max value of X(t)                                                      */
    DOUBLE MinT;                                        /**< X(MinT) = Min                                                          */
    DOUBLE MaxT;                                        /**< X(MaxT) = Max                                                          */
} AMBA_DRAW_CUBIC_SPLINE_s;

/************************************************************************\
 * APIs
\************************************************************************/

/**
 *  Calculate the Cubic Spline Interpolation
 *
 *  @param [in]  Config             The spline config
 *  @param [out] Spline             The calculated cubic spline interpolation
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCubicSpline_Calc(const AMBA_DRAW_CUBIC_SPLINE_CFG_s *Config, AMBA_DRAW_CUBIC_SPLINE_s *Spline);

/**
 *  Evaluate the Cubic Spline Interpolation
 *
 *  @param [in]  Spline             The calculated cubic spline interpolation formula X(t)
 *  @param [in]  TEval              The evaluated value. Range: T[0] ~ T[ControlPointNum-1]
 *  @param [out] Value              The returned X(TEval) value
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCubicSpline_Eval(const AMBA_DRAW_CUBIC_SPLINE_s *Spline, DOUBLE TEval, DOUBLE *Value);

#endif /* AMBADRAW_CUBIC_SPLINE_H */

