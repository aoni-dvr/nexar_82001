/**
 * @file AmbaDraw_BMP.c
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
#include "bmp/AmbaDraw_BMP.h"
#include "AmbaDraw_Render.h"

static UINT32 AmbaDrawBMP_RAW(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_BMP_CFG_s *Bmp, INT32 OffsetX, INT32 OffsetY)
{
    UINT32 Rval;
    AMBA_DRAW_RENDER_s SrcRender;

    Rval = DRAW_WRAP2D(AmbaWrap_memset(&SrcRender, 0, sizeof(SrcRender)));
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
    } else {
        //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBMP_RAW: BMP Flags %u W %u H %u TransColor %u", Bmp->Flags, Bmp->Width, Bmp->Height, Bmp->TransparentColor, 0U);

        /* PixelFormat of the object and the draw buffer (Render) should be the same. User should take care of this. */
        SrcRender.BufferInfo.PixelFormat = Render->BufferInfo.PixelFormat;
        SrcRender.BufferInfo.Width = (UINT32)Bmp->Width;
        SrcRender.BufferInfo.Height = (UINT32)Bmp->Height;
        SrcRender.BufferInfo.Pitch = (UINT32)Bmp->Width * Render->BufferPixelSize;
        if (SrcRender.BufferInfo.PixelFormat == AMBA_DRAW_BUFFER_YUV420_WITH_ALPHA) {
            UINT32 ImageSize = SrcRender.BufferInfo.Pitch * SrcRender.BufferInfo.Height;
            SrcRender.BufferInfo.YAddr       = Bmp->Data;
            SrcRender.BufferInfo.UVAddr      = &SrcRender.BufferInfo.YAddr[ImageSize/2U];
            SrcRender.BufferInfo.AlphaYAddr  = &SrcRender.BufferInfo.UVAddr[ImageSize];
            SrcRender.BufferInfo.AlphaUVAddr = &SrcRender.BufferInfo.AlphaYAddr[ImageSize/2U];
        } else if (SrcRender.BufferInfo.PixelFormat == AMBA_DRAW_BUFFER_YUV422_WITH_ALPHA) {
            UINT32 ImageSize = SrcRender.BufferInfo.Pitch * SrcRender.BufferInfo.Height;
            SrcRender.BufferInfo.YAddr       = Bmp->Data;
            SrcRender.BufferInfo.UVAddr      = &SrcRender.BufferInfo.YAddr[ImageSize];
            SrcRender.BufferInfo.AlphaYAddr  = &SrcRender.BufferInfo.UVAddr[ImageSize];
            SrcRender.BufferInfo.AlphaUVAddr = &SrcRender.BufferInfo.AlphaYAddr[ImageSize];
        } else {
            SrcRender.BufferInfo.RGBAddr     = Bmp->Data;
        }
        SrcRender.BufferPixelSize = Render->BufferPixelSize; /* PixelFormat is the same, so BufferPixelSize should be equal. */
        SrcRender.IsBlending = 0U;
        SrcRender.DrawArea.X = 0;
        SrcRender.DrawArea.Y = 0;
        SrcRender.DrawArea.Width = SrcRender.BufferInfo.Width;
        SrcRender.DrawArea.Height = SrcRender.BufferInfo.Height;

        Rval = AmbaDrawRender_Init(&SrcRender);
        if (Rval == DRAW_OK) {
            for (INT32 Y = 0; Y < (INT32)Bmp->Height; Y++) {
                for (INT32 X = 0; X < (INT32)Bmp->Width; X++) {
                    AMBA_DRAW_COLOR_s Value = {0};
                    Rval = SrcRender.GetPixelAt_f(&SrcRender, X, Y, &Value);
                    if (Rval == DRAW_OK) {
                        INT32 ScanX = X + OffsetX;
                        INT32 ScanY = Y + OffsetY;
                        if (Bmp->Transparent != 0U) {
                            INT32 CmpResult;
                            Rval = DRAW_WRAP2D(AmbaWrap_memcmp(&Value, &Bmp->TransparentColor, sizeof(AMBA_DRAW_COLOR_s), &CmpResult));
                            if (Rval == DRAW_OK) {
                                /* Write opaque pixels only (Ignore transparent pixels) */
                                if (CmpResult != 0) {
                                    Render->PlotPixelAt_f(Render, ScanX, ScanY, &Value);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memcmp fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            /* Write all pixels (including transparent pixels) */
                            Render->PlotPixelAt_f(Render, ScanX, ScanY, &Value);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] GetPixelAt_f fail", __func__, NULL, NULL, NULL, NULL);
                    }

                    if (Rval != DRAW_OK) {
                        break;
                    }
                }

                if (Rval != DRAW_OK) {
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRender_Init fail", __func__, NULL, NULL, NULL, NULL);
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
static UINT32 AmbaDrawBMP_IsValidBMP(const AMBA_DRAW_BMP_CFG_s *ObjCfg)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjCfg != NULL) {
        if (ObjCfg->Width > 0U) {
            if (ObjCfg->Height > 0U) {
                if (ObjCfg->Data != NULL) {
                    Rval = DRAW_OK;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Data is NULL", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBMP_IsValidConfig: [ERROR] Invalid Height %u", (UINT32)ObjCfg->Height, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBMP_IsValidConfig: [ERROR] Invalid Width %u", (UINT32)ObjCfg->Width, 0U, 0U, 0U, 0U);
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
static UINT32 AmbaDrawBMP_CalcArea(const AMBA_DRAW_BMP_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_OK;

    ObjArea->X = ObjDesc->OffsetX;
    ObjArea->Y = ObjDesc->OffsetY;
    ObjArea->Width  = (UINT32)ObjDesc->ObjCfg.Width;
    ObjArea->Height = (UINT32)ObjDesc->ObjCfg.Height;

    //AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawBMP_CalcArea: Area(%u %u %u %u)", ObjArea->X, ObjArea->Y, ObjArea->Width, ObjArea->Height, 0U);

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
static UINT32 AmbaDrawBMP_GetObjArea_ParamCheck(const AMBA_DRAW_BMP_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjDesc != NULL) {
        if (ObjArea != NULL) {
            Rval = AmbaDrawBMP_IsValidBMP(&ObjDesc->ObjCfg);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBMP_IsValidBMP failed", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDrawBMP_GetObjArea(const AMBA_DRAW_BMP_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea)
{
    UINT32 Rval;
    Rval = AmbaDrawBMP_GetObjArea_ParamCheck(ObjDesc, ObjArea);
    if (Rval == DRAW_OK) {
        Rval = AmbaDrawBMP_CalcArea(ObjDesc, ObjArea);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBMP_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Draw the BMP object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawBMP_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_BMP_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area)
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
            Rval = AmbaDrawBMP_CalcArea(ObjDesc, &ObjArea);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBMP_CalcArea failed", __func__, NULL, NULL, NULL, NULL);
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
            Rval = AmbaDrawBMP_RAW(Render, &ObjDesc->ObjCfg, ObjDesc->OffsetX, ObjDesc->OffsetY);
        }
    }

    return Rval;
}

