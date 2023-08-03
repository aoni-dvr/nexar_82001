/**
 * @file AmbaDraw_String.c
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
#include "string/AmbaDraw_String.h"

/*************************************************************************
 * String APIs
 ************************************************************************/
static UINT32 AmbaDrawStr_DrawShadow(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_STR_DESC_s *ObjDesc)
{
    const AMBA_DRAW_STRING_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
    const AMBADRAW_FONT_s *FontAttr = ObjDesc->FontAttr;
    AMBADRAW_FONT_DRAW_STRING_CONFIG_s DrawConfig = {0};
    INT32 OffsetX = ObjDesc->StringArea.X;
    INT32 OffsetY = ObjDesc->StringArea.Y;
    UINT32 Rval = DRAW_OK;

    /* Calculate X-shift */
    if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_LEFT) != 0U) {
        OffsetX -= (INT32)ObjCfg->Shadow.Distance;
    } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_H_RIGHT) != 0U) {
        OffsetX += (INT32)ObjCfg->Shadow.Distance;
    } else {
        /* Do nothing */
    }

    /* Calculate Y-shift */
    if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_TOP) != 0U) {
        OffsetY -= (INT32)ObjCfg->Shadow.Distance;
    } else if ((ObjCfg->Shadow.ShadowPostion & AMBA_DRAW_ALIGN_V_BOTTOM) != 0U) {
        OffsetY += (INT32)ObjCfg->Shadow.Distance;
    } else {
        /* Do nothing */
    }

    DrawConfig.FontId = ObjCfg->FontId;
    DrawConfig.String = ObjCfg->Msg;
    DrawConfig.ColorFore = ObjCfg->Shadow.Color;
    DrawConfig.ColorBack = ObjCfg->Shadow.Color;
    DrawConfig.StringX = OffsetX;
    DrawConfig.StringY = OffsetY;

    if (FontAttr->Func.Draw_f != NULL) {
        Rval = FontAttr->Func.Draw_f(Render, &DrawConfig);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Draw_f failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

static UINT32 AmbaDrawStr_DrawString(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_STR_DESC_s *ObjDesc)
{
    const AMBA_DRAW_STRING_CFG_s *ObjCfg = &ObjDesc->ObjCfg;
    const AMBADRAW_FONT_s *FontAttr = ObjDesc->FontAttr;
    AMBADRAW_FONT_DRAW_STRING_CONFIG_s DrawConfig = {0};
    UINT32 Rval = DRAW_OK;

    DrawConfig.FontId = ObjCfg->FontId;
    DrawConfig.String = ObjCfg->Msg;
    DrawConfig.ColorFore = ObjCfg->Color;
    DrawConfig.ColorBack = ObjCfg->Color;
    DrawConfig.StringX = ObjDesc->StringArea.X;
    DrawConfig.StringY = ObjDesc->StringArea.Y;

    if (FontAttr->Func.Draw_f != NULL) {
        Rval = FontAttr->Func.Draw_f(Render, &DrawConfig);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Draw_f failed", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDrawStr_IsValidConfig(const AMBA_DRAW_STRING_CFG_s *ObjCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjCfg != NULL) {
        if (ObjCfg->Width > 0U) {
            if (ObjCfg->Height > 0U) {
                Rval = AmbaDraw_IsValidPos(ObjCfg->Alignment);
                if (Rval == DRAW_OK) {
                    Rval = AmbaDraw_IsValidShadow(&(ObjCfg->Shadow));
                    if (Rval != DRAW_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_IsValidShadow failed", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawStr_IsValidConfig: [ERROR] Invalid Alignment %u", ObjCfg->Alignment, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawStr_IsValidConfig: [ERROR] Invalid Height %u", ObjCfg->Height, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawStr_IsValidConfig: [ERROR] Invalid Width %u", ObjCfg->Width, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
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
static UINT32 AmbaDrawStr_CalcArea(AMBA_DRAW_STR_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    const AMBA_DRAW_STRING_CFG_s *ObjCfg = &ObjDesc->ObjCfg;

    Rval = ObjDesc->FontAttr->Func.GetStrWidth_f(ObjCfg->FontId, ObjCfg->Msg, &ObjDesc->StringArea.Width);
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] GetStrWidth_f failed", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == DRAW_OK) {
        Rval = ObjDesc->FontAttr->Func.GetStrHeight_f(ObjCfg->FontId, ObjCfg->Msg, &ObjDesc->StringArea.Height);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] GetStrHeight_f failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == DRAW_OK) {
        INT32 DeltaX = 0;
        INT32 DeltaY = 0;

        /* Calculate X-shift */
        if ((ObjCfg->Alignment & AMBA_DRAW_ALIGN_H_LEFT) != 0U) {
            /* Do nothing */
        } else if ((ObjCfg->Alignment & AMBA_DRAW_ALIGN_H_RIGHT) != 0U) {
            DeltaX = (INT32)ObjCfg->Width - (INT32)ObjDesc->StringArea.Width;
        } else {
            DeltaX = ((INT32)ObjCfg->Width - (INT32)ObjDesc->StringArea.Width) / 2;
        }

        /* Calculate Y-shift */
        if ((ObjCfg->Alignment & AMBA_DRAW_ALIGN_V_TOP) != 0U) {
            /* Do nothing */
        } else if ((ObjCfg->Alignment & AMBA_DRAW_ALIGN_V_BOTTOM) != 0U) {
            DeltaY = (INT32)ObjCfg->Height - (INT32)ObjDesc->StringArea.Height;
        } else {
            DeltaY = ((INT32)ObjCfg->Height - (INT32)ObjDesc->StringArea.Height) / 2;
        }

        /* Allow X, Y to be negative */
        ObjDesc->StringArea.X = ObjDesc->OffsetX + DeltaX;
        ObjDesc->StringArea.Y = ObjDesc->OffsetY + DeltaY;
        ObjArea->X = ObjDesc->StringArea.X;
        ObjArea->Y = ObjDesc->StringArea.Y;
        ObjArea->Width = ObjDesc->StringArea.Width;
        ObjArea->Height = ObjDesc->StringArea.Height;
    }

    /* Combine the area of StringWidth*StringHeight with shadow area. */
    if ((Rval == DRAW_OK) && (ObjCfg->Shadow.Enable == 1U)) {
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
static UINT32 AmbaDrawStr_GetObjArea_ParamCheck(const AMBA_DRAW_STR_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjDesc != NULL) {
        if (ObjArea != NULL) {
            Rval = AmbaDrawBMPFont_IsValidFont(ObjDesc->FontAttr);
            if (Rval == DRAW_OK) {
                Rval = AmbaDrawStr_IsValidConfig(&ObjDesc->ObjCfg);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawStr_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBMPFont_IsValidFont failed", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDrawStr_GetObjArea(AMBA_DRAW_STR_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    Rval = AmbaDrawStr_GetObjArea_ParamCheck(ObjDesc, ObjArea);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawStr_CalcArea(ObjDesc, ObjArea);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawStr_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Draw the String object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawStr_Draw(const AMBA_DRAW_RENDER_s *Render, AMBA_DRAW_STR_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;

    /* Check parameters. Area can be NULL */
    if (Render == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Render is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (ObjDesc == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjDesc is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else if (AmbaDrawBMPFont_IsValidFont(ObjDesc->FontAttr) != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBMPFont_IsValidFont failed", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    } else {
        AMBA_DRAW_AREA_s ObjArea;
        Rval = DRAW_WRAP2D(AmbaWrap_memset(&ObjArea, 0x0, sizeof(ObjArea)));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == DRAW_OK) {
            Rval = AmbaDrawStr_CalcArea(ObjDesc, &ObjArea);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawStr_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
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
            if ((ObjDesc->ObjCfg.Shadow.Enable == 1U) && (ObjDesc->ObjCfg.Shadow.ShadowPostion != AMBA_DRAW_POSITION_MM)) {
                Rval = AmbaDrawStr_DrawShadow(Render, ObjDesc);
            }
        }

        /* Draw string */
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawStr_DrawString(Render, ObjDesc);
        }
    }

    return Rval;
}

