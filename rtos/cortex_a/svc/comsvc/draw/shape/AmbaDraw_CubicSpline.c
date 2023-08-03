/**
 * @file AmbaDraw_CubicSpline.c
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

#include "AmbaDraw_Common.h"
#include "AmbaDraw_CubicSpline.h"

/************************************************************************\
 * APIs
\************************************************************************/

/*
 *  Get the value of cubic spline interpolation.
 *  X(t) = A + B*(t-T) + C*(t-T)^2 + D*(t-T)^3
 *  Make sure that T is in the range T[I]~T[I+1]
 */
static DOUBLE AmbaDrawCubicSpline_GetVal(DOUBLE TEval, DOUBLE T, DOUBLE A, DOUBLE B, DOUBLE C, DOUBLE D)
{
    DOUBLE Ret;
    DOUBLE TempT = TEval - T;
    DOUBLE TempT2 = TempT * TempT;
    DOUBLE TempT3 = TempT2 * TempT;
    Ret = A + (B*TempT) + (C*TempT2) + (D*TempT3);

    return Ret;
}

/*
 *  Find the roots of the below equation.
 *  F(X) = A*X^2 + B*X + C = 0
 *  Return the number of roots and the value of roots.
 *  Ignore imaginary number.
 *
 *  @param [in]  A                  The A value of the equation
 *  @param [in]  B                  The B value of the equation
 *  @param [in]  C                  The C value of the equation
 *  @param [out] Num                The number of roots (0 <= Num <= 2)
 *  @param [out] Root               Array of the roots with number of data = Num.
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCubicSpline_FindQuadraticRoot(DOUBLE A, DOUBLE B, DOUBLE C, UINT32 *Num, DOUBLE Root[2])
{
    UINT32 Rval = DRAW_OK;

    if (A == 0.0) {
        /* B*X + C = 0 */
        if (B == 0.0) {
            /* C = 0 */
            /*
                Two possible cases:
                1. No solution (when C != 0)
                2. X can be any value (when C = 0)
                Since this function is to find the local extremum of the spline, there won't be extremum in both cases.
                So simply return Num = 0.
             */
            *Num = 0U;
        } else {
            /* B*X + C = 0 */
            /* X = -C/B */
            Root[0] = -C/B;
            *Num = 1U;
        }
    } else if (B == 0.0) {
        /* A*X^2 + C = 0 */
        /* X = +-sqrt(-C/A) */
        DOUBLE Temp = -C/A;
        if (Temp > 0.0) {
            Rval = AmbaWrap_sqrt(Temp, &Temp);
            if (Rval != OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_sqrt failed", __func__, NULL, NULL, NULL, NULL);
            } else {
                Root[0] = Temp;
                Root[1] = -Temp;
            }
            *Num = 2U;
        } else if (Temp == 0.0) {
            Root[0] = 0.0;
            *Num = 1U;
        } else {
            *Num = 0U;
        }
    } else {
        /* A*X^2 + B*X + C = 0 */
        DOUBLE Discriminant = (B*B) - (4.0*A*C);
        if (Discriminant > 0.0) {
            Rval = AmbaWrap_sqrt(Discriminant, &Discriminant);
            if (Rval != OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_sqrt failed", __func__, NULL, NULL, NULL, NULL);
            } else {
                /*
                   Root = (-B +- sqrt(B^2 - 4AC))/2A
                   Do not use the formula directly because the error of floating point might be significant when "B" is close to "sqrt(B^2 - 4AC)".
                   It's more numerically stable to get the larger root first:
                       X1 = (-B - sign(B)*sqrt(B^2 - 4AC))/2A
                   Let W  = (-B - sign(B)*sqrt(B^2 - 4AC))/2
                       X1 = W/A
                   Then derive another root:
                       X2 = C/(A*X1) = C/W
                   The key is to avoid subtracting two closed floating point variables.
                 */
                DOUBLE SignB = (B >= 0.0) ? (1.0) : (-1.0);
                DOUBLE W = (-B - (SignB*Discriminant)) / 2.0;
                Root[0] = W/A;
                Root[1] = C/W;
                *Num = 2U;
            }
        } else if (Discriminant == 0.0) {
            Root[0] = -B/(2.0*A);
            *Num = 1U;
        } else {
            *Num = 0U;
        }
    }

    return Rval;
}

