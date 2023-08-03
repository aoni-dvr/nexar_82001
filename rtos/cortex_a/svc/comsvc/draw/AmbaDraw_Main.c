/**
 * @file AmbaDraw_Main.c
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

#include <AmbaDraw.h>
#include <AmbaDSP_VOUT.h>
#include "AmbaDraw_Main.h"

/*************************************************************************
 * Struct
 ************************************************************************/
/**
 * Draw global configuration
 */
typedef struct {
    AMBA_DRAW_INIT_CONFIG_s InitConfig;                  /**< Module init config                                     */
    AMBADRAW_FONT_s Font;                                /**< Font config                                            */
    UINT32 FreeBufferSize;                               /**< Remained free space in InitConfig.BufferAddr           */
    UINT16 MaxObjNum;                                    /**< Max number of objects                                  */

    /* Configuration for creation */
    UINT16 ObjNum;                                       /**< Total number of created objects. Next Object ID.       */
    UINT16 IdxMapping[AMBA_DRAW_OBJ_MAX_NUM];            /**< Index of each object in description array (ex: BmpDesc)*/
    UINT8 TypeMapping[AMBA_DRAW_OBJ_MAX_NUM];            /**< Type of each object (ex: AMBA_DRAW_OBJ_BMP)            */

    UINT16 LineObjNum;                                   /**< Number of data in LineDesc                             */
    UINT16 RectObjNum;                                   /**< Number of data in RectDesc                             */
    UINT16 CircleObjNum;                                 /**< Number of data in CircDesc                             */
    UINT16 BmpObjNum;                                    /**< Number of data in BmpDesc                              */
    UINT16 StrObjNum;                                    /**< Number of data in StrDesc                              */
    UINT16 PolyObjNum;                                   /**< Number of data in PolyDesc                             */
    UINT16 CurveObjNum;                                  /**< Number of data in CurveDesc                            */
    AMBA_DRAW_LINE_DESC_s *LineDesc;                     /**< Line object description array                          */
    AMBA_DRAW_RECT_DESC_s *RectDesc;                     /**< Rectangle object description array                     */
    AMBA_DRAW_CIRCLE_DESC_s *CircDesc;                   /**< Circle object description array                        */
    AMBA_DRAW_BMP_DESC_s *BmpDesc;                       /**< BMP object description array                           */
    AMBA_DRAW_STR_DESC_s *StrDesc;                       /**< String object description array                        */
    AMBA_DRAW_POLY_DESC_s *PolyDesc;                     /**< Polygon object description array                       */
    AMBA_DRAW_CURVE_DESC_s *CurveDesc;                   /**< Curve object description array                         */

    AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s *FontPageInfo;     /**< Font page info array                                   */
} AMBA_DRAW_GLOBAL_CONFIG_s;

/*************************************************************************
 * Variable
 ************************************************************************/
static AMBA_DRAW_GLOBAL_CONFIG_s GlobalCfg GNU_SECTION_NOZEROINIT;

/*************************************************************************
 * Function
 ************************************************************************/
