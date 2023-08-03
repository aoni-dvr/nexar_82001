/**
 * @file FlexidagAmbaSofFex.c
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
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ambasoffex.h"
#ifdef CONFIG_BUILD_CV_THREADX
#include "AmbaTypes.h"
#include "AmbaFS.h"
#include <AmbaPrint.h>
#include <AmbaMisraFix.h>
#include <AmbaWrap.h>
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "AmbaCache.h"
#endif

inline static UINT32 ValidPointerCheck(const void* pointer)
{
    UINT32 Rval = 0U;
    if ( pointer == NULL ) {
        Rval = 1U;
    }
    return Rval;
}


/**
 *  AmbaSofFex initial function
 *
 *  @param [in] pHandler allocated SofFex handler
 *
 *  @return 0 - OK, 1 - NG
 */
UINT32 AmbaSofFex_InitSofHdlr(SOF_FEX_FD_HANDLE_s *pHandler)
{
    UINT32 Rval = 0U;

    Rval |= ValidPointerCheck(pHandler);

    if ( Rval == 0U ) {
        pHandler->RegisterHdlrNum = 0U;
        Rval |= SofFex_SetWorkBuffAligned(pHandler);
    }

    return Rval;
}


/**
 *  AmbaSofFex configuration function
 *
 *  @param [in] pHandler allocated and initialized SofFex handler
 *  @param [in] pCfg taking structure defined in AMBA_CV_SPUFEX_CFG_s
 *
 *  @return 0 - OK, 1 - NG
 */
UINT32 AmbaSofFex_Cfg(SOF_FEX_FD_HANDLE_s *pHandler, const AMBA_CV_SPUFEX_CFG_s *pCfg)
{
    UINT32 Rval = 0U;

    Rval |= ValidPointerCheck(pHandler);
    Rval |= ValidPointerCheck(pCfg);

    if ( Rval == 0U ) {
        UINT32 VP_InstIdx;
        UINT32 OctaveId;

        // For each enabled scale, check if there is a corresponding registered flexiDag, otherwise return fail
        for (OctaveId = 0U; OctaveId < MAX_HALF_OCTAVES; OctaveId++) {
            const memio_source_recv_picinfo_t *pInPicInfo = NULL;
            UINT32 OctaveIndex = 0U;
            UINT32 RoiEnable = 1U;
            UINT32 GetProcVPInst;
            const roi_t Roi = pCfg->SpuScaleCfg[OctaveId].Roi;

            if ( (pCfg->SpuScaleCfg[OctaveId].Enable == 1U) &&
                 (pCfg->SpuScaleCfg[OctaveId].RoiEnable == 1U) ) {
                GetProcVPInst = SofFex_GetProcVPInstIdx(pHandler,
                                                        pInPicInfo, OctaveIndex, RoiEnable,
                                                        &Roi, &VP_InstIdx);
                if ( GetProcVPInst != 0U ) {
                    Rval |= 1U;
                }
            }

            if ( (pCfg->FexScaleCfg[OctaveId].Enable != CV_FEX_DISABLE) &&
                 (pCfg->FexScaleCfg[OctaveId].RoiEnable == 1U) ) {
                GetProcVPInst = SofFex_GetProcVPInstIdx(pHandler,
                                                        pInPicInfo, OctaveIndex, RoiEnable,
                                                        &Roi, &VP_InstIdx);
                if ( GetProcVPInst != 0U ) {
                    Rval |= 1U;
                }
            }

            if ( Rval != 0U ) {
                break;
            }
        }

        if ( Rval == 0U ) {
            if (AmbaWrap_memcpy(&pHandler->Config, pCfg, sizeof(AMBA_CV_SPUFEX_CFG_s)) != 0U) {
                AmbaPrint_PrintStr5("Error AmbaSofFex_Cfg() call AmbaWrap_memcpy() Fail", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }

    return Rval;
}


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
                          AMBA_CV_SPU_BUF_s *pOutSof, AMBA_CV_FEX_BUF_s *pOutFex)
{
    UINT32 Rval = 0U;

    Rval |= ValidPointerCheck(pHandler);
    Rval |= ValidPointerCheck(pImgInfo);
    Rval |= ValidPointerCheck(pOutSof);
    Rval |= ValidPointerCheck(pOutFex);

    if ( Rval == 0U ) {
        AmbaMisra_TouchUnused(pOutSof);
        AmbaMisra_TouchUnused(pOutFex);

        Rval |= SofFex_HarrisCornerDetProc(pHandler, pImgInfo);
        Rval |= SofFex_LKOpticalFlowProc(pHandler, pImgInfo);
    }

    return Rval;
}


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
                            AMBA_CV_SPU_BUF_s *pOutSof, AMBA_CV_FEX_BUF_s *pOutFex)
{
    UINT32 Rval = 0U;

    AmbaMisra_TouchUnused(pImgInfo);

    Rval |= ValidPointerCheck(pHandler);
    Rval |= ValidPointerCheck(pOutFex);
    Rval |= ValidPointerCheck(pOutSof);

    if ( Rval == 0U ) {
        pOutSof->pBuffer = &pHandler->OutSofData;
        pOutFex->pBuffer = &pHandler->OutFexData;
    }

    return Rval;
}


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
                                  SOF_FEX_FD_HANDLE_s *pSofFexHdlr)
{
    UINT32 VP_ProcIdx;
    UINT32 Rval = 0U;

    Rval |= ValidPointerCheck(pHarrisHdlr);
    Rval |= ValidPointerCheck(pLKHdlr);
    Rval |= ValidPointerCheck(pSofFexHdlr);

    if ( Rval == 0U ) {
        if ( pSofFexHdlr->RegisterHdlrNum < MAX_VP_SOF_FEX_INSTANCE ) {
            VP_ProcIdx = pSofFexHdlr->RegisterHdlrNum;

            if ( (ProcWidth <= MAX_VP_SOF_FEX_WIDTH) &&
                 (ProcHeight <= MAX_VP_SOF_FEX_HEIGHT)) {

                pSofFexHdlr->ProcWidth[VP_ProcIdx] = ProcWidth;
                pSofFexHdlr->ProcHeight[VP_ProcIdx] = ProcHeight;

                pSofFexHdlr->pHarrisHandler[VP_ProcIdx] = pHarrisHdlr;
                pSofFexHdlr->pLKOpcFlowHandler[VP_ProcIdx] = pLKHdlr;

                Rval = SofFex_InitBriefDescPosLUT(InImagePitch, VP_ProcIdx, pSofFexHdlr);

                pSofFexHdlr->RegisterHdlrNum += 1U;
            } else {
                Rval = 1U;
            }
        } else {
            Rval = 1U;
        }
    }

    return Rval;
}