/*
 *  Get the min and max value of cubic spline interpolation.
 *  X(t) = A + B*(t-T) + C*(t-T)^2 + D*(t-T)^3
 *
 *  @param [in,out] Spline          The cubic spline interpolation and the returned min/max value
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCubicSpline_Range(AMBA_DRAW_CUBIC_SPLINE_s *Spline)
{
    UINT32 Rval = DRAW_OK;
    DOUBLE Min = Spline->A[0];
    DOUBLE Max = Spline->A[0];
    DOUBLE MinT = Spline->Cfg.T[0];
    DOUBLE MaxT = Spline->Cfg.T[0];

    for (UINT32 i = 0U; i < (Spline->Cfg.ControlPointNum - 1U); ++i) {
        DOUBLE Val;
        UINT32 RootNum = 0U;
        DOUBLE Root[2] = {0.0};
        /* The min and max value can be found at the boundary or local extremum */

        /* Check the boundary */
        /* The value of T[I] is already checked. Only need to check T[I+1]. */
        Val = AmbaDrawCubicSpline_GetVal(Spline->Cfg.T[i+1U], Spline->Cfg.T[i], Spline->A[i], Spline->B[i], Spline->C[i], Spline->D[i]);
        if (Val < Min) {
            Min = Val;
            MinT = Spline->Cfg.T[i+1U];
        }
        if (Val > Max) {
            Max = Val;
            MaxT = Spline->Cfg.T[i+1U];
        }

        /*
           Check the local extremum
           Find the T in the range T[I]~T[I+1] where the first derivative is 0
           Solve the below equation:
               X'(t) = B + 2C*(t-T) + 3D*(t-T)^2 = 0
               Assume u = t-T,
               B + 2C*u + 3D*u^2 = 0
         */
        Rval = AmbaDrawCubicSpline_FindQuadraticRoot((3.0*Spline->D[i]), (2.0*Spline->C[i]), Spline->B[i], &RootNum, Root);
        if (Rval != OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_sqrt failed", __func__, NULL, NULL, NULL, NULL);
        } else {
            for (UINT32 J = 0U; J < RootNum; ++J) {
                DOUBLE TRoot = Spline->Cfg.T[i] + Root[J];
                /* Only consider TRoot in the range T[I]~T[I+1] */
                if ((Spline->Cfg.T[i] <= TRoot) && (TRoot <= Spline->Cfg.T[i+1U])) {
                    Val = AmbaDrawCubicSpline_GetVal(TRoot, Spline->Cfg.T[i], Spline->A[i], Spline->B[i], Spline->C[i], Spline->D[i]);
                    if (Val < Min) {
                        Min = Val;
                        MinT = TRoot;
                    }
                    if (Val > Max) {
                        Max = Val;
                        MaxT = TRoot;
                    }
                }
            }
        }

        if (Rval != OK) {
            break;
        }
    }

    if (Rval == DRAW_OK) {
        Spline->Min = Min;
        Spline->Max = Max;
        Spline->MinT = MinT;
        Spline->MaxT = MaxT;
    }

    return Rval;
}

/*
 *  Solve an equation of tridiagonal matrix by the Thomas algorithm.
 *  Given Pi, Qi, Ri, and Si, return Xi.
 *  [Q0 R0             0][ X0 ]   [ S0 ]
 *  [P1 Q1 R1           ][ X1 ]   [ S1 ]
 *  [   P2 Q2           ][ X2 ] = [ S2 ]
 *  [      ... ...      ][ ...]   [ ...]
 *  [               Rn-2][Xn-2]   [Sn-2]
 *  [0         Pn-1 Qn-1][Xn-1]   [Sn-1]
 */
static void AmbaDrawCubicSpline_SolveTriDiagonal(UINT32 N, const DOUBLE P[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM], const DOUBLE Q[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM], const DOUBLE R[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM], const DOUBLE S[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM], DOUBLE X[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM])
{
    DOUBLE TempQ[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM] = {0.0};
    DOUBLE TempS[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM] = {0.0};

    /* N should be 2 ~ AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM */
    /* T should be strickly ascending, so Q[I] should not be 0. So no need to check "divide by 0". */

    /*
        Initialize TempQ and TempS
        Fix the below VCast error: (Array initialization not working)
            Symbol 'TempQ' conceivably not initialized [MISRA 2012 Rule 9.1, mandatory]
            Symbol 'TempS' conceivably not initialized [MISRA 2012 Rule 9.1, mandatory]
     */
    for (INT32 i = 0; i < (INT32)AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM; ++i) {
        TempQ[i] = 0.0;
        TempS[i] = 0.0;
    }

    /* Copy Q and S */
    for (INT32 i = 0; i < (INT32)N; ++i) {
        TempQ[i] = Q[i];
        TempS[i] = S[i];
    }

    /* Solve tridiagonal matrix */
    for (INT32 i = 1; i < (INT32)N; ++i) {
        DOUBLE W = P[i] / TempQ[i - 1];
        TempQ[i] -= (W * R[i-1]);
        TempS[i] -= (W * TempS[i-1]);
    }
    X[N-1U] = TempS[N-1U] / TempQ[N-1U];
    for (INT32 i = ((INT32)N-2); i >= 0; --i) {
        X[i] = (TempS[i] - (R[i] * X[i+1])) / TempQ[i];
    }
}

