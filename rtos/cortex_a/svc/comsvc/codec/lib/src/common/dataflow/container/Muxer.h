/**
 * @file Muxer.h
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
#ifndef CONTAINER_MUXER_H
#define CONTAINER_MUXER_H

#include "format/SvcFormatDef.h"
#include "format/SvcFormat.h"
#include "stream/SvcStream.h"

#define MUXER_OK                OK              /**< Execution OK */
#define MUXER_ERR_INVALID_ARG   MUXER_ERR_0000  /**< Invalid argument */
#define MUXER_ERR_FATAL_ERROR   MUXER_ERR_0001  /**< Fatal error */
#define MUXER_ERR_IO_ERROR      MUXER_ERR_0002  /**< IO error */

#define SVC_MUXER_PRINT_MODULE_ID        ((UINT16)(MUXER_ERR_BASE >> 16U))     /**< Module ID for AmbaPrint */

static inline UINT32 W2M(UINT32 Ret)
{
    UINT32 Rval = MUXER_OK;
    if (Ret != OK) {
        Rval = MUXER_ERR_FATAL_ERROR;
    }
    return Rval;
}

static inline UINT32 K2M(UINT32 Ret)
{
    UINT32 Rval = MUXER_OK;
    if (Ret != OK) {
        Rval = MUXER_ERR_FATAL_ERROR;
    }
    return Rval;
}

static inline UINT32 S2M(UINT32 Ret)
{
    UINT32 Rval = MUXER_OK;
    if (Ret != OK) {
        if (Ret == STREAM_ERR_0000) {
            Rval = MUXER_ERR_INVALID_ARG;
        } else if (Ret == STREAM_ERR_0002) {
            Rval = MUXER_ERR_IO_ERROR;
        } else {
            Rval = MUXER_ERR_FATAL_ERROR;
        }
    }
    return Rval;
}

/**
 *  Iniitialize the muxing format handler for a movie object.
 *
 *  @param [out] Format Muxing format handler
 *  @param [in] Func The interface of muxing format
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
void SvcMuxer_InitMovieMuxHdlr(SVC_MUX_FORMAT_HDLR_s *Format, SVC_MUX_FORMAT_s *Func, SVC_STREAM_HDLR_s *Stream);
void SvcMuxer_InitImageMuxHdlr(SVC_MUX_FORMAT_HDLR_s *Format, SVC_MUX_FORMAT_s *Func, SVC_STREAM_HDLR_s *Stream);

#endif

