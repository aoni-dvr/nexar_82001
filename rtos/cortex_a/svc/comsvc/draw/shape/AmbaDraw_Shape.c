/**
 * @file AmbaDraw_Shape.c
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

#define AMBA_DRAW_CURVE_SEGMENT (500U)      /**< Number of curve segments   */

/**
 * Draw cursor movement
 * The positions of new points when cursor moves
 */
typedef struct {
    UINT32 Num;                             /**< Number of data             */
    INT32 X[AMBA_DRAW_LINE_WIDTH_MAX*2U];   /**< X position                 */
    INT32 Y[AMBA_DRAW_LINE_WIDTH_MAX*2U];   /**< Y position                 */
    INT32 L[AMBA_DRAW_LINE_WIDTH_MAX*2U];   /**< Length (>= 0). 0: No line  */
} AMBA_DRAW_CURSOR_SHAPE_s;

/**
 * Draw cursor
 */
typedef struct {
    const AMBA_DRAW_RENDER_s *Render;       /**< Render handler             */
    INT32 X;                                /**< Cursor start X             */
    INT32 Y;                                /**< Cursor start Y             */
    INT32 NextX;                            /**< Cursor new X               */
    UINT32 Thickness;                       /**< Line thickness             */
    AMBA_DRAW_COLOR_s Color;                /**< Line color                 */
    // TODO Support different type of pen (circle, square, ...)
    AMBA_DRAW_CURSOR_SHAPE_s MoveR;         /**< Move right                 */
    AMBA_DRAW_CURSOR_SHAPE_s MoveUR;        /**< Move upper-right           */
} AMBA_DRAW_CURSOR_s;

static AMBA_DRAW_CURSOR_s G_Cursor GNU_SECTION_NOZEROINIT; /* AmbaDraw_DrawBuffer is protected by the same mutex, so only 1 instance of Cursor is needed. */
static UINT8 G_CursorStart = 0U;         /* Cursor starts drawing        */

static UINT32 AmbaDrawCirc_Render(const AMBA_DRAW_RENDER_s *Render, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2, UINT32 Thickness, const AMBA_DRAW_COLOR_s *Color);

/**
 *  Plot horizontal line from (X1, Y) to (X2, Y)
 *  Support (X1 > X2) or (X1 < X2)
 *  Plot pixel if (X1 == X2)
 *
 *  @param [in]  Render             Render handler
 *  @param [in]  X1                 X start coordinate
 *  @param [in]  X2                 X end coordinate
 *  @param [in]  Y                  Y coordinate
 *  @param [in]  Color              Line color
 */
static inline void AmbaDrawShape_PlotHLine(const AMBA_DRAW_RENDER_s *Render, INT32 X1, INT32 X2, INT32 Y, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 StartX;
    INT32 Length; /* Must be >= 0 */
    StartX = (X1 <= X2) ? (X1) : (X2);
    Length = AmbaDraw_Abs(X2 - X1) + 1;

    if (Length == 1) {
        /* Plot point */
        //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawShape_PlotHLine: Plot(%d %d) Color %d", StartX, Y, (INT32)Color, 0, 0);
        Render->PlotPixelAt_f(Render, StartX, Y, Color);
    } else {
        /* Plot horizontal line */
        //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawShape_PlotHLine: Plot(%d %d)->(%d %d) Color %d", StartX, Y, StartX + Length - 1, Y, (INT32)Color);
        Render->PlotHLineAt_f(Render, StartX, Y, (UINT32)Length, Color);
    }
}

/**
 *  Plot vertical line from (X, Y1) to (X, Y2)
 *  Support (Y1 > Y2) or (Y1 < Y2)
 *  Plot pixel if (Y1 == Y2)
 *
 *  @param [in]  Render             Render handler
 *  @param [in]  X                  X coordinate
 *  @param [in]  Y1                 Y start coordinate
 *  @param [in]  Y2                 Y end coordinate
 *  @param [in]  Color              Line color
 */
static inline void AmbaDrawShape_PlotVLine(const AMBA_DRAW_RENDER_s *Render, INT32 X, INT32 Y1, INT32 Y2, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 StartY;
    INT32 EndY;
    StartY = AmbaDraw_Min(Y1, Y2);
    EndY   = AmbaDraw_Max(Y1, Y2);

    for (INT32 i = StartY; i <= EndY; ++i) {
        /* Plot point */
        //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawShape_PlotVLine: Plot(%d %d) Color %d", X, i, (INT32)Color, 0, 0);
        Render->PlotPixelAt_f(Render, X, i, Color);
    }
}

/**
 *  Set cursor movement
 *  Set the initial pixels when the pen move "Right" and "Upper Right".
 *
 *  @param [in]  Cursor             Cursor
 *  @param [in]  PlotX1             Line start X (PlotX1 <= PlotX2)
 *  @param [in]  PlotX2             Line end X (PlotX1 <= PlotX2)
 *  @param [in]  PlotY              Line Y position
 *  @param [in]  YStart             Min Y value in MoveR
 */
static inline void AmbaDrawCursor_SetMove_Init(AMBA_DRAW_CURSOR_s *Cursor, INT32 PlotX1, INT32 PlotX2, INT32 PlotY, INT32 YStart)
{
    /*
        Iteration = 0: Move right. Save each line in MoveR.
                       Calculate upper right. Save each line in MoveUR.
     */

    INT32 ArrayID = PlotY - YStart;
    if ((ArrayID >= 0) && (ArrayID < ((INT32)AMBA_DRAW_LINE_WIDTH_MAX*2))) {
        /* Move right */
        Cursor->MoveR.X[ArrayID] = PlotX1 + 1;
        Cursor->MoveR.Y[ArrayID] = PlotY;
        Cursor->MoveR.L[ArrayID] = PlotX2 - PlotX1 + 1;
        Cursor->MoveR.Num++;
        /* Move upper right */
        Cursor->MoveUR.X[ArrayID] = PlotX1 + 1;
        Cursor->MoveUR.Y[ArrayID] = PlotY + 1;
        Cursor->MoveUR.L[ArrayID] = PlotX2 - PlotX1 + 1;
        Cursor->MoveUR.Num++;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Unexpected ArrayID", __func__, NULL, NULL, NULL, NULL);
    }
}

/**
 *  Set cursor movement
 *  Input each line of a cursor pen. Then calculate the new pixels when the pen move "Right" and "Upper Right".
 *  The values of MoveR.Num and MoveUR.Num should be set to 0 before the first time calling this function.
 *  The function should be called by Iteration = 0 first, and then Iteration = 1 with the same lines.
 *
 *  The shape of the pen should be CONVEX.
 *  Currently NOT support concave shape for better efficiency.
 *  For an NxN shape, the memory needed for MoveUR will be N^2, so the time complexity is O(N^2).
 *  However, the memory needed for a convex shape is 2N. So the time complexity is O(N).
 *
 *  @param [in]  Cursor             Cursor
 *  @param [in]  Iteration          0: Calculate shifted position, 1: Exclude original position from shifted position
 *  @param [in]  PlotX1             Line start X (PlotX1 <= PlotX2)
 *  @param [in]  PlotX2             Line end X (PlotX1 <= PlotX2)
 *  @param [in]  PlotY              Line Y position
 *  @param [in]  YStart             Min Y value in MoveR
 */
static inline void AmbaDrawCursor_SetMove(AMBA_DRAW_CURSOR_s *Cursor, UINT32 Iteration, INT32 PlotX1, INT32 PlotX2, INT32 PlotY, INT32 YStart)
{
    /*
        Iteration = 0: Move right. Save each line in MoveR.
                       Calculate upper right. Save each line in MoveUR.
        Iteration = 1: Original position. Update each line in MoveR and MoveUR.
     */

    if (Iteration == 0U) {
        AmbaDrawCursor_SetMove_Init(Cursor, PlotX1, PlotX2, PlotY, YStart);
    } else if (Iteration == 1U) {
        /* Presume that Iteration = 0 is called, so MoveR and MoveUR are storing the shifted area. */
        /* Exclude the original area from the shifted area */
        INT32 MoveRID  = PlotY - YStart;
        INT32 MoveURID = PlotY - YStart - 1;
        /* "Move right" excluding "Original position" */
        if ((MoveRID >= 0) && (MoveRID < ((INT32)AMBA_DRAW_LINE_WIDTH_MAX*2))) {
            if (Cursor->MoveR.L[MoveRID] > 0) {
                INT32 StartX = Cursor->MoveR.X[MoveRID];
                INT32 EndX = StartX + Cursor->MoveR.L[MoveRID] - 1;
                if (Cursor->MoveR.Y[MoveRID] != PlotY) { /* Should not happen */
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Inconsistent Y", __func__, NULL, NULL, NULL, NULL);
                }
                /*
                    It's known that:
                    1. StartX <= EndX
                    2. PlotX1 <= PlotX2
                    3. PlotX2 <= EndX   (Because of "Move right")
                    4. PlotX1 <= StartX (Because of "Move right")
                 */
                if (PlotX2 >= EndX) {
                    Cursor->MoveR.L[MoveRID] = 0; /* No line */
                } else {
                    StartX = AmbaDraw_Max(StartX, PlotX2 + 1);
                    Cursor->MoveR.X[MoveRID] = StartX;
                    Cursor->MoveR.L[MoveRID] = EndX - StartX + 1;
                }
            }
        }
        /* "Move upper right" excluding "Original position" */
        /* It's possible to get 2 lines. Insert one of the line into array. */
        if ((MoveURID >= 0) && (MoveURID < ((INT32)AMBA_DRAW_LINE_WIDTH_MAX*2))) {
            if (Cursor->MoveUR.L[MoveURID] > 0) {
                INT32 StartX = Cursor->MoveUR.X[MoveURID];
                INT32 EndX = StartX + Cursor->MoveUR.L[MoveURID] - 1;
                if (Cursor->MoveUR.Y[MoveURID] != PlotY) { /* Should not happen */
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Inconsistent Y", __func__, NULL, NULL, NULL, NULL);
                }
                /*
                    It's known that:
                    1. StartX <= EndX
                    2. PlotX1 <= PlotX2
                 */
                if (PlotX2 < EndX) {
                    if (PlotX2 < StartX) {
                        /* PlotX1 <= PlotX2 < StartX <= EndX */
                        /* Do nothing. No intersection. */
                    } else if (PlotX1 <= StartX) {
                        /* PlotX1 <= StartX <= PlotX2 < EndX */
                        Cursor->MoveUR.X[MoveURID] = PlotX2 + 1;
                        Cursor->MoveUR.L[MoveURID] = EndX - PlotX2;
                    } else {
                        /* StartX < PlotX1 <= PlotX2 < EndX */
                        /* Result in 2 lines. Insert one of the line to the end of array. */

                        /* First line */
                        /* MoveUR.X not changed */
                        Cursor->MoveUR.L[MoveURID] = PlotX1 - StartX;

                        /* Second line */
                        Cursor->MoveUR.X[Cursor->MoveUR.Num] = PlotX2 + 1;
                        Cursor->MoveUR.Y[Cursor->MoveUR.Num] = Cursor->MoveUR.Y[MoveURID];
                        Cursor->MoveUR.L[Cursor->MoveUR.Num] = EndX - PlotX2;
                        Cursor->MoveUR.Num++;
                    }
                } else if (PlotX2 > EndX) {
                    if (PlotX1 > EndX) {
                        /* StartX <= EndX < PlotX1 <= PlotX2 */
                        /* Do nothing. No intersection. */
                    } else if (PlotX1 > StartX) {
                        /* StartX < PlotX1 <= EndX < PlotX2 */
                        /* MoveUR.X not changed */
                        Cursor->MoveUR.L[MoveURID] = PlotX1 - StartX;
                    } else {
                        /* PlotX1 <= StartX <= EndX < PlotX2 */
                        Cursor->MoveUR.L[MoveURID] = 0; /* No line */
                    }
                } else {
                    /* PlotX2 == EndX */
                    if (PlotX1 > StartX) {
                        /* StartX < PlotX1 <= PlotX2 == EndX */
                        /* MoveUR.X not changed */
                        Cursor->MoveUR.L[MoveURID] = PlotX1 - StartX;
                    } else {
                        /* PlotX1 <= StartX <= EndX == PlotX2 */
                        Cursor->MoveUR.L[MoveURID] = 0; /* No line */
                    }
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid Iteration", __func__, NULL, NULL, NULL, NULL);
    }
}

/**
 *  Caalculate LineWidth at T.
 *
 *  @param [in]  T                  T value (0 ~ 1)
 *  @param [in]  T1                 T1 value (0 ~ 1)
 *  @param [in]  T2                 T2 value (0 ~ 1)
 *  @param [in]  W1                 LineWidth at T1
 *  @param [in]  W2                 LineWidth at T2
 *
 *  @return LineWidth
 */
static inline UINT32 AmbaDrawCurve_CalcLineWidth(DOUBLE T, DOUBLE T1, DOUBLE T2, UINT32 W1, UINT32 W2)
{
    /* Presume that 0 <= T1 <= T <= T2 <= 1 */
    DOUBLE W = (DOUBLE)W1 + (((T - T1) * ((DOUBLE)W2 - (DOUBLE)W1)) / (T2 - T1));

    return (UINT32)AmbaDraw_Round(W);
}

/**
 *  Get LineWidth at T.
 *
 *  @param [in]  Curve              Curve config
 *  @param [in]  T                  T value (0 ~ 1)
 *
 *  @return LineWidth
 */
static inline UINT32 AmbaDrawCurve_GetLineWidth(const AMBA_DRAW_CURVE_DESC_s *Curve, DOUBLE T)
{
    UINT32 RetWidth = 1U;

    for (UINT32 i = 0U; i < (Curve->ObjCfg.ControlPointNum - 1U); ++i) {
        DOUBLE T1 = Curve->SplineX.Cfg.T[i];
        DOUBLE T2 = Curve->SplineX.Cfg.T[i+1U];
        UINT32 W1 = Curve->ObjCfg.LineWidth[i];
        UINT32 W2 = Curve->ObjCfg.LineWidth[i+1U];
        if ((T1 <= T) && (T <= T2)) {
            RetWidth = AmbaDrawCurve_CalcLineWidth(T, T1, T2, W1, W2);
            break;
        }
    }

    return RetWidth;
}

/**
 *  Start cursor
 *
 *  @param [in]  Cursor             Cursor
 *  @param [in]  Diameter           Diameter
 *  @param [in]  Radius             Radius
 *  @param [in]  YStart             Y coordinate
 */
static inline void AmbaDrawCursor_Start_SetMove(AMBA_DRAW_CURSOR_s *Cursor, INT32 Diameter, INT32 Radius, INT32 YStart)
{
    if (((UINT32)Diameter % 2U) == 1U) {
        /*
            Set the difference of moving right in MoveR, and set the difference of moving upper-right in MoveUR by the following steps:
            1. Calculate a filled circle with offset = 0 and set the line position by AmbaDrawCursor_SetMove().
            2. Call AmbaDrawCursor_SetMove() twice with i = 0 and 1.
         */
        for (UINT32 i = 0U; i < 2U; ++i) {
            /*
                When Diameter is odd, apply the following condition to Bresenham's algorithm:
                    Center = ((X1+X2)/2, (Y1+Y2)/2) ==> Both X and Y are integers
                    Radius = (Diameter - 1)/2 ==> Integer
             */
            INT32 X;        /* X value assuming center in (0, 0) */
            INT32 Y;        /* Y value assuming center in (0, 0) */
            INT32 D;        /* Decision variable */

            D = 5 - (4 * Radius);
            X = 0;
            Y = Radius;
            /* Evaluate the eighth part of the circle. And calculate the rest by symmetry of circle. */
            while (X <= Y) {
                /* Filled circle */
                INT32 PlotX1;
                INT32 PlotX2;
                INT32 PlotY;
                INT32 NextY = Y;
                UINT8 IsPlot = 0U;

                /*
                    To avoid drawing redundant lines, only draw in the following conditions:
                    1. Y will change (D >= 0) in the next iteration, or
                    2. This is the last iteration ((X + 1) <= NextY)
                    Consider symetric points. Apply the same rule to (OffsetY - Y).
                    However, draw (OffsetY + X) and (OffsetY - X) because X changes in every iterations.
                 */
                if (D >= 0) {
                    --NextY;
                    IsPlot = 1U;
                }

                if ((X + 1) > NextY) {
                    IsPlot = 1U;
                }

                /*
                    Plot 4 lines (OffsetX = 0 and OffsetY = 0):
                        (OffsetX - X, OffsetY + Y) -> (OffsetX + X, OffsetY + Y)
                        (OffsetX - X, OffsetY - Y) -> (OffsetX + X, OffsetY - Y)
                        (OffsetX - Y, OffsetY + X) -> (OffsetX + Y, OffsetY + X)
                        (OffsetX - Y, OffsetY - X) -> (OffsetX + Y, OffsetY - X)
                 */
                if (IsPlot == 1U) {
                    PlotX1 = -X;
                    PlotX2 = X;
                    PlotY = Y;
                    AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);
                    PlotX1 = -X;
                    PlotX2 = X;
                    PlotY = -Y;
                    AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);
                }
                PlotX1 = -Y;
                PlotX2 = Y;
                PlotY = X;
                AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);
                PlotX1 = -Y;
                PlotX2 = Y;
                PlotY = -X;
                AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);

                if (D < 0) {
                    D += ((8 * X) + 12);
                    ++X;
                } else {
                    D += ((8 * (X - Y)) + 20);
                    ++X;
                    --Y;
                }
            }
        }
    } else {
        /*
            Set the difference of moving right in MoveR, and set the difference of moving upper-right in MoveUR by the following steps:
            1. Calculate a filled circle with offset = 0 and set the line position by AmbaDrawCursor_SetMove().
            2. Call AmbaDrawCursor_SetMove() twice with i = 0 and 1.
         */
        for (UINT32 i = 0U; i < 2U; ++i) {
            /*
                When Diameter is even, apply the following condition to Bresenham's algorithm:
                    Center = ((X1+X2)/2, (Y1+Y2)/2) ==> Both X and Y are not integers
                    Radius = Diameter/2 ==> Integer
                Center is not integer. However, X1, X2, Y1, and Y2 must be integers.
                So this case can still be handled by integer arithmetic.
             */
            INT32 X;        /* X value assuming center in (0.5, 0.5) */
            INT32 Y;        /* Y value assuming center in (0.5, 0.5) */
            INT32 D;        /* Decision variable */

            D = 0;
            X = 1;
            /*
                Precisely, Y = (1 + sqrt((2R)^2 - 1)) / 2.
                So Y is slightly less than (1 + sqrt((2R)^2)) / 2 = R + 1/2.
                Since R is integer, Y must be R after rounding.
             */
            Y = Radius;
            /* Evaluate the eighth part of the circle. And calculate the rest by symmetry of circle. */
            while (X <= Y) {
                /* Filled circle */
                INT32 PlotX1;
                INT32 PlotX2;
                INT32 PlotY;
                INT32 NextY = Y;
                UINT8 IsPlot = 0U;

                /*
                    To avoid drawing redundant lines, only draw in the following conditions:
                    1. Y will change (D >= 0) in the next iteration, or
                    2. This is the last iteration ((X + 1) <= NextY)
                    Consider symetric points. Apply the same rule to (OffsetY - Y + 1).
                    However, draw (OffsetY + X) and (OffsetY - X + 1) because X changes in every iterations.
                 */
                if (D >= 0) {
                    --NextY;
                    IsPlot = 1U;
                }

                if ((X + 1) > NextY) {
                    IsPlot = 1U;
                }

                /*
                    Plot 4 lines (OffsetX = 0 and OffsetY = 0):
                        (OffsetX - X + 1, OffsetY + Y)     -> (OffsetX + X, OffsetY + Y)
                        (OffsetX - X + 1, OffsetY - Y + 1) -> (OffsetX + X, OffsetY - Y + 1)
                        (OffsetX - Y + 1, OffsetY + X)     -> (OffsetX + Y, OffsetY + X)
                        (OffsetX - Y + 1, OffsetY - X + 1) -> (OffsetX + Y, OffsetY - X + 1)
                 */
                if (IsPlot == 1U) {
                    PlotX1 = -X + 1;
                    PlotX2 = X;
                    PlotY = Y;
                    AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);
                    PlotX1 = -X + 1;
                    PlotX2 = X;
                    PlotY = -Y + 1;
                    AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);
                }
                PlotX1 = -Y + 1;
                PlotX2 = Y;
                PlotY = X;
                AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);
                PlotX1 = -Y + 1;
                PlotX2 = Y;
                PlotY = -X + 1;
                AmbaDrawCursor_SetMove(Cursor, i, PlotX1, PlotX2, PlotY, YStart);

                if (D < 0) {
                    D += ((2 * X) + 2);
                    ++X;
                } else {
                    D += ((2 * (X - Y)) + 5);
                    ++X;
                    --Y;
                }
            }
        }
    }
}

