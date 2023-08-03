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
#ifndef CVAPI_FLEXIDAG_AMBAFMA_H
#define CVAPI_FLEXIDAG_AMBAFMA_H

#include "cvapi_flexidag_ambaspufex_cv2.h"

#define CV_FMA_DISABLE                 0U
#define CV_FMA_STEREO_MATCH            1U
#define CV_FMA_TEMPORAL_MATCH          2U
#define CV_FMA_ENABLE_ALL (CV_FMA_STEREO_MATCH|CV_FMA_TEMPORAL_MATCH)


//config
typedef struct {
    UINT8 Mode;                             /**< Stereo (CV_FMA_STEREO_MATCH), Temporal (monocular) mode(CV_FMA_TEMPORAL_MATCH), or Both (CV_FMA_TEMPORAL_MATCH|CV_FMA_STEREO_MATCH) */
} AMBA_CV_FMA_SCALE_CFG_s;

typedef struct {
    UINT16 MatchScoreTh;
    UINT16 SearchRadiusX;
    UINT16 SearchRadiusY;
    UINT16 SearchDistanceThXLeft;
    UINT16 SearchDistanceThXRight;
    UINT16 SearchDistanceThYUp;
    UINT16 SearchDistanceThYDown;
    UINT16 RatioTh;
} AMBA_CV_FMA_MATCH_CFG_s;

typedef struct {
    UINT8 Mode;                             /**< Stereo (CV_FMA_STEREO_MATCH), Temporal (monocular) mode(CV_FMA_TEMPORAL_MATCH), or Both (CV_FMA_TEMPORAL_MATCH|CV_FMA_STEREO_MATCH) */
    AMBA_CV_FMA_MATCH_CFG_s TemporalMatchCfg;
    AMBA_CV_FMA_MATCH_CFG_s StereoMatchCfg;
} AMBA_CV_FMA_SCALE_CFG_V1_s;

typedef struct {
    AMBA_CV_FMA_SCALE_CFG_s         ScaleCfg[MAX_HALF_OCTAVES];
} AMBA_CV_FMA_CFG_s;

typedef struct {
    AMBA_CV_FMA_SCALE_CFG_V1_s         ScaleCfg[MAX_HALF_OCTAVES];
} AMBA_CV_FMA_CFG_V1_s;

typedef struct {
    UINT16 SearchRadiusX;     //adjust to decrease runtime
    UINT16 SearchRadiusY;     // adjust to decrease runtime
    FLOAT DistanceThUp;      // to deal with pitch/roll rotation. unit: pixel
    FLOAT DistanceThDown;    // to deal with pitch/roll rotation. unit: pixel
    UINT16 MatchScoreTh;     //adjust to decrease runtime
} AMBA_CV_FMA_STEREO_MATCH_CFG_s;

typedef struct {
    UINT16 SearchRadiusX;     //adjust to decrease runtime
    UINT16 SearchRadiusY;     // adjust to decrease runtime
    UINT16 MatchScoreTh;      //adjust to decrease runtime
} AMBA_CV_FMA_TEMPORAL_MATCH_CFG_s;

typedef struct {
    UINT8 Mode;                            /**< Stereo (M_FMA_STEREO_MATCH), temporal (monocular) mode(M_FMA_TEMPORAL_MATCH), or both (M_FMA_TEMPORAL_MATCH|M_FMA_STEREO_MATCH), or (FMA_DISABLE) */
    AMBA_CV_FMA_STEREO_MATCH_CFG_s StereoMatch;
    AMBA_CV_FMA_TEMPORAL_MATCH_CFG_s TemporalMatch;
} AMBA_CV_FMA_MVAC_SCALE_CFG_s;

typedef struct {
    AMBA_CV_FMA_MVAC_SCALE_CFG_s      ScaleCfg[MAX_HALF_OCTAVES];
} AMBA_CV_FMA_MVAC_CFG_s;

//output
typedef struct {
    UINT32             Valid;              /**< Valid flag to indicate if the result is valid. */
    relative_ptr_t     MpScore;            /**< The pointer to the score data (uint8). */
    relative_ptr_t     MpIndex;            /**< The pointer to the index data (uint16). */
} AMBA_CV_MATCH_s;

typedef struct {
    UINT32                      Reserved_0;
    UINT32                      NumHalfOctaves;                       /**< number of valid outputs */
    AMBA_CV_FEX_FEATURE_LIST_s  FeatureList[MAX_HALF_OCTAVES];        /**< Current updated right(PRIMARY) */
    AMBA_CV_MATCH_s             StereoMatch[MAX_HALF_OCTAVES];        /**< Stereo output      */
    AMBA_CV_MATCH_s             TemporalMatch[MAX_HALF_OCTAVES];      /**< Temporal output    */
    UINT32                      Reserved_1;
} AMBA_CV_FMA_DATA_s;

typedef struct {
    AMBA_CV_FMA_DATA_s         *pBuffer;
    ULONG                       BufferDaddr;
    UINT32                      BufferCacheable :1;
    UINT32                      BufferSize      :31;
    ULONG                       BufferCaddr;
    UINT32                      Reserved;
} AMBA_CV_FMA_BUF_s;

