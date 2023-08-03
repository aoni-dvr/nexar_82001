/**
 *  @file SCA_AmbaFex.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Implementation of Amba FlexiDAG
 *
 */

#include "cvapi_svccvalgo_ambafex.h"
#include "cvapi_flexidag_ambafex_cv2.h"
#if (defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_QNX))
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#else
#include "AmbaRTOSWrapper.h"
#define AmbaMisra_TypeCast(a, b) AmbaMisra_TypeCast64(a, b)
#endif

static UINT32 AmbaFex_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;
    if ((pHdlr != NULL) && (pCfg != NULL)) {
        // assign obj
        pHdlr->pAlgoObj = pCfg->pAlgoObj;
        pCfg->TotalReqBufSz = ((UINT32)1U<<20U); //1M
    } else {
        Ret = CVALGO_NG;
        AmbaPrint_PrintStr5("[ERR] AmbaFex_Query: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
    }

    return Ret;
}

static UINT32 AmbaFex_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;

    if ((pHdlr != NULL) && (pCfg != NULL)) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;
        CCF_OUTPUT_INFO_s OutInfo;
        UINT32 StateBufSz = 0U;
        UINT32 TempBufSz = 0U;

        // Create MemPool
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
        Ret |= ArmMemPool_Create(pHdlr->pAlgoBuf, &pHdlr->MemPoolId);
        if (pHdlr->pAlgoCtrl != NULL) { // StereoHdlr is already prepared by APP
            if (AmbaWrap_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(AMBA_STEREO_FD_HANDLE_s)) != CVALGO_OK) {
                Ret |= CVALGO_NG;
            }
            AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
        } else { // Prepare StereoHdlr Here
            flexidag_memblk_t CtrlBuf;
            Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, (UINT32)sizeof(AMBA_STEREO_FD_HANDLE_s), &CtrlBuf);
            if (Ret == CVALGO_OK) {
                pHdlr->pAlgoCtrl = CtrlBuf.pBuffer;
                if(AmbaWrap_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(AMBA_STEREO_FD_HANDLE_s)) != CVALGO_OK) {
                    Ret |= CVALGO_NG;
                }
                AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
            }
        }

        if ((Ret == CVALGO_OK) && (pStereoHdlr != NULL)) {
            UINT32 CfgBinBufSize = (pCfg->pBin[0] != NULL)? (pCfg->pBin[0]->buffer_size): 0U;
            Ret |= AmbaFex_Openv1(pCfg->pBin[0],
                                  CfgBinBufSize,
                                  &StateBufSz,
                                  &TempBufSz,
                                  &OutInfo.OutputNum,
                                  OutInfo.OutputSz,
                                  pStereoHdlr);
            if (StateBufSz == 0U) {
                StateBufSz = 1U;
            }
            if (TempBufSz == 0U) {
                TempBufSz = 1U;
            }

            if (Ret == CVALGO_OK) {
                flexidag_memblk_t StateBuf;
                flexidag_memblk_t TmpBuf;
                flexidag_memblk_t OutBuf;

                pCfg->OutputNum = OutInfo.OutputNum;
                for (UINT32 Idx = 0U; Idx < OutInfo.OutputNum; Idx ++) {
                    pCfg->OutputSz[Idx] = OutInfo.OutputSz[Idx];
                }
                Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, StateBufSz, &StateBuf);
                Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, TempBufSz, &TmpBuf);

                //Allocate output data buffer
                Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, (UINT32)sizeof(AMBA_CV_FLEXIDAG_IO_s), &OutBuf);
                AmbaMisra_TypeCast(&pHdlr->FDs[0].OutData.pOut, &OutBuf.pBuffer);

                if (Ret == CVALGO_OK) {
                    Ret |= AmbaFex_Initv1(pStereoHdlr, &StateBuf, &TmpBuf);

                    if (Ret == CVALGO_OK) {
                        pHdlr->State = 1U;
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERR] AmbaFex_Create: Working buffer allocate fail %d", Ret, 0U, 0U, 0U, 0U);
                    pHdlr->FDs[0].OutData.pOut = NULL;
                }
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaFex_Create: AmbaFex_Open fail %d", Ret, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ERR] AmbaFex_Create: Algo buffer create fail %d", Ret, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintStr5("[ERR] AmbaFex_Create: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
        Ret = CVALGO_NG;
    }
    return Ret;
}

