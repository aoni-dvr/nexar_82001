/**
 *  @file RefDcf_Util.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 *  @details Header File of DCF Utility Functions
 *
 */
#ifndef REF_DCF_UTIL_H
#define REF_DCF_UTIL_H

#include <AmbaDef.h>
#include "RefDcf.h"
#include <AmbaPrint.h>
#include <AmbaWrap.h>

#define DCF_OK                      (0x0U)  /**< Execution OK */
#define DCF_ERR_INVALID_ARG         DCF_ERR_0000
#define DCF_ERR_GENERAL_ERROR       DCF_ERR_0001
#define DCF_ERR_IO_ERROR            DCF_ERR_0002
#define DCF_ERR_OBJECT_NOT_EXISTED  DCF_ERR_0003
#define DCF_ERR_OBJECT_EXISTS       DCF_ERR_0004
#define DCF_ERR_NOT_READY           DCF_ERR_0005


//#define DCF_TABLE_DEBUG (0U)

#define ID_NOT_EXISTED      (0U)

static inline UINT32 K2D(UINT32 Ret)
{
    UINT32 Rval = DCF_OK;
    if (Ret != OK) {
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

#endif /* REF_DCF_UTIL_H */