/**
 *  Start cursor
 *
 *  @param [in]  Render             Render handler
 *  @param [in]  CursorX            X coordinate
 *  @param [in]  CursorY            Y coordinate
 *  @param [in]  Thickness          Line thickness
 *  @param [in]  Color              Line color
 *  @param [out] Cursor             Cursor
 */
static inline void AmbaDrawCursor_Start(const AMBA_DRAW_RENDER_s *Render, INT32 CursorX, INT32 CursorY, UINT32 Thickness, const AMBA_DRAW_COLOR_s *Color, AMBA_DRAW_CURSOR_s *Cursor)
{
    static UINT8 G_CursorInit  = 0U;         /* Cursor is initialized        */
    if (G_CursorInit == 0U) {
        if (DRAW_WRAP2D(AmbaWrap_memset(&G_Cursor, 0, sizeof(AMBA_DRAW_CURSOR_s))) != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }
        G_CursorInit = 1U;
    }

    if (G_CursorStart == 1U) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Already started", __func__, NULL, NULL, NULL, NULL);
    } else {
        const UINT8 IsSameThickness = (Cursor->Thickness == Thickness) ? (1U) : (0U);
        const INT32 Diameter = (INT32)Thickness;
        const INT32 Radius = Diameter / 2; /* FLOOR(Diameter / 2) */
        /* The boundary of a circle with diameter = Thickness and center at (0, 0) or (0.5, 0.5) (when diameter is odd or even) */
        const INT32 XStart = Radius + 1 - Diameter; // 1 - ROOF(Diameter / 2)
        const INT32 YStart = Radius + 1 - Diameter; /* 1 - ROOF(Diameter / 2) */
        const INT32 XEnd = Radius;
        const INT32 YEnd = Radius;
        Cursor->Render = Render;
        Cursor->X = CursorX;
        Cursor->Y = CursorY;
        Cursor->NextX = CursorX;
        Cursor->Thickness = Thickness;
        Cursor->Color = *Color;
        G_CursorStart = 1U;
        /* Draw a filled circle (Thickness = 0) at the current position */
        if (AmbaDrawCirc_Render(Render, XStart + CursorX, YStart + CursorY, XEnd + CursorX, YEnd + CursorY, 0U, Color) != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCirc_Render failed", __func__, NULL, NULL, NULL, NULL);
        }
        if (IsSameThickness == 0U) {
            /* Initialize MoveR and MoveUR. ("L == 0" means no line) */
            for (UINT32 i = 0U; i < ((UINT32)AMBA_DRAW_LINE_WIDTH_MAX*2U); ++i) {
                Cursor->MoveR.L[i] = 0;
                Cursor->MoveUR.L[i] = 0;
            }

            /* Set Num to 0 before calling AmbaDrawCursor_SetMove */
            Cursor->MoveR.Num = 0;
            Cursor->MoveUR.Num = 0;

            /*
                Use Bresenham Circle Drawing Algorithm
             */
            AmbaDrawCursor_Start_SetMove(Cursor, Diameter, Radius, YStart);
        }
    }
}

/**
 *  Move cursor up or down
 *
 *  @param [in] Cursor              Cursor
 *  @param [in] Y                   Y coordinate
 *  @param [in] Adjust              Adjustment when calculating mirror points
 */