/**
 *  AmbaFma open function, it will give user the required buffer size. MUST run before you create each instance (Using the same buffer for the same flexidag is okay)
 *
 *  @param [in] pFlexiDagBinBuf Path to flexidag binary, buffer should allocated in GNU_SECTION_CV_RTOS_USER and align to 128 in length and start address
 *  @param [in] FlexiDagBinSz Size of flexidag binary
 *  @param [out] pStateBufSz Pointer to U32 to return state buffer size
 *  @param [out] pTempBufSz Pointer to U32 to return temp buffer size
 *  @param [out] pOutputBufNum Pointer to U32 to return output buffer number
 *  @param [out] pOutputBufSz Pointer to U32[8] to return output buffer size
 *  @param [out] pHandler Handler for AmbaFma
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Open(const UINT8* pFlexiDagBinBuf, UINT32 FlexiDagBinSz, UINT32* pStateBufSz, UINT32* pTempBufSz, UINT32* pOutputBufNum, UINT32 *pOutputBufSz, const AMBA_STEREO_FD_HANDLE_s* pHandler);

/**
 *  AmbaFma open v1 function support linux, it will give user the required buffer size. MUST run before you create each instance (Using the same buffer for the same flexidag is okay)
 *
 *  @param [in] pFlexiDagBinBuf Path to flexidag binary, buffer should allocated in GNU_SECTION_CV_RTOS_USER and align to 128 in length and start address
 *  @param [in] FlexiDagBinSz Size of flexidag binary
 *  @param [out] pStateBufSz Pointer to U32 to return state buffer size
 *  @param [out] pTempBufSz Pointer to U32 to return temp buffer size
 *  @param [out] pOutputBufNum Pointer to U32 to return output buffer number
 *  @param [out] pOutputBufSz Pointer to U32[8] to return output buffer size
 *  @param [out] pHandler Handler for AmbaFma
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Openv1(const flexidag_memblk_t* pFlexiDagBinBuf, UINT32 FlexiDagBinSz, UINT32* pStateBufSz, UINT32* pTempBufSz, UINT32* pOutputBufNum, UINT32 *pOutputBufSz, const AMBA_STEREO_FD_HANDLE_s* pHandler);


/**
 *  AmbaFma initial function
 *
 *  @param [in] pHandler Handler from open function
 *  @param [in] pStateBuf pointer to state buffer
 *  @param [in] pTempBuf pointer to temp buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Init(const AMBA_STEREO_FD_HANDLE_s* pHandler, UINT8* pStateBuf, UINT8* pTempBuf);

/**
 *  AmbaFma initial v1 function support linux
 *
 *  @param [in] pHandler Handler from open function
 *  @param [in] pStateBuf pointer to state buffer
 *  @param [in] pTempBuf pointer to temp buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Initv1(const AMBA_STEREO_FD_HANDLE_s* pHandler, const flexidag_memblk_t* pStateBuf, const flexidag_memblk_t* pTempBuf);

/**
 *  AmbaFma configuration function
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pCfg taking structure defined in AMBA_CV_FMA_CFG_V1_s
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Cfgv1(const AMBA_STEREO_FD_HANDLE_s* pHandler, const AMBA_CV_FMA_CFG_V1_s* pCfg);

/**
 *  AmbaFma configuration function
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pCfg taking structure defined in AMBA_CV_FMA_CFG_s
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Cfg(const AMBA_STEREO_FD_HANDLE_s* pHandler, const AMBA_CV_FMA_CFG_s* pCfg);

/**
 *  AmbaFma Mvac configuration function
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pCfg taking structure defined in AMBA_CV_FMA_MVAC_CFG_s
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_MvacCfg(const AMBA_STEREO_FD_HANDLE_s *pHandler, const AMBA_CV_FMA_MVAC_CFG_s* pCfg);

/**
 *  AmbaFma process function
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pFexInfo pointer to fex
 *  @param [in] FexBufSz buffer size of fex
 *  @param [in] pOutFma pointer to fma output buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Process(const AMBA_STEREO_FD_HANDLE_s* pHandler, const AMBA_CV_FEX_DATA_s* pFexInfo, UINT32 FexBufSz, const AMBA_CV_FMA_DATA_s* pOutFma);

/**
 *  AmbaFma process v1 function support linux
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pFexInfo pointer to fex
 *  @param [in] FexBufSz buffer size of fex
 *  @param [in] pOutFma pointer to fma output buffer
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Processv1(const AMBA_STEREO_FD_HANDLE_s* pHandler, const AMBA_CV_FEX_BUF_s* pFexInfo, UINT32 FexBufSz, const AMBA_CV_FMA_BUF_s* pOutFma);


/**
 *  AmbaFma get result function
 *
 *  @param [in] pHandler handler from open function
 *  @param [out] pFexInfo pointer to fex which generate the result
 *  @param [out] pOutFma pointer to output buffer (AMBA_CV_FMA_DATA_s)
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_GetResult(const AMBA_STEREO_FD_HANDLE_s* pHandler, AMBA_CV_FEX_DATA_s** pFexInfo, AMBA_CV_FMA_DATA_s** pOutFma);

/**
 *  AmbaFma get result v1 function support linux
 *
 *  @param [in] pHandler handler from open function
 *  @param [out] pFexInfo pointer to fex which generate the result
 *  @param [out] pOutFma pointer to output buffer (AMBA_CV_FMA_BUF_s)
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_GetResultv1(const AMBA_STEREO_FD_HANDLE_s* pHandler, AMBA_CV_FEX_BUF_s *pFexInfo, AMBA_CV_FMA_BUF_s *pOutFma);


/**
 *  AmbaFma dump log function
 *
 *  @param [in] pHandler handler from open function
 *  @param [in] pLogPath path to log file
 *
 * @return STEREO_OK - OK
 */
UINT32 AmbaFma_DumpLog(const AMBA_STEREO_FD_HANDLE_s* pHandler, const char* pLogPath);


/**
 *  AmbaFma close function
 *
 *  @param [in] pHandler handler from open function
 *
 *  @return STEREO_OK - OK
 */
UINT32 AmbaFma_Close(const AMBA_STEREO_FD_HANDLE_s *pHandler);


#endif //CVAPI_FLEXIDAG_AMBAFMA_H

