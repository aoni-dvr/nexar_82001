/**
 * @file AmbaDraw_Main.h
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
#ifndef AMBADRAW_MAIN_H
#define AMBADRAW_MAIN_H

#include "AmbaDraw_Common.h"

#define AMBA_DRAW_OBJ_MAX_NUM (1024U)                    /**< Max number of draw objects */

/*************************************************************************
 * Struct
 ************************************************************************/

/*************************************************************************
 * Variable
 ************************************************************************/

/*************************************************************************
 * Define
 ************************************************************************/
#define AMBA_DRAW_TIMEOUT_MUTEX  (30U * 1000U)           /**< Mutex wait timeout value */

/*************************************************************************
 * Function
 ************************************************************************/

/**
 *  Get Buffer size
 *
 *  @param [in]  InitConfig         The structure pointer of initial config
 *  @param [out] BufferSize         Required buffer size
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_GetInitBufferSize_Impl(const AMBA_DRAW_INIT_CONFIG_s *InitConfig, UINT32 *BufferSize);

/**
 *  Init AmbaDraw module
 *
 *  @param [in] InitConfig          The structure pointer of initial config
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_Init_Impl(const AMBA_DRAW_INIT_CONFIG_s *InitConfig);

/**
 *  Create new line object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object descriptor
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateLine_Impl(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 *ObjId);

/**
 *  Create new rectangle object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object descriptor
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateRect_Impl(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 *ObjId);

/**
 *  Create new circle object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object descriptor
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateCircle_Impl(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 *ObjId);

/**
 *  Create new BMP object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object descriptor
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateBMP_Impl(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 *ObjId);

/**
 *  Create new string object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object descriptor
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateString_Impl(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 *ObjId);

/**
 *  Create new polygon object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object descriptor
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreatePoly_Impl(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 *ObjId);

/**
 *  Create new curve object and get object ID
 *
 *  @param [in]  ObjCfg             The structure pointer of new object descriptor
 *  @param [out] ObjId              The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_CreateCurve_Impl(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 *ObjId);

/**
 *  Draw object to buffer
 *  User needs to make sure that the PixelFormat of the object is consistent with the buffer.
 *
 *  @param [in] ObjId               Object ID
 *  @param [in] X_offset            X-offset
 *  @param [in] Y_offset            Y-offset
 *  @param [in] BufferInfo          The structure pointer of buffer to draw
 *  @param [in] Area                The structure pointer of draw object area (NULL = Draw the whole object)
 *  @param [in] DrawOption          The flag of draw options (Ex. AMBA_DRAW_OPTION_ALPHA_BLENDING)
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawMain_DrawBuffer_Impl(UINT16 ObjId, INT32 X_offset, INT32 Y_offset, const AMBA_DRAW_BUFFER_INFO_s *BufferInfo, const  AMBA_DRAW_AREA_s *Area, UINT32 DrawOption);

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
UINT32 AmbaDrawMain_GetObjArea_Impl(UINT16 ObjId, INT32 X, INT32 Y, AMBA_DRAW_AREA_s *Area);

/**
 *  Update line object
 *
 *  @param [in] ObjCfg              The structure pointer of new object descriptor
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateLine_Impl(const AMBA_DRAW_LINE_CFG_s *ObjCfg, UINT16 ObjId);

/**
 *  Update rectangle object
 *
 *  @param [in] ObjCfg              The structure pointer of new object descriptor
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateRect_Impl(const AMBA_DRAW_RECT_CFG_s *ObjCfg, UINT16 ObjId);

/**
 *  Update circle object
 *
 *  @param [in] ObjCfg              The structure pointer of new object descriptor
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateCircle_Impl(const AMBA_DRAW_CIRCLE_CFG_s *ObjCfg, UINT16 ObjId);

/**
 *  Update BMP object
 *
 *  @param [in] ObjCfg              The structure pointer of new object config
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateBMP_Impl(const AMBA_DRAW_BMP_CFG_s *ObjCfg, UINT16 ObjId);

/**
 *  Update string object
 *
 *  @param [in] ObjCfg              The structure pointer of new object descriptor
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateString_Impl(const AMBA_DRAW_STRING_CFG_s *ObjCfg, UINT16 ObjId);

/**
 *  Update polygon object
 *
 *  @param [in] ObjCfg              The structure pointer of new object descriptor
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdatePoly_Impl(const AMBA_DRAW_POLY_CFG_s *ObjCfg, UINT16 ObjId);

/**
 *  Update curve object
 *
 *  @param [in] ObjCfg              The structure pointer of new object descriptor
 *  @param [in] ObjId               The object ID
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDraw_UpdateCurve_Impl(const AMBA_DRAW_CURVE_CFG_s *ObjCfg, UINT16 ObjId);


#endif // AMBADRAW_MAIN_H