static inline void AmbaDrawCursor_MoveY(AMBA_DRAW_CURSOR_s *Cursor, INT32 Y, INT32 Adjust)
{
    if (Y > Cursor->Y) {
        /* Move Up */
        INT32 StartY = Cursor->Y;
        INT32 EndY   = Y - 1; /* The reason of "-1" : Movement draws the position of 1-pixel shift, so do not draw at the end position. */
        for (UINT32 i = 0U; i < Cursor->MoveR.Num; ++i) {
            if (Cursor->MoveR.L[i] > 0) {
                INT32 Y1 = StartY + Cursor->MoveR.X[i];
                INT32 Y2 = EndY   + Cursor->MoveR.X[i] + Cursor->MoveR.L[i] - 1;
                AmbaDrawShape_PlotVLine(Cursor->Render, Cursor->X + Cursor->MoveR.Y[i], Y1, Y2, &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->Y = Y;
    } else if (Y < Cursor->Y) {
        /* Move Down */
        INT32 StartY = Y + 1; /* The reason of "+1" : Movement draws the position of 1-pixel shift, so do not draw at the end position. */
        INT32 EndY   = Cursor->Y;
        for (UINT32 i = 0U; i < Cursor->MoveR.Num; ++i) {
            if (Cursor->MoveR.L[i] > 0) {
                INT32 Y1 = StartY - (Cursor->MoveR.X[i] + Cursor->MoveR.L[i] - 1) + Adjust;
                INT32 Y2 = EndY   - Cursor->MoveR.X[i] + Adjust;
                AmbaDrawShape_PlotVLine(Cursor->Render, Cursor->X + Cursor->MoveR.Y[i], Y1, Y2, &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->Y = Y;
    } else {
        /* Do nothing */
    }
}

/**
 *  Move cursor upper right or lower right
 *
 *  @param [in] Cursor              Cursor
 *  @param [in] X                   X coordinate
 *  @param [in] Y                   Y coordinate
 *  @param [in] Adjust              Adjustment when calculating mirror points
 */
static inline void AmbaDrawCursor_MoveYR(AMBA_DRAW_CURSOR_s *Cursor, INT32 X, INT32 Y, INT32 Adjust)
{
    if (Y > Cursor->Y) {
        /* Move Upper Right */
        for (UINT32 i = 0U; i < Cursor->MoveUR.Num; ++i) {
            if (Cursor->MoveUR.L[i] > 0) {
                INT32 X1 = Cursor->X + Cursor->MoveUR.X[i];
                INT32 X2 = Cursor->X + Cursor->MoveUR.X[i] + Cursor->MoveUR.L[i] - 1;
                AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y + Cursor->MoveUR.Y[i], &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->X = X;
        Cursor->Y = Y;
        Cursor->NextX = X;
    } else if (Y < Cursor->Y) {
        /* Move Lower Right */
        for (UINT32 i = 0U; i < Cursor->MoveUR.Num; ++i) {
            if (Cursor->MoveUR.L[i] > 0) {
                INT32 X1 = Cursor->X + Cursor->MoveUR.X[i];
                INT32 X2 = Cursor->X + Cursor->MoveUR.X[i] + Cursor->MoveUR.L[i] - 1;
                AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y - Cursor->MoveUR.Y[i] + Adjust, &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->X = X;
        Cursor->Y = Y;
        Cursor->NextX = X;
    } else {
        AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCursor_MoveYR: [ERROR] Y %d CursorY %d", Y, Cursor->Y, 0, 0, 0);
    }
}

/**
 *  Move cursor upper left or lower left
 *
 *  @param [in] Cursor              Cursor
 *  @param [in] X                   X coordinate
 *  @param [in] Y                   Y coordinate
 *  @param [in] Adjust              Adjustment when calculating mirror points
 */
static inline void AmbaDrawCursor_MoveYL(AMBA_DRAW_CURSOR_s *Cursor, INT32 X, INT32 Y, INT32 Adjust)
{
    if (Y > Cursor->Y) {
        /* Move Upper Left */
        for (UINT32 i = 0U; i < Cursor->MoveUR.Num; ++i) {
            if (Cursor->MoveUR.L[i] > 0) {
                INT32 X1 = Cursor->X - (Cursor->MoveUR.X[i] + Cursor->MoveUR.L[i] - 1) + Adjust;
                INT32 X2 = Cursor->X - Cursor->MoveUR.X[i] + Adjust;
                AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y + Cursor->MoveUR.Y[i], &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->X = X;
        Cursor->Y = Y;
        Cursor->NextX = X;
    } else if (Y < Cursor->Y) {
        /* Move Lower Left */
        for (UINT32 i = 0U; i < Cursor->MoveUR.Num; ++i) {
            if (Cursor->MoveUR.L[i] > 0) {
                INT32 X1 = Cursor->X - (Cursor->MoveUR.X[i] + Cursor->MoveUR.L[i] - 1) + Adjust;
                INT32 X2 = Cursor->X - Cursor->MoveUR.X[i] + Adjust;
                AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y - Cursor->MoveUR.Y[i] + Adjust, &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->X = X;
        Cursor->Y = Y;
        Cursor->NextX = X;
    } else {
        AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCursor_MoveYL: [ERROR] Y %d CursorY %d", Y, Cursor->Y, 0, 0, 0);
    }
}

/**
 *  Move cursor
 *
 *  @param [in] Cursor              Cursor
 *  @param [in] X                   X coordinate
 *  @param [in] Y                   Y coordinate
 */
static inline void AmbaDrawCursor_Move_Plot(AMBA_DRAW_CURSOR_s *Cursor, INT32 X, INT32 Y)
{
    INT32 DiffY = Y - Cursor->Y;
    INT32 Adjust = 0;
    /* Adjustment when calculating mirror points */
    if ((Cursor->Thickness % 2U) == 0U) {
        Adjust = 1;
    }
    /* Draw X direction */
    if (Cursor->NextX > Cursor->X) {
        /* Move Right */
        INT32 StartX = Cursor->X;
        INT32 EndX   = Cursor->NextX - 1; /* The reason of "-1" : Movement draws the position of 1-pixel shift, so do not draw at the end position. */
        for (UINT32 i = 0U; i < Cursor->MoveR.Num; ++i) {
            if (Cursor->MoveR.L[i] > 0) {
                INT32 X1 = StartX + Cursor->MoveR.X[i];
                INT32 X2 = EndX   + Cursor->MoveR.X[i] + Cursor->MoveR.L[i] - 1;
                AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y + Cursor->MoveR.Y[i], &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->X = Cursor->NextX;
    } else if (Cursor->NextX < Cursor->X) {
        /* Move Left */
        INT32 StartX = Cursor->NextX + 1; /* The reason of "+1" : Movement draws the position of 1-pixel shift, so do not draw at the end position. */
        INT32 EndX   = Cursor->X;
        for (UINT32 i = 0U; i < Cursor->MoveR.Num; ++i) {
            if (Cursor->MoveR.L[i] > 0) {
                INT32 X1 = StartX - (Cursor->MoveR.X[i] + Cursor->MoveR.L[i] - 1) + Adjust;
                INT32 X2 = EndX   - Cursor->MoveR.X[i] + Adjust;
                AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y + Cursor->MoveR.Y[i], &Cursor->Color);
            }
        }
        /* Update cursor */
        Cursor->X = Cursor->NextX;
    } else {
        /* Cursor has not moved along X axis */
        /* Do nothing */
    }

    /* Draw Y or diagonal direction */
    if (DiffY != 0) {
        /* Get direction */
        if (X == Cursor->X) {
            /* Move Up or Down */
            AmbaDrawCursor_MoveY(Cursor, Y, Adjust);
        } else if (X > Cursor->X) {
            /* Move Upper Right or Lower Right */
            AmbaDrawCursor_MoveYR(Cursor, X, Y, Adjust);
        } else {
            /* Move Upper Left or Lower Left */
            AmbaDrawCursor_MoveYL(Cursor, X, Y, Adjust);
        }
    }
}

/**
 *  Move cursor
 *
 *  @param [in] Cursor              Cursor
 *  @param [in] X                   X coordinate
 *  @param [in] Y                   Y coordinate
 */
static inline void AmbaDrawCursor_Move(AMBA_DRAW_CURSOR_s *Cursor, INT32 X, INT32 Y)
{
    INT32 DiffX = X - Cursor->NextX;
    INT32 DiffY = Y - Cursor->Y;
    UINT8 IsPlot = 0U;
    if (G_CursorStart == 0U) {
        /* Do nothing */
    } else if ((DiffX == 0) && (DiffY == 0)) {
        /* Do nothing */
    } else if (DiffY == 0) {
        /* Plotting line is faster than plotting each point separately. So keep the change of X and plot line when Y changes or cursor stop. */

        /* Check the change of direction */
        if (DiffX > 0) {
            if ((Cursor->NextX - Cursor->X) < 0) {
                /* Plot and change direction */
                IsPlot = 1U;
            } else {
                /* Case1: Direction not changed. Accumulate the change. */
                /* Case2: (Cursor->NextX == Cursor->X). Cursor has not moved along X axis. */
                Cursor->NextX = X;
            }
        } else {
            if ((Cursor->NextX - Cursor->X) > 0) {
                /* Plot and change direction */
                IsPlot = 1U;
            } else {
                /* Case1: Direction not changed. Accumulate the change. */
                /* Case2: (Cursor->NextX == Cursor->X). Cursor has not moved along X axis. */
                Cursor->NextX = X;
            }
        }
    } else {
        /* Y is changed */
        if (DiffX == 0) {
            IsPlot = 1U;
        } else if ((DiffX == 1) || (DiffX == -1)) {
            if ((DiffY == 1) || (DiffY == -1)) {
                IsPlot = 1U;
            } else {
                AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCursor_Move: [ERROR] DiffX %d DiffY %d", DiffX, DiffY, 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCursor_Move: [ERROR] DiffX %d DiffY %d", DiffX, DiffY, 0, 0, 0);
        }
    }

    if (IsPlot == 1U) {
        AmbaDrawCursor_Move_Plot(Cursor, X, Y);
    }
}

/**
 *  Stop cursor
 *
 *  @param [in] Cursor              Cursor
 */
static inline void AmbaDrawCursor_Stop(AMBA_DRAW_CURSOR_s *Cursor)
{
    UINT32 i;
    /* Movement in Y axis is already drawn in AmbaDrawCursor_Move(), so only consider X axis. */
    /* Plot the remain point or line. */
    if (G_CursorStart == 0U) {
        /* Do nothing */
    } else {
        INT32 Adjust = 0;
        /* Adjustment when calculating mirror points */
        if ((Cursor->Thickness % 2U) == 0U) {
            Adjust = 1;
        }
        /* Draw X direction */
        if (Cursor->NextX > Cursor->X) {
            /* Move Right */
            INT32 StartX = Cursor->X;
            INT32 EndX   = Cursor->NextX - 1; /* The reason of "-1" : Movement draws the position of 1-pixel shift, so do not draw at the end position. */
            for (i = 0U; i < Cursor->MoveR.Num; ++i) {
                if (Cursor->MoveR.L[i] > 0) {
                    INT32 X1 = StartX + Cursor->MoveR.X[i];
                    INT32 X2 = EndX   + Cursor->MoveR.X[i] + Cursor->MoveR.L[i] - 1;
                    AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y + Cursor->MoveR.Y[i], &Cursor->Color);
                }
            }
            /* Update cursor */
            Cursor->X = Cursor->NextX;
        } else if (Cursor->NextX < Cursor->X) {
            /* Move Left */
            INT32 StartX = Cursor->NextX + 1; /* The reason of "+1" : Movement draws the position of 1-pixel shift, so do not draw at the end position. */
            INT32 EndX   = Cursor->X;
            for (i = 0U; i < Cursor->MoveR.Num; ++i) {
                if (Cursor->MoveR.L[i] > 0) {
                    INT32 X1 = StartX - (Cursor->MoveR.X[i] + Cursor->MoveR.L[i] - 1) + Adjust;
                    INT32 X2 = EndX   - Cursor->MoveR.X[i] + Adjust;
                    AmbaDrawShape_PlotHLine(Cursor->Render, X1, X2, Cursor->Y + Cursor->MoveR.Y[i], &Cursor->Color);
                }
            }
            /* Update cursor */
            Cursor->X = Cursor->NextX;
        } else {
            /* Cursor has not moved along X axis */
            /* Do nothing */
        }

        /* Update */
        G_CursorStart = 0U;
    }
}

/**
 *  Check postion
 *
 *  @param [in] Postion             Postion
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_IsValidPos(UINT32 Postion)
{
    UINT32 Rval = DRAW_ERROR_ARG;
    switch (Postion) {
    case AMBA_DRAW_POSITION_TL:
    case AMBA_DRAW_POSITION_TM:
    case AMBA_DRAW_POSITION_TR:
    case AMBA_DRAW_POSITION_ML:
    case AMBA_DRAW_POSITION_MM:
    case AMBA_DRAW_POSITION_MR:
    case AMBA_DRAW_POSITION_BL:
    case AMBA_DRAW_POSITION_BM:
    case AMBA_DRAW_POSITION_BR:
        Rval = DRAW_OK;
        break;
    default:
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_IsValidPos: [ERROR] Invalid Postion %u", Postion, 0U, 0U, 0U, 0U);
        break;
    }
    return Rval;
}

/**
 *  Check shadow config
 *
 *  @param [in] Shadow              Shadow config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_IsValidShadow(const AMBA_DRAW_SHADOW_s *Shadow)
{
    UINT32 Rval = DRAW_ERROR_ARG;
    if (Shadow != NULL) {
        if (Shadow->Enable == 1U) {
            if (Shadow->Distance > 0U) {
                Rval = AmbaDraw_IsValidPos(Shadow->ShadowPostion);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_IsValidPos failed", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_IsValidShadow: [ERROR] Invalid Distance %u", Shadow->Distance, 0U, 0U, 0U, 0U);
            }
        } else if (Shadow->Enable == 0U) {
            Rval = DRAW_OK;
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_IsValidShadow: [ERROR] Invalid Enable %u", Shadow->Enable, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Shadow is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawLine_IsValidConfig(const AMBA_DRAW_LINE_CFG_s *ObjCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;
    if (ObjCfg != NULL) {
        /* Both points should be the corner of the object */
        if (((ObjCfg->X1 == 0) && (ObjCfg->Y1 == 0)) || ((ObjCfg->Y1 == 0) && (ObjCfg->X2 == 0))) {
            /* Two points should not overlap */
            if ((ObjCfg->X1 != ObjCfg->X2) || (ObjCfg->Y1 != ObjCfg->Y2)) {
                if ((ObjCfg->LineWidth > 0U) && (ObjCfg->LineWidth <= AMBA_DRAW_LINE_WIDTH_MAX)) {
                    UINT32 XDistU32 = (UINT32)AmbaDraw_Abs(ObjCfg->X2 - ObjCfg->X1);
                    UINT32 YDistU32 = (UINT32)AmbaDraw_Abs(ObjCfg->Y2 - ObjCfg->Y1);
                    UINT64 XDist = (UINT64)XDistU32;
                    UINT64 YDist = (UINT64)YDistU32;
                    UINT64 LineLengthSquare = (XDist * XDist) + (YDist * YDist);
                    UINT64 DashIntervalSquare = (UINT64)ObjCfg->DashInterval * (UINT64)ObjCfg->DashInterval * 4ULL;
                    /* Line length should be larger than 2*DashInterval */
                    if (LineLengthSquare > DashIntervalSquare) {
                        Rval = DRAW_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument DashInterval", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_IsValidConfig: [ERROR] Invalid LineWidth %u", ObjCfg->LineWidth, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_IsValidConfig: [ERROR] Invalid coordinate (%d %d) (%d %d)", ObjCfg->X1, ObjCfg->Y1, ObjCfg->X2, ObjCfg->Y2, 0);
            }
        } else {
            AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_IsValidConfig: [ERROR] Invalid coordinate (%d %d) (%d %d)", ObjCfg->X1, ObjCfg->Y1, ObjCfg->X2, ObjCfg->Y2, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Draw line from (X1, Y1) to (X2, Y2) with |Slope| <= 1
 *
 *  @param [in] Render              Render handler
 *  @param [in] X1                  X1 value (in pixel)
 *  @param [in] Y1                  Y1 value (in pixel)
 *  @param [in] X2                  X2 value (in pixel)
 *  @param [in] Y2                  Y2 value (in pixel)
 *  @param [in] Thickness           Line thickness (in pixel)
 *  @param [in] DashInterval        Dash interval (in pixel)
 *  @param [in] Color               Line color
 */
static void AmbaDrawLine_Render_GentleSlope(const AMBA_DRAW_RENDER_s *Render, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2, UINT32 Thickness, UINT32 DashInterval, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 DiffX = X2 - X1;
    INT32 DiffY = Y2 - Y1;
    INT32 X;        /* X value to plot point */
    INT32 Y;        /* Y value to plot point */
    INT32 D;        /* Decision variable */
    INT32 E1;       /* Change of D when D < 0 */
    INT32 E2;       /* Change of D when D >= 0 */
    INT32 StepX;    /* Change of X */
    INT32 StepY1;   /* Change of Y when D < 0 */
    INT32 StepY2;   /* Change of Y when D >= 0 */
    UINT32 IntervalCount;
    const UINT32 DoubleDashInterval = DashInterval << 1U;
    if (DiffX >= 0) {
        StepX = 1;
        if (DiffY >= 0) {
            D = (2 * DiffY) - DiffX;
            E1 = 2 * DiffY;
            E2 = 2 * (DiffY - DiffX);
            StepY1 = 0;
            StepY2 = 1;
        } else {
            D = (2 * DiffY) + DiffX;
            E1 = 2 * (DiffY + DiffX);
            E2 = 2 * DiffY;
            StepY1 = -1;
            StepY2 = 0;
        }
    } else {
        StepX = -1;
        if (DiffY <= 0) {
            D = -(2 * DiffY) + DiffX;
            E1 = -2 * DiffY;
            E2 = -2 * (DiffY - DiffX);
            StepY1 = 0;
            StepY2 = -1;
        } else {
            D = -(2 * DiffY) - DiffX;
            E1 = -2 * (DiffY + DiffX);
            E2 = -2 * DiffY;
            StepY1 = 1;
            StepY2 = 0;
        }
    }
    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_Render: Case2 (DiffY %d) D %d E1 %d E2 %d", DiffY, D, E1, E2, 0);

    /* Scan X from X1 to X2, and determine Y for each X */
    X = X1;
    Y = Y1;
    IntervalCount = 0U;
    for ( ; ; ) {
        UINT32 IsPlot;  /* Whether to plot the point */
        /* Check Dashed line */
        if (DashInterval == 0U) {
            /* Solid line */
            IsPlot = 1U;
        } else {
            /* Dashed line */
            if (IntervalCount < DashInterval) {
                /* 0 <= IntervalCount < DashInterval */
                IsPlot = 1U;
            } else {
                /* DashInterval <= IntervalCount < 2*DashInterval */
                IsPlot = 0U;
            }
            ++IntervalCount;
            if (IntervalCount >= DoubleDashInterval) {
                IntervalCount = 0U;
            }
        }

        /* Plot point */
        if (IsPlot == 1U) {
            //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_Render: Plot(%d %d) Color %d D %d", X, Y, (INT32)Color, D, 0);
            if (G_CursorStart == 0U) {
                AmbaDrawCursor_Start(Render, X, Y, Thickness, Color, &G_Cursor);
            } else {
                AmbaDrawCursor_Move(&G_Cursor, X, Y);
            }
        } else {
            AmbaDrawCursor_Stop(&G_Cursor);
        }

        if (X == X2) {
            if (G_CursorStart == 1U) {
                AmbaDrawCursor_Move(&G_Cursor, X, Y);
            } else {
                /* Do nothing */
            }
            break;
        } else {
            if (D < 0) {
                D += E1;
                Y += StepY1;
            } else {
                D += E2;
                Y += StepY2;
            }

            X += StepX;
        }
    }
}

/**
 *  Draw line from (X1, Y1) to (X2, Y2) with |Slope| > 1
 *
 *  @param [in] Render              Render handler
 *  @param [in] X1                  X1 value (in pixel)
 *  @param [in] Y1                  Y1 value (in pixel)
 *  @param [in] X2                  X2 value (in pixel)
 *  @param [in] Y2                  Y2 value (in pixel)
 *  @param [in] Thickness           Line thickness (in pixel)
 *  @param [in] DashInterval        Dash interval (in pixel)
 *  @param [in] Color               Line color
 */
static void AmbaDrawLine_Render_SteepSlope(const AMBA_DRAW_RENDER_s *Render, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2, UINT32 Thickness, UINT32 DashInterval, const AMBA_DRAW_COLOR_s *Color)
{
    INT32 DiffX = X2 - X1;
    INT32 DiffY = Y2 - Y1;
    INT32 X;        /* X value to plot point */
    INT32 Y;        /* Y value to plot point */
    INT32 D;        /* Decision variable */
    INT32 E1;       /* Change of D when D < 0 */
    INT32 E2;       /* Change of D when D >= 0 */
    INT32 StepX1;   /* Change of X when D < 0 */
    INT32 StepX2;   /* Change of X when D >= 0 */
    INT32 StepY;    /* Change of Y */
    UINT32 IntervalCount;
    const UINT32 DoubleDashInterval = DashInterval << 1U;
    if (DiffY >= 0) {
        StepY = 1;
        if (DiffX >= 0) {
            D = DiffY - (2 * DiffX);
            E1 = 2 * (DiffY - DiffX);
            E2 = -2 * DiffX;
            StepX1 = 1;
            StepX2 = 0;
        } else {
            D = -DiffY - (2 * DiffX);
            E1 = -2 * DiffX;
            E2 = -2 * (DiffY + DiffX);
            StepX1 = 0;
            StepX2 = -1;
        }
    } else {
        StepY = -1;
        if (DiffX <= 0) {
            D = -DiffY + (2 * DiffX);
            E1 = -2 * (DiffY - DiffX);
            E2 = 2 * DiffX;
            StepX1 = -1;
            StepX2 = 0;
        } else {
            D = DiffY + (2 * DiffX);
            E1 = 2 * DiffX;
            E2 = 2 * (DiffY + DiffX);
            StepX1 = 0;
            StepX2 = 1;
        }
    }
    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_Render: Case2 (DiffX %d) D %d E1 %d E2 %d", DiffX, D, E1, E2, 0);

    /* Scan Y from Y1 to Y2, and determine X for each Y */
    X = X1;
    Y = Y1;
    IntervalCount = 0U;
    for ( ; ; ) {
        UINT32 IsPlot;  /* Whether to plot the point */
        /* Check Dashed line */
        if (DashInterval == 0U) {
            /* Solid line */
            IsPlot = 1U;
        } else {
            /* Dashed line */
            if (IntervalCount < DashInterval) {
                /* 0 <= IntervalCount < DashInterval */
                IsPlot = 1U;
            } else {
                /* DashInterval <= IntervalCount < 2*DashInterval */
                IsPlot = 0U;
            }
            ++IntervalCount;
            if (IntervalCount >= DoubleDashInterval) {
                IntervalCount = 0U;
            }
        }

        /* Plot point */
        if (IsPlot == 1U) {
            //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_Render: Plot(%d %d) Color %d D %d", X, Y, (INT32)Color, D, 0);
            if (G_CursorStart == 0U) {
                AmbaDrawCursor_Start(Render, X, Y, Thickness, Color, &G_Cursor);
            } else {
                AmbaDrawCursor_Move(&G_Cursor, X, Y);
            }
        } else {
            AmbaDrawCursor_Stop(&G_Cursor);
        }

        if (Y == Y2) {
            if (G_CursorStart == 1U) {
                AmbaDrawCursor_Move(&G_Cursor, X, Y);
            } else {
                /* Do nothing */
            }
            break;
        } else {
            if (D < 0) {
                D += E1;
                X += StepX1;
            } else {
                D += E2;
                X += StepX2;
            }

            Y += StepY;
        }
    }
}

/**
 *  Draw line from (X1, Y1) to (X2, Y2)
 *
 *  @param [in] Render              Render handler
 *  @param [in] X1                  X1 value (in pixel)
 *  @param [in] Y1                  Y1 value (in pixel)
 *  @param [in] X2                  X2 value (in pixel)
 *  @param [in] Y2                  Y2 value (in pixel)
 *  @param [in] Thickness           Line thickness (in pixel)
 *  @param [in] DashInterval        Dash interval (in pixel)
 *  @param [in] Color               Line color
 *  @param [in] IsContinue          There's another line starts from the end point of this line
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawLine_Render(const AMBA_DRAW_RENDER_s *Render, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2, UINT32 Thickness, UINT32 DashInterval, const AMBA_DRAW_COLOR_s *Color, UINT8 IsContinue)
{
    UINT32 Rval = DRAW_OK;
    INT32 DiffX = X2 - X1;
    INT32 DiffY = Y2 - Y1;
    INT32 DiffXAbs = AmbaDraw_Abs(DiffX);
    INT32 DiffYAbs = AmbaDraw_Abs(DiffY);

    /*
        Use Bresenham Line-Drawing Algorithm
        It's more efficient bacause:
        1. Use only integer arithmetic. No floating point is involved.
        2. Most of the arithmetic are additions. Only few multiplications are used.

        NOTE: Do not swap (X1, Y1) and (X2, Y2) (intended for simplifying the process) in any case!!
        When drawing line strip, it's necessary to draw points in order (because of the method of drawing thick lines).
     */

    //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_Render: (%u %u) -> (%u %u)", X1, Y1, X2, Y2, 0U);

    if (Thickness == 0U) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Thickness is 0", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if ((DiffX == 0) && (DiffY == 0)) {
        /* Case1: (X1, Y1) == (X2, Y2) */
        /* Plot point */
        if (IsContinue == 0U) {
            if (G_CursorStart == 0U) {
                AmbaDrawCursor_Start(Render, X1, Y1, Thickness, Color, &G_Cursor);
            }
            AmbaDrawCursor_Stop(&G_Cursor);
        }
    } else if (DiffYAbs <= DiffXAbs) {
        /* Case2: |Slope| <= 1 */
        AmbaDrawLine_Render_GentleSlope(Render, X1, Y1, X2, Y2, Thickness, DashInterval, Color);
    } else {
        /* Case3: |Slope| > 1 */
        AmbaDrawLine_Render_SteepSlope(Render, X1, Y1, X2, Y2, Thickness, DashInterval, Color);
    }

    /* Make sure Cursor is stopped when IsContinue = 0U */
    if (IsContinue == 0U) {
        AmbaDrawCursor_Stop(&G_Cursor);
    }

    return Rval;
}

/**
 *  Calculate the object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            The returned object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawLine_CalcArea(const AMBA_DRAW_LINE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_OK;
    const AMBA_DRAW_LINE_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
    INT32 MinX;
    INT32 MinY;
    UINT32 HalfThicknessL;

    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_CreateObj: X1 %d Y1 %d X2 %d Y2 %d LineWidth %d", ObjCfg->X1, ObjCfg->Y1, ObjCfg->X2, ObjCfg->Y2, (INT32)ObjCfg->LineWidth);

    HalfThicknessL = ObjCfg->LineWidth / 2U;
    if ((ObjCfg->LineWidth > 0U) && ((ObjCfg->LineWidth % 2U) != 1U)) { /* Ex: 4/2 = 2, TL = 1, TR = 2 */
        HalfThicknessL -= 1U;
    }

    MinX = AmbaDraw_Min(ObjCfg->X1, ObjCfg->X2);
    MinY = AmbaDraw_Min(ObjCfg->Y1, ObjCfg->Y2);
    ObjArea->X = MinX + ObjDesc->OffsetX - (INT32)HalfThicknessL;
    ObjArea->Y = MinY + ObjDesc->OffsetY - (INT32)HalfThicknessL;
    ObjArea->Width = ObjCfg->LineWidth + (UINT32)AmbaDraw_Abs(ObjCfg->X2 - ObjCfg->X1);
    ObjArea->Height = ObjCfg->LineWidth + (UINT32)AmbaDraw_Abs(ObjCfg->Y2 - ObjCfg->Y1);

    return Rval;
}

/**
 *  Check parameter
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawLine_GetObjArea_ParamCheck(const AMBA_DRAW_LINE_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjDesc != NULL) {
        if (ObjArea != NULL) {
            Rval = AmbaDrawLine_IsValidConfig(&ObjDesc->ObjCfg);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawLine_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjArea is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawLine_GetObjArea(const AMBA_DRAW_LINE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    Rval = AmbaDrawLine_GetObjArea_ParamCheck(ObjDesc, ObjArea);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawLine_CalcArea(ObjDesc, ObjArea);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawLine_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Draw the Line object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawLine_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_LINE_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;

    /* Check parameters. Area can be NULL */
    if (Render == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Render is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (ObjDesc == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else {
        AMBA_DRAW_AREA_s ObjArea;
        Rval = DRAW_WRAP2D(AmbaWrap_memset(&ObjArea, 0x0, sizeof(ObjArea)));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == DRAW_OK) {
            Rval = AmbaDrawLine_CalcArea(ObjDesc, &ObjArea);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawLine_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Check area */
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_CheckObjArea(Render, &ObjArea, Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_CheckObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if (Rval == DRAW_OK) {
            INT32 X1 = ObjDesc->OffsetX + ObjDesc->ObjCfg.X1;
            INT32 Y1 = ObjDesc->OffsetY + ObjDesc->ObjCfg.Y1;
            INT32 X2 = ObjDesc->OffsetX + ObjDesc->ObjCfg.X2;
            INT32 Y2 = ObjDesc->OffsetY + ObjDesc->ObjCfg.Y2;
            Rval = AmbaDrawLine_Render(Render, X1, Y1, X2, Y2, ObjDesc->ObjCfg.LineWidth, ObjDesc->ObjCfg.DashInterval, &ObjDesc->ObjCfg.LineColor, 0U);
        }
    }

    return Rval;
}

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRect_IsValidConfig(const AMBA_DRAW_RECT_CFG_s *ObjCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjCfg != NULL) {
        if (ObjCfg->Width > 0U) {
            if (ObjCfg->Height > 0U) {
                if (ObjCfg->LineWidth <= AMBA_DRAW_LINE_WIDTH_MAX) {
                    Rval = AmbaDraw_IsValidShadow(&(ObjCfg->Shadow));
                    if (Rval != DRAW_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_IsValidShadow failed", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRect_IsValidConfig: [ERROR] Invalid LineWidth %u", ObjCfg->LineWidth, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRect_IsValidConfig: [ERROR] Invalid Height %u", ObjCfg->Height, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRect_IsValidConfig: [ERROR] Invalid Width %u", ObjCfg->Width, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  @brief Init the rendoer of rectangle
 *
 *  Init the rendoer of rectangle
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  X1                 The top left position of the updated rectangle
 *  @param [in]  Y1                 The top left position of the updated rectangle
 *  @param [in]  X2                 The bottom right position of the updated rectangle
 *  @param [in]  Y2                 The bottom right position of the updated rectangle
 *  @param [in]  Thickness          The width of rectangle
 *  @param [in]  Color              The Color of rectangle
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRect_Render(const AMBA_DRAW_RENDER_s *Render, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2, UINT32 Thickness, const AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval = DRAW_OK;

    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRect_Render: (%d %d) -> (%d %d)", X1, Y1, X2, Y2, 0);

    if ((X1 > X2) || (Y1 > Y2)) {
        AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRect_Render: [ERROR] Invalid (%d %d) (%d %d)", X1, Y1, X2, Y2, 0);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        /*
            Thickness = TL + TR + 1
            Example:
                Thickness = 4, TL = 1, TR = 2
                Thickness = 5, TL = 2, TR = 2
         */
        INT32 TL = 0;
        INT32 TR = 0;
        INT32 StartX = X1;
        INT32 StartY = Y1;
        INT32 EndX = X2;
        INT32 EndY = Y2;
        INT32 Length;

        if (Thickness > 0U) {
            TL = ((INT32)Thickness - 1) / 2;
            TR = (INT32)Thickness / 2;
            StartX -= TL;
            StartY -= TL;
            EndX += TR;
            EndY += TR;
        }
        Length = EndX - StartX + 1;
        if (StartY == EndY) { /* Check horizontal line */
            Render->PlotHLineAt_f(Render, StartX, StartY, (UINT32)Length, Color);
        } else if (StartX == EndX) { /* Check vertical line */
            AmbaDrawShape_PlotVLine(Render, StartX, StartY, EndY, Color);
        } else if (Thickness == 0U) {
            /* Filled rectangle */
            for (INT32 Y = StartY; Y <= EndY; Y++) {
                Render->PlotHLineAt_f(Render, StartX, Y, (UINT32)Length, Color);
            }
        } else {
            for (INT32 Y = StartY; Y < (StartY + (INT32)Thickness); Y++) {
                Render->PlotHLineAt_f(Render, StartX, Y, (UINT32)Length, Color);
            }

            for (INT32 Y = (EndY - (INT32)Thickness + 1); Y <= EndY; Y++) {
                Render->PlotHLineAt_f(Render, StartX, Y, (UINT32)Length, Color);
            }

            for (INT32 Y = (StartY + (INT32)Thickness); Y <= (EndY - (INT32)Thickness); Y++) {
                Render->PlotHLineAt_f(Render, StartX, Y, Thickness, Color);
                Render->PlotHLineAt_f(Render, EndX - (INT32)Thickness + 1, Y, Thickness, Color);
            }
        }
    }

    return Rval;
}

/**
 *  Calculate the object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            The returned object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRect_CalcArea(const AMBA_DRAW_RECT_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_OK;
    const AMBA_DRAW_RECT_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
    UINT32 HalfThicknessL = ObjCfg->LineWidth / 2U;
    if ((ObjCfg->LineWidth > 0U) && ((ObjCfg->LineWidth % 2U) != 1U)) { /* Ex: 4/2 = 2, TL = 1, TR = 2 */
        HalfThicknessL -= 1U;
    }

    /* Set object area */
    ObjArea->X = ObjDesc->OffsetX - (INT32)HalfThicknessL;
    ObjArea->Y = ObjDesc->OffsetY - (INT32)HalfThicknessL;
    ObjArea->Width = ObjCfg->LineWidth + ObjCfg->Width;
    ObjArea->Height = ObjCfg->LineWidth + ObjCfg->Height;

    /* Add shadow area */
    if (ObjCfg->Shadow.Enable == 1U) {
        if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_LEFT) != 0U) {
            ObjArea->X -= (INT32)ObjCfg->Shadow.Distance;
            ObjArea->Width += ObjCfg->Shadow.Distance;
        } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_RIGHT) != 0U) {
            ObjArea->Width += ObjCfg->Shadow.Distance;
        } else {
            /* Do nothing */
        }

        if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_TOP) != 0U) {
            ObjArea->Y -= (INT32)ObjCfg->Shadow.Distance;
            ObjArea->Height += ObjCfg->Shadow.Distance;
        } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_BOTTOM) != 0U) {
            ObjArea->Height += ObjCfg->Shadow.Distance;
        } else {
            /* Do nothing */
        }
    }

    return Rval;
}

/**
 *  Check parameter
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawRect_GetObjArea_ParamCheck(const AMBA_DRAW_RECT_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjDesc != NULL) {
        if (ObjArea != NULL) {
            Rval = AmbaDrawRect_IsValidConfig(&ObjDesc->ObjCfg);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRect_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjArea is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRect_GetObjArea(const AMBA_DRAW_RECT_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    Rval = AmbaDrawRect_GetObjArea_ParamCheck(ObjDesc, ObjArea);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawRect_CalcArea(ObjDesc, ObjArea);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRect_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Draw the Rectangle object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawRect_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_RECT_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;

    /* Check parameters. Area can be NULL */
    if (Render == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Render is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (ObjDesc == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else {
        const AMBA_DRAW_RECT_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
        AMBA_DRAW_AREA_s ObjArea;
        Rval = DRAW_WRAP2D(AmbaWrap_memset(&ObjArea, 0x0, sizeof(ObjArea)));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == DRAW_OK) {
            Rval = AmbaDrawRect_CalcArea(ObjDesc, &ObjArea);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRect_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Check area */
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_CheckObjArea(Render, &ObjArea, Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_CheckObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Draw shadow */
        if (Rval == DRAW_OK) {
            if ((ObjCfg->Shadow.Enable == 1U) && (ObjCfg->Shadow.ShadowPostion != AMBA_DRAW_POSITION_MM)) {
                INT32 ShadowX1 = ObjDesc->OffsetX;
                INT32 ShadowY1 = ObjDesc->OffsetY;
                INT32 ShadowX2;
                INT32 ShadowY2;

                /* Set Postion */
                if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_LEFT) != 0U) {
                    ShadowX1 -= (INT32)ObjCfg->Shadow.Distance;
                } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_RIGHT) != 0U) {
                    ShadowX1 += (INT32)ObjCfg->Shadow.Distance;
                } else {
                    /* Do nothing */
                }

                if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_TOP) != 0U) {
                    ShadowY1 -= (INT32)ObjCfg->Shadow.Distance;
                } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_BOTTOM) != 0U) {
                    ShadowY1 += (INT32)ObjCfg->Shadow.Distance;
                } else {
                    /* Do nothing */
                }

                ShadowX2 = ShadowX1 + (INT32)ObjCfg->Width - 1;
                ShadowY2 = ShadowY1 + (INT32)ObjCfg->Height - 1;
                Rval = AmbaDrawRect_Render(Render, ShadowX1, ShadowY1, ShadowX2, ShadowY2, ObjCfg->LineWidth, &ObjCfg->Shadow.Color);

                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRect_Draw: [Shadow] X1 %d Y1 %d X2 %d Y2 %d Color %d", ShadowX1, ShadowY1, ShadowX2, ShadowY2, (INT32)ObjCfg->Shadow.Color);
            }
        }

        /* Draw object */
        if (Rval == DRAW_OK) {
            const AMBA_DRAW_COLOR_s *Color = (ObjCfg->LineWidth == 0U) ? (&ObjCfg->FillColor) : (&ObjCfg->LineColor);
            INT32 X1 = ObjDesc->OffsetX;
            INT32 Y1 = ObjDesc->OffsetY;
            INT32 X2 = X1 + (INT32)ObjCfg->Width - 1;
            INT32 Y2 = Y1 + (INT32)ObjCfg->Height - 1;
            Rval = AmbaDrawRect_Render(Render, X1, Y1, X2, Y2, ObjCfg->LineWidth, Color);
            //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawRect_Draw: [Rect] X1 %d Y1 %d X2 %d Y2 %d Color %d", X1, Y1, X2, Y2, (INT32)Color);
        }
    }

    return Rval;
}

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCirc_IsValidConfig(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjCfg != NULL) {
        if (ObjCfg->Radius > 0U) {
            if (ObjCfg->LineWidth <= AMBA_DRAW_LINE_WIDTH_MAX) {
                Rval = DRAW_OK;
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawLine_IsValidConfig: [ERROR] Invalid LineWidth %u", ObjCfg->LineWidth, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_IsValidConfig: [ERROR] Invalid Radius %u", ObjCfg->Radius, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

static UINT32 AmbaDrawCirc_Render(const AMBA_DRAW_RENDER_s *Render, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2, UINT32 Thickness, const AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval = DRAW_OK;
    INT32 Diameter = 0;
    INT32 LineThickness = (INT32)Thickness;

    /*
        Use Bresenham Circle Drawing Algorithm
        It's more efficient bacause:
           Use only integer arithmetic. No floating point is involved.
     */

    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: (%d %d) (%d %d)", X1, Y1, X2, Y2, 0U);

    if (AmbaDraw_Abs(X1-X2) <= AmbaDraw_Abs(Y1-Y2)) {
        Diameter = AmbaDraw_Abs(X1-X2) + 1;
    } else {
        Diameter = AmbaDraw_Abs(Y1-Y2) + 1;
    }

    /* Adjust Diameter when Thickness > 0 */
    if (LineThickness > 0) {
        /*
            Thickness = TL + TR + 1
            Example:
                Thickness = 4, TL = 1, TR = 2
                Thickness = 5, TL = 2, TR = 2
            When Thickness > 1:
                The outer diameter = Diameter + (2*TL)
                The inner diameter = Diameter - (2*TR)
         */
        INT32 TL = ((INT32)Thickness - 1) / 2;
        INT32 TR = (INT32)Thickness / 2;

        /* It becomes a filled circle when inner diameter <= 1 */
        if (Diameter <= ((2 * TR) + 1)) {
            LineThickness = 0; /* Filled circle */
        }

        Diameter += (TL * 2);
    }

    if (((UINT32)Diameter % 2U) == 1U) {
        /*
            When Diameter is odd, apply the following condition to Bresenham's algorithm:
                Center = ((X1+X2)/2, (Y1+Y2)/2) ==> Both X and Y are integers
                Radius = (Diameter - 1)/2 ==> Integer
         */
        INT32 X;        /* X value assuming center in (0, 0) */
        INT32 Y;        /* Y value assuming center in (0, 0) */
        INT32 OffsetX = (X1 + X2) / 2; /* Offset of X value */
        INT32 OffsetY = (Y1 + Y2) / 2; /* Offset of Y value */
        INT32 D;        /* Decision variable */
        INT32 Radius = (Diameter - 1) / 2;

        //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Offset (%d %d) Diameter %d R %d", OffsetX, OffsetY, Diameter, Radius, 0);

        D = 5 - (4 * Radius);
        X = 0;
        Y = Radius;
        /* Evaluate the eighth part of the circle. And calculate the rest by symmetry of circle. */
        while (X <= Y) {
            if (LineThickness == 0) {
                /* Filled circle */
                INT32 PlotX1;
                INT32 PlotX2;
                INT32 PlotY;
                INT32 NextY = Y;
                UINT8 IsPlot = 0U;

                /*
                    To avoid drawing redundant lines, only draw in the following conditions:
                    1. Y will change (D >= 0) in the next iteration, or
                    2. This is the last iteration ((X + 1) <= NextY)
                    Consider symetric points. Apply the same rule to (OffsetY - Y).
                    However, draw (OffsetY + X) and (OffsetY - X) because X changes in every iterations.
                 */
                if (D >= 0) {
                    --NextY;
                    IsPlot = 1U;
                }

                if ((X + 1) > NextY) {
                    IsPlot = 1U;
                }

                /*
                    Plot 4 lines:
                        Line#1: (OffsetX - X, OffsetY + Y) -> (OffsetX + X, OffsetY + Y)
                        Line#2: (OffsetX - X, OffsetY - Y) -> (OffsetX + X, OffsetY - Y)
                        Line#3: (OffsetX - Y, OffsetY + X) -> (OffsetX + Y, OffsetY + X)
                        Line#4: (OffsetX - Y, OffsetY - X) -> (OffsetX + Y, OffsetY - X)
                    To avoid drawing redundant lines:
                        1. When X == Y, we'll get Line#1 == Line#3 and Line#2 == Line#4.
                           So skip Line#1 and Line#2 by setting IsPlot = 0U.
                        2. When Y == 0, we'll get Line#1 == Line#2.
                           Do not draw Line#2 in this case.
                           However, the loop stops when "X > Y" so Y won't be 0. Skip this check.
                        3. When X == 0, we'll get Line#3 == Line#4.
                           Do not draw Line#4 in this case.
                 */
                if (X == Y) {
                    IsPlot = 0U;
                }
                if (IsPlot == 1U) {
                    PlotX1 = OffsetX - X;
                    PlotX2 = OffsetX + X;
                    PlotY = OffsetY + Y;
                    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                    AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
                    PlotX1 = OffsetX - X;
                    PlotX2 = OffsetX + X;
                    PlotY = OffsetY - Y;
                    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                    AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
                }
                PlotX1 = OffsetX - Y;
                PlotX2 = OffsetX + Y;
                PlotY = OffsetY + X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
                if (X != 0) {
                    PlotX1 = OffsetX - Y;
                    PlotX2 = OffsetX + Y;
                    PlotY = OffsetY - X;
                    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                    AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
                }
            } else {
                INT32 PlotX;
                INT32 PlotY;
                /*
                    Plot 8 symmetric points:
                        (OffsetX + X, OffsetY + Y)
                        (OffsetX + X, OffsetY - Y)
                        (OffsetX - X, OffsetY + Y)
                        (OffsetX - X, OffsetY - Y)
                        (OffsetX + Y, OffsetY + X)
                        (OffsetX + Y, OffsetY - X)
                        (OffsetX - Y, OffsetY + X)
                        (OffsetX - Y, OffsetY - X)
                 */
                PlotX = OffsetX + X;
                PlotY = OffsetY + Y;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY - LineThickness + 1, PlotY, Color);
                PlotX = OffsetX + X;
                PlotY = OffsetY - Y;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY, PlotY + LineThickness - 1, Color);
                PlotX = OffsetX - X;
                PlotY = OffsetY + Y;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY - LineThickness + 1, PlotY, Color);
                PlotX = OffsetX - X;
                PlotY = OffsetY - Y;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY, PlotY + LineThickness - 1, Color);
                PlotX = OffsetX + Y;
                PlotY = OffsetY + X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX - LineThickness + 1, PlotX, PlotY, Color);
                PlotX = OffsetX + Y;
                PlotY = OffsetY - X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX - LineThickness + 1, PlotX, PlotY, Color);
                PlotX = OffsetX - Y;
                PlotY = OffsetY + X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX, PlotX + LineThickness - 1, PlotY, Color);
                PlotX = OffsetX - Y;
                PlotY = OffsetY - X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX, PlotX + LineThickness - 1, PlotY, Color);
            }

            if (D < 0) {
                D += ((8 * X) + 12);
                ++X;
            } else {
                D += ((8 * (X - Y)) + 20);
                ++X;
                --Y;
            }
        }
    } else {
        /*
            When Diameter is even, apply the following condition to Bresenham's algorithm:
                Center = ((X1+X2)/2, (Y1+Y2)/2) ==> Both X and Y are not integers
                Radius = Diameter/2 ==> Integer
            Center is not integer. However, X1, X2, Y1, and Y2 must be integers.
            So this case can still be handled by integer arithmetic.
         */
        INT32 X;        /* X value assuming center in (0.5, 0.5) */
        INT32 Y;        /* Y value assuming center in (0.5, 0.5) */
        INT32 OffsetX = (X1 + X2) / 2; /* Offset of X value */
        INT32 OffsetY = (Y1 + Y2) / 2; /* Offset of Y value */
        INT32 D;        /* Decision variable */
        INT32 Radius = Diameter / 2;

        //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Offset (%d %d) Diameter %d R %d", OffsetX, OffsetY, Diameter, Radius, 0);

        D = 0;
        X = 1;
        /*
            Precisely, Y = (1 + sqrt((2R)^2 - 1)) / 2.
            So Y is slightly less than (1 + sqrt((2R)^2)) / 2 = R + 1/2.
            Since R is integer, Y must be R after rounding.
         */
        Y = Radius;
        /* Evaluate the eighth part of the circle. And calculate the rest by symmetry of circle. */
        while (X <= Y) {
            if (LineThickness == 0) {
                /* Filled circle */
                INT32 PlotX1;
                INT32 PlotX2;
                INT32 PlotY;
                INT32 NextY = Y;
                UINT8 IsPlot = 0U;

                /*
                    To avoid drawing redundant lines, only draw in the following conditions:
                    1. Y will change (D >= 0) in the next iteration, or
                    2. This is the last iteration ((X + 1) <= NextY)
                    Consider symetric points. Apply the same rule to (OffsetY - Y + 1).
                    However, draw (OffsetY + X) and (OffsetY - X + 1) because X changes in every iterations.
                 */
                if (D >= 0) {
                    --NextY;
                    IsPlot = 1U;
                }

                if ((X + 1) > NextY) {
                    IsPlot = 1U;
                }

                /*
                    Plot 4 lines:
                        Line#1: (OffsetX - X + 1, OffsetY + Y)     -> (OffsetX + X, OffsetY + Y)
                        Line#2: (OffsetX - X + 1, OffsetY - Y + 1) -> (OffsetX + X, OffsetY - Y + 1)
                        Line#3: (OffsetX - Y + 1, OffsetY + X)     -> (OffsetX + Y, OffsetY + X)
                        Line#4: (OffsetX - Y + 1, OffsetY - X + 1) -> (OffsetX + Y, OffsetY - X + 1)
                    To avoid drawing redundant lines:
                        When X == Y, we'll get Line#1 == Line#3 and Line#2 == Line#4.
                        So skip Line#1 and Line#2 by setting IsPlot = 0U.
                 */
                if (X == Y) {
                    IsPlot = 0U;
                }
                if (IsPlot == 1U) {
                    PlotX1 = OffsetX - X + 1;
                    PlotX2 = OffsetX + X;
                    PlotY = OffsetY + Y;
                    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                    AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
                    PlotX1 = OffsetX - X + 1;
                    PlotX2 = OffsetX + X;
                    PlotY = OffsetY - Y + 1;
                    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                    AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
                }
                PlotX1 = OffsetX - Y + 1;
                PlotX2 = OffsetX + Y;
                PlotY = OffsetY + X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
                PlotX1 = OffsetX - Y + 1;
                PlotX2 = OffsetX + Y;
                PlotY = OffsetY - X + 1;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d -> %d, %d) Color %d D %d", PlotX1, PlotX2, PlotY, (INT32)Color, D);
                AmbaDrawShape_PlotHLine(Render, PlotX1, PlotX2, PlotY, Color);
            } else {
                INT32 PlotX;
                INT32 PlotY;
                /*
                    Plot 8 symmetric points:
                        (OffsetX + X,     OffsetY + Y)
                        (OffsetX + X,     OffsetY - Y + 1)
                        (OffsetX - X + 1, OffsetY + Y)
                        (OffsetX - X + 1, OffsetY - Y + 1)
                        (OffsetX + Y,     OffsetY + X)
                        (OffsetX + Y,     OffsetY - X + 1)
                        (OffsetX - Y + 1, OffsetY + X)
                        (OffsetX - Y + 1, OffsetY - X + 1)
                 */
                PlotX = OffsetX + X;
                PlotY = OffsetY + Y;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY - LineThickness + 1, PlotY, Color);
                PlotX = OffsetX + X;
                PlotY = OffsetY - Y + 1;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY, PlotY + LineThickness - 1, Color);
                PlotX = OffsetX - X + 1;
                PlotY = OffsetY + Y;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY - LineThickness + 1, PlotY, Color);
                PlotX = OffsetX - X + 1;
                PlotY = OffsetY - Y + 1;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotVLine(Render, PlotX, PlotY, PlotY + LineThickness - 1, Color);
                PlotX = OffsetX + Y;
                PlotY = OffsetY + X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX - LineThickness + 1, PlotX, PlotY, Color);
                PlotX = OffsetX + Y;
                PlotY = OffsetY - X + 1;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX - LineThickness + 1, PlotX, PlotY, Color);
                PlotX = OffsetX - Y + 1;
                PlotY = OffsetY + X;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX, PlotX + LineThickness - 1, PlotY, Color);
                PlotX = OffsetX - Y + 1;
                PlotY = OffsetY - X + 1;
                //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Render: Plot(%d %d) Color %d D %d", PlotX, PlotY, (INT32)Color, D, 0);
                AmbaDrawShape_PlotHLine(Render, PlotX, PlotX + LineThickness - 1, PlotY, Color);
            }

            if (D < 0) {
                D += ((2 * X) + 2);
                ++X;
            } else {
                D += ((2 * (X - Y)) + 5);
                ++X;
                --Y;
            }
        }
    }

    return Rval;
}

