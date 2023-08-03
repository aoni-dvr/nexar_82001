/**
 * @file AmbaDraw_BMP.h
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

#ifndef AMBADRAW_BMP_H
#define AMBADRAW_BMP_H

/*************************************************************************
 * BMP Enums
 ************************************************************************/

/*************************************************************************
 * BMP Structures
 ************************************************************************/

/**
 * BMP object description
 */
typedef struct {
    AMBA_DRAW_BMP_CFG_s ObjCfg;             /**< Object config                          */
    INT32 OffsetX;                          /**< X offset                               */
    INT32 OffsetY;                          /**< Y offset                               */
} AMBA_DRAW_BMP_DESC_s;

/*************************************************************************
 * BMP Functions
 ************************************************************************/

/**
 *  Get object area
 *
 *  @param [in]  ObjDesc            The object description
 *  @param [out] ObjArea            Object area
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawBMP_GetObjArea(const AMBA_DRAW_BMP_DESC_s *ObjDesc, AMBA_DRAW_AREA_s *ObjArea);

/**
 *  Draw the BMP object
 *
 *  @param [in]  Render             The draw render
 *  @param [in]  ObjDesc            The object description
 *  @param [in]  Area               The draw area of buffer. The parameter of AmbaDrawMain_DrawBuffer.
 *
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaDrawBMP_Draw(const AMBA_DRAW_RENDER_s *Render, const AMBA_DRAW_BMP_DESC_s *ObjDesc, const AMBA_DRAW_AREA_s *Area);

#endif /* AMBADRAW_BMP_H */