static UINT32 AmbaFex_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;

    (void)pHdlr;
    if ((pHdlr != NULL) && (pCfg != NULL)) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;

        if (pHdlr->pAlgoCtrl != NULL) {
            AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
            if (pCfg->pCtrlParam != NULL) {
                const AMBA_CV_FEX_CFG_s* pFexCfg = NULL;
                AmbaMisra_TypeCast(&pFexCfg, &pCfg->pCtrlParam);
                Ret |= AmbaFex_Cfg(pStereoHdlr, pFexCfg);
            }
        }
    }

    return Ret;
}

static UINT32 AmbaFex_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;
    (void)pHdlr;
    (void)pCfg;
    if (pHdlr != NULL) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;
        AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
        if (pStereoHdlr != NULL) {
            Ret |= AmbaFex_Close(pStereoHdlr);
            if (Ret == CVALGO_OK) {
                Ret |= ArmMemPool_Free(pHdlr->MemPoolId, pHdlr->pAlgoBuf);
                Ret |= ArmMemPool_Delete(pHdlr->MemPoolId);
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaFex_Delete: Handler close fail %d", Ret, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Ret;
}

static UINT32 AmbaFex_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;
    (void)pHdlr;

    if ((pHdlr != NULL) && (pCfg != NULL)) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;

        if (pHdlr->pAlgoCtrl != NULL) {
            AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
        }

        if ((pStereoHdlr != NULL) && (pCfg->pIn != NULL) && (pCfg->pOut != NULL)) {
            AMBA_CV_IMG_BUF_s InInfo;
            AMBA_CV_FEX_BUF_s OutFex;

            if (AmbaWrap_memcpy(&InInfo, &pCfg->pIn->buf[0], sizeof(AMBA_CV_IMG_BUF_s)) != CVALGO_OK) {
                Ret |=CVALGO_NG;
            }
            if(AmbaWrap_memcpy(&OutFex, &pCfg->pOut->buf[0], sizeof(AMBA_CV_FEX_BUF_s)) != CVALGO_OK) {
                Ret |=CVALGO_NG;
            }

            if ((InInfo.pBuffer != NULL) && (OutFex.pBuffer != NULL)) {
                Ret |= AmbaFex_Processv1(pStereoHdlr,&InInfo,&OutFex);

                if (Ret == CVALGO_OK) {
                    Ret |= AmbaFex_GetResultv1(pStereoHdlr,&InInfo,&OutFex);

                    if (Ret == CVALGO_OK) {
                        if ((pHdlr->Callback[0] != NULL) && (pHdlr->FDs[0].OutData.pOut != NULL)) {
                            SVC_CV_ALGO_OUTPUT_s Out;

                            Out.pOutput = pHdlr->FDs[0].OutData.pOut;
                            Out.pOutput->num_of_buf = 2;
                            if(AmbaWrap_memcpy(&Out.pOutput->buf[0], &OutFex, sizeof(flexidag_memblk_t)) != CVALGO_OK) {
                                Ret |= CVALGO_NG;
                            }
                            Out.pUserData = pCfg->pUserData;
                            Out.pExtOutput = NULL;
                            (void)pHdlr->Callback[0](CALLBACK_EVENT_FREE_INPUT, &Out);
                            (void)pHdlr->Callback[0](CALLBACK_EVENT_OUTPUT, &Out);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERR] AmbaFex_Feed: GetResult fail %d", Ret, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERR] AmbaFex_Feed: Process fail %d", Ret, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            AmbaPrint_PrintStr5("[ERR] AmbaFex_Feed: Invalid input parameter", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        Ret = CVALGO_NG;
        AmbaPrint_PrintStr5("[ERR] AmbaFex_Feed: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

SVC_CV_ALGO_OBJ_s AmbaFexAlgoObj = {
    .Query = AmbaFex_Query,
    .Create = AmbaFex_Create,
    .Delete = AmbaFex_Delete,
    .Feed = AmbaFex_Feed,
    .Control = AmbaFex_Control
};
