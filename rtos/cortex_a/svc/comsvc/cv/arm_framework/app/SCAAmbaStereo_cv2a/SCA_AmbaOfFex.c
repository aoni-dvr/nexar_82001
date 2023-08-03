/**
 *  @file SCA_AmbaOfFex.c
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
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_amba_mask_interface.h"
#include "idsp_roi_msg.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "cvapi_svccvalgo_ambaoffex.h"
#include "cvapi_flexidag_ambaoffex_cv2fs.h"

static void ScaOfFexWrap_memcpy(void *pDst, const void *pSrc, SIZE_t Num)
{
    if (AmbaWrap_memcpy(pDst, pSrc, Num) != CVALGO_OK) {
        AmbaPrint_PrintUInt5("[ERR] ScaOfFexWrap_memcpy: AmbaWrap_memcpy fail", 0U, 0U, 0U, 0U, 0U);
    }
}

static UINT32 AmbaOfFex_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    // assign obj
    pHdlr->pAlgoObj = pCfg->pAlgoObj;
    // pCfg->TotalReqBufSz = ((UINT32)1U<<20U); //1M
    pCfg->TotalReqBufSz = SCA_REQ_BUF_SZ;

    return CVALGO_OK;
}

static UINT32 AmbaOfFex_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Ret;

    if ((pHdlr != NULL) && (pCfg != NULL)) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;
        CCF_OUTPUT_INFO_s OutInfo;
        UINT32 StateBufSz = 0U;
        UINT32 TempBufSz = 0U;

        // Create MemPool
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
        Ret = ArmMemPool_Create(pHdlr->pAlgoBuf, &pHdlr->MemPoolId);

        if (pHdlr->pAlgoCtrl != NULL) { // StereoHdlr is already prepared by APP
            (void) ArmStdC_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(AMBA_STEREO_FD_HANDLE_s));
            AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);

        } else { // Prepare StereoHdlr Here
            flexidag_memblk_t CtrlBuf;
            Ret = ArmMemPool_Allocate(pHdlr->MemPoolId, sizeof(AMBA_STEREO_FD_HANDLE_s), &CtrlBuf);
            if (Ret == CVALGO_OK) {
                pHdlr->pAlgoCtrl = CtrlBuf.pBuffer;
                (void)ArmStdC_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(AMBA_STEREO_FD_HANDLE_s));
                AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
            }
        }

        if ((Ret == CVALGO_OK) && (pStereoHdlr != NULL)) {
            UINT32 CfgBinBufSize = (pCfg->pBin[0] != NULL)? (pCfg->pBin[0]->buffer_size): 0U;
            Ret |= AmbaOfFex_Open(pCfg->pBin[0], CfgBinBufSize,
                                  &StateBufSz, &TempBufSz,
                                  &OutInfo.OutputNum, OutInfo.OutputSz,
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
                for (UINT32 Idx = 0U ; Idx < OutInfo.OutputNum ; Idx ++) {
                    pCfg->OutputSz[Idx] = OutInfo.OutputSz[Idx];
                }
                Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, StateBufSz, &StateBuf);
                Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, TempBufSz, &TmpBuf);

                //Allocate output working buffer
                Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, sizeof(AMBA_CV_FLEXIDAG_IO_s), &OutBuf);
                AmbaMisra_TypeCast(&pHdlr->FDs[0].OutData.pOut, &OutBuf.pBuffer);

                if (Ret == CVALGO_OK) {
                    Ret |= AmbaOfFex_Init(pStereoHdlr, &StateBuf, &TmpBuf);

                    if (Ret == CVALGO_OK) {
                        pHdlr->State = 1U;
                    } else {
                        AmbaPrint_PrintUInt5("[ERR] AmbaOfFex_Create: AmbaOfFex_Init fail %d", Ret, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERR] AmbaOfFex_Create: Working buffer allocate fail %d", Ret, 0U, 0U, 0U, 0U);
                    pHdlr->FDs[0].OutData.pOut = NULL;
                }
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaOfFex_Create: AmbaOfFex_Open fail %d", Ret, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ERR] AmbaOfFex_Create: Algo buffer create fail %d", Ret, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintStr5("[ERR] AmbaOfFex_Create: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
        Ret = CVALGO_NG;
    }
    return Ret;
}

static UINT32 AmbaOfFex_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;

    (void)pHdlr;
    if ((pHdlr != NULL) && (pCfg != NULL)) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;

        if (pHdlr->pAlgoCtrl != NULL) {
            AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
            if (pCfg->pCtrlParam != NULL) {
                const AMBA_CV_SPUFEX_CFG_s* pOfFexCfg = NULL;
                AmbaMisra_TypeCast(&pOfFexCfg, &pCfg->pCtrlParam);
                Ret |= AmbaOfFex_Cfg(pStereoHdlr, pOfFexCfg);
            }
        }
    }

    return Ret;
}

static UINT32 AmbaOfFex_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;
    (void)pHdlr;
    (void)pCfg;
    if (pHdlr != NULL) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;
        AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
        if (pStereoHdlr != NULL) {
            Ret |= AmbaOfFex_Close(pStereoHdlr);
            if (Ret == CVALGO_OK) {
                Ret |= ArmMemPool_Free(pHdlr->MemPoolId, pHdlr->pAlgoBuf);
                Ret |= ArmMemPool_Delete(pHdlr->MemPoolId);
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaOfFex_Delete: Handler close fail %d", Ret, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Ret;
}

static UINT32 AmbaOfFex_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;

    (void)pHdlr;
    if ((pHdlr != NULL) && (pCfg != NULL)) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;

        if (pHdlr->pAlgoCtrl != NULL) {
            AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
        }

        if ((pStereoHdlr != NULL) && (pCfg->pIn != NULL) && (pCfg->pOut != NULL)) {
            static AMBA_CV_IMG_BUF_s InInfo;
            static AMBA_CV_SPU_BUF_s OutOf;
            static AMBA_CV_FEX_BUF_s OutFex;

            ScaOfFexWrap_memcpy(&InInfo, &pCfg->pIn->buf[0], sizeof(AMBA_CV_IMG_BUF_s));
            ScaOfFexWrap_memcpy(&OutOf, &pCfg->pOut->buf[0], sizeof(AMBA_CV_SPU_BUF_s));
            ScaOfFexWrap_memcpy(&OutFex, &pCfg->pOut->buf[1], sizeof(AMBA_CV_FEX_BUF_s));

            if ((InInfo.pBuffer != NULL) && (OutOf.pBuffer != NULL) && (OutFex.pBuffer != NULL)) {
                Ret |= AmbaOfFex_Process(pStereoHdlr, &InInfo, &OutOf, &OutFex);

                if (Ret == CVALGO_OK) {
                    Ret |= AmbaOfFex_GetResult(pStereoHdlr, &InInfo, &OutOf, &OutFex);
                    if (Ret == CVALGO_OK) {
                        if ((pHdlr->Callback[0] != NULL) && (pHdlr->FDs[0].OutData.pOut != NULL)) {
                            SVC_CV_ALGO_OUTPUT_s Out;

                            Out.pOutput = pHdlr->FDs[0].OutData.pOut;
                            Out.pOutput->num_of_buf = 2;
                            ScaOfFexWrap_memcpy(&Out.pOutput->buf[0], &OutOf, sizeof(flexidag_memblk_t));
                            ScaOfFexWrap_memcpy(&Out.pOutput->buf[1], &OutFex, sizeof(flexidag_memblk_t));
                            Out.pUserData = pCfg->pUserData;
                            Out.pExtOutput = NULL;
                            (void)pHdlr->Callback[0](CALLBACK_EVENT_FREE_INPUT, &Out);
                            (void)pHdlr->Callback[0](CALLBACK_EVENT_OUTPUT, &Out);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERR] AmbaOfFex_Feed: AmbaOfFex_GetResult fail %d", Ret, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERR] AmbaOfFex_Feed: AmbaOfFex_Process fail %d", Ret, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            AmbaPrint_PrintStr5("[ERR] AmbaOfFex_Feed: Invalid input data", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("[ERR] AmbaOfFex_Feed: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
        Ret = CVALGO_NG;
    }
    return Ret;
}

SVC_CV_ALGO_OBJ_s AmbaOfFexAlgoObj = {
    .Query = AmbaOfFex_Query,
    .Create = AmbaOfFex_Create,
    .Delete = AmbaOfFex_Delete,
    .Feed = AmbaOfFex_Feed,
    .Control = AmbaOfFex_Control
};