/**
 *  Parameter check
 *
 *  @param [in]  Config             The spline config
 *  @param [in]  Spline             The calculated cubic spline interpolation
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCubicSpline_Calc_ParamCheck(const AMBA_DRAW_CUBIC_SPLINE_CFG_s *Config, const AMBA_DRAW_CUBIC_SPLINE_s *Spline)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (Config == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Config is NULL", __func__, NULL, NULL, NULL, NULL);
    } else if (Spline == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Spline is NULL", __func__, NULL, NULL, NULL, NULL);
    } else if ((Config->ControlPointNum < 2U) || (Config->ControlPointNum > AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM)) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCubicSpline_Calc_ParamCheck: [ERROR] Invalid ControlPointNum %u", Config->ControlPointNum, 0U, 0U, 0U, 0U);
    } else {
        UINT32 i;
        for (i = 1U; i < Config->ControlPointNum; ++i) {
            /* T should be strictly ascending (no repeats and in ascending order) */
            if (Config->T[i-1U] >= Config->T[i]) {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCubicSpline_Calc_ParamCheck: [ERROR] T[%u] >= T[%u]", i-1U, i, 0U, 0U, 0U);
                break;
            }
        }

        if (i >= Config->ControlPointNum) {
            Rval = DRAW_OK;
        }
    }

    return Rval;
}

/**
 *  Calculate the Cubic Spline Interpolation
 *
 *  @param [in]  Config             The spline config
 *  @param [out] Spline             The calculated cubic spline interpolation
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCubicSpline_Calc(const AMBA_DRAW_CUBIC_SPLINE_CFG_s *Config, AMBA_DRAW_CUBIC_SPLINE_s *Spline)
{
    UINT32 Rval;

    Rval = AmbaDrawCubicSpline_Calc_ParamCheck(Config, Spline);
    if (Rval == DRAW_OK) {
        UINT32 N = Config->ControlPointNum; /* N = 2 ~ AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM */
        DOUBLE P[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM] = {0.0};
        DOUBLE Q[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM] = {0.0};
        DOUBLE R[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM] = {0.0};
        DOUBLE S[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM] = {0.0};
        DOUBLE M[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM] = {0.0};
        DOUBLE H[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM-1U] = {0.0};

        /* Copy config */
        Spline->Cfg = *Config;

        /*
            The cubic spline interpolation is a peicewise polynomial of degree = 3.
            Let the formula be:
                Xi(t) = Ai + Bi*(t-Ti) + Ci*(t-Ti)^2 + Di*(t-Ti)^3,   Ti <= t <= Ti+1, i = 0 ~ n-2.
            Given Xi and Ti, we need to determine each Ai, Bi, Ci, and Di. So we need 4n-4 conditions.
            Condition:
            1. The curve passes through each Xi:
                Fi(Ti)   = Xi,    i = 0 ~ n-2
                Fi(Ti+1) = Xi+1,  i = 0 ~ n-2
            2. The first derivative of the curve is continuous:
                Fi'(Ti+1) = Fi+1'(Ti+1),  i = 0 ~ n-3
            3. The second derivative of the curve is continuous:
                Fi''(Ti+1) = Fi+1''(Ti+1),  i = 0 ~ n-3
            4. Boundary condition: Natural
               The second derivatives of both end points are 0.
               Meaning that it's a straight line at the beginning and the end of the curve.
                F0''(T0) = 0
                Fn-2''(Tn-1) = 0

            Solve the following equation for natural cubic spline interpolation.
            Let Hi = Ti+1 - Ti, i = 0 ~ n-2.
            [ 1      0        0     ...                     0 ][  M0  ]       [                  0                  ]
            [ H0  2(H0+H1)    H1     0    ...               0 ][  M1  ]       [       (X2-X1)/H1 - (X1-X0)/H0       ]
            [ 0      H1    2(H1+H2)  H2   0     ...           ][  M2  ] = 6 * [       (X3-X2)/H2 - (X2-X1)/H1       ]
            [                 ...                             ][  ... ]       [                 ...                 ]
            [                          Hn-3 2(Hn-3+Hn-2) Hn-2 ][ Mn-2 ]       [ (Xn-1-Xn-2)/Hn-2 - (Xn-2-Xn-3)/Hn-3 ]
            [ 0                         0         0         1 ][ Mn-1 ]       [                  0                  ]

            After solving M0 ~ Mn-1, the parameters of the formula can be determined by:
                Ai = Xi
                Bi = (Xi+1 - Xi)/Hi - Hi*Mi/2 - Hi(Mi+1 - Mi)/6
                Ci = Mi/2
                Di = (Mi+1 - Mi)/6Hi
         */
        for (UINT32 i = 0U; i < (N - 1U); ++i) {
            H[i] = Config->T[i+1U] - Config->T[i];
        }

        /* Fill the tridiagonal matrix */
        P[0]    = 0.0;
        P[N-1U] = 0.0;
        Q[0]    = 1.0;
        Q[N-1U] = 1.0;
        R[0]    = 0.0;
        R[N-1U] = 0.0;
        S[0]    = 0.0;
        S[N-1U] = 0.0;
        for (UINT32 i = 1U; i < (N - 1U); ++i) {
            P[i] = H[i-1U];
            Q[i] = 2.0 * (H[i-1U] + H[i]);
            R[i] = H[i];
            S[i] = 6.0 * (((Config->X[i+1U] - Config->X[i]) / H[i]) - ((Config->X[i] - Config->X[i-1U]) / H[i-1U]));
        }

        /* Solve the eualtion and get M0 ~ Mn-1 */
        AmbaDrawCubicSpline_SolveTriDiagonal(N, P, Q, R, S, M);

        /* Determine Ai, Bi, Ci, and Di */
        for (UINT32 i = 0U; i < (N - 1U); ++i) {
            Spline->A[i] = Config->X[i];
            Spline->B[i] = ((Config->X[i+1U] - Config->X[i]) / H[i]) - ((H[i] * M[i]) / 2.0) - ((H[i] * (M[i+1U] - M[i])) / 6.0);
            Spline->C[i] = M[i] / 2.0;
            Spline->D[i] = ((M[i+1U] - M[i]) / H[i]) / 6.0;
        }

        /* Calculate Min/Max value */
        Rval = AmbaDrawCubicSpline_Range(Spline);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Range fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

