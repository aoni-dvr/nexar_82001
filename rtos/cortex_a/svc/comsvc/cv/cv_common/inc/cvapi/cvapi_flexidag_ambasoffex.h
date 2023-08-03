/**
 * @file cvapi_flexidag_ambasoffex.h
 */
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
*/
#ifndef CVAPI_FLEXIDAG_AMBASOFFEX_H
#define CVAPI_FLEXIDAG_AMBASOFFEX_H

#include "cvapi_flexidag_ref_util.h"
#include "cvapi_flexidag_ambasoffex_def.h"


/**
 *  AmbaSofFex initial function
 *
 *  @param [in] pHandler allocated SofFex handler
 *
 *  @return 0 - OK, 1 - NG
 */
UINT32 AmbaSofFex_InitSofHdlr(SOF_FEX_FD_HANDLE_s *pHandler);


/**
 *  AmbaSofFex configuration function
 *
 *  @param [in] pHandler allocated and initialized SofFex handler
 *  @param [in] pCfg taking structure defined in AMBA_CV_SPUFEX_CFG_s
 *
 *  @return 0 - OK, 1 - NG
 */
UINT32 AmbaSofFex_Cfg(SOF_FEX_FD_HANDLE_s *pHandler, const AMBA_CV_SPUFEX_CFG_s *pCfg);


/**
 *  AmbaSofFex process function
 *
 *  @param [in] pHandler allocated and initialized SofFex handler
 *  @param [in] pImgInfo pointer to image pyramid
 *  @param [in] pOutSof pointer to of output buffer
 *  @param [in] pOutFex pointer to fex output buffer
 *
 *  @return 0 - OK, 1 - NG
 */
UINT32 AmbaSofFex_Process(SOF_FEX_FD_HANDLE_s *pHandler, const AMBA_CV_IMG_BUF_s *pImgInfo,
                          AMBA_CV_SPU_BUF_s *pOutSof, AMBA_CV_FEX_BUF_s *pOutFex);


/**
 *  AmbaSofFex get result function
 *
 *  @param [in] pHandler allocated and initialized SofFex handler
 *  @param [in] pImgInfo pointer to image pyramid which generate the result
 *  @param [out] pOutSof pointer to output buffer (AMBA_CV_SPU_BUF_s)
 *  @param [out] pOutFex pointer to output buffe (AMBA_CV_FEX_BUF_s)
 *
 *  @return 0 - OK, 1 - NG
 */
UINT32 AmbaSofFex_GetResult(SOF_FEX_FD_HANDLE_s *pHandler, AMBA_CV_IMG_BUF_s *pImgInfo,
                            AMBA_CV_SPU_BUF_s *pOutSof, AMBA_CV_FEX_BUF_s *pOutFex);


/**
 *  AmbaSofFex registration function
 *
 *  @param [in] pHarrisHdlr opened and initialized harris corner detection handler
 *  @param [in] pLKHdlr opened and initialized LK optical flow handler
 *  @param [in] ProcWidth processed width of registered handlers
 *  @param [in] ProcHeight processed height of registered handlers
 *  @param [in] InImagePitch pitch of processed image
 *  @param [in] pSofFexHdlr allocated and initialized SofFex handler
 *
 *  @return 0 - OK, 1 - NG
 */
UINT32 AmbaSofFex_RegHarrisLKHdlr(REF_FD_HANDLE_s *const pHarrisHdlr,
                                  REF_FD_HANDLE_s *const pLKHdlr,
                                  UINT16 ProcWidth, UINT16 ProcHeight, UINT32 InImagePitch,
                                  SOF_FEX_FD_HANDLE_s *pSofFexHdlr);

#endif //CVAPI_FLEXIDAG_AMBASOFFEX_H