/**
 *  Calculate the object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            The returned object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCirc_CalcArea(const AMBA_DRAW_CIRCLE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_OK;
    const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg = &ObjDesc->ObjCfg;

    if (ObjCfg->LineWidth == 0U) {
        ObjArea->X = ObjDesc->OffsetX;
        ObjArea->Y = ObjDesc->OffsetY;
        ObjArea->Width  = (ObjCfg->Radius * 2U) + 1U;
        ObjArea->Height = ObjArea->Width;
    } else {
        /*
            Thickness = TL + TR + 1
            Example:
                Thickness = 4, TL = 1, TR = 2
                Thickness = 5, TL = 2, TR = 2
         */
        UINT32 TL;
        TL = (ObjCfg->LineWidth - 1U) / 2U;
        ObjArea->X = ObjDesc->OffsetX - (INT32)TL;
        ObjArea->Y = ObjDesc->OffsetY - (INT32)TL;
        /*
            CenterX = OffsetX + Radius
            CenterY = OffsetY + Radius
            The outer radius = Radius + TL
            The inner radius = Radius - TR
            StartX = CenterX - Radius - TL
            EndX   = CenterX + Radius + TL
            Width  = EndX - StartX + 1 = 2*Radius + 2*TL + 1
            (Not "2*Radius + (Thickness-1) + 1". It's different in integer arithmetic.)
         */
        ObjArea->Width  = ((ObjCfg->Radius + TL) * 2U) + 1U;
        ObjArea->Height = ObjArea->Width;
    }
    //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_CalcArea: ObjArea (%d %d %d %d)", ObjArea->X, ObjArea->Y, (INT32)ObjArea->Width, (INT32)ObjArea->Height, 0);

    return Rval;
}

