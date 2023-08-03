/**
 *  @file SCA_AmbaSpuFusion.c
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

#include "cvapi_svccvalgo_ambaspufusion.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#if (defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_QNX))
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#else
#include "AmbaRTOSWrapper.h"
#define AmbaMisra_TypeCast(a, b) AmbaMisra_TypeCast64(a, b)
#endif

static UINT32 AmbaSpuFusion_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;
    if ((pHdlr != NULL) && (pCfg != NULL)) {
        // assign obj
        pHdlr->pAlgoObj = pCfg->pAlgoObj;
        pCfg->TotalReqBufSz = ((UINT32)2U<<20U);
    } else {
        AmbaPrint_PrintStr5("[ERR] AmbaSpuFusion_Query: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
        Ret = CVALGO_NG;
    }
    return Ret;
}

static UINT32 AmbaSpuFusion_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;

    if ((pHdlr != NULL) && (pCfg != NULL)) {
        CCF_OUTPUT_INFO_s OutInfo;
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;
        UINT32 StateBufSz = 0U;
        UINT32 TempBufSz = 0U;

        if(AmbaWrap_memset(&OutInfo, 0x0, sizeof(CCF_OUTPUT_INFO_s)) != CVALGO_OK) {
            Ret |= CVALGO_NG;
        }
        // Create MemPool
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
        Ret |= ArmMemPool_Create(pHdlr->pAlgoBuf, &pHdlr->MemPoolId);

        if (pHdlr->pAlgoCtrl != NULL) { // StereoHdlr is already prepared by APP
            if(AmbaWrap_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(AMBA_STEREO_FD_HANDLE_s)) != CVALGO_OK) {
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
            UINT32 LayerNum = 0U;
            UINT32 CfgBinBufSize = (pCfg->pBin[0] != NULL)? (pCfg->pBin[0]->buffer_size): 0U;

            if (pCfg->pExtCreateCfg != NULL) {
                if(AmbaWrap_memcpy(&LayerNum, pCfg->pExtCreateCfg, sizeof(UINT32)) != CVALGO_OK) {
                    Ret |= CVALGO_NG;
                }
            }

            if (LayerNum == 2U) {
                Ret |= AmbaSpuFusion_Openv1(pCfg->pBin[0], CfgBinBufSize,
                                            &StateBufSz, &TempBufSz,
                                            &OutInfo.OutputNum, OutInfo.OutputSz,
                                            pStereoHdlr);
            } else if (LayerNum == 3U) {
                Ret |= AmbaSpuFusion3Layer_Openv1(pCfg->pBin[0], CfgBinBufSize,
                                                  &StateBufSz, &TempBufSz,
                                                  &OutInfo.OutputNum, OutInfo.OutputSz,
                                                  pStereoHdlr);
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Create: Please assign UINT32* with layer num 2 or 3 to config field pExtCreateCfg %d", LayerNum, 0U, 0U, 0U, 0U);
                Ret = CVALGO_NG;
            }

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

                Ret |= ArmMemPool_Allocate(pHdlr->MemPoolId, (UINT32)sizeof(AMBA_CV_FLEXIDAG_IO_s), &OutBuf);
                AmbaMisra_TypeCast(&pHdlr->FDs[0].OutData.pOut, &OutBuf.pBuffer);

                if (Ret == CVALGO_OK) {
                    Ret |= AmbaSpuFusion_Initv1(pStereoHdlr, &StateBuf, &TmpBuf);

                    if (Ret == CVALGO_OK) {
                        pHdlr->State = 1U;
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Create: Working buffer allocate fail %d", Ret, 0U, 0U, 0U, 0U);
                    pHdlr->FDs[0].OutData.pOut = NULL;
                }
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Create: Openv1 fail %d", Ret, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Create: Algo buffer create fail %d", Ret, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintStr5("[ERR] AmbaSpuFusion_Create: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
        Ret = CVALGO_NG;
    }
    return Ret;
}

static UINT32 AmbaSpuFusion_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;

    //Stereopriv does not support CV2 AmbaSpuFusion_Cfg
    AmbaMisra_TouchUnused(pHdlr); //(void)pHdlr;
    (void)pCfg;

    return Ret;
}

static UINT32 AmbaSpuFusion_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;
    (void)pHdlr;
    (void)pCfg;
    if (pHdlr != NULL) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;
        AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
        if (pStereoHdlr != NULL) {
            Ret |= AmbaSpuFusion_Close(pStereoHdlr);
            if (Ret == CVALGO_OK) {
                Ret |= ArmMemPool_Free(pHdlr->MemPoolId, pHdlr->pAlgoBuf);
                Ret |= ArmMemPool_Delete(pHdlr->MemPoolId);
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Delete: Handler close fail %d", Ret, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Ret;
}

static UINT32 AmbaSpuFusion_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Ret = CVALGO_OK;

    (void)pHdlr;
    if ((pHdlr != NULL) && (pCfg != NULL)) {
        const AMBA_STEREO_FD_HANDLE_s* pStereoHdlr = NULL;

        if (pHdlr->pAlgoCtrl != NULL) {
            AmbaMisra_TypeCast(&pStereoHdlr, &pHdlr->pAlgoCtrl);
        }

        if ((pStereoHdlr != NULL) && (pCfg->pIn != NULL) && (pCfg->pOut != NULL)) {
            AMBA_CV_SPU_BUF_s InDisparity;
            AMBA_CV_IMG_BUF_s InImgInfo;
            AMBA_CV_SPU_BUF_s OutSpu;

            if(AmbaWrap_memcpy(&InDisparity, &pCfg->pIn->buf[0], sizeof(AMBA_CV_SPU_BUF_s)) != CVALGO_OK) {
                Ret |= CVALGO_NG;
            }
            if(AmbaWrap_memcpy(&InImgInfo, &pCfg->pIn->buf[1], sizeof(AMBA_CV_IMG_BUF_s)) != CVALGO_OK) {
                Ret |= CVALGO_NG;
            }
            if(AmbaWrap_memcpy(&OutSpu, &pCfg->pOut->buf[0], sizeof(AMBA_CV_SPU_BUF_s)) != CVALGO_OK) {
                Ret |= CVALGO_NG;
            }
            if ((InDisparity.pBuffer != NULL) && (InImgInfo.pBuffer != NULL) && (OutSpu.pBuffer != NULL)) {
                Ret |= AmbaSpuFusion_Processv1(pStereoHdlr, &InDisparity, &InImgInfo, &OutSpu);

                if (Ret == CVALGO_OK) {
                    Ret |= AmbaSpuFusion_GetResultv1(pStereoHdlr, &InDisparity, &InImgInfo, &OutSpu);

                    if (Ret == CVALGO_OK) {
                        if ((pHdlr->Callback[0] != NULL) && (pHdlr->FDs[0].OutData.pOut != NULL)) {
                            SVC_CV_ALGO_OUTPUT_s Out;

                            Out.pOutput = pHdlr->FDs[0].OutData.pOut;
                            Out.pOutput->num_of_buf = 1;
                            if(AmbaWrap_memcpy(&Out.pOutput->buf[0], &OutSpu, sizeof(flexidag_memblk_t)) != CVALGO_OK) {
                                Ret |= CVALGO_NG;
                            }
                            Out.pUserData = pCfg->pUserData;
                            Out.pExtOutput = NULL;
                            (void)pHdlr->Callback[0](CALLBACK_EVENT_FREE_INPUT, &Out);
                            (void)pHdlr->Callback[0](CALLBACK_EVENT_OUTPUT, &Out);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Feed: GetResultv1 fail %d", Ret, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Feed: Processv1 fail %d", Ret, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[ERR] AmbaSpuFusion_Feed: NULL IO buffer", 0U, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintStr5("[ERR] AmbaSpuFusion_Feed: Invalid input parameter", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("[ERR] AmbaSpuFusion_Feed: NULL input parameter", NULL, NULL, NULL, NULL, NULL);
        Ret = CVALGO_NG;
    }

    return Ret;
}

SVC_CV_ALGO_OBJ_s AmbaSpuFusionAlgoObj = {
    .Query = AmbaSpuFusion_Query,
    .Create = AmbaSpuFusion_Create,
    .Delete = AmbaSpuFusion_Delete,
    .Feed = AmbaSpuFusion_Feed,
    .Control = AmbaSpuFusion_Control
};