static UINT32 AmbaDrawMain_AllocateMemory(UINT8 **Buffer, UINT8 **RawBuffer, UINT32 BuffSize)
{
    // TODO: Return aligned address? (Need to increase the size from AmbaDraw_GetInitBufferSize_Impl)
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.FreeBufferSize < BuffSize) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_AllocateMemory: [ERROR] Invalid BuffSize %u Free space %u", BuffSize, GlobalCfg.FreeBufferSize, 0U, 0U, 0U);
        Rval = DRAW_ERROR_OUT_OF_MEMORY;
    } else {
        *Buffer = &GlobalCfg.InitConfig.BufferAddr[GlobalCfg.FreeBufferSize - BuffSize];
        *RawBuffer = &GlobalCfg.InitConfig.BufferAddr[GlobalCfg.FreeBufferSize - BuffSize];
        GlobalCfg.FreeBufferSize -= BuffSize;

        Rval = DRAW_WRAP2D(AmbaWrap_memset(*Buffer, 0x0, BuffSize));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

static UINT32 AmbaDrawMain_InitFont(void)
{
    UINT32 Rval = DRAW_OK;

    /* Error check */
    if (GlobalCfg.Font.Buffer == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Font buffer is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        Rval = AmbaDrawBMPFont_GetFunc((AMBADRAW_FONT_FUNCTION_s *)&GlobalCfg.Font.Func);
        if (Rval == DRAW_OK) {
            /* Load font data into allocated memory */
            Rval = GlobalCfg.Font.Func.Load_f(GlobalCfg.Font.Buffer, GlobalCfg.Font.Size, GlobalCfg.FontPageInfo, GlobalCfg.InitConfig.MaxFontPageNum);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Load_f failed", __func__, NULL, NULL, NULL, NULL);
                Rval = DRAW_ERROR_GENERAL_ERROR;
            } else {
                /* Initialize font module */
                Rval = GlobalCfg.Font.Func.Init_f();
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Init_f failed", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }

    return Rval;
}

/**
 *  Create new line object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object config
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateLine_Impl(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.ObjNum >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.LineObjNum >= GlobalCfg.InitConfig.MaxLineNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] LineObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *ObjId = GlobalCfg.ObjNum;

        GlobalCfg.LineDesc[GlobalCfg.LineObjNum].ObjCfg = *ObjCfg;
        GlobalCfg.TypeMapping[GlobalCfg.ObjNum] = AMBA_DRAW_OBJ_LINE;
        GlobalCfg.IdxMapping[GlobalCfg.ObjNum] = GlobalCfg.LineObjNum;

        GlobalCfg.LineObjNum++;
        GlobalCfg.ObjNum++;
    }

    return Rval;
}

/**
 *  Create new rectangle object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object config
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateRect_Impl(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.ObjNum >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.RectObjNum >= GlobalCfg.InitConfig.MaxRectNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] RectObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *ObjId = GlobalCfg.ObjNum;

        GlobalCfg.RectDesc[GlobalCfg.RectObjNum].ObjCfg = *ObjCfg;
        GlobalCfg.TypeMapping[GlobalCfg.ObjNum] = AMBA_DRAW_OBJ_RECT;
        GlobalCfg.IdxMapping[GlobalCfg.ObjNum] = GlobalCfg.RectObjNum;

        GlobalCfg.RectObjNum++;
        GlobalCfg.ObjNum++;
    }

    return Rval;
}

/**
 *  Create new circle object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object config
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateCircle_Impl(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.ObjNum >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.CircleObjNum >= GlobalCfg.InitConfig.MaxCircleNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] CircleObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *ObjId = GlobalCfg.ObjNum;

        GlobalCfg.CircDesc[GlobalCfg.CircleObjNum].ObjCfg = *ObjCfg;
        GlobalCfg.TypeMapping[GlobalCfg.ObjNum] = AMBA_DRAW_OBJ_CIRCLE;
        GlobalCfg.IdxMapping[GlobalCfg.ObjNum] = GlobalCfg.CircleObjNum;

        GlobalCfg.CircleObjNum++;
        GlobalCfg.ObjNum++;
    }

    return Rval;
}

/**
 *  Create new BMP object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object config
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateBMP_Impl(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.ObjNum >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.BmpObjNum >= GlobalCfg.InitConfig.MaxBmpNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] BmpObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *ObjId = GlobalCfg.ObjNum;

        GlobalCfg.BmpDesc[GlobalCfg.BmpObjNum].ObjCfg = *ObjCfg;
        GlobalCfg.TypeMapping[GlobalCfg.ObjNum] = AMBA_DRAW_OBJ_BMP;
        GlobalCfg.IdxMapping[GlobalCfg.ObjNum] = GlobalCfg.BmpObjNum;

        GlobalCfg.BmpObjNum++;
        GlobalCfg.ObjNum++;
    }

    return Rval;
}

/**
 *  Create new string object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object config
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateString_Impl(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.ObjNum >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.StrObjNum >= GlobalCfg.InitConfig.MaxStringNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] StrObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *ObjId = GlobalCfg.ObjNum;

        GlobalCfg.StrDesc[GlobalCfg.StrObjNum].ObjCfg = *ObjCfg;
        GlobalCfg.TypeMapping[GlobalCfg.ObjNum] = AMBA_DRAW_OBJ_STRING;
        GlobalCfg.IdxMapping[GlobalCfg.ObjNum] = GlobalCfg.StrObjNum;

        GlobalCfg.StrObjNum++;
        GlobalCfg.ObjNum++;
    }

    return Rval;
}

/**
 *  Create new polygon object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object config
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreatePoly_Impl(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.ObjNum >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.PolyObjNum >= GlobalCfg.InitConfig.MaxPolyNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] PolyObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *ObjId = GlobalCfg.ObjNum;

        GlobalCfg.PolyDesc[GlobalCfg.PolyObjNum].ObjCfg = *ObjCfg;
        GlobalCfg.TypeMapping[GlobalCfg.ObjNum] = AMBA_DRAW_OBJ_POLY;
        GlobalCfg.IdxMapping[GlobalCfg.ObjNum] = GlobalCfg.PolyObjNum;

        GlobalCfg.PolyObjNum++;
        GlobalCfg.ObjNum++;
    }

    return Rval;
}

/**
 *  Create new curve object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object config
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateCurve_Impl(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (GlobalCfg.ObjNum >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.CurveObjNum >= GlobalCfg.InitConfig.MaxCurveNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] CurveObjNum out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        *ObjId = GlobalCfg.ObjNum;

        GlobalCfg.CurveDesc[GlobalCfg.CurveObjNum].ObjCfg = *ObjCfg;
        GlobalCfg.TypeMapping[GlobalCfg.ObjNum] = AMBA_DRAW_OBJ_CURVE;
        GlobalCfg.IdxMapping[GlobalCfg.ObjNum] = GlobalCfg.CurveObjNum;

        GlobalCfg.CurveObjNum++;
        GlobalCfg.ObjNum++;
    }

    return Rval;
}

/**
 *  Update line object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateLine_Impl(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.TypeMapping[ObjId] != AMBA_DRAW_OBJ_LINE) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId type not match", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];
        GlobalCfg.LineDesc[ID].ObjCfg = *ObjCfg;
    }

    return Rval;
}

/**
 *  Update rectangle object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateRect_Impl(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.TypeMapping[ObjId] != AMBA_DRAW_OBJ_RECT) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId type not match", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];
        GlobalCfg.RectDesc[ID].ObjCfg = *ObjCfg;
    }

    return Rval;
}

/**
 *  Update circle object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateCircle_Impl(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.TypeMapping[ObjId] != AMBA_DRAW_OBJ_CIRCLE) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId type not match", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];
        GlobalCfg.CircDesc[ID].ObjCfg = *ObjCfg;
    }

    return Rval;
}

/**
 *  Update BMP object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateBMP_Impl(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.TypeMapping[ObjId] != AMBA_DRAW_OBJ_BMP) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId type not match", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];
        GlobalCfg.BmpDesc[ID].ObjCfg = *ObjCfg;
    }

    return Rval;
}

/**
 *  Update string object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateString_Impl(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.TypeMapping[ObjId] != AMBA_DRAW_OBJ_STRING) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId type not match", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];
        GlobalCfg.StrDesc[ID].ObjCfg = *ObjCfg;
    }

    return Rval;
}

/**
 *  Update polygon object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdatePoly_Impl(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.TypeMapping[ObjId] != AMBA_DRAW_OBJ_POLY) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId type not match", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];
        GlobalCfg.PolyDesc[ID].ObjCfg = *ObjCfg;
    }

    return Rval;
}

/**
 *  Update curve object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateCurve_Impl(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.MaxObjNum) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId out of range", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (ObjCfg == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjCfg is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (GlobalCfg.TypeMapping[ObjId] != AMBA_DRAW_OBJ_CURVE) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] ObjId type not match", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];
        GlobalCfg.CurveDesc[ID].ObjCfg = *ObjCfg;
    }

    return Rval;
}

/**
 *  Initialize draw render
 *
 *  @param [in]  BufferInfo         The structure pointer of buffer to draw
 *  @param [in]  Area               The structure pointer of draw object area (NULL = Draw the whole object)
 *  @param [in]  DrawOption         The flag of draw options (Ex. AMBA_DRAW_OPTION_ALPHA_BLENDING)
 *  @param [out] Render             The draw render
 *
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDrawMain_InitRender(const AMBA_DRAW_BUFFER_INFO_s *BufferInfo, const AMBA_DRAW_AREA_s *Area, UINT32 DrawOption, AMBA_DRAW_RENDER_s *Render)
{
    UINT32 Rval;
    UINT32 BufferPixelSize = 1U;

    /* Get pixel size */
    Rval = AmbaDraw_GetPixelSize(BufferInfo->PixelFormat, &BufferPixelSize);
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDraw_GetPixelSize fail", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == DRAW_OK) {
        Render->BufferInfo = *BufferInfo;
        Render->BufferPixelSize = BufferPixelSize;
        if ((DrawOption & AMBA_DRAW_OPTION_ALPHA_BLENDING) != 0U) {
            Render->IsBlending = 1U;
        } else {
            Render->IsBlending = 0U;
        }
        if (Area != NULL) {
            Render->DrawArea = *Area;
        } else {
            Render->DrawArea.X = 0;
            Render->DrawArea.Y = 0;
            Render->DrawArea.Width = Render->BufferInfo.Width;
            Render->DrawArea.Height = Render->BufferInfo.Height;
        }

        Rval = AmbaDrawRender_Init(Render);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRender_Init fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

/**
 *  Draw object to buffer
 *  User needs to make sure that the PixelFormat of the object is consistent with the buffer.
 *
 *  @param [in] ObjId               Object ID
 *  @param [in] X_offset            X-offset (in Pixel)
 *  @param [in] Y_offset            Y-offset (in Pixel)
 *  @param [in] BufferInfo          The structure pointer of buffer to draw
 *  @param [in] Area                The structure pointer of draw object area (NULL = Draw the whole object)
 *  @param [in] DrawOption          The flag of draw options (Ex. AMBA_DRAW_OPTION_ALPHA_BLENDING)
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_DrawBuffer_Impl(UINT16 ObjId, INT32 X_offset, INT32 Y_offset, const AMBA_DRAW_BUFFER_INFO_s *BufferInfo, const AMBA_DRAW_AREA_s *Area, UINT32 DrawOption)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId >= GlobalCfg.ObjNum) {
        AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] ObjId %u out of range", ObjId, 0U, 0U, 0U, 0U);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else if (BufferInfo->RGBAddr == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] RGBAddr is NULL", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    } else {
        static AMBA_DRAW_RENDER_s Render; /* Not declairing as local a variable for MisraC Rule 18.6 */

        Rval = AmbaDrawMain_InitRender(BufferInfo, Area, DrawOption, &Render);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_InitRender fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Rval == DRAW_OK) {
            UINT16 ID = GlobalCfg.IdxMapping[ObjId];
            switch (GlobalCfg.TypeMapping[ObjId]) {
            case AMBA_DRAW_OBJ_LINE:
                GlobalCfg.LineDesc[ID].OffsetX = X_offset;
                GlobalCfg.LineDesc[ID].OffsetY = Y_offset;
                Rval = AmbaDrawLine_Draw(&Render, &GlobalCfg.LineDesc[ID], Area);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] AmbaDrawLine_Draw fail. Offset (%d %d)", X_offset, Y_offset, 0, 0, 0);
                }
                break;
            case AMBA_DRAW_OBJ_RECT:
                GlobalCfg.RectDesc[ID].OffsetX = X_offset;
                GlobalCfg.RectDesc[ID].OffsetY = Y_offset;
                Rval = AmbaDrawRect_Draw(&Render, &GlobalCfg.RectDesc[ID], Area);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] AmbaDrawRect_Draw fail. Offset (%d %d)", X_offset, Y_offset, 0, 0, 0);
                }
                break;
            case AMBA_DRAW_OBJ_CIRCLE:
                GlobalCfg.CircDesc[ID].OffsetX = X_offset;
                GlobalCfg.CircDesc[ID].OffsetY = Y_offset;
                Rval = AmbaDrawCirc_Draw(&Render, &GlobalCfg.CircDesc[ID], Area);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] AmbaDrawCirc_Draw fail. Offset (%d %d)", X_offset, Y_offset, 0, 0, 0);
                }
                break;
            case AMBA_DRAW_OBJ_BMP:
                GlobalCfg.BmpDesc[ID].OffsetX = X_offset;
                GlobalCfg.BmpDesc[ID].OffsetY = Y_offset;
                Rval = AmbaDrawBMP_Draw(&Render, &GlobalCfg.BmpDesc[ID], Area);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] AmbaDrawBMP_Draw fail. Offset (%d %d)", X_offset, Y_offset, 0, 0, 0);
                }
                break;
            case AMBA_DRAW_OBJ_STRING:
                GlobalCfg.StrDesc[ID].OffsetX = X_offset;
                GlobalCfg.StrDesc[ID].OffsetY = Y_offset;
                GlobalCfg.StrDesc[ID].FontAttr = &GlobalCfg.Font;
                Rval = AmbaDrawStr_Draw(&Render, &GlobalCfg.StrDesc[ID], Area);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] AmbaDrawStr_Draw fail. Offset (%d %d)", X_offset, Y_offset, 0, 0, 0);
                }
                break;
            case AMBA_DRAW_OBJ_POLY:
                GlobalCfg.PolyDesc[ID].OffsetX = X_offset;
                GlobalCfg.PolyDesc[ID].OffsetY = Y_offset;
                Rval = AmbaDrawPoly_Draw(&Render, &GlobalCfg.PolyDesc[ID], Area);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] AmbaDrawPoly_Draw fail. Offset (%d %d)", X_offset, Y_offset, 0, 0, 0);
                }
                break;
            case AMBA_DRAW_OBJ_CURVE:
                GlobalCfg.CurveDesc[ID].OffsetX = X_offset;
                GlobalCfg.CurveDesc[ID].OffsetY = Y_offset;
                Rval = AmbaDrawCurve_Draw(&Render, &GlobalCfg.CurveDesc[ID], Area);
                if (Rval != DRAW_OK) {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] AmbaDrawCurve_Draw fail. Offset (%d %d)", X_offset, Y_offset, 0, 0, 0);
                }
                break;
            default:
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_DrawBuffer_Impl: [ERROR] Invalid Type %u", GlobalCfg.TypeMapping[ObjId], 0U, 0U, 0U, 0U);
                Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
                break;
            }
        }
    }

    return Rval;
}