/**
 *  Check parameter
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCirc_GetObjArea_ParamCheck(const AMBA_DRAW_CIRCLE_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjDesc != NULL) {
        if (ObjArea != NULL) {
            Rval = AmbaDrawCirc_IsValidConfig(&ObjDesc->ObjCfg);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCirc_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjArea is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCirc_GetObjArea(const AMBA_DRAW_CIRCLE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    Rval = AmbaDrawCirc_GetObjArea_ParamCheck(ObjDesc, ObjArea);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawCirc_CalcArea(ObjDesc, ObjArea);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCirc_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Draw the Circle object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCirc_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_CIRCLE_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;

    /* Check parameters. Area can be NULL */
    if (Render == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Render is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (ObjDesc == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else {
        AMBA_DRAW_AREA_s ObjArea;
        Rval = DRAW_WRAP2D(AmbaWrap_memset(&ObjArea, 0x0, sizeof(ObjArea)));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == DRAW_OK) {
            Rval = AmbaDrawCirc_CalcArea(ObjDesc, &ObjArea);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCirc_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Check area */
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_CheckObjArea(Render, &ObjArea, Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_CheckObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if (Rval == DRAW_OK) {
            const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
            const AMBA_DRAW_COLOR_s *Color = (ObjCfg->LineWidth == 0U) ? (&ObjCfg->FillColor) : (&ObjCfg->LineColor);
            INT32 Diameter  = (INT32)ObjCfg->Radius * 2;
            INT32 X1 = ObjDesc->OffsetX;
            INT32 Y1 = ObjDesc->OffsetY;
            INT32 X2 = X1 + Diameter;
            INT32 Y2 = Y1 + Diameter;
            //AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_Draw: Offset (%d, %d) Diameter %d LineWidth %d Color %d", X1, Y1, Diameter, (INT32)ObjCfg->LineWidth, (INT32)Color);

            Rval = AmbaDrawCirc_Render(Render, X1, Y1, X2, Y2, ObjCfg->LineWidth, Color);
        }
    }

    return Rval;
}

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawPoly_IsValidConfig(const AMBA_DRAW_POLY_CFG_s *ObjCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjCfg != NULL) {
        if ((ObjCfg->Corner > 1U) && (ObjCfg->Corner <= AMBA_DRAW_MAX_POLY_CORNER_NUM)) {
            // TODO : Check redundant corner
            Rval = AmbaDraw_IsValidShadow(&(ObjCfg->Shadow));
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_IsValidShadow failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_IsValidConfig: [ERROR] Invalid Corner %u", ObjCfg->Corner, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  When a row of polygon intersects with a corner, update the line segments in this row.
 *
 *  @param [in]  PrevY              The previous corner Y
 *  @param [in]  CurX               The current corner X
 *  @param [in]  CurY               The current corner Y
 *  @param [in]  NextX              The next corner X
 *  @param [in]  NextY              The next corner Y
 *  @param [in]  Next2Y             The second next corner Y
 *  @param [in,out] NodeX           An array of the X coordinates of lines to draw in the row.
 *  @param [in,out] RetNodeCount    The size of NodeX[].
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawPoly_Render_Corner(INT32 PrevY, INT32 CurX, INT32 CurY, INT32 NextX, INT32 NextY, INT32 Next2Y, INT32 *NodeX, UINT32 *RetNodeCount)
{
    UINT32 Rval = DRAW_OK;
    UINT32 NodeCount = *RetNodeCount;
    if (NextY == CurY) {
        if (PrevY == CurY) {
            // (PrevY == CurY) && (NextY == CurY)
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Cannot support consecutive horizontal lines", __func__, NULL, NULL, NULL, NULL);
            Rval = DRAW_ERROR_ARG;
        } else {
            // (PrevY != CurY) && (NextY == CurY)
            if (PrevY < CurY) {
                if (Next2Y < CurY) {
                    // (PrevY < CurY) && (Next2Y < CurY)
                    NodeX[NodeCount] = CurX;
                    NodeCount++;
                    NodeX[NodeCount] = NextX;
                    NodeCount++;
                } else if (Next2Y > CurY) {
                    // (PrevY < CurY) && (Next2Y > CurY)
                    NodeX[NodeCount] = CurX;
                    NodeCount++;
                } else {
                    /* Do nothing */
                }
            } else if (PrevY > CurY) {
                if (Next2Y < CurY) {
                    // (PrevY > CurY) && (Next2Y < CurY)
                    NodeX[NodeCount] = CurX;
                    NodeCount++;
                } else if (Next2Y > CurY) {
                    // (PrevY > CurY) && (Next2Y > CurY)
                    NodeX[NodeCount] = CurX;
                    NodeCount++;
                    NodeX[NodeCount] = NextX;
                    NodeCount++;
                } else {
                    /* Do nothing */
                }
            } else {
                /* Do nothing */
            }

        }
    } else if (NextY < CurY) {
        if (PrevY == CurY) {
            // (PrevY == CurY) && (NextY < CurY)
            // Do nothing
        } else if (PrevY < CurY) {
            // (PrevY < CurY) && (NextY < CurY)
            /* Duplicate X for drawing a point at this location */
            NodeX[NodeCount] = CurX;
            NodeCount++;
            NodeX[NodeCount] = CurX;
            NodeCount++;
        } else {
            // (PrevY > CurY) && (NextY < CurY)
            NodeX[NodeCount] = CurX;
            NodeCount++;
        }
    } else {
        if (PrevY == CurY) {
            // (PrevY == CurY) && (NextY > CurY)
            // Do nothing
        } else if (PrevY < CurY) {
            // (PrevY < CurY) && (NextY > CurY)
            NodeX[NodeCount] = CurX;
            NodeCount++;
        } else {
            // (PrevY > CurY) && (NextY > CurY)
            /* Duplicate X for drawing a point at this location */
            NodeX[NodeCount] = CurX;
            NodeCount++;
            NodeX[NodeCount] = CurX;
            NodeCount++;
        }
    }
    *RetNodeCount = NodeCount;
    return Rval;
}

/**
 *  Scan a row of a polygon and return the line segments in this row.
 *
 *  @param [in]  PixelY             The Y coordinate of the row
 *  @param [in]  Corner             The number of corners of the polygon. The size of X[] and Y[].
 *  @param [in]  X                  An array of the X coordinates of the corners.
 *  @param [in]  Y                  An array of the Y coordinates of the corners.
 *  @param [out] NodeX              An array of the X coordinates of lines to draw in the row.
 *  @param [out] RetNodeCount       The size of NodeX[].
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawPoly_Render_ScanRow(INT32 PixelY, UINT32 Corner, const INT32 *X, const INT32 *Y, INT32 *NodeX, UINT32 *RetNodeCount)
{
    UINT32 Rval = DRAW_OK;
    UINT32 PrevI = Corner - 1U;
    UINT32 NodeCount = 0U;
    /* Build a list of nodes. */

    /* Get the intersections of the horizontal line (Y = PixelY) and the polygon. Store the X coordinates in NodeX[]. */
    /* If there's a corner on Y = PixelY, there will be duplicate X coordinates of this corner in NodeX[]. */
    /* The number of data in NodeX[] (i.e. NodeCount) should be even. */
    for (UINT32 i = 0U; i < Corner; i++) {
        INT32 PrevX = X[PrevI];
        INT32 PrevY = Y[PrevI];
        INT32 CurX = X[i];
        INT32 CurY = Y[i];
        UINT32 NextID = ((i + 1U) < Corner) ? (i + 1U) : 0U;
        INT32 NextX = X[NextID];
        INT32 NextY = Y[NextID];
        /*
            Find intersections and store them in NodeX
            Assume there are 3 corners: (X0, Y0), (X1, Y1), (X2, Y2)
            Consider some corner cases:
            A. Y0 and Y2 < PixelY, Y1 == PixelY
                NodeX = {X1, X1}
            B. Y0 and Y2 > PixelY, Y1 == PixelY
                NodeX = {X1, X1}
            C. Y0 and Y2 == PixelY, Y1 > PixelY
                NodeX = {X0, X2}
            D. Y0 and Y2 == PixelY, Y1 < PixelY
                NodeX = {X0, X2}
            Assume there are 6 corners to form a hexagon: (X0, Y0), (X1, Y1), (X2, Y2), (X3, Y3), (X4, Y4), (X5, Y5)
            Consider the following case:
            E.  Y0 == Y1 < Y2 == Y5 == PixelY < Y3 == Y4
                NodeX = {X2, X5}
            To satisfy all the above cases, the method of choosing NodeX is:
            1. Intersection with edges
                1-1. When Y[I] == Y[I-1]
                    Don't get the intersection
                2-2. When Y[I] != Y[I-1]
                    Get the intersection of PixelY and "Y[I-1] ~ Y[I]" (excluding Y[I-1] and Y[I])
            2. Intersection with corners
                2-1. When Y[I-1] == Y[I], Y[I+1] == Y[I]
                    2-1-1. When X[I-1] < X[I], X[I+1] < X[I]
                        Add X[I] into NodeX
                    2-1-2. When X[I-1] > X[I], X[I+1] > X[I]
                        Add X[I] into NodeX
                    2-1-3. Otherwise
                        [ERROR] Cannot support consecutive horizontal lines
                2-2. When Y[I-1] < Y[I], Y[I+1] < Y[I]
                    Add duplicate X[I] into NodeX
                    ==> For Case A
                2-3. When Y[I-1] > Y[I], Y[I+1] > Y[I]
                    Add duplicate X[I] into NodeX
                    ==> For Case B
                2-4. When Y[I-1] < Y[I], Y[I+1] > Y[I]
                    Add X[I] into NodeX
                2-5. When Y[I-1] > Y[I], Y[I+1] < Y[I]
                    Add X[I] into NodeX
                2-6. When Y[I+1] == Y[I]
                    2-6-1. When Y[I-1] < Y[I], Y[I+2] < Y[I+1]
                        Add X[I] and X[I+1] into NodeX
                    2-6-2. When Y[I-1] > Y[I], Y[I+2] > Y[I+1]
                        Add X[I] and X[I+1] into NodeX
                    2-6-3. When Y[I-1] < Y[I], Y[I+2] > Y[I+1]
                        Add X[I] into NodeX. The horizontal edge might be ignored. Need to draw it afterwards.
                    2-6-4. When Y[I-1] > Y[I], Y[I+2] < Y[I+1]
                        Add X[I] into NodeX. The horizontal edge might be ignored. Need to draw it afterwards.
                    2-6-5. Otherwise
                        [ERROR] Cannot support consecutive horizontal lines
                2-7. When Y[I-1] == Y[I]
                    Do nothing. Already handled in 2-6
            3. Horizontal edge
                Horizontal edges could be ignored after the above process, so draw horizontal edges at the end of process.
         */

        if (((CurY < PixelY) && (PixelY < PrevY)) || ((PrevY < PixelY) && (PixelY < CurY))) {
            /* Intersection with edge */
            DOUBLE Temp = (((DOUBLE)PixelY - (DOUBLE)CurY) * ((DOUBLE)PrevX - (DOUBLE)CurX)) / ((DOUBLE)PrevY - (DOUBLE)CurY);
            Temp = Temp + (DOUBLE)CurX;
            NodeX[NodeCount] = AmbaDraw_Round(Temp);
            NodeCount++;
            // TODO: Return error when "NodeCount" > AMBA_DRAW_MAX_POLY_CORNER_NUM
        } else if (CurY == PixelY) {
            /* Intersection with corner */
            UINT32 Next2ID = ((NextID + 1U) < Corner) ? (NextID + 1U) : 0U;
            INT32 Next2Y = Y[Next2ID];
            Rval = AmbaDrawPoly_Render_Corner(PrevY, CurX, CurY, NextX, NextY, Next2Y, NodeX, &NodeCount);
        } else {
            /* No intersection. Do nothing. */
        }

        PrevI = i;

        /* Break the loop if there's error */
        if (Rval != DRAW_OK) {
            break;
        }
    }

    if (Rval == DRAW_OK) {
        if ((NodeCount & 1U) != 0U) {
            AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawPoly_Render: [ERROR] NodeCount %d is not even at Y = %d", (INT32)NodeCount, PixelY, 0, 0, 0);
            Rval = DRAW_ERROR_GENERAL_ERROR;
        } else {
            *RetNodeCount = NodeCount;
        }
    }
    return Rval;
}

static UINT32 AmbaDrawPoly_Render(const AMBA_DRAW_RENDER_s *Render, UINT32 Corner, const INT32 *X, const INT32 *Y, const AMBA_DRAW_AREA_s *ObjArea, const AMBA_DRAW_COLOR_s *Color)
{
    UINT32 Rval = DRAW_OK;
    INT32 Yt = ObjArea->Y;
    INT32 Yb = ObjArea->Y + (INT32)ObjArea->Height;

    /* Fix the VCast error: Variable 'NodeX' may not have been initialized [MISRA 2012 Rule 9.1, mandatory] */
    //(void)AmbaWrap_memset(NodeX, 0, sizeof(NodeX));

    /* Loop through the rows of the image. */
    for (INT32 PixelY = Yt; PixelY <= Yb; PixelY++) {
        INT32 NodeX[AMBA_DRAW_MAX_POLY_CORNER_NUM] = {0};
        UINT32 NodeCount = 0U; /* Number of nodes in NodeX[] */
        Rval = AmbaDrawPoly_Render_ScanRow(PixelY, Corner, X, Y, NodeX, &NodeCount);
        if (Rval == DRAW_OK) {
            /* Sort the nodes, via a simple Bubble sort. */
            UINT32 i = 0U;
            while ((i + 1U) < NodeCount) {
                if (NodeX[i] > NodeX[i+1U]) {
                    INT32 Swap = NodeX[i];
                    NodeX[i] = NodeX[i+1U];
                    NodeX[i+1U] = Swap;
                    if (i != 0U) {
                        i--;
                    }
                } else {
                    i++;
                }
            }

            /* Fill the pixels between node pairs. */
            for (i = 0U; i < NodeCount; i += 2U) {
                INT32 Length = NodeX[i+1U] - NodeX[i] + 1;
                /* Render will check the draw area, so no need to check the boundary (Xl Xr Yt Yb) */
                Render->PlotHLineAt_f(Render, NodeX[i], PixelY, (UINT32)Length, Color);
            }
        }

        /* Break the loop if there's error */
        if (Rval != DRAW_OK) {
            break;
        }
    }

    if (Rval == DRAW_OK) {
        /* Draw horizontal edges */
        for (UINT32 i = 0U; i < Corner; i++) {
            INT32 CurY = Y[i];
            UINT32 NextID = ((i + 1U) < Corner) ? (i + 1U) : 0U;
            INT32 NextY = Y[NextID];
            if (CurY == NextY) {
                INT32 Length = X[NextID] - X[i] + 1;
                Render->PlotHLineAt_f(Render, X[i], CurY, (UINT32)Length, Color);
            }
        }
    }

    return Rval;
}

/**
 *  Calculate the object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            The returned object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawPoly_CalcArea(const AMBA_DRAW_POLY_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_OK;
    const AMBA_DRAW_POLY_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
    INT32 XMin;
    INT32 XMax;
    INT32 YMin;
    INT32 YMax;
    INT32 Width;
    INT32 Height;
    UINT32 i;
    //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawPoly_CalcArea: Corner %u Color %u", ObjCfg->Corner, ObjCfg->FillColor, 0U, 0U, 0U);

    XMax = ObjCfg->X[0];
    XMin = XMax;
    YMax = ObjCfg->Y[0];
    YMin = YMax;
    for (i = 1U; i < ObjCfg->Corner; ++i) {
        XMin = AmbaDraw_Min(XMin, ObjCfg->X[i]);
        XMax = AmbaDraw_Max(XMax, ObjCfg->X[i]);
        YMin = AmbaDraw_Min(YMin, ObjCfg->Y[i]);
        YMax = AmbaDraw_Max(YMax, ObjCfg->Y[i]);
    }
    Width  = XMax - XMin;
    Height = YMax - YMin;

    ObjArea->X = XMin + ObjDesc->OffsetX;
    ObjArea->Y = YMin + ObjDesc->OffsetY;
    ObjArea->Width  = (UINT32)Width;
    ObjArea->Height = (UINT32)Height;

    /* Add shadow area */
    if (ObjCfg->Shadow.Enable == 1U) {
        if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_LEFT) != 0U) {
            ObjArea->X -= (INT32)ObjCfg->Shadow.Distance;
            ObjArea->Width += ObjCfg->Shadow.Distance;
        } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_RIGHT) != 0U) {
            ObjArea->Width += ObjCfg->Shadow.Distance;
        } else {
            /* Do nothing */
        }

        if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_TOP) != 0U) {
            ObjArea->Y -= (INT32)ObjCfg->Shadow.Distance;
            ObjArea->Height += ObjCfg->Shadow.Distance;
        } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_BOTTOM) != 0U) {
            ObjArea->Height += ObjCfg->Shadow.Distance;
        } else {
            /* Do nothing */
        }
    }

    return Rval;
}

