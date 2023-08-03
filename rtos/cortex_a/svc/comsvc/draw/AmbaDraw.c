/**
 * @file AmbaDraw.c
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
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include "AmbaDraw_Main.h"

/*************************************************************************
 * Definitions
 ************************************************************************/
#define AMBA_DRAW_DEFAULT_PAGE_NUM (4096U)             /**< Max number of Pages */

/*************************************************************************
 * Struct
 ************************************************************************/


/*************************************************************************
 * Variable
 ************************************************************************/
static AMBA_KAL_MUTEX_t DrawMutex;                     /**< AmbaDraw core mutex */

/*************************************************************************
 * APIs
 ************************************************************************/
/**
 *  Create CFS streams' mutex
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDraw_CreateMutex(void)
{
    UINT32 Rval = DRAW_OK;
    if (AmbaKAL_MutexCreate(&DrawMutex, NULL) != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexCreate fail", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_OS;
    }
    return Rval;
}

/**
 *  Lock Draw mutex
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDraw_Lock(void)
{
    UINT32 Rval = DRAW_OK;
    if (AmbaKAL_MutexTake(&DrawMutex, AMBA_DRAW_TIMEOUT_MUTEX) != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_OS;
    }
    return Rval;
}

/**
 *  Unlock Draw mutex
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaDraw_Unlock(void)
{
    UINT32 Rval = DRAW_OK;
    if (AmbaKAL_MutexGive(&DrawMutex) != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_OS;
    }
    return Rval;
}

static UINT32 AmbaDraw_Init_ParamCheck(const AMBA_DRAW_INIT_CONFIG_s *InitConfig)
{
    UINT32 Rval = DRAW_ERROR_ARG;
    if (InitConfig->FontBufSize != 0U) {
        if (InitConfig->FontAddr != NULL) {
            ULONG BufferAddrUL = 0U;
            AmbaMisra_TypeCast(&BufferAddrUL, &InitConfig->BufferAddr);
            /* Check BufferAddr valid and aligned */
            if ((InitConfig->BufferAddr != NULL) && (BufferAddrUL == AmbaDraw_GetAlignedValUL(BufferAddrUL, AMBA_CACHE_LINE_SIZE))) {
                /* Check BufferSize valid and aligned */
                if ((InitConfig->BufferSize != 0U) && (InitConfig->BufferSize == GetAlignedValU32(InitConfig->BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE))) {
                    UINT16 TotalObjNum = InitConfig->MaxLineNum + InitConfig->MaxRectNum + InitConfig->MaxCircleNum + InitConfig->MaxBmpNum + InitConfig->MaxStringNum + InitConfig->MaxPolyNum + InitConfig->MaxCurveNum;
                    if (TotalObjNum <= AMBA_DRAW_OBJ_MAX_NUM) {
                        Rval = DRAW_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid sum of MaxLineNum, MaxRectNum, MaxCircleNum, MaxBmpNum, MaxStringNum, MaxPolyNum", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_Init_ParamCheck: [ERROR] Invalid argument BufferSize %p", InitConfig->BufferSize, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_Init_ParamCheck: [ERROR] Invalid argument BufferAddr %p", (UINT32)BufferAddrUL, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument FontAddr", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument FontBufSize", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_Init(const AMBA_DRAW_INIT_CONFIG_s *InitConfig)
{
    UINT32 Rval;

    Rval = AmbaDraw_Init_ParamCheck(InitConfig);
    if (Rval == DRAW_OK) {
        static UINT8 InitFlag = 0U;
        if (InitFlag == 1U) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Already init", __func__, NULL, NULL, NULL, NULL);
            Rval = DRAW_ERROR_GENERAL_ERROR;
        } else {
            Rval = AmbaDraw_CreateMutex();
            if (Rval == DRAW_OK) {
                Rval = AmbaDraw_Lock();
                if (Rval == DRAW_OK) {
                    Rval = AmbaDrawMain_Init_Impl(InitConfig);
                    if (Rval == DRAW_OK) {
                        InitFlag = 1U;
                    }

                    if (AmbaDraw_Unlock() != DRAW_OK) {
                        Rval = DRAW_ERROR_OS;
                    }
                }
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_GetInitDefaultCfg_ParamCheck(const AMBA_DRAW_INIT_CONFIG_s *InitConfig)
{
    UINT32 Rval = DRAW_OK;
    if (InitConfig == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument InitConfig", __func__, NULL, NULL, NULL, NULL);
        Rval = DRAW_ERROR_ARG;
    }

    return Rval;
}

/**
 *  Get AmbaDraw Initial config
 *
 *  @param [out] InitConfig         The structure pointer of initial config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_GetInitDefaultCfg(AMBA_DRAW_INIT_CONFIG_s *InitConfig)
{
    UINT32 Rval;
    Rval = AmbaDraw_GetInitDefaultCfg_ParamCheck(InitConfig);
    if (Rval == DRAW_OK) {
        InitConfig->FontBufSize = 0U;
        InitConfig->FontAddr = NULL;
        InitConfig->BufferAddr = NULL;
        InitConfig->BufferSize = 0U;

        InitConfig->MaxLineNum = 1U;
        InitConfig->MaxRectNum = 1U;
        InitConfig->MaxCircleNum = 1U;
        InitConfig->MaxBmpNum = 1U;
        InitConfig->MaxStringNum = 1U;
        InitConfig->MaxPolyNum = 1U;
        InitConfig->MaxCurveNum = 1U;

        InitConfig->MaxFontPageNum = AMBA_DRAW_DEFAULT_PAGE_NUM;
    }

    return Rval;
}

static UINT32 AmbaDraw_GetInitBufferSize_ParamCheck(const AMBA_DRAW_INIT_CONFIG_s *InitConfig, const UINT32 *BufferSize)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (BufferSize != NULL) {
        if (InitConfig != NULL) {
            UINT16 TotalObjNum = InitConfig->MaxLineNum + InitConfig->MaxRectNum + InitConfig->MaxCircleNum + InitConfig->MaxBmpNum + InitConfig->MaxStringNum + InitConfig->MaxPolyNum + InitConfig->MaxCurveNum;
            if (TotalObjNum <= AMBA_DRAW_OBJ_MAX_NUM) {
                Rval = DRAW_OK;
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_GetInitBufferSize_ParamCheck: [ERROR] TotalObjNum %u out of range (%u)", TotalObjNum, AMBA_DRAW_OBJ_MAX_NUM, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] InitConfig is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] BufferSize is NULL", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_GetInitBufferSize(const AMBA_DRAW_INIT_CONFIG_s *InitConfig, UINT32 *BufferSize)
{
    UINT32 Rval;
    Rval = AmbaDraw_GetInitBufferSize_ParamCheck(InitConfig, BufferSize);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_GetInitBufferSize_Impl(InitConfig, BufferSize);
    }

    return Rval;
}


static UINT32 AmbaDraw_DrawBuffer_ParamCheck(UINT16 ObjId, INT32 X, INT32 Y, const AMBA_DRAW_BUFFER_INFO_s *BufferInfo, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        if (BufferInfo != NULL) {
            if (BufferInfo->PixelFormat <= AMBA_DRAW_BUFFER_YUV422_WITH_ALPHA) {
                if (BufferInfo->PixelFormat <= AMBA_DRAW_BUFFER_32BIT_ARGB_8888) {
                    if (BufferInfo->RGBAddr != NULL) {
                        Rval = DRAW_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument BufferInfo->RGBAddr", __func__, NULL, NULL, NULL, NULL);
                    }
                } else { // YUV
                    if (BufferInfo->RGBAddr != NULL) {
                        if (BufferInfo->YAddr != NULL) {
                            if (BufferInfo->UVAddr != NULL) {
                                if (BufferInfo->AlphaYAddr != NULL) {
                                    if (BufferInfo->AlphaUVAddr != NULL) {
                                        Rval = DRAW_OK;
                                    } else {
                                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument BufferInfo->AlphaUVAddr", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                   AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument BufferInfo->AlphaYAddr", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                               AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument BufferInfo->UVAddr", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                           AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument BufferInfo->YAddr", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument BufferInfo->RGBAddr", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument BufferInfo->PixelFormat", __func__, NULL, NULL, NULL, NULL);
            }

            if (Rval == DRAW_OK) {
                if (Area != NULL) {
                    if ((Area->X >= 0) &&
                        (Area->Y >= 0) &&
                        ((Area->X + (INT32)Area->Width) <= (INT32)BufferInfo->Width) &&
                        ((Area->Y + (INT32)Area->Height) <= (INT32)BufferInfo->Height)) {
                        /* OK */
                    } else {
                        AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_DrawBuffer_ParamCheck: [ERROR] Area (%d %d %d %d) out of range", Area->X, Area->Y, (INT32)Area->Width, (INT32)Area->Height, 0);
                        Rval = DRAW_ERROR_ARG;
                    }
                } else {
                    /* OK. Area can be NULL. */
                }
            }

            if (Rval == DRAW_OK) {
                if (X < (INT32)BufferInfo->Width) {
                    if (Y < (INT32)BufferInfo->Height) {
                        /* OK */
                    } else {
                        AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_DrawBuffer_ParamCheck: [ERROR] Invalid argument Y %d BufferHeight %d", Y, (INT32)BufferInfo->Height, 0, 0, 0);
                        Rval = DRAW_ERROR_ARG;
                    }
                } else {
                    AmbaPrint_ModulePrintInt5(AMBA_DRAW_PRINT_MODULE_ID, "AmbaDraw_DrawBuffer_ParamCheck: [ERROR] Invalid argument X %d BufferWidth %d", X, (INT32)BufferInfo->Width, 0, 0, 0);
                    Rval = DRAW_ERROR_ARG;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument MaxLineNum", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Draw object to buffer
 *  User needs to make sure that the PixelFormat of the object is consistent with the buffer.
 *
 *  @param [in] ObjId               Object ID
 *  @param [in] X                   X-offset (in Pixel)
 *  @param [in] Y                   Y-offset (in Pixel)
 *  @param [in] BufferInfo          The structure pointer of buffer to draw
 *  @param [in] Area                The structure pointer of draw object area (NULL = Draw the whole object)
 *  @param [in] DrawOption          The flag of draw options (Ex. AMBA_DRAW_OPTION_ALPHA_BLENDING)
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_DrawBuffer(UINT16 ObjId, INT32 X, INT32 Y, const AMBA_DRAW_BUFFER_INFO_s *BufferInfo, const AMBA_DRAW_AREA_s *Area, UINT32 DrawOption)
{
    UINT32 Rval;

    Rval = AmbaDraw_DrawBuffer_ParamCheck(ObjId, X, Y, BufferInfo, Area);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_DrawBuffer_Impl(ObjId, X, Y, BufferInfo, Area, DrawOption);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_GetObjArea_ParamCheck(UINT16 ObjId, const AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        if (Area != NULL) {
            Rval = DRAW_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Area is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

/**
 *  Get object area when drawing at the position (X, Y)
 *
 *  @param [in]  ObjId              Object ID
 *  @param [in]  X                  X-offset (in Pixel)
 *  @param [in]  Y                  Y-offset (in Pixel)
 *  @param [out] Area               Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_GetObjArea(UINT16 ObjId, INT32 X, INT32 Y, AMBA_DRAW_AREA_s *Area)
{
    UINT32 Rval;

    Rval = AmbaDraw_GetObjArea_ParamCheck(ObjId, Area);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_GetObjArea_Impl(ObjId, X, Y, Area);

            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_CreateLine_ParamCheck(const AMBA_DRAW_LINE_CFG_s *ObjCfg, const UINT16 *ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId != NULL) {
        Rval = AmbaDrawLine_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawLine_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_CreateLine(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_CreateLine_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_CreateLine_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_CreateRect_ParamCheck(const AMBA_DRAW_RECT_CFG_s *ObjCfg, const UINT16 *ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId != NULL) {
        Rval = AmbaDrawRect_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRect_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_CreateRect(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_CreateRect_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_CreateRect_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_CreateCircle_ParamCheck(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, const UINT16 *ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId != NULL) {
        Rval = AmbaDrawCirc_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCirc_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_CreateCircle(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_CreateCircle_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_CreateCircle_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_CreateBMP_ParamCheck(const AMBA_DRAW_BMP_CFG_s *ObjCfg, const UINT16 *ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId != NULL) {
        if (ObjCfg != NULL) {
            if (ObjCfg->Width > 0U) {
                if (ObjCfg->Height > 0U) {
                    if (ObjCfg->Data != NULL) {
                        Rval = DRAW_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg->Data", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg->Height", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg->Width", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_CreateBMP(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_CreateBMP_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_CreateBMP_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_CreateString_ParamCheck(const AMBA_DRAW_STRING_CFG_s *ObjCfg, const UINT16 *ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId != NULL) {
        Rval = AmbaDrawStr_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawStr_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_CreateString(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_CreateString_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_CreateString_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_CreatePoly_ParamCheck(const AMBA_DRAW_POLY_CFG_s *ObjCfg, const UINT16 *ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId != NULL) {
        Rval = AmbaDrawPoly_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawPoly_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_CreatePoly(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_CreatePoly_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_CreatePoly_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_CreateCurve_ParamCheck(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, const UINT16 *ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId != NULL) {
        Rval = AmbaDrawCurve_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCurve_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_CreateCurve(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 *ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_CreateCurve_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDrawMain_CreateCurve_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_UpdateLine_ParamCheck(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        Rval = AmbaDrawLine_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawLine_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_UpdateLine(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_UpdateLine_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_UpdateLine_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_UpdateRect_ParamCheck(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        Rval = AmbaDrawRect_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawRect_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_UpdateRect(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_UpdateRect_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_UpdateRect_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_UpdateCircle_ParamCheck(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        Rval = AmbaDrawCirc_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCirc_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_UpdateCircle(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_UpdateCircle_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_UpdateCircle_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_UpdateBMP_ParamCheck(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        if (ObjCfg != NULL) {
            if (ObjCfg->Width > 0U) {
                if (ObjCfg->Height > 0U) {
                    if (ObjCfg->Data != NULL) {
                        Rval = DRAW_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg->Data", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg->Height", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg->Width", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjCfg", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_UpdateBMP(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_UpdateBMP_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
           Rval = AmbaDraw_UpdateBMP_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_UpdateString_ParamCheck(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        Rval = AmbaDrawStr_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawStr_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_UpdateString(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_UpdateString_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
           Rval = AmbaDraw_UpdateString_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_UpdatePoly_ParamCheck(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        Rval = AmbaDrawPoly_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawPoly_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_UpdatePoly(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_UpdatePoly_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_UpdatePoly_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaDraw_UpdateCurve_ParamCheck(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval = DRAW_ERROR_ARG;

    if (ObjId < AMBA_DRAW_OBJ_MAX_NUM) {
        Rval = AmbaDrawCurve_IsValidConfig(ObjCfg);
        if (Rval != DRAW_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] AmbaDrawCurve_IsValidConfig failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DRAW_PRINT_MODULE_ID, "%s: [ERROR] Invalid argument ObjId", __func__, NULL, NULL, NULL, NULL);
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
UINT32 AmbaDraw_UpdateCurve(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 ObjId)
{
    UINT32 Rval;

    Rval = AmbaDraw_UpdateCurve_ParamCheck(ObjCfg, ObjId);
    if (Rval == DRAW_OK) {
        Rval = AmbaDraw_Lock();
        if (Rval == DRAW_OK) {
            Rval = AmbaDraw_UpdateCurve_Impl(ObjCfg, ObjId);
            if (AmbaDraw_Unlock() != DRAW_OK) {
                Rval = DRAW_ERROR_OS;
            }
        }
    }

    return Rval;
}