/**
 *  Get object area
 *
 *  @param [in]  ObjId              Object id
 *  @param [in]  X                  X-offset (in Pixel)
 *  @param [in]  Y                  Y-offset (in Pixel)
 *  @param [out] Area               Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_GetObjArea_Impl(UINT16 ObjId, INT32 X, INT32 Y, AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_OK;

    if (ObjId < GlobalCfg.ObjNum) {
        UINT16 ID = GlobalCfg.IdxMapping[ObjId];

        switch (GlobalCfg.TypeMapping[ObjId]) {
        case AMBA_DRAW_OBJ_LINE:
            GlobalCfg.LineDesc[ID].OffsetX = X;
            GlobalCfg.LineDesc[ID].OffsetY = Y;
            Rval = AmbaDrawLine_GetObjArea(&GlobalCfg.LineDesc[ID], Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawLine_GetObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case AMBA_DRAW_OBJ_RECT:
            GlobalCfg.RectDesc[ID].OffsetX = X;
            GlobalCfg.RectDesc[ID].OffsetY = Y;
            Rval = AmbaDrawRect_GetObjArea(&GlobalCfg.RectDesc[ID], Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRect_GetObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case AMBA_DRAW_OBJ_CIRCLE:
            GlobalCfg.CircDesc[ID].OffsetX = X;
            GlobalCfg.CircDesc[ID].OffsetY = Y;
            Rval = AmbaDrawCirc_GetObjArea(&GlobalCfg.CircDesc[ID], Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCirc_GetObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case AMBA_DRAW_OBJ_BMP:
            GlobalCfg.BmpDesc[ID].OffsetX = X;
            GlobalCfg.BmpDesc[ID].OffsetY = Y;
            Rval = AmbaDrawBMP_GetObjArea(&GlobalCfg.BmpDesc[ID], Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawBMP_GetObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case AMBA_DRAW_OBJ_STRING:
            GlobalCfg.StrDesc[ID].OffsetX = X;
            GlobalCfg.StrDesc[ID].OffsetY = Y;
            GlobalCfg.StrDesc[ID].FontAttr = &GlobalCfg.Font;
            Rval = AmbaDrawStr_GetObjArea(&GlobalCfg.StrDesc[ID], Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawStr_GetObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case AMBA_DRAW_OBJ_POLY:
            GlobalCfg.PolyDesc[ID].OffsetX = X;
            GlobalCfg.PolyDesc[ID].OffsetY = Y;
            Rval = AmbaDrawPoly_GetObjArea(&GlobalCfg.PolyDesc[ID], Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawPoly_GetObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case AMBA_DRAW_OBJ_CURVE:
            GlobalCfg.CurveDesc[ID].OffsetX = X;
            GlobalCfg.CurveDesc[ID].OffsetY = Y;
            Rval = AmbaDrawCurve_GetObjArea(&GlobalCfg.CurveDesc[ID], Area);
            if (Rval != DRAW_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCurve_GetObjArea fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        default:
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Unecpected type", __func__, NULL, NULL, NULL, NULL);
            Rval = DRAW_ERROR_PARAM_VALUE_RANGE;
            break;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid ObjId", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_GENERAL_ERROR;
    }

    return Rval;
}

/**
 *  Get Buffer size
 *
 *  @param [in]  InitConfig         The structure pointer of initial config
 *  @param [out] BufferSize         Required buffer size (in Byte)
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_GetInitBufferSize_Impl(const AMBA_DRAW_INIT_CONFIG_s *InitConfig, UINT32 *BufferSize)
{
    UINT32 Size = ((UINT32)sizeof(AMBA_DRAW_LINE_DESC_s) * InitConfig->MaxLineNum)
                    + ((UINT32)sizeof(AMBA_DRAW_RECT_DESC_s) * InitConfig->MaxRectNum)
                    + ((UINT32)sizeof(AMBA_DRAW_CIRCLE_DESC_s) * InitConfig->MaxCircleNum)
                    + ((UINT32)sizeof(AMBA_DRAW_BMP_DESC_s) * InitConfig->MaxBmpNum)
                    + ((UINT32)sizeof(AMBA_DRAW_STR_DESC_s) * InitConfig->MaxStringNum)
                    + ((UINT32)sizeof(AMBA_DRAW_POLY_DESC_s) * InitConfig->MaxPolyNum)
                    + ((UINT32)sizeof(AMBA_DRAW_CURVE_DESC_s) * InitConfig->MaxCurveNum)
                    + ((UINT32)sizeof(AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s) * InitConfig->MaxFontPageNum);

    *BufferSize = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
    return DRAW_OK;
}

static UINT32 AmbaDrawMain_CfgGolbalVar_Impl(const AMBA_DRAW_INIT_CONFIG_s *InitConfig)
{
    UINT32 Rval;
    UINT32 BufferSize = 0U;

    Rval = AmbaDraw_GetInitBufferSize_Impl(InitConfig, &BufferSize);
    if (Rval == DRAW_OK) {
        if (BufferSize > InitConfig->BufferSize) {
            AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDrawMain_CfgGolbalVar_Impl: [ERROR] BufferSize %u not enough (Need %u)", InitConfig->BufferSize, BufferSize, 0U, 0U, 0U);
            Rval = DRAW_ERROR_OUT_OF_MEMORY;
        }
    }

    if (Rval == DRAW_OK) {
        /* Initialize GlobalCfg */
        Rval = DRAW_WRAP2D(AmbaWrap_memset(&GlobalCfg, 0x0, sizeof(GlobalCfg)));
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        } else {
            GlobalCfg.InitConfig     = *InitConfig;
            GlobalCfg.Font.Size      = InitConfig->FontBufSize;
            GlobalCfg.Font.Buffer    = InitConfig->FontAddr;
            GlobalCfg.FreeBufferSize = InitConfig->BufferSize;

            /* The sum of MaxXXXNum is already checked in AmbaDraw_Init_ParamCheck */
            GlobalCfg.MaxObjNum = InitConfig->MaxLineNum + InitConfig->MaxRectNum + InitConfig->MaxCircleNum
                                + InitConfig->MaxBmpNum + InitConfig->MaxStringNum + InitConfig->MaxPolyNum + InitConfig->MaxCurveNum;
        }
    }

    return Rval;
}