/**
 *  Check parameter
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawPoly_GetObjArea_ParamCheck(const AMBA_DRAW_POLY_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjDesc != NULL) {
        if (ObjArea != NULL) {
            Rval = AmbaDrawPoly_IsValidConfig(&ObjDesc->ObjCfg);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawPoly_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjArea is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawPoly_GetObjArea(const AMBA_DRAW_POLY_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    Rval = AmbaDrawPoly_GetObjArea_ParamCheck(ObjDesc, ObjArea);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawPoly_CalcArea(ObjDesc, ObjArea);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawPoly_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Draw the Polygon object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawPoly_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_POLY_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;

    /* Check parameters. Area can be NULL */
    if (Render == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Render is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (ObjDesc == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else {
        const AMBA_DRAW_POLY_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
        AMBA_DRAW_AREA_s ObjArea;
        Rval = DRAW_WRAP2D(AmbaWrap_memset(&ObjArea, 0x0, sizeof(ObjArea)));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == DRAW_OK) {
            Rval = AmbaDrawPoly_CalcArea(ObjDesc, &ObjArea);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawPoly_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Check area */
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_CheckObjArea(Render, &ObjArea, Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_CheckObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Draw shadow */
        if (Rval == DRAW_OK) {
            if ((ObjCfg->Shadow.Enable == 1U) && (ObjCfg->Shadow.ShadowPostion != AMBA_DRAW_POSITION_MM)) {
                INT32 ShadowX[AMBA_DRAW_MAX_POLY_CORNER_NUM] = {0};
                INT32 ShadowY[AMBA_DRAW_MAX_POLY_CORNER_NUM] = {0};
                INT32 OffsetX = ObjDesc->OffsetX;
                INT32 OffsetY = ObjDesc->OffsetY;

                /* Set Postion */
                if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_LEFT) != 0U) {
                    OffsetX -= (INT32)ObjCfg->Shadow.Distance;
                } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_RIGHT) != 0U) {
                    OffsetX += (INT32)ObjCfg->Shadow.Distance;
                } else {
                    /* Do nothing */
                }

                if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_TOP) != 0U) {
                    OffsetY -= (INT32)ObjCfg->Shadow.Distance;
                } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_BOTTOM) != 0U) {
                    OffsetY += (INT32)ObjCfg->Shadow.Distance;
                } else {
                    /* Do nothing */
                }

                for (UINT32 i = 0U; i < ObjCfg->Corner; ++i) {
                    ShadowX[i] = ObjCfg->X[i] + OffsetX;
                    ShadowY[i] = ObjCfg->Y[i] + OffsetY;
                }

                Rval = AmbaDrawPoly_Render(Render, ObjCfg->Corner, ShadowX, ShadowY, &ObjArea, &ObjCfg->Shadow.Color);
            }
        }

        /* Draw Object */
        if (Rval == DRAW_OK) {
            INT32 PolyX[AMBA_DRAW_MAX_POLY_CORNER_NUM] = {0};
            INT32 PolyY[AMBA_DRAW_MAX_POLY_CORNER_NUM] = {0};
            for (UINT32 i = 0U; i < ObjCfg->Corner; ++i) {
                PolyX[i] = ObjCfg->X[i] + ObjDesc->OffsetX;
                PolyY[i] = ObjCfg->Y[i] + ObjDesc->OffsetY;
            }
            Rval = AmbaDrawPoly_Render(Render, ObjCfg->Corner, PolyX, PolyY, &ObjArea, &ObjCfg->FillColor);
        }
    }

    return Rval;
}