/**
 *  Parameter check
 *
 *  @param [in]  Spline             The calculated cubic spline interpolation formula X(t)
 *  @param [in]  TEval              The evaluated value. Range: T[0] ~ T[ControlPointNum-1]
 *  @param [in]  Value              The X(TEval) value
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCubicSpline_Eval_ParamCheck(const AMBA_DRAW_CUBIC_SPLINE_s *Spline, DOUBLE TEval, const DOUBLE *Value)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (Spline == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Spline is NULL", __func__, NULL, NULL, NULL, NULL);
    } else if (Value == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Value is NULL", __func__, NULL, NULL, NULL, NULL);
    } else if ((TEval < Spline->Cfg.T[0]) || (TEval > Spline->Cfg.T[Spline->Cfg.ControlPointNum-1U])) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] TEval out of range", __func__, NULL, NULL, NULL, NULL);
    } else {
        Rval = DRAW_OK;
    }

    return Rval;
}

/**
 *  Evaluate the Cubic Spline Interpolation
 *
 *  @param [in]  Spline             The calculated cubic spline interpolation formula X(t)
 *  @param [in]  TEval              The evaluated value. Range: T[0] ~ T[ControlPointNum-1]
 *  @param [out] Value              The returned X(TEval) value
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCubicSpline_Eval(const AMBA_DRAW_CUBIC_SPLINE_s *Spline, DOUBLE TEval, DOUBLE *Value)
{
    UINT32 Rval;

    Rval = AmbaDrawCubicSpline_Eval_ParamCheck(Spline, TEval, Value);
    if (Rval == DRAW_OK) {
        for (UINT32 i = 0U; i < (Spline->Cfg.ControlPointNum - 1U); ++i) {
            if ((Spline->Cfg.T[i] <= TEval) && (TEval <= Spline->Cfg.T[i+1U])) {
                *Value = AmbaDrawCubicSpline_GetVal(TEval, Spline->Cfg.T[i], Spline->A[i], Spline->B[i], Spline->C[i], Spline->D[i]);
                break;
            }
        }
    }

    return Rval;
}