/**
 *  Init AmbaDraw module
 *
 *  @param [in] InitConfig          The structure pointer of initial config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_Init_Impl(const AMBA_DRAW_INIT_CONFIG_s *InitConfig)
{
    UINT32 Rval;

    Rval = AmbaDrawMain_CfgGolbalVar_Impl(InitConfig);
    if (Rval != DRAW_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_CfgGolbalVar_Impl failed", __func__, NULL, NULL, NULL, NULL);
    }

    /* Allocate Line buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_LINE_DESC_s) * GlobalCfg.InitConfig.MaxLineNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.LineDesc, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(LINE) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Allocate Rectangle buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_RECT_DESC_s) * GlobalCfg.InitConfig.MaxRectNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.RectDesc, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(RECT) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Allocate Circle buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_CIRCLE_DESC_s) * GlobalCfg.InitConfig.MaxCircleNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.CircDesc, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(CIRCLE) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Allocate BMP buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_BMP_DESC_s) * GlobalCfg.InitConfig.MaxBmpNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.BmpDesc, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(BMP) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Allocate String buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_STR_DESC_s) * GlobalCfg.InitConfig.MaxStringNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.StrDesc, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(STRING) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Allocate Polygon buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_POLY_DESC_s) * GlobalCfg.InitConfig.MaxPolyNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.PolyDesc, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(POLY) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Allocate Curve buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_CURVE_DESC_s) * GlobalCfg.InitConfig.MaxCurveNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.CurveDesc, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(CURVE) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Allocate Font Info buffer size */
    if (Rval == DRAW_OK) {
        UINT8 *Addr = NULL;
        UINT8 *RawAddr = NULL;
        UINT32 Size = (UINT32)sizeof(AMBA_DRAW_BMPFONT_BIN_FONT_INFO_s) * GlobalCfg.InitConfig.MaxFontPageNum;
        Rval = AmbaDrawMain_AllocateMemory(&Addr, &RawAddr, Size);
        if (Rval == DRAW_OK) {
            AmbaMisra_TypeCast(&GlobalCfg.FontPageInfo, &Addr);
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawMain_AllocateMemory(FONT) failed", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == DRAW_OK) {
        Rval = AmbaDrawMain_InitFont();
    }

    return Rval;
}
