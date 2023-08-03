/*
* Copyright (c) 2020 Ambarella International LP
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
*/
#ifndef CVAPI_FLEXIDAG_AMBASPUFUSION_H
#define CVAPI_FLEXIDAG_AMBASPUFUSION_H
/**
 * @defgroup API_VisionDriver filter api
 * @brief 
 */

/**
 * @addtogroup API_VisionDriver api for vision driver control
 * @{
 */

#include "cvapi_idsp_interface.h"
#include "cvapi_flexidag_ambaspufex_cv2.h"

/**
 *  AmbaSpuFusion open function, it will give user the required buffer size. MUST run before you create each instance (Using the same buffer for the same flexidag is okay)
 *
 *  @param [in] pFlexiDagBinBuf Path to flexidag binary, buffer should allocated in GNU_SECTION_CV_RTOS_USER and align to 128 in length and start address
 *  @param [in] FlexiDagBinSz Size of flexidag binary
 *  @param [out] pStateBufSz Pointer to U32 to return state buffer size
 *  @param [out] pTempBufSz Pointer to U32 to return temp buffer size
 *  @param [out] pOutputBufNum Pointer to U32 to return output buffer number
 *  @param [out] pOutputBufSz Pointer to U32[8] to return output buffer size
 *  @param [out] pHandler Handler for AmbaSpuFusion
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_Open(const UINT8 *pFlexiDagBinBuf, UINT32 FlexiDagBinSz,
                          UINT32 *pStateBufSz, UINT32 *pTempBufSz, UINT32 *pOutputBufNum, UINT32 *pOutputBufSz,
                          const AMBA_STEREO_FD_HANDLE_s *pHandler);

/**
 *  AmbaSpuFusion open v1 function support linux, it will give user the required buffer size. MUST run before you create each instance (Using the same buffer for the same flexidag is okay)
 *
 *  @param [in] pFlexiDagBinBuf Path to flexidag binary, buffer should allocated in GNU_SECTION_CV_RTOS_USER and align to 128 in length and start address
 *  @param [in] FlexiDagBinSz Size of flexidag binary
 *  @param [out] pStateBufSz Pointer to U32 to return state buffer size
 *  @param [out] pTempBufSz Pointer to U32 to return temp buffer size
 *  @param [out] pOutputBufNum Pointer to U32 to return output buffer number
 *  @param [out] pOutputBufSz Pointer to U32[8] to return output buffer size
 *  @param [out] pHandler Handler for AmbaSpuFusion
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_Openv1(const flexidag_memblk_t *pFlexiDagBinBuf, UINT32 FlexiDagBinSz,
                            UINT32 *pStateBufSz, UINT32 *pTempBufSz, UINT32 *pOutputBufNum, UINT32 *pOutputBufSz,
                            const AMBA_STEREO_FD_HANDLE_s *pHandler);


/**
 *  AmbaSpuFusion 3 layer open function, it will give user the required buffer size. MUST run before you create each instance (Using the same buffer for the same flexidag is okay)
 *
 *  @param [in] pFlexiDagBinBuf Path to flexidag binary, buffer should allocated in GNU_SECTION_CV_RTOS_USER and align to 128 in length and start address
 *  @param [in] FlexiDagBinSz Size of flexidag binary
 *  @param [out] pStateBufSz Pointer to U32 to return state buffer size
 *  @param [out] pTempBufSz Pointer to U32 to return temp buffer size
 *  @param [out] pOutputBufNum Pointer to U32 to return output buffer number
 *  @param [out] pOutputBufSz Pointer to U32[8] to return output buffer size
 *  @param [out] pHandler Handler for AmbaSpuFusion
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion3Layer_Open(const UINT8 *pFlexiDagBinBuf, UINT32 FlexiDagBinSz,
                                UINT32 *pStateBufSz, UINT32 *pTempBufSz, UINT32 *pOutputBufNum, UINT32 *pOutputBufSz,
                                const AMBA_STEREO_FD_HANDLE_s *pHandler);

/**
 *  AmbaSpuFusion 3 layer open v1 function support linux, it will give user the required buffer size. MUST run before you create each instance (Using the same buffer for the same flexidag is okay)
 *
 *  @param [in] pFlexiDagBinBuf Path to flexidag binary, buffer should allocated in GNU_SECTION_CV_RTOS_USER and align to 128 in length and start address
 *  @param [in] FlexiDagBinSz Size of flexidag binary
 *  @param [out] pStateBufSz Pointer to U32 to return state buffer size
 *  @param [out] pTempBufSz Pointer to U32 to return temp buffer size
 *  @param [out] pOutputBufNum Pointer to U32 to return output buffer number
 *  @param [out] pOutputBufSz Pointer to U32[8] to return output buffer size
 *  @param [out] pHandler Handler for AmbaSpuFusion
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion3Layer_Openv1(const flexidag_memblk_t *pFlexiDagBinBuf, UINT32 FlexiDagBinSz,
                                  UINT32 *pStateBufSz, UINT32 *pTempBufSz, UINT32 *pOutputBufNum, UINT32 *pOutputBufSz,
                                  const AMBA_STEREO_FD_HANDLE_s *pHandler);


/**
 *  AmbaSpuFusion initial function
 *
 *  @param [in] pHandler Handler from open function
 *  @param [in] pStateBuf pointer to state buffer
 *  @param [in] pTempBuf pointer to temp buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_Init(const AMBA_STEREO_FD_HANDLE_s* pHandler, UINT8* pStateBuf, UINT8* pTempBuf);

/**
 *  AmbaSpuFusion initial v1 function support linux
 *
 *  @param [in] pHandler Handler from open function
 *  @param [in] pStateBuf pointer to state buffer
 *  @param [in] pTempBuf pointer to temp buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_Initv1(const AMBA_STEREO_FD_HANDLE_s* pHandler, const flexidag_memblk_t* pStateBuf, const flexidag_memblk_t* pTempBuf);


/**
 *  AmbaSpuFusion process function
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pDisparity pointer to image disparity
 *  @param [in] pImgInfo pointer to image pyramid
 *  @param [in] pOutFusion pointer to fusion output buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_Process(const AMBA_STEREO_FD_HANDLE_s *pHandler,
                             const AMBA_CV_SPU_DATA_s* pDisparity, const memio_source_recv_picinfo_t* pImgInfo, UINT8 *pOutFusion);

/**
 *  AmbaSpuFusion process v1 function support linux
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pDisparity pointer to image disparity
 *  @param [in] pImgInfo pointer to image pyramid
 *  @param [in] pOutFusion pointer to fusion output buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_Processv1(const AMBA_STEREO_FD_HANDLE_s *pHandler,
                               const AMBA_CV_SPU_BUF_s *pDisparity, const AMBA_CV_IMG_BUF_s *pImgInfo, const AMBA_CV_SPU_BUF_s *pOutFusion);


/**
 *  AmbaSpuFusion get result function
 *
 *  @param [in] pHandler handler from open function
 *  @param [out] pDisparity pointer to disparity pyramid which generate the result
 *  @param [out] pImgInfo pointer to image pyramid which generate the result
 *  @param [out] pOutFusion pointer to output buffer (AMBA_CV_SPU_DATA_s)
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_GetResult(const AMBA_STEREO_FD_HANDLE_s *pHandler, AMBA_CV_SPU_DATA_s **pDisparity,
                               memio_source_recv_picinfo_t **pImgInfo, AMBA_CV_SPU_DATA_s **pOutFusion);

/**
 *  AmbaSpuFusion get result v1 function for linux
 *
 *  @param [in] pHandler handler from open function
 *  @param [out] pDisparity pointer to disparity pyramid which generate the result
 *  @param [out] pImgInfo pointer to image pyramid which generate the result
 *  @param [out] pOutFusion pointer to output buffer (AMBA_CV_SPU_BUF_s)
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_GetResultv1(const AMBA_STEREO_FD_HANDLE_s *pHandler, AMBA_CV_SPU_BUF_s *pDisparity,
                                 AMBA_CV_IMG_BUF_s *pImgInfo, AMBA_CV_SPU_BUF_s *pOutFusion);


/**
 * AmbaSpuFusion process function
 *
 * @param pHandler - (IN) handler from init function
 * @param pLogPath - (OUT) path to log file
 * @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_DumpLog(const AMBA_STEREO_FD_HANDLE_s* pHandler, const char* pLogPath);

/**
 * AmbaSpuFusion close function
 *
 * @param pHandler - (IN) handler from init function
 * @return STEREO_OK - OK
 */
UINT32 AmbaSpuFusion_Close(const AMBA_STEREO_FD_HANDLE_s *pHandler);

/**
 * @}
 */
#endif //CVAPI_FLEXIDAG_AMBASPUFUSION_H