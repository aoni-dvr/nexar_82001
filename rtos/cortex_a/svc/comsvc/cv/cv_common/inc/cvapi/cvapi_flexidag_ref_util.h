/**
 *  @file cvapi_flexidag_simple_pipeline.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of Cv simple pipeline
 *
 */

#ifndef CVAPI_FLEXIDAG_REF_UTIL_H
#define CVAPI_FLEXIDAG_REF_UTIL_H

#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "AmbaKAL.h"


#define MAX_DEPTH       (4U)

typedef struct {
    UINT32                  Token;
    UINT32                  Used;
    AMBA_CV_FLEXIDAG_IO_s   Input;
    AMBA_CV_FLEXIDAG_IO_s   Output;
} FD_IO_QUEUE_s;

typedef struct {
    UINT32                          State;                //0-pre-init 1-ready
    AMBA_CV_FLEXIDAG_HANDLE_s       FDGenHandle;
    AMBA_CV_FLEXIDAG_INIT_s         InitData;
    UINT32                          InitCfg;
    flexidag_memblk_t               FlexidagBinBuf;
    FD_IO_QUEUE_s                   IOQueue[MAX_DEPTH];
    AMBA_KAL_MUTEX_t                IOQueueMutex;
    UINT32                          IOTokenMsgSpace[MAX_DEPTH];
    AMBA_KAL_MSG_QUEUE_t            MqueueToRslt;
} REF_FD_HANDLE_s;

/**
 * Open a REF FD handler
 *
 * @param FlexiDagBinBuf - (IN) pointer to a handler to init
 * @param FlexiDagBinSz - (IN) state buffer with size from RefCVUtil_FlexiDagOpen
 * @param StateBufSz - (OUT) required state buffer size
 * @param TempBufSz - (OUT) required temp buffer size
 * @param OutputBufNum - (OUT) output buffer number
 * @param OutputBufSz - (IN)(OUT) pointer to a UINT32[8], will output output buffer size for each output
 * @Param Handler - (IN)(OUT) pointer to a Handler to open, will ouput opened handler.
 */
UINT32 RefCVUtil_FlexiDagOpen(const UINT8* FlexiDagBinBuf, UINT32 FlexiDagBinSz, UINT32* StateBufSz, UINT32* TempBufSz, UINT32* OutputBufNum, UINT32 *OutputBufSz, REF_FD_HANDLE_s* Handler);

/**
 * Init a REF FD handler
 *
 * @param Handler - (IN) pointer to a handler to init
 * @param StateBuf - (IN) state buffer with size from RefCVUtil_FlexiDagOpen
 * @param TempBuf - (IN) Temp buffer with size from RefCVUtil_FlexiDagOpen
 */
UINT32 RefCVUtil_FlexiDagInit(REF_FD_HANDLE_s* Handler, UINT8* StateBuf, UINT8* TempBuf);

/**
 * Process a flexidag, non-blocking function
 *
 * @param Handler - (IN) pointer to a handler to process
 * @param In - (IN) pointer array of to input buffer address which store as void*.
 * @param InSz - (IN) pointer array of to input buffer size.
 * @param InNum - (IN) number of input buffer.
 * @param Out - (IN) pointer array of to output buffer address which store as void*.
 * @param OutNum - (IN) number of output buffer.
 */
UINT32 RefCVUtil_FlexiDagProc(REF_FD_HANDLE_s* Handler, void** In, const UINT32* InSz, UINT32 InNum, void **Out, UINT32 OutNum);
/**
 * Get result from a flexidag blocking function
 *
 * @param Handler - (IN) pointer to a handler to process
 * @param In - (IN)(OUT) input that use to generate the output
 * @param Out - (IN)(OUT) outptu of flexidag
 */

UINT32 RefCVUtil_FlexiDagGetResult(REF_FD_HANDLE_s* Handler, AMBA_CV_FLEXIDAG_IO_s* In, AMBA_CV_FLEXIDAG_IO_s* Out);

#endif //CVAPI_FLEXIDAG_REF_UTIL_H
