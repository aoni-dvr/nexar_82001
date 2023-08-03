/**
 * @file AmbaDraw_Common.c
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
// #include <stdarg.h>
#include "AmbaDraw_Common.h"



/*************************************************************************
 * Enum
 ************************************************************************/

/*************************************************************************
 * Struct
 ************************************************************************/

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Variable
 ************************************************************************/


/*************************************************************************
 * Function
 ************************************************************************/
/**
 *  Get object size
 *
 *  @param [in] PixelFormat         Pixel format
 *  @param [out] PixelSize          Pixel size (byte)
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_GetPixelSize(UINT8 PixelFormat, UINT32 *PixelSize)
{
    UINT32 Rval = DRAW_OK;
    if (PixelFormat < OSD_16BIT_VYU_RGB_565) {
        *PixelSize = 1U;
    } else if (PixelFormat < OSD_32BIT_AYUV_8888) {
        *PixelSize = 2U;
    } else if (PixelFormat <= OSD_32BIT_ARGB_8888) {
        *PixelSize = 4U;
    } else if (PixelFormat <= AMBA_DRAW_BUFFER_YUV422_WITH_ALPHA) {
        *PixelSize = 1U;
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_GetPixelSize: [ERROR] Invalid PixelFormat %u", (UINT32)PixelFormat, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
    }
    return Rval;
}

/**
 *  Check the object area.
 *
 *  @param [in] Render              The draw render
 *  @param [in] ObjArea             The object area
 *  @param [in] Area                The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_CheckObjArea(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_AREA_s *ObjArea, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;
    if (Area != NULL) {
        /* When Area is not NULL, allow ObjArea to be outside of buffer. */

        /* There should be intersection of ObjArea and Area. */
        if (((Area->X + (INT32)Area->Width) <= ObjArea->X) ||
            ((Area->Y + (INT32)Area->Height) <= ObjArea->Y) ||
            (Area->X >= (ObjArea->X + (INT32)ObjArea->Width)) ||
            (Area->Y >= (ObjArea->Y + (INT32)ObjArea->Height))) {
            AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_CheckObjArea: [ERROR] Area (%d %d %d %d) does not overlap ObjArea", Area->X, Area->Y, (INT32)Area->Width, (INT32)Area->Height, 0);
            AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_CheckObjArea: [ERROR] ObjArea (%d %d %d %d)", ObjArea->X, ObjArea->Y, (INT32)ObjArea->Width, (INT32)ObjArea->Height, 0);
            Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
        }
        /* Area cannot be outside of buffer. */
        if ((Area->X < 0) ||
            (Area->Y < 0) ||
            ((Area->X + (INT32)Area->Width) > (INT32)Render->BufferInfo.Width) ||
            ((Area->Y + (INT32)Area->Height) > (INT32)Render->BufferInfo.Height)) {
            AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_CheckObjArea: [ERROR] Area (%d %d %d %d) out of buffer range", Area->X, Area->Y, (INT32)Area->Width, (INT32)Area->Height, 0);
            Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
        }
    } else {
        /* When Area is NULL, ObjArea cannot be outside of buffer. */

        /* Check buffer boundary */
        if ((ObjArea->X < 0) ||
            (ObjArea->Y < 0) ||
            ((ObjArea->X + (INT32)ObjArea->Width) > (INT32)Render->BufferInfo.Width) ||
            ((ObjArea->Y + (INT32)ObjArea->Height) > (INT32)Render->BufferInfo.Height)) {
            AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_CheckObjArea: [ERROR] ObjArea (%d %d %d %d) out of buffer range", ObjArea->X, ObjArea->Y, (INT32)ObjArea->Width, (INT32)ObjArea->Height, 0);
            Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
        }
    }

    return Rval;
}