/**
 *  Check config
 *
 *  @param [in] ObjCfg              Object config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCurve_IsValidConfig(const AMBA_DRAW_CURVE_CFG_s *ObjCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjCfg != NULL) {
        if ((ObjCfg->ControlPointNum >= 2U) && (ObjCfg->ControlPointNum <= AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM)) {
            UINT32 i;
            for (i = 0U; i < ObjCfg->ControlPointNum; ++i) {
                UINT8 IsBreak = 0U;
                /* Check redundant points */
                /* Redundant points could cause "divide by 0" error in the later process or could cause singular matrix. */
                if (i >= 1U) {
                    if ((ObjCfg->X[i-1U] == ObjCfg->X[i]) && (ObjCfg->Y[i-1U] == ObjCfg->Y[i])) {
                        AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCurve_IsValidConfig: [ERROR] Redundant Point at [%d] and [%d] (%d %d)", (INT32)i-1, (INT32)i, ObjCfg->X[i], ObjCfg->Y[i], 0);
                        IsBreak = 1U;
                    }
                }
                /* Check line width */
                if ((ObjCfg->LineWidth[i] == 0U) || (ObjCfg->LineWidth[i] > AMBA_DRAW_LINE_WIDTH_MAX)) {
                    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCurve_IsValidConfig: [ERROR] Invalid LineWidth %u", ObjCfg->LineWidth[i], 0U, 0U, 0U, 0U);
                    IsBreak = 1U;
                }
                if (IsBreak == 1U) {
                    break;
                }
            }
            if (i >= ObjCfg->ControlPointNum) {
                Rval = DRAW_OK;
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCirc_IsValidConfig: [ERROR] Invalid ControlPointNum %u", ObjCfg->ControlPointNum, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

static UINT32 AmbaDrawCurve_Render(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_CURVE_DESC_s *Curve)
{
    UINT32 Rval;
    DOUBLE PreX = 0.0;
    DOUBLE PreY = 0.0;

    /* Get previous point */
    Rval = AmbaDrawCubicSpline_Eval(&Curve->SplineX, 0.0, &PreX);
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Eval(X) fail", __func__, NULL, NULL, NULL, NULL);
    } else {
        Rval = AmbaDrawCubicSpline_Eval(&Curve->SplineY, 0.0, &PreY);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Eval(Y) fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    // TODO: Improve performance. Determine the number of segments (AMBA_DRAW_CURVE_SEGMENT) dynamically.
    if (Rval == DRAW_OK) {
        DOUBLE CurX = 0.0;
        DOUBLE CurY = 0.0;
        UINT32 PrevWidth = Curve->ObjCfg.LineWidth[0];
        UINT32 CurWidth = Curve->ObjCfg.LineWidth[0];
        for (UINT32 i = 0U; i < (Curve->ObjCfg.ControlPointNum - 1U); ++i) {
            DOUBLE T1 = Curve->SplineX.Cfg.T[i];
            DOUBLE T2 = Curve->SplineX.Cfg.T[i+1U];
            UINT32 W1 = Curve->ObjCfg.LineWidth[i];
            UINT32 W2 = Curve->ObjCfg.LineWidth[i+1U];
            for (UINT32 J = 0U; J <= AMBA_DRAW_CURVE_SEGMENT; ++J) {
                // TODO: T might not include the values for Max/Min X/Y
                //       So the actual size of the curve might smaller than the return value of "GetObjArea"
                DOUBLE T = T1 + ((T2 - T1) * (DOUBLE)J / (DOUBLE)AMBA_DRAW_CURVE_SEGMENT);
                /* Get current point */
                Rval = AmbaDrawCubicSpline_Eval(&Curve->SplineX, T, &CurX);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Eval(X) fail", __func__, NULL, NULL, NULL, NULL);
                } else {
                    Rval = AmbaDrawCubicSpline_Eval(&Curve->SplineY, T, &CurY);
                    if (Rval != DRAW_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Eval(Y) fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }

                /* Draw a line from previous to current point */
                if (Rval == DRAW_OK) {
                    INT32 PreXI = AmbaDraw_Round(PreX);
                    INT32 PreYI = AmbaDraw_Round(PreY);
                    INT32 CurXI = AmbaDraw_Round(CurX);
                    INT32 CurYI = AmbaDraw_Round(CurY);
                    /* Only draw line when the 2 points are different */
                    /* Don't rule out "PreX,PreY,CurX,CurY < 0". The line is possible to appear when Thickness is large enough. */
                    /* When drawing the last point in this segment, call AmbaDrawLine_Render even if the (X,Y) is not changed so that the cursor can be stopped. */
                    if ((PreXI != CurXI) || (PreYI != CurYI) || (J == AMBA_DRAW_CURVE_SEGMENT)) {
                        UINT8 IsLineContinue = 1U;
                        CurWidth = AmbaDrawCurve_CalcLineWidth(T, T1, T2, W1, W2);
                        if (J == AMBA_DRAW_CURVE_SEGMENT) { /* End of a segment */
                            /* End line when changing color or the end of curve */
                            if (i == (Curve->ObjCfg.ControlPointNum - 2U)) { /* Last segment */
                                IsLineContinue = 0U;
                            } else {
                                INT32 CmpResult;
                                Rval = DRAW_WRAP2D(AmbaWrap_memcmp(&Curve->ObjCfg.LineColor[i], &Curve->ObjCfg.LineColor[i+1U], sizeof(AMBA_DRAW_COLOR_s), &CmpResult));
                                if (Rval != DRAW_OK) {
                                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memcmp fail", __func__, NULL, NULL, NULL, NULL);
                                } else {
                                    if (CmpResult != 0) { /* Changing color */
                                        IsLineContinue = 0U;
                                    }
                                }
                            }
                        }

                        if (Rval == DRAW_OK) {
                            /* End line when changning line width */
                            if (PrevWidth != CurWidth) {
                                AmbaDrawCursor_Stop(&G_Cursor);
                            }

                            Rval = AmbaDrawLine_Render(Render, PreXI, PreYI, CurXI, CurYI, CurWidth, 0U, &Curve->ObjCfg.LineColor[i], IsLineContinue);
                            if (Rval != DRAW_OK) {
                                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawLine_Render fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                    }
                }

                if (Rval == DRAW_OK) {
                    PreX = CurX;
                    PreY = CurY;
                    PrevWidth = CurWidth;
                } else {
                    break;
                }
            }
        }
    }

    return Rval;
}

/**
 *  Calculate cubic spline interpolation
 *
 *  @param [in, out] Curve          Curve config and the returned cubic spline interpolation
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCurve_CalcCubicSpline(AMBA_DRAW_CURVE_DESC_s *Curve)
{
    UINT32 Rval;
    AMBA_DRAW_CUBIC_SPLINE_CFG_s SplineCfgX = {0};
    AMBA_DRAW_CUBIC_SPLINE_CFG_s SplineCfgY = {0};
    UINT32 N = Curve->ObjCfg.ControlPointNum;
    // Parameterization method
    DOUBLE G_Alpha = 0.5;
    DOUBLE Len[AMBA_DRAW_MAX_CURVE_CONTROL_POINT_NUM-1U] = {0.0};
    DOUBLE TotalLen = 0.0;

    /* Fix the VCast error: Symbol 'Len' conceivably not initialized [MISRA 2012 Rule 9.1, mandatory] */
    Rval = DRAW_WRAP2D(AmbaWrap_memset(Len, 0, sizeof(Len)));
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == DRAW_OK) {
        SplineCfgX.ControlPointNum = Curve->ObjCfg.ControlPointNum;
        SplineCfgY.ControlPointNum = Curve->ObjCfg.ControlPointNum;
        for (UINT32 i = 0U; i < N; i++) {
            SplineCfgX.X[i] = (DOUBLE)Curve->ObjCfg.X[i] + (DOUBLE)Curve->OffsetX;
            SplineCfgY.X[i] = (DOUBLE)Curve->ObjCfg.Y[i] + (DOUBLE)Curve->OffsetY;
        }

        /* Determine T by Centripetal method */
        for (UINT32 i = 0U; i < (N - 1U); i++) {
            DOUBLE XDiff = SplineCfgX.X[i + 1U] - SplineCfgX.X[i];
            DOUBLE YDiff = SplineCfgY.X[i + 1U] - SplineCfgY.X[i];
            Rval = AmbaWrap_pow((XDiff * XDiff) + (YDiff * YDiff), 0.5 * G_Alpha, &Len[i]);
            if (Rval != OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_pow fail", __func__, NULL, NULL, NULL, NULL);
                break;
            }
            TotalLen += Len[i];
        }
    }

    if (Rval == DRAW_OK) {
        if (TotalLen > 0.0) {
            DOUBLE CurLen = 0.0;
            for (UINT32 i = 0U; i < N; i++) {
                SplineCfgX.T[i] = CurLen / TotalLen;
                SplineCfgY.T[i] = SplineCfgX.T[i];
                CurLen += Len[i];
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] TotalLen is 0", __func__, NULL, NULL, NULL, NULL);
            Rval = DRAW_ERROR_GENERAL_ERROR;
        }
    }

    /* Calculate cubic spline interpolation */
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawCubicSpline_Calc(&SplineCfgX, &Curve->SplineX);
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawCubicSpline_Calc(&SplineCfgY, &Curve->SplineY);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Calc(Y) fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Calc(X) fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

/**
 *  Calculate the object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            The returned object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCurve_CalcArea(AMBA_DRAW_CURVE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;

    //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCurve_CreateObj: Point num %u", ObjDesc->ObjCfg.ControlPointNum, 0U, 0U, 0U, 0U);
    //for (UINT32 i = 0U; i < ObjDesc->ObjCfg.ControlPointNum; ++i) {
    //    if (i != (ObjDesc->ObjCfg.ControlPointNum - 1U)) {
    //        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCurve_CreateObj: Point[%u] (%u,%u) Thickness %u Color %u", i, ObjDesc->ObjCfg.X[i], ObjDesc->ObjCfg.Y[i], ObjDesc->ObjCfg.LineWidth, ObjDesc->ObjCfg.Color[i]);
    //    } else {
    //        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawCurve_CreateObj: Point[%u] (%u,%u) Thickness %u", i, ObjDesc->ObjCfg.X[i], ObjDesc->ObjCfg.Y[i], ObjDesc->ObjCfg.LineWidth, 0U);
    //    }
    //}

    Rval = AmbaDrawCurve_CalcCubicSpline(ObjDesc);
    if (Rval == DRAW_OK) {
        INT32 XMin;
        INT32 YMin;
        INT32 XMax;
        INT32 YMax;
        /*
            Thickness = TL + TR + 1
            Example:
                Thickness = 4, TL = 1, TR = 2
                Thickness = 5, TL = 2, TR = 2
         */
        INT32 TL;
        INT32 TR;
        INT32 LineWidth;

        /*
            The boundary is at one of the following location:
            1. Control point
            2. Spline Min/Max
            3. The positions where LineWidth changes
         */

        /* Control point */
        /* Set initial Min/Max by the first control point */
        LineWidth = (INT32)ObjDesc->ObjCfg.LineWidth[0];
        TL = (LineWidth - 1) / 2;
        TR = LineWidth / 2;
        XMin = ObjDesc->ObjCfg.X[0] + ObjDesc->OffsetX - TL;
        XMax = ObjDesc->ObjCfg.X[0] + ObjDesc->OffsetX + TR;
        YMin = ObjDesc->ObjCfg.Y[0] + ObjDesc->OffsetY - TL;
        YMax = ObjDesc->ObjCfg.Y[0] + ObjDesc->OffsetY + TR;
        /* Get Min/Max by the rest control points */
        for (UINT32 i = 1U; i < ObjDesc->ObjCfg.ControlPointNum; ++i) {
            LineWidth = (INT32)ObjDesc->ObjCfg.LineWidth[i];
            TL = (LineWidth - 1) / 2;
            TR = LineWidth / 2;
            XMin = AmbaDraw_Min(XMin, ObjDesc->ObjCfg.X[i] + ObjDesc->OffsetX - TL);
            XMax = AmbaDraw_Max(XMax, ObjDesc->ObjCfg.X[i] + ObjDesc->OffsetX + TR);
            YMin = AmbaDraw_Min(YMin, ObjDesc->ObjCfg.Y[i] + ObjDesc->OffsetY - TL);
            YMax = AmbaDraw_Max(YMax, ObjDesc->ObjCfg.Y[i] + ObjDesc->OffsetY + TR);
        }

        /* Spline Min/Max */
        LineWidth = (INT32)AmbaDrawCurve_GetLineWidth(ObjDesc, ObjDesc->SplineX.MinT);
        TL = (LineWidth - 1) / 2;
        TR = LineWidth / 2;
        XMin = AmbaDraw_Min(XMin, AmbaDraw_Round(ObjDesc->SplineX.Min) - TL);

        LineWidth = (INT32)AmbaDrawCurve_GetLineWidth(ObjDesc, ObjDesc->SplineX.MaxT);
        TL = (LineWidth - 1) / 2;
        TR = LineWidth / 2;
        XMax = AmbaDraw_Max(XMax, AmbaDraw_Round(ObjDesc->SplineX.Max) + TR);

        LineWidth = (INT32)AmbaDrawCurve_GetLineWidth(ObjDesc, ObjDesc->SplineY.MinT);
        TL = (LineWidth - 1) / 2;
        TR = LineWidth / 2;
        YMin = AmbaDraw_Min(YMin, AmbaDraw_Round(ObjDesc->SplineY.Min) - TL);

        LineWidth = (INT32)AmbaDrawCurve_GetLineWidth(ObjDesc, ObjDesc->SplineY.MaxT);
        TL = (LineWidth - 1) / 2;
        TR = LineWidth / 2;
        YMax = AmbaDraw_Max(YMax, AmbaDraw_Round(ObjDesc->SplineY.Max) + TR);

        /* The positions where LineWidth changes */
        for (UINT32 i = 0U; i < (ObjDesc->ObjCfg.ControlPointNum - 1U); ++i) {
            DOUBLE T1 = ObjDesc->SplineX.Cfg.T[i];
            DOUBLE T2 = ObjDesc->SplineX.Cfg.T[i+1U];

            if (ObjDesc->ObjCfg.LineWidth[i] != ObjDesc->ObjCfg.LineWidth[i+1U]) {
                INT32 StepW = 1;
                DOUBLE W1 = (DOUBLE)ObjDesc->ObjCfg.LineWidth[i];
                DOUBLE W2 = (DOUBLE)ObjDesc->ObjCfg.LineWidth[i+1U];
                DOUBLE DiffW = (W1 < W2) ? (W2 - W1) : (W1 - W2);
                DOUBLE DiffT = (T2 - T1) / DiffW;
                DOUBLE CurT = T1 + (DiffT / 2.0);
                if (W1 > W2) {
                    StepW = -1;
                }
                /* Find all CurT where LineWidth changes */
                for (INT32 W = (INT32)W1; W < (INT32)W2; W += StepW) {
                    DOUBLE CurX = 0.0;
                    DOUBLE CurY = 0.0;
                    /* The LineWidth at CurT changes from (W) to (W + StepW) */
                    /* Get the max LineWidth at CurT */
                    /* A small error of CurT would cause the LineWidth error of 1, so do not use AmbaDrawCurve_GetLineWidth to get LineWidth. */
                    if (StepW > 0) {
                        LineWidth = W + StepW;
                    } else {
                        LineWidth = W;
                    }
                    TL = (LineWidth - 1) / 2;
                    TR = LineWidth / 2;
                    Rval = AmbaDrawCubicSpline_Eval(&ObjDesc->SplineX, CurT, &CurX);
                    if (Rval != DRAW_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Eval(X) fail", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        Rval = AmbaDrawCubicSpline_Eval(&ObjDesc->SplineY, CurT, &CurY);
                        if (Rval != DRAW_OK) {
                            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCubicSpline_Eval(Y) fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }

                    if (Rval == DRAW_OK) {
                        INT32 CurXI = AmbaDraw_Round(CurX);
                        INT32 CurYI = AmbaDraw_Round(CurY);
                        XMin = AmbaDraw_Min(XMin, CurXI - TL);
                        XMax = AmbaDraw_Max(XMax, CurXI + TR);
                        YMin = AmbaDraw_Min(YMin, CurYI - TL);
                        YMax = AmbaDraw_Max(YMax, CurYI + TR);
                    } else {
                        break;
                    }

                    CurT += DiffT;
                }
            }
        }

        if (Rval == DRAW_OK) {
            INT32 Width  = XMax - XMin + 1;
            INT32 Height = YMax - YMin + 1;
            ObjArea->X = XMin;
            ObjArea->Y = YMin;
            ObjArea->Width  = (UINT32)Width;
            ObjArea->Height = (UINT32)Height;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCurve_CalcCubicSpline fail", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Check parameter
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawCurve_GetObjArea_ParamCheck(const AMBA_DRAW_CURVE_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjDesc != NULL) {
        if (ObjArea != NULL) {
            Rval = AmbaDrawCurve_IsValidConfig(&ObjDesc->ObjCfg);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCurve_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjArea is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCurve_GetObjArea(AMBA_DRAW_CURVE_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    Rval = AmbaDrawCurve_GetObjArea_ParamCheck(ObjDesc, ObjArea);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawCurve_CalcArea(ObjDesc, ObjArea);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCurve_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Draw the Curve object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawCurve_Draw(const AMBA_DRAW_RENDER_s *Render, AMBA_DRAW_CURVE_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;

    /* Check parameters. Area can be NULL */
    if (Render == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Render is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (ObjDesc == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else {
        AMBA_DRAW_AREA_s ObjArea;
        Rval = DRAW_WRAP2D(AmbaWrap_memset(&ObjArea, 0x0, sizeof(ObjArea)));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == DRAW_OK) {
            Rval = AmbaDrawCurve_CalcArea(ObjDesc, &ObjArea);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCurve_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Check area */
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_CheckObjArea(Render, &ObjArea, Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_CheckObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Draw Object */
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawCurve_Render(Render, ObjDesc);
        }
    }

    return Rval;
}

