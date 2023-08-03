/**
 *  @file AmbaImgSystem.c
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Amba Image System
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaWrap.h"

#include "AmbaImgMem.h"
#include "AmbaImgVar.h"
#include "AmbaImgChannel.h"
#include "AmbaImgChannel_Internal.h"
#include "AmbaImgSensorHAL.h"
#include "AmbaImgSystem.h"
#include "AmbaImgSystem_Internal.h"
#include "AmbaImgSystem_Platform.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_IMAGE_SYSTEM_MEM_INFO_u_*/ {
    AMBA_IMG_MEM_ADDR              Data;
    void                           *pVoid;
    AMBA_IMG_SYSTEM_PAIK_INFO_s    *pPostAikInfo;
} IMAGE_SYSTEM_MEM_INFO_u;

typedef struct /*_IMAGE_SYSTEM_MEM_INFO_s_*/ {
    IMAGE_SYSTEM_MEM_INFO_u  Ctx;
} IMAGE_SYSTEM_MEM_INFO_s;

/**
 *  Amba image system init
 *  @return error code
 */
UINT32 AmbaImgSystem_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j, k, m, n, p;

    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_INFO_s *pInfo;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_VIN_s *pVin;

    AMBA_IMG_SYSTEM_AUX_INFO_s *pAux = &AmbaImgSystem_AuxInfo;

    const AMBA_IMG_SYSTEM_AIK_s *pAik;

    FuncRetCode = AmbaKAL_MutexCreate(&(pAux->Ik.Mutex), NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    /* chan find */
    for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
        /* chan check */
        if (AmbaImgSystem_Chan[i] == NULL) {
            continue;
        }

        /* vin get */
        pVin = &(AmbaImgSystem_Vin[i]);
        /* vin param init */
        pVin->Debug.Data = 0U;
        pVin->Op.Data = 0U;
        pVin->Counter.Raw = 0U;

        /* chain find */
        for (j = 0U; AmbaImgSystem_Chan[i][j].Magic == 0xCafeU; j++) {
            /* ctx check */
            if (AmbaImgSystem_Chan[i][j].pCtx == NULL) {
                continue;
            }

            /* info get */
            pInfo = AmbaImgSystem_Chan[i][j].pInfo;
            /* info reset */
            if (pInfo != NULL) {
                pInfo->Vin.Cap.Data = 0ULL;
                pInfo->Pipe.Mode.Data = 0ULL;
                pInfo->Pipe.Hdr.Data = 0ULL;
                pInfo->pAux = &AmbaImgSystem_AuxInfo;
            }

            /* ctx get */
            pCtx = AmbaImgSystem_Chan[i][j].pCtx;

            for (k = 0U; pCtx[k].Magic == 0xCafeU; k++) {
                /* zone check */
                if (pCtx[k].pZone == NULL) {
                    continue;
                }
                /* zone get */
                pZone = pCtx[k].pZone;
                /* zone init */
                if (pZone->InitFlag == 0U) {
                    /* op */
                    pZone->Op.Mode = 0U;
                    pZone->Op.R2yUserDelay = 0U;
                    pZone->Op.R2yDelay = 0U;
                    /* cfg */
                    pZone->Cfg.In.Data = 0ULL;
                    pZone->Cfg.Out.Data = 0ULL;
                    /* video/still */
                    for (m = 0U; m < AMBA_IMG_SYSTEM_ZONE_TYPE_NUM; m++) {
                        /* pipe mutex */
                        FuncRetCode = AmbaKAL_MutexCreate(&(pZone->Pipe[m].Mutex), NULL);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        /* blc */
                        FuncRetCode = AmbaKAL_MutexCreate(&(pZone->Pipe[m].Blc.Mutex), NULL);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        for (n = 0U; n < 4U; n++) {
                            pZone->Pipe[m].Blc.Offset[n] = 0;
                        }
                        /* wb (fe) */
                        for (n = 0U; n < 3U; n++) {
                            pZone->Pipe[m].Wb.Fe[n].Op.Data = 0U;
                            pZone->Pipe[m].Wb.Fe[n].Op.Bits.Scaler = 4096U;
                            pZone->Pipe[m].Wb.Fe[n].Op.Bits.Mix = 1U;
                            /* wb (fe) mutex */
                            FuncRetCode = AmbaKAL_MutexCreate(&(pZone->Pipe[m].Wb.Fe[n].Mutex), NULL);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                            /* wgc */
                            for (p = 0U; p < 3U; p++) {
                                pZone->Pipe[m].Wb.Fe[n].Wgc[p] = 4096U;
                                pZone->Pipe[m].Wb.Fe[n].FineWgc[p] = 4096U;
                            }
                            /* igc */
                            pZone->Pipe[m].Wb.Fe[n].Igc = 4096U;
                            pZone->Pipe[m].Wb.Fe[n].FineIgc = 4096U;
                            /* dgc */
                            pZone->Pipe[m].Wb.Fe[n].Dgc = 4096U;
                            pZone->Pipe[m].Wb.Fe[n].FineDgc = 4096U;
                            /* ggc */
                            pZone->Pipe[m].Wb.Fe[n].Ggc = 4096U;
                            pZone->Pipe[m].Wb.Fe[n].FineGgc = 4096U;
                        }

                        /* wb (be) */
                        pZone->Pipe[m].Wb.Be.Op.Data = 0U;
                        pZone->Pipe[m].Wb.Be.Op.Bits.Scaler = 4096U;
                        pZone->Pipe[m].Wb.Be.Op.Bits.Mix = 1U;
                        /* wb (be) mutex */
                        FuncRetCode = AmbaKAL_MutexCreate(&(pZone->Pipe[m].Wb.Be.Mutex), NULL);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        /* wgc */
                        for (p = 0U; p < 3U; p++) {
                            pZone->Pipe[m].Wb.Be.Wgc[p] = 4096U;
                            pZone->Pipe[m].Wb.Be.FineWgc[p] = 4096U;
                        }
                        /* igc */
                        pZone->Pipe[m].Wb.Be.Igc = 4096U;
                        pZone->Pipe[m].Wb.Be.FineIgc = 4096U;
                        /* dgc */
                        pZone->Pipe[m].Wb.Be.Dgc = 4096U;
                        pZone->Pipe[m].Wb.Be.FineDgc = 4096U;
                        /* ggc */
                        pZone->Pipe[m].Wb.Be.Ggc = 4096U;
                        pZone->Pipe[m].Wb.Be.FineGgc = 4096U;

                        /* wb (be alt) */
                        pZone->Pipe[m].Wb.BeAlt.Op.Data = 0U;
                        pZone->Pipe[m].Wb.BeAlt.Op.Bits.Scaler = 4096U;
                        pZone->Pipe[m].Wb.BeAlt.Op.Bits.Mix = 1U;
                        /* wb (be alt) mutex */
                        FuncRetCode = AmbaKAL_MutexCreate(&(pZone->Pipe[m].Wb.BeAlt.Mutex), NULL);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        /* wgc */
                        for (p = 0U; p < 3U; p++) {
                            pZone->Pipe[m].Wb.BeAlt.Wgc[p] = 4096U;
                            pZone->Pipe[m].Wb.BeAlt.FineWgc[p] = 4096U;
                        }
                        /* igc */
                        pZone->Pipe[m].Wb.BeAlt.Igc = 4096U;
                        pZone->Pipe[m].Wb.BeAlt.FineIgc = 4096U;
                        /* dgc */
                        pZone->Pipe[m].Wb.BeAlt.Dgc = 4096U;
                        pZone->Pipe[m].Wb.BeAlt.FineDgc = 4096U;
                        /* ggc */
                        pZone->Pipe[m].Wb.BeAlt.Ggc = 4096U;
                        pZone->Pipe[m].Wb.BeAlt.FineGgc = 4096U;
                        /* img mode */
                        pZone->Pipe[m].Mode.ContextId = 0U;
                    }
                    /* aik */
                    if (pZone->pAik != NULL) {
                        /* aik get */
                        pAik = pZone->pAik;
                        /* aik status */
                        if (pAik->pStatus != NULL) {
                            /* mutex */
                            FuncRetCode = AmbaKAL_MutexCreate(&(pAik->pStatus->Mutex), NULL);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                        }
                        /* post aik */
                        if (pAik->pPost != NULL) {
                            /*ring mutex */
                            FuncRetCode = AmbaKAL_MutexCreate(&(pAik->pPost->Ring.Mutex), NULL);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                            /* post aik mtx */
                            FuncRetCode = AmbaKAL_MutexCreate(&(pAik->pPost->Mtx.Ack), NULL);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                            /* post aik sem (data) */
                            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pAik->pPost->Sem.Data), NULL, 0U);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                            /* post aik sem (timer) */
                            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pAik->pPost->Sem.Timer), NULL, 0U);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                            /* post aik sem (ack) */
                            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pAik->pPost->Sem.Ack), NULL, 0U);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                        }
                        /* r2y user */
                        if (pAik->pR2yUser != NULL) {
                            /* r2y usre rint mutex */
                            FuncRetCode = AmbaKAL_MutexCreate(&(pAik->pR2yUser->Ring.Mutex), NULL);
                            if (FuncRetCode != KAL_ERR_NONE) {
                                RetCode = NG_UL;
                            }
                        }
                    }
                    /* ring */
                    if (pZone->pRing != NULL) {
                        FuncRetCode = AmbaKAL_MutexCreate(&(pZone->pRing->Mutex), NULL);
                        if (FuncRetCode != KAL_ERR_NONE) {
                            RetCode = NG_UL;
                        }
                        pZone->pRing->Index.Data = 0U;
                    }
                    /* zone init flag */
                    pZone->InitFlag = 1U;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image system debug
 *  @param[in] VinId vin id
 *  @param[in] Flag debug flag (bits)
 *  @return error code
 */
UINT32 AmbaImgSystem_Debug(UINT32 VinId, UINT32 Flag)
{
    UINT32 RetCode = OK_UL;

    if (VinId < (UINT32) AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AmbaImgSystem_Vin[VinId].Debug.Data = Flag;
    } else {
        /* id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system reset
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSystem_Reset(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j, m, n, p;

    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_AIK_s *pAik;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;
    UINT32 ChainId;

    /* vin check */
    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL)) {
        /* chain check */
        for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                /* image channel id get */
                ImageChanId.Ctx.Data = pFrwImageChannel[VinId][i].pCtx->Id.Ctx.Data;
                /* chain id get */
                ChainId = ImageChanId.Ctx.Bits.ChainId;
                if ((AmbaImgSystem_Chan[VinId] != NULL) &&
                    (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                    /* ctx check */
                    if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                        /* ctx get */
                        pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                        /* zone check */
                        for (j = 0U; pCtx[j].Magic == 0xCafeU; j++) {
                            /* zone find */
                            if ((ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << j)) > 0U) {
                                /* zone check */
                                if (pCtx[j].pZone != NULL) {
                                    /* zone get */
                                    pZone = pCtx[j].pZone;
                                    /* info assign */
                                    pZone->pInfo = AmbaImgSystem_Chan[VinId][ChainId].pInfo;
                                    /* op reset */
                                    pZone->Op.R2yUserDelay = 0U;
                                    pZone->Op.R2yDelay = 0U;
                                    /* video/still reset */
                                    for (m = 0U; m < AMBA_IMG_SYSTEM_ZONE_TYPE_NUM; m++) {
                                        /* pipe mutex take */
                                        FuncRetCode = AmbaKAL_MutexTake(&(pZone->Pipe[m].Mutex), AMBA_KAL_WAIT_FOREVER);
                                        if (FuncRetCode == KAL_ERR_NONE) {
                                            /* wb (fe) reset */
                                            for (n = 0U; n < 3U; n++) {
                                                FuncRetCode = AmbaKAL_MutexTake(&(pZone->Pipe[m].Wb.Fe[n].Mutex), AMBA_KAL_WAIT_FOREVER);
                                                if (FuncRetCode == KAL_ERR_NONE) {
                                                    /* wgc */
                                                    for (p = 0U; p < 3U; p++) {
                                                        pZone->Pipe[m].Wb.Fe[n].Wgc[p] = 4096U;
                                                        pZone->Pipe[m].Wb.Fe[n].FineWgc[p] = 4096U;
                                                    }
                                                    /* igc */
                                                    pZone->Pipe[m].Wb.Fe[n].Igc = 4096U;
                                                    pZone->Pipe[m].Wb.Fe[n].FineIgc = 4096U;
                                                    /* dgc */
                                                    pZone->Pipe[m].Wb.Fe[n].Dgc = 4096U;
                                                    pZone->Pipe[m].Wb.Fe[n].FineDgc = 4096U;
                                                    /* ggc */
                                                    pZone->Pipe[m].Wb.Fe[n].Ggc = 4096U;
                                                    pZone->Pipe[m].Wb.Fe[n].FineGgc = 4096U;
                                                    /* wb mutex give */
                                                    FuncRetCode = AmbaKAL_MutexGive(&(pZone->Pipe[m].Wb.Fe[n].Mutex));
                                                    if (FuncRetCode != KAL_ERR_NONE) {
                                                        /* */
                                                    }
                                                }
                                            }

                                            /* wb (be) reset */
                                            FuncRetCode = AmbaKAL_MutexTake(&(pZone->Pipe[m].Wb.Be.Mutex), AMBA_KAL_WAIT_FOREVER);
                                            if (FuncRetCode == KAL_ERR_NONE) {
                                                /* wgc */
                                                for (p = 0U; p < 3U; p++) {
                                                    pZone->Pipe[m].Wb.Be.Wgc[p] = 4096U;
                                                    pZone->Pipe[m].Wb.Be.FineWgc[p] = 4096U;
                                                }
                                                /* igc */
                                                pZone->Pipe[m].Wb.Be.Igc = 4096U;
                                                pZone->Pipe[m].Wb.Be.FineIgc = 4096U;
                                                /* dgc */
                                                pZone->Pipe[m].Wb.Be.Dgc = 4096U;
                                                pZone->Pipe[m].Wb.Be.FineDgc = 4096U;
                                                /* ggc */
                                                pZone->Pipe[m].Wb.Be.Ggc = 4096U;
                                                pZone->Pipe[m].Wb.Be.FineGgc = 4096U;
                                                /* wb mutex give */
                                                FuncRetCode = AmbaKAL_MutexGive(&(pZone->Pipe[m].Wb.Be.Mutex));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                            }

                                            /* wb (be alt) reset */
                                            FuncRetCode = AmbaKAL_MutexTake(&(pZone->Pipe[m].Wb.BeAlt.Mutex), AMBA_KAL_WAIT_FOREVER);
                                            if (FuncRetCode == KAL_ERR_NONE) {
                                                /* wgc */
                                                for (p = 0U; p < 3U; p++) {
                                                    pZone->Pipe[m].Wb.BeAlt.Wgc[p] = 4096U;
                                                    pZone->Pipe[m].Wb.BeAlt.FineWgc[p] = 4096U;
                                                }
                                                /* igc */
                                                pZone->Pipe[m].Wb.BeAlt.Igc = 4096U;
                                                pZone->Pipe[m].Wb.BeAlt.FineIgc = 4096U;
                                                /* dgc */
                                                pZone->Pipe[m].Wb.BeAlt.Dgc = 4096U;
                                                pZone->Pipe[m].Wb.BeAlt.FineDgc = 4096U;
                                                /* ggc */
                                                pZone->Pipe[m].Wb.BeAlt.Ggc = 4096U;
                                                pZone->Pipe[m].Wb.BeAlt.FineGgc = 4096U;
                                                /* wb mutex give */
                                                FuncRetCode = AmbaKAL_MutexGive(&(pZone->Pipe[m].Wb.BeAlt.Mutex));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                            }
                                            /* pipe mutex give */
                                            FuncRetCode = AmbaKAL_MutexGive(&(pZone->Pipe[m].Mutex));
                                            if (FuncRetCode != KAL_ERR_NONE) {
                                                /* */
                                            }
                                        } else {
                                            /* pipe mutex fail */
                                            RetCode = NG_UL;
                                        }
                                    }
                                    /* aik */
                                    if (pZone->pAik != NULL) {
                                        /* aik get */
                                        pAik = pZone->pAik;
                                        /* aik status */
                                        if (pAik->pStatus != NULL) {
                                            /* mutex take */
                                            FuncRetCode = AmbaKAL_MutexTake(&(pAik->pStatus->Mutex), AMBA_KAL_WAIT_FOREVER);
                                            if (FuncRetCode == KAL_ERR_NONE) {
                                                /* index reset */
                                                pAik->pStatus->Index.Data = 0U;
                                                /* mutex give */
                                                FuncRetCode = AmbaKAL_MutexGive(&(pAik->pStatus->Mutex));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                            }
                                        }
                                        /* aik post */
                                        if (pAik->pPost != NULL) {
                                            /* post aik ring */
                                            FuncRetCode = AmbaKAL_MutexTake(&(pAik->pPost->Ring.Mutex), AMBA_KAL_WAIT_FOREVER);
                                            if (FuncRetCode == KAL_ERR_NONE) {
                                                /* index reset */
                                                pAik->pPost->Ring.Index.Data = 0U;
                                                /* mutex give */
                                                FuncRetCode = AmbaKAL_MutexGive(&(pAik->pPost->Ring.Mutex));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                            }
                                            /* sem reset */
                                            while (AmbaKAL_SemaphoreTake(&(pAik->pPost->Sem.Data), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                                            while (AmbaKAL_SemaphoreTake(&(pAik->pPost->Sem.Timer), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                                            while (AmbaKAL_SemaphoreTake(&(pAik->pPost->Sem.Ack), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                                        }
                                        /* r2y user */
                                        if (pAik->pR2yUser != NULL) {
                                            /* r2y user */
                                            FuncRetCode = AmbaKAL_MutexTake(&(pAik->pR2yUser->Ring.Mutex), AMBA_KAL_WAIT_FOREVER);
                                            if (FuncRetCode == KAL_ERR_NONE) {
                                                /* ring reset */
                                                pAik->pR2yUser->Ring.Index = 0U;
                                                pAik->pR2yUser->Ring.Count = 0U;
                                                pAik->pR2yUser->Ring.Size = 0U;
                                                pAik->pR2yUser->Ring.pMem = NULL;
                                                /* mutex give */
                                                FuncRetCode = AmbaKAL_MutexGive(&(pAik->pR2yUser->Ring.Mutex));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                            }
                                        }
                                    }
                                    /* ring reset */
                                    if (pZone->pRing != NULL) {
                                        FuncRetCode = AmbaKAL_MutexTake(&(pZone->pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
                                        if (FuncRetCode == KAL_ERR_NONE) {
                                            /* index reset */
                                            pZone->pRing->Index.Data = 0U;
                                            /* mutex give */
                                            FuncRetCode = AmbaKAL_MutexGive(&(pZone->pRing->Mutex));
                                            if (FuncRetCode != KAL_ERR_NONE) {
                                                /* */
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    /* chain null or invalid */
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    /*  platform dependence reset */
    FuncRetCode = AmbaImgSystem_PlatformReset(VinId);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system raw capture sequence put
 *  @param[in] VinId vin id
 *  @param[in] RawCapSeq raw capture sequence
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_RawCapSeqPut(UINT32 VinId, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AmbaImgSystem_Vin[VinId].Counter.Raw = RawCapSeq;
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system ring put
 *  @param[in] ImageChanId image channel id
 *  @param[in] pPipe pointer to the ring pipe
 *  @param[in] RawCapSeq raw capture sequence
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_RingPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SYSTEM_RING_PIPE_s *pPipe, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    AMBA_IMG_SYSTEM_RING_s *pRing;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if (pPipe != NULL) {
        /* chan check */
        if ((AmbaImgSystem_Chan[VinId] != NULL) &&
            (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
            /* ctx check */
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                /* zone check */
                for (i = 0U; pCtx[i].Magic == 0xCafeU; i++) {
                    if ((ZoneId >> i) == 0U) {
                        /* done */
                        break;
                    }
                    /* zone find */
                    if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                        /* zone check */
                        if ((pCtx[i].pZone != NULL) &&
                            (pCtx[i].pZone->pRing != NULL) &&
                            (pCtx[i].pZone->pRing->pCtx != NULL)) {
                            /* ring get */
                            pRing = pCtx[i].pZone->pRing;
                            /* mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                UINT8 VarB;
                                /* wr */
                                UINT32 Wr = (UINT32) pRing->Index.Bits.Wr;
                                /* raw cap seq */
                                pRing->pCtx[Wr].RawCapSeq = RawCapSeq;
                                /* fe */
                                for (j = 0U; j < 3U; j++) {
                                    pRing->pCtx[Wr].Pipe.Wb.Fe[j] = pPipe->Wb.Fe[j];
                                }
                                /* be */
                                pRing->pCtx[Wr].Pipe.Wb.Be = pPipe->Wb.Be;
                                /* be alt */
                                pRing->pCtx[Wr].Pipe.Wb.BeAlt = pPipe->Wb.BeAlt;
                                /* index update */
                                VarB = (UINT8) (((((UINT32) pRing->Index.Bits.Wr) + 1U) % pRing->Num) & 0xFFU);
                                pRing->Index.Bits.Wr = VarB;
                                if (pRing->Index.Bits.Rd == pRing->Index.Bits.Wr) {
                                    VarB = (UINT8) (((((UINT32) pRing->Index.Bits.Rd) + 1U) % pRing->Num) & 0xFFU);
                                    pRing->Index.Bits.Rd = VarB;
                                } else {
                                    pRing->Index.Bits.Count++;
                                }
                                /* mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pRing->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* mutex fail */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* zone/ring/ctx null */
                            RetCode = NG_UL;
                        }
                    }
                }
            } else {
                /* ctx null */
                RetCode = NG_UL;
            }
        } else {
            /* chan null or invalid */
            RetCode = NG_UL;
        }
    } else {
        /* pipe null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system ring get
 *  @param[in] ImageChanId image channel id
 *  @param[in] pPipe pointer to the pointer of ring pipe
 *  @param[in] RawCapSeq raw capture sequence
 *  @return error code
 */
UINT32 AmbaImgSystem_RingGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_RING_PIPE_s **pPipe, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, k;

    UINT32 FindFlag;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    AMBA_IMG_SYSTEM_RING_s *pRing;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if (pPipe != NULL) {
        /* chan check */
        if ((AmbaImgSystem_Chan[VinId] != NULL) &&
            (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
            /* ctx check */
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                /* find flag reset */
                FindFlag = 0U;
                /* zone check */
                for (i = 0U; pCtx[i].Magic == 0xCafeU; i++) {
                    if (((ZoneId >> i) == 0U) || (FindFlag > 0U)) {
                        /* done */
                        break;
                    }
                    /* zone find */
                    if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                        /* zone null? */
                        if ((pCtx[i].pZone != NULL) &&
                            (pCtx[i].pZone->pRing != NULL) &&
                            (pCtx[i].pZone->pRing->pCtx != NULL)) {
                            /* ring get */
                            pRing = pCtx[i].pZone->pRing;
                            /* mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* wr get */
                                UINT32 Wr = pRing->Index.Bits.Wr;
                                for (k = 0U; k < pRing->Index.Bits.Count; k++) {
                                    /* past get */
                                    Wr = (Wr + (pRing->Num - 1U)) % pRing->Num;
                                    /* raw cap seq match? */
                                    if (pRing->pCtx[Wr].RawCapSeq == RawCapSeq) {
                                        break;
                                    }
                                }

                                if (k < pRing->Index.Bits.Count) {
                                    /* pipe get */
                                    *pPipe = &(pRing->pCtx[Wr].Pipe);
                                    /* found */
                                    FindFlag = 1U;
                                } else {
                                    RetCode = NG_UL;
                                }
                                /* mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pRing->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* mutex fail */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* zone/ring/ctx null */
                            RetCode = NG_UL;
                        }
                    }
                }
                /* match? */
                if (FindFlag == 0U) {
                    /* no match */
                    RetCode = NG_UL;
                }
            } else {
                /* ctx null */
                RetCode = NG_UL;
            }
        } else {
            /* chan null or invalid */
            RetCode = NG_UL;
        }
    } else {
        /* pipe null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system active aik ctx get
 *  @param[in] ImageChanId image channel id
 *  @param[in] pActAikCtx pointer to active aik ctx
 *  @return error code
 */
UINT32 AmbaImgSystem_ActAikCtxGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_AIK_CTX_s *pActAikCtx)
{
    UINT32 RetCode = NG_UL;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if (pActAikCtx != NULL) {
        /* chan check */
        if ((AmbaImgSystem_Chan[VinId] != NULL) &&
            (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
            /* ctx check */
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                /* zone check */
                for (i = 0U; pCtx[i].Magic == 0xCafeU; i++) {
                    /* zone find */
                    if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                        /* ack aik ctx get */
                        *pActAikCtx = pCtx[i].pZone->ActAikCtx;
                        /* found */
                        RetCode = OK_UL;
                        break;
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system aik status put
 *  @param[in] pZone pointer to the zone data
 *  @param[in] pCtx pointer to the aik context
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_AikStatusPut(const AMBA_IMG_SYSTEM_ZONE_s *pZone, const AMBA_IMG_SYSTEM_AIK_CTX_s *pCtx)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
     UINT8 VarB;

    AMBA_IMG_SYSTEM_AIK_STATUS_s *pStatus;

    if ((pZone != NULL) &&
        (pCtx != NULL) &&
        (pZone->pAik != NULL) &&
        (pZone->pAik->pStatus != NULL)) {
        /* aik status get */
        pStatus = pZone->pAik->pStatus;
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(pStatus->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            if (pStatus->pCtx != NULL) {
                /* wr */
                UINT32 Wr = (UINT32) pStatus->Index.Bits.Wr;
                /* ctx put */
                pStatus->pCtx[Wr] = *pCtx;
                /* wr update */
                VarB = (UINT8) (((((UINT32) pStatus->Index.Bits.Wr) + 1U) % pStatus->Num) & 0xFFU);
                pStatus->Index.Bits.Wr = VarB;
                /* rd check */
                if (pStatus->Index.Bits.Wr == pStatus->Index.Bits.Rd) {
                    /* rd update */
                    VarB = (UINT8) (((((UINT32) pStatus->Index.Bits.Rd) + 1U) % pStatus->Num) & 0xFFU);
                    pStatus->Index.Bits.Rd = VarB;
                } else {
                    /* count accumulated */
                    pStatus->Index.Bits.Count++;
                }
            } else {
                /* ctx null */
                RetCode = NG_UL;
            }
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(pStatus->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* aik/status null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system aik status get
 *  @param[in] pZone pointer to the zone data
 *  @param[out] pCtx pointer to the aik context
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_AikStatusGet(const AMBA_IMG_SYSTEM_ZONE_s *pZone, AMBA_IMG_SYSTEM_AIK_CTX_s *pCtx)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
     UINT8 VarB;

    AMBA_IMG_SYSTEM_AIK_STATUS_s *pStatus;

    if ((pZone != NULL) &&
        (pCtx != NULL) &&
        (pZone->pAik != NULL) &&
        (pZone->pAik->pStatus != NULL)) {
        /* aik status get */
        pStatus = pZone->pAik->pStatus;
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(pStatus->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == OK_UL) {
            if (pStatus->Index.Bits.Count > 0U) {
                if (pStatus->pCtx != NULL) {
                    /* rd */
                    UINT32 Rd = (UINT32) pStatus->Index.Bits.Rd;
                    /* ctx get */
                    *pCtx = pStatus->pCtx[Rd];
                    /* rd update */
                    VarB = (UINT8) (((((UINT32) pStatus->Index.Bits.Rd) + 1U) % pStatus->Num) & 0xFFU);
                    pStatus->Index.Bits.Rd = VarB;
                    /* count decrease */
                    pStatus->Index.Bits.Count--;
                } else {
                    /* ctx null */
                    RetCode = NG_UL;
                }
            } else {
                /* empty */
                RetCode = NG_UL;
            }
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(pStatus->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* aik/status null */
        RetCode = NG_UL;
    }

    return RetCode;
}
#if 0
/**
 *  @private
 *  Amba image system r2y user buffer reset
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_R2yUserBufferRst(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_USER_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                /* zone select */
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        if ((pZone->pAik != NULL) &&
                            (pZone->pAik->pR2yUser != NULL)) {
                            /* ring get */
                            pRing = &(pZone->pAik->pR2yUser->Ring);
                            /* ring mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* ring reset */
                                pRing->Index = 0U;
                                pRing->Count = 0U;
                                pRing->Size = 0U;
                                pRing->pMem = NULL;
                                /* ring mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pRing->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                        } else {
                            /* aik/r2yuser null */
                            RetCode = NG_UL;
                        }
                    } else {
                        /* zone null */
                        RetCode = NG_UL;
                    }
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}
#endif
/**
 *  Amba image system r2y user buffer configuration
 *  @param[in] ImageChanId image channel id
 *  @param[in] Size buffer size
 *  @param[in] Count buffer num
 *  @param[in] pMem pointer to the buffer memory
 *  @return error code
 */
UINT32 AmbaImgSystem_R2yUserBuffer(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pMem)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_USER_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((pMem != NULL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                /* zone select */
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        if ((pZone->pAik != NULL) && (pZone->pAik->pR2yUser != NULL)) {
                            /* ring get */
                            pRing = &(pZone->pAik->pR2yUser->Ring);
                            /* ring mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* ring settle */
                                pRing->Index = 0U;
                                pRing->Count = Count;
                                pRing->Size = Size;
                                pRing->pMem = pMem;
                                /* ring mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pRing->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                        } else {
                            /* aik/r2yuser null */
                            RetCode = NG_UL;
                        }
                    } else {
                        /* zone null */
                        RetCode = NG_UL;
                    }
                    break;
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system r2y user buffer get
 *  @param[in] ImageChanId image channel id
 *  @return pointer to the buffer memory
 *  @note this function is intended for internal use only
 */
static void *AmbaImgSystem_R2yUserBufferGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
      void *pRetMem = NULL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_USER_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* ctx check */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                /* zone select */
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        if ((pZone->pAik != NULL) &&
                            (pZone->pAik->pR2yUser != NULL)) {
                            /* ring get */
                            pRing = &(pZone->pAik->pR2yUser->Ring);
                            /* ring mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* user buffer get */
                                if (pRing->pMem != NULL) {
                                    pRetMem = &(pRing->pMem[pRing->Size * pRing->Index]);
                                    pRing->Index = (pRing->Index + 1U) % pRing->Count;
                                }
                                /* ring mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pRing->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    return pRetMem;
}

/**
 *  Amba image system r2y user delay
 *  @param[in] ImageChanId image channel id
 *  @param[in] R2yUserDelay r2y user delay (reserved time for algorithm)
 *  @param[in] R2yDelay r2y delay (r2y total delay)
 *  @return error code
 */
UINT32 AmbaImgSystem_R2yUserDelay(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 R2yUserDelay, UINT32 R2yDelay)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* ctx check */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                /* zone select */
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* r2y user dealy */
                        pZone->Op.R2yUserDelay = R2yUserDelay;
                        /* r2y delay */
                        pZone->Op.R2yDelay = R2yDelay;
                    }
                }
            }
            /* dsp r2y delay */
            FuncRetCode = AmbaImgSystem_R2yDelay(ImageChanId, R2yDelay);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system post aik put
 *  @param[in] ImageChanId image channel id
 *  @param[in] RawCapSeq raw capture sequence number
 *  @param[in] PostAikCnt post aik counter
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_PostAikPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 RawCapSeq, UINT32 PostAikCnt)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    UINT32 Wr;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;

    AMBA_IMG_SYSTEM_PAIK_s *pPostAik;
    AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* ctx check */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        if ((pZone->pAik != NULL) &&
                            (pZone->pAik->pPost != NULL)) {
                            /* post aik get */
                            pPostAik = pZone->pAik->pPost;
                            /* mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pPostAik->Ring.Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == OK_UL) {
                                /* buffer check */
                                if (pPostAik->Ring.pInfo != NULL) {
                                    if (pPostAik->Ring.Index.Bits.Count < pPostAik->Ring.Num) {
                                        /* wr get */
                                        Wr = pPostAik->Ring.Index.Bits.Wr;
                                        /* post aik info get */
                                        pPostAikInfo = &(pPostAik->Ring.pInfo[Wr]);
                                        /* image channel id */
                                        pPostAikInfo->ImageChanId.Ctx.Data = ImageChanId.Ctx.Data;
                                        /* post zone id */
                                        pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
                                        /* raw cap seq */
                                        pPostAikInfo->RawCapSeq = RawCapSeq;
                                        /* post aik cnt */
                                        pPostAikInfo->PostAikCnt = PostAikCnt;
                                        /* r2y user delay */
                                        pPostAikInfo->R2yUserDelay = pZone->Op.R2yUserDelay;
                                        /* magic clear */
                                        pPostAikInfo->Magic = 0U;
                                        /* wr update */
                                        Wr = (Wr + 1U) % pPostAik->Ring.Num;
                                        Wr &= 0xFFU;
                                        pPostAik->Ring.Index.Bits.Wr = (UINT8) Wr;
                                        /* count update */
                                        pPostAik->Ring.Index.Bits.Count++;
                                    } else {
                                        /* ring full */
                                        RetCode = NG_UL;
                                    }
                                } else {
                                    /* info null */
                                    RetCode = NG_UL;
                                }
                                /* mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pPostAik->Ring.Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                        } else {
                            /* aik/post null */
                            RetCode = NG_UL;
                        }
                    } else {
                        /* zone null */
                        RetCode = NG_UL;
                    }
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system post aik get
 *  @param[in] ImageChanId image channel id
 *  @param[out] pPostAikInfo pointer to the post aik information
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_PostAikGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_PAIK_INFO_s **pPostAikInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    UINT32 Rd;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_PAIK_s *pPostAik;

    IMAGE_SYSTEM_MEM_INFO_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* ctx check */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        if ((pZone->pAik != NULL) &&
                            (pZone->pAik->pPost != NULL)) {
                            /* post aik get */
                            pPostAik = pZone->pAik->pPost;
                            /* mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pPostAik->Ring.Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == OK_UL) {
                                /* buffer check */
                                if (pPostAik->Ring.pInfo != NULL) {
                                    if (pPostAik->Ring.Index.Bits.Count > 0U) {
                                        /* Rd get */
                                        Rd = (UINT32) pPostAik->Ring.Index.Bits.Rd;
                                        /* post aik info get */
                                        *pPostAikInfo = &(pPostAik->Ring.pInfo[Rd]);
                                        /* id */
                                        (*pPostAikInfo)->Id = Rd;
                                        /* r2y user */
                                        MemInfo.Ctx.pVoid = AmbaImgSystem_R2yUserBufferGet(ImageChanId);
                                        (*pPostAikInfo)->R2yUser = (UINT64) MemInfo.Ctx.Data;
                                        /* rd update */
                                        Rd = (Rd + 1U) % pPostAik->Ring.Num;
                                        Rd &= 0xFFU;
                                        pPostAik->Ring.Index.Bits.Rd = (UINT8) Rd;
                                        /* count update */
                                        pPostAik->Ring.Index.Bits.Count--;
                                    } else {
                                        /* ring empty */
                                        RetCode = NG_UL;
                                    }
                                } else {
                                    /* ring null */
                                    RetCode = NG_UL;
                                }
                                /* mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pPostAik->Ring.Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* mutex fail */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* aik/post null */
                            RetCode = NG_UL;
                        }
                    } else {
                        /* zone null */
                        RetCode = NG_UL;
                    }
                    break;
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system post aik go
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_PostAikGo(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_PAIK_s *pPostAik;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* ctx check */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* post aik check */
                        if ((pZone->pAik != NULL) &&
                            (pZone->pAik->pPost != NULL)) {
                            /* post aik get */
                            pPostAik = pZone->pAik->pPost;
                            /* post aik go */
                            FuncRetCode = AmbaKAL_SemaphoreGive(&(pPostAik->Sem.Data));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* sem fail */
                                RetCode = NG_UL;
                            }
                        }
                    } else {
                        /* zone null */
                        RetCode = NG_UL;
                    }
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sytem post aik wait
 *  @param[in] ImageChanId image channel id
 *  @param[out] pPostAikInfo pointer to the post aik information pointer
 *  @return error code
 */
UINT32 AmbaImgSystem_PostAikWait(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_PAIK_INFO_s **pPostAikInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 Rd2;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_PAIK_s *pPostAik;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* ctx check */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* post aik check */
                        if ((pZone->pAik != NULL) &&
                            (pZone->pAik->pPost != NULL)) {
                            /* post aik get */
                            pPostAik = pZone->pAik->pPost;
                            /* post aik wait */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&(pPostAik->Sem.Data), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* ring mutex take */
                                FuncRetCode = AmbaKAL_MutexTake(&(pPostAik->Ring.Mutex), AMBA_KAL_WAIT_FOREVER);
                                if (FuncRetCode == OK_UL) {
                                    /* rd2 get */
                                    Rd2 = (UINT32) pPostAik->Ring.Index.Bits.Rd2;
                                    /* post aik info get */
                                    *pPostAikInfo = &(pPostAik->Ring.pInfo[Rd2]);
                                    /* rd2 update */
                                    Rd2 = (Rd2 + 1U) % pPostAik->Ring.Num;
                                    pPostAik->Ring.Index.Bits.Rd2 = (UINT8) Rd2;
                                    /* ring mutex give */
                                    FuncRetCode = AmbaKAL_MutexGive(&(pPostAik->Ring.Mutex));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                }
                            } else {
                                /* sem fail */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* aik/post null */
                            RetCode = NG_UL;
                        }
                    } else {
                        /* zone null */
                        RetCode = NG_UL;
                    }
                    break;
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sytem post aik execute
 *  @param[in] pPostAikInfo pointer to the post aik information
 *  @return error code
 */
UINT32 AmbaImgSystem_PostAik(const AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 ZoneIsoDisId = 0U;

    AMBA_IMG_SYSTEM_AIK_CTX_s ActAikCtx;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pPostAikInfo != NULL) &&
        (pPostAikInfo->Magic == 0xCafeU)) {
        FuncRetCode = AmbaImgSystem_AikExecute(pPostAikInfo->ImageChanId, AIK_EXECUTE_WARP_ONLY);
        if (FuncRetCode != OK_UL) {
            /* aik execute fail */
            RetCode = NG_UL;
        } else {
            /* vin id get */
            VinId = pPostAikInfo->ImageChanId.Ctx.Bits.VinId;
            /* chain id get */
            ChainId = pPostAikInfo->ImageChanId.Ctx.Bits.ChainId;
            /* check */
            if ((pFrwImageChannel != NULL) &&
                (pFrwImageChannel[VinId] != NULL) &&
                (pFrwImageChannel[VinId][ChainId].Magic == 0xCafeU) &&
                (pFrwImageChannel[VinId][ChainId].pCtx != NULL)) {
                /* zone iso disable id get */
                ZoneIsoDisId = ((UINT32) pFrwImageChannel[VinId][ChainId].pCtx->Aik.Ctx.Bits.IsoDisId) & ((UINT32) pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId);
                /* zone iso disable check */
                if ((ZoneIsoDisId > 0U) &&
                    (pFrwImageChannel[VinId][ChainId].IsoCb != NULL)) {
                    /* image channel id get */
                    ImageChanId.Ctx.Data = pPostAikInfo->ImageChanId.Ctx.Data;
                    /* post zone iso disable found */
                    for (i = 0U; (i < AMBA_IMGFRW_NUM_FOV_CHANNEL) && ((ZoneIsoDisId >> i) > 0U); i++) {
                        if ((ZoneIsoDisId & (((UINT32) 1U) << i)) > 0U) {
                            /* post zone id assign */
                            ImageChanId.Ctx.Bits.ZoneId = ((UINT32) 1U) << i;
                            /* active post aik ctx get */
                            FuncRetCode = AmbaImgSystem_ActAikCtxGet(ImageChanId, &(ActAikCtx));
                            if (FuncRetCode == OK_UL) {
                                AMBA_IMG_CHANNEL_ISO_INFO_s IsoInfo;
                                /* raw cap seq put */
                                IsoInfo.RawCapSeq = pPostAikInfo->RawCapSeq;
                                /* post aik cnt put */
                                IsoInfo.AikCnt = pPostAikInfo->PostAikCnt;
                                /* vin skip frame put */
                                IsoInfo.VinSkipFrame = pPostAikInfo->VinSkipFrame;
                                /* zone id put */
                                IsoInfo.ZoneId = i | ((UINT32) 0x80000000U);
                                /* context id put */
                                IsoInfo.ContextId = AmbaImgSystem_ContextIdGet(i);
                                /* iso ik id put */
                                IsoInfo.IkId = ActAikCtx.Id;
                                /* iso cfg put */
                                IsoInfo.pIsoCfg = ActAikCtx.pIsoCfg;
                                /* user data put */
                                IsoInfo.UserData = pPostAikInfo->R2yUser;
                                /* iso cb */
                                FuncRetCode = pFrwImageChannel[VinId][ChainId].IsoCb(&IsoInfo);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PIso_Cb");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PIso_Cb!");
                                }
#if 1
                                /* debug message */
                                if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                                    IMAGE_SYSTEM_MEM_INFO_s MemInfo;
                                    char str[11];
                                    str[0] = 'r';str[1] = ' ';
                                    var_utoa(IsoInfo.RawCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 'n';
                                    var_utoa(IsoInfo.AikCnt, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 's';
                                    var_utoa(IsoInfo.VinSkipFrame, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 'f';
                                    var_utoa(IsoInfo.ZoneId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 'c';
                                    var_utoa(IsoInfo.ContextId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 'k';
                                    var_utoa(IsoInfo.IkId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 'i';
                                    MemInfo.Ctx.pVoid = IsoInfo.pIsoCfg;
                                    var_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 'u';
                                    var_utoa((UINT32) IsoInfo.UserData, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                }
#endif
                            } else {
                                /* aik ctx fail */
                                RetCode = NG_UL;
                            }
                        }
                    }
                }
            }
        }
    } else {
        /* null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sytem r2y user wait
 *  @param[in] pPostAikInfo pointer to the post aik information
 *  @return error code
 */
UINT32 AmbaImgSystem_R2yUserWait(const AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    UINT32 Timetick;
    UINT32 Timetick2;
    UINT32 TimetickW;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_PAIK_s *pPostAik;

    if ((pPostAikInfo != NULL) &&
        (pPostAikInfo->Magic == 0xCafeU)) {
        /* vin id get */
        VinId = pPostAikInfo->ImageChanId.Ctx.Bits.VinId;
        /* chain id get */
        ChainId = pPostAikInfo->ImageChanId.Ctx.Bits.ChainId;
        /* post zone id get */
        PostZoneId = pPostAikInfo->ImageChanId.Ctx.Bits.ZoneId;
        if ((AmbaImgSystem_Chan[VinId] != NULL) &&
            (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
            if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                /* ctx check */
                for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                    if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                        /* zone check */
                        if (pCtx[i].pZone != NULL) {
                            /* zone get */
                            pZone = pCtx[i].pZone;
                            /* post aik check */
                            if ((pZone->pAik != NULL) &&
                                (pZone->pAik->pPost != NULL)) {
                                /* post aik get */
                                pPostAik = pZone->pAik->pPost;
                                /* timetick get */
                                FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    Timetick2 = pPostAikInfo->EorTimetick + pPostAikInfo->R2yUserDelay;
                                    if (Timetick2 > Timetick) {
                                        TimetickW = Timetick2 - Timetick;
                                        /* sem timer clr */
                                        while (AmbaKAL_SemaphoreTake(&(pPostAik->Sem.Timer), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                                        /* sem timer wait */
                                        FuncRetCode = AmbaKAL_SemaphoreTake(&(pPostAik->Sem.Timer), TimetickW);
                                        if (FuncRetCode != KAL_ERR_NONE) {
                                            /* timeout */
                                            RetCode = NG_UL;
                                        }
                                    } else {
                                        /* too late */
                                        RetCode = NG_UL;
                                    }
                                } else {
                                    /* timetick fail */
                                    RetCode = NG_UL;
                                }
                                /* ack mutex take  */
                                FuncRetCode = AmbaKAL_MutexTake(&(pPostAik->Mtx.Ack), AMBA_KAL_WAIT_FOREVER);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    /* ack sem give */
                                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pPostAik->Sem.Ack));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                    /* ack mutex give */
                                    FuncRetCode = AmbaKAL_MutexGive(&(pPostAik->Mtx.Ack));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                }
                            } else {
                                /* aik/post null */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* zone null */
                            RetCode = NG_UL;
                        }
                        break;
                    }
                }
            } else {
                /* ctx null */
                RetCode = NG_UL;
            }
        } else {
            /* chan null */
            RetCode = NG_UL;
        }
    } else {
        /* post aik null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sytem r2y user done
 *  @param[in] PostAikId post aik id (pointer to the post aik info)
 *  @return error code
 */
UINT32 AmbaImgSystem_R2yUserDone(UINT64 PostAikId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 PostZoneId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    const AMBA_IMG_SYSTEM_ZONE_s *pZone;
    AMBA_IMG_SYSTEM_PAIK_s *pPostAik;

    IMAGE_SYSTEM_MEM_INFO_s MemInfo;

    if (PostAikId != 0ULL) {
        MemInfo.Ctx.Data = (AMBA_IMG_MEM_ADDR) PostAikId;
        if (MemInfo.Ctx.pPostAikInfo->Magic == 0xCafeU) {
            /* image channel id get */
            ImageChanId.Ctx.Data = MemInfo.Ctx.pPostAikInfo->ImageChanId.Ctx.Data;
            /* vin id get */
            VinId = (UINT32) ImageChanId.Ctx.Bits.VinId;
            /* chain id get */
            ChainId = (UINT32) ImageChanId.Ctx.Bits.ChainId;
            /* post zone id get */
            PostZoneId = (UINT32) ImageChanId.Ctx.Bits.ZoneId;
            if ((AmbaImgSystem_Chan[VinId] != NULL) &&
                (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                    /* ctx get */
                    pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                    /* ctx check */
                    for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((PostZoneId >> i) > 0U); i++) {
                        if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                            /* zone check */
                            if (pCtx[i].pZone != NULL) {
                                /* zone get */
                                pZone = pCtx[i].pZone;
                                /* post aik check */
                                if ((pZone->pAik != NULL) &&
                                    (pZone->pAik->pPost != NULL)) {
                                    /* post aik get */
                                    pPostAik = pZone->pAik->pPost;
                                    /* ack mutex take */
                                    FuncRetCode = AmbaKAL_MutexTake(&(pPostAik->Mtx.Ack), AMBA_KAL_WAIT_FOREVER);
                                    if (FuncRetCode == KAL_ERR_NONE) {
                                        FuncRetCode = AmbaKAL_SemaphoreTake(&(pPostAik->Sem.Ack), AMBA_KAL_NO_WAIT);
                                        if (FuncRetCode == KAL_ERR_NONE) {
                                            /* timeout already */
                                            RetCode = NG_UL;
                                            /* ack mutex give */
                                            FuncRetCode = AmbaKAL_MutexGive(&(pPostAik->Mtx.Ack));
                                            if (FuncRetCode != KAL_ERR_NONE) {
                                                /* */
                                            }
                                        } else {
                                            /* sem timer stop */
                                            FuncRetCode = AmbaKAL_SemaphoreGive(&(pPostAik->Sem.Timer));
                                            if (FuncRetCode == KAL_ERR_NONE) {
                                                /* ack mutex give */
                                                FuncRetCode = AmbaKAL_MutexGive(&(pPostAik->Mtx.Ack));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                                /* ack wait */
                                                FuncRetCode = AmbaKAL_SemaphoreTake(&(pPostAik->Sem.Ack), AMBA_KAL_WAIT_FOREVER);
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* ack fail */
                                                    RetCode = NG_UL;
                                                }
                                            } else {
                                                /* stop fail */
                                                RetCode = NG_UL;
                                                /* ack mutex give */
                                                FuncRetCode = AmbaKAL_MutexGive(&(pPostAik->Mtx.Ack));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                            }
                                        }
                                    }
                                } else {
                                    /* aik/post null */
                                    RetCode = NG_UL;
                                }
                            } else {
                                /* zone null */
                                RetCode = NG_UL;
                            }
                            break;
                        }
                    }
                } else {
                    /* ctx null */
                    RetCode = NG_UL;
                }
            } else {
                /* chan null */
                RetCode = NG_UL;
            }
        } else {
            /* post aik invalid */
            RetCode = NG_UL;
        }
    } else {
        /* post aik null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sytem aik execute
 *  @param[in] ImageChanId image channel id
 *  @return error code
 */
UINT32 AmbaImgSystem_AikExec(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL) &&
        (pFrwImageChannel[VinId][ChainId].Magic == 0xCafeU)) {
        /* image channel id get */
        ImageChanId2.Ctx.Data = pFrwImageChannel[VinId][ChainId].pCtx->Id.Ctx.Data;
        /* aik execute */
        FuncRetCode = AmbaImgSystem_AikExecute(ImageChanId2, AIK_EXECUTE_ATTACH);
        if (FuncRetCode != OK_UL) {
            /* aik execute fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system vin captue line get
 *  @param[in] VinId vin id
 *  @param[in] ChainId chain id in vin
 *  @return capture line
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_VinCaptureLineGet(UINT32 VinId, UINT32 ChainId)
{
    UINT32 CaptureLine = 0U;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSystem_Chan[VinId][ChainId].pInfo != NULL) {
            CaptureLine = AmbaImgSystem_Chan[VinId][ChainId].pInfo->Vin.Cap.Bits.Height;
        }
    }

    return CaptureLine;
}

/**
 *  Amba image system context id get
 *  @param[in] ZoneIndex zone index
 *  @return context id
 */
UINT32 AmbaImgSystem_ContextIdGet(UINT32 ZoneIndex)
{
    /* zone index to ik context id */
    return AmbaImgSystem_IkContextIdGet(ZoneIndex);
}

/**
 *  Amba image system image mode set
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode (AMBA_IK_PIPE_VIDEO/AMBA_IK_PIPE_STILL)
 *  @param[in] pImgMode pointer to the image mode configuration
 *  @return error code
 */
UINT32 AmbaImgSystem_ImageModeSet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;
    UINT32 SetCount = 0U;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    AMBA_IMG_SYSTEM_PIPE_s *pPipe;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((pImgMode != NULL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* zone find */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((ZoneId >> i) > 0U); i++) {
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* pipe get */
                        if (Mode == AMBA_IK_PIPE_VIDEO) {
                            /* video image pipe */
                            pPipe = &(pZone->Pipe[0]);
                        } else {
                            /* still image pipe */
                            pPipe = &(pZone->Pipe[1]);
                        }
                        /* pipe mutex take */
                        FuncRetCode = AmbaKAL_MutexTake(&(pPipe->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* video image mode */
                            FuncRetCode = AmbaWrap_memcpy(&(pPipe->Mode), pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            /* counter */
                            SetCount++;
                            /* pipe mutex give */
                            FuncRetCode = AmbaKAL_MutexGive(&(pPipe->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    }
                }
            }

            if (SetCount == 0U) {
                /* not found */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system image mode get
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode (AMBA_IK_PIPE_VIDEO/AMBA_IK_PIPE_STILL)
 *  @param[out] pImgMode pointer to the image mode configuration pointer
 *  @return error code
 */
UINT32 AmbaImgSystem_ImageModeGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, AMBA_IK_MODE_CFG_s **pImgMode)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;
    UINT32 GetCount = 0U;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    AMBA_IMG_SYSTEM_PIPE_s *pPipe;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((pImgMode != NULL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* zone find */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((ZoneId >> i) > 0U); i++) {
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* pipe get */
                        if (Mode == AMBA_IK_PIPE_VIDEO) {
                            /* video image pipe */
                            pPipe = &(pZone->Pipe[0]);
                        } else {
                            /* still image pipe */
                            pPipe = &(pZone->Pipe[1]);
                        }
                        /* image mode get */
                        *pImgMode = &(pPipe->Mode);
                        /* counter */
                        GetCount++;
                        break;
                    }
                }
            }

            if (GetCount == 0U) {
                /* not found */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system black level write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode (AMBA_IK_PIPE_VIDEO/AMBA_IK_PIPE_STILL)
 *  @param[in] pOffset pointer to the black level data
 *  @return error code
 */
UINT32 AmbaImgSystem_BlcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const INT32 *pOffset)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;
    UINT32 WriteCount = 0U;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    AMBA_IMG_SYSTEM_PIPE_s *pPipe;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_PIPE_BLC_s *pBlc;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((pOffset != NULL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            /* zone find */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((ZoneId >> i) > 0U); i++) {
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* pipe get */
                        if (Mode == AMBA_IK_PIPE_VIDEO) {
                            /* video */
                            pPipe = &(pZone->Pipe[0]);
                        } else {
                            /* still */
                            pPipe = &(pZone->Pipe[1]);
                        }
                        /* blc get */
                        pBlc = &(pPipe->Blc);
                        /* zone id get */
                        ImageChanId2.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
                        /* pipe mutex take */
                        FuncRetCode = AmbaKAL_MutexTake(&(pPipe->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* blc mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pBlc->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                for(j = 0U; j < 4U; j++) {
                                    /* offset get */
                                    pBlc->Offset[j] = pOffset[j];
                                }
                                /* blc write */
                                FuncRetCode = AmbaImgSystem_StaticBlcWrite(ImageChanId2, Mode, pBlc, i);
                                if (FuncRetCode != OK_UL) {
                                    /* write fail */
                                    RetCode = NG_UL;
                                } else {
                                    /* write ok */
                                    WriteCount++;
                                }
                                /* blc mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pBlc->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* blc mutex fail */
                                RetCode = NG_UL;
                            }
                            /* pipe mutex give */
                            FuncRetCode = AmbaKAL_MutexGive(&(pPipe->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        } else {
                            /* pipe mutex fail */
                            RetCode = NG_UL;
                        }
                    }
                }
            }

            if (WriteCount == 0U) {
                /* no write */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system black level read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode (AMBA_IK_PIPE_VIDEO/AMBA_IK_PIPE_STILL)
 *  @param[out] pOffset pointer to the black level data
 *  @return error code
 */
UINT32 AmbaImgSystem_BlcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, INT32 *pOffset)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;
    UINT32 ReadCount = 0U;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    AMBA_IMG_SYSTEM_PIPE_s *pPipe;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_PIPE_BLC_s *pBlc;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((pOffset != NULL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* zone find */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((ZoneId >> i) > 0U); i++) {
                /* zone check */
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* pipe get */
                        if (Mode == AMBA_IK_PIPE_VIDEO) {
                            pPipe = &(pZone->Pipe[0]);
                        } else {
                            pPipe = &(pZone->Pipe[1]);
                        }
                        /* blc get */
                        pBlc = &(pPipe->Blc);
                        /* pipe mutex take */
                        FuncRetCode = AmbaKAL_MutexTake(&(pPipe->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* blc mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pBlc->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* offset get */
                                for(j = 0U; j < 4U; j++) {
                                    pOffset[j] = pBlc->Offset[j];
                                }
                                ReadCount++;
                                /* blc mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pBlc->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* blc mutex fail */
                                RetCode = NG_UL;
                            }
                            /* pipe mutex give */
                            FuncRetCode = AmbaKAL_MutexGive(&(pPipe->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        } else {
                            /* pipe mutex fail */
                            RetCode = NG_UL;
                        }
                        /* count check */
                        if (ReadCount > 0U) {
                            break;
                        }
                    }
                }
            }

            if (ReadCount == 0U) {
                /* not found */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system white balance gain alternate enable
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] Type operation type
 *  @param[in] Enable enable or disable
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSystem_WbGainAltEnable(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Type, UINT32 Enable)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;
    UINT32 EnableCount = 0U;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    UINT32 WbId = Mode >> 16U;
    UINT32 PipeMode = Mode % 0x10000U;

    AMBA_IMG_SYSTEM_PIPE_s *pPipe;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_PIPE_WB_s *pWb;

    AMBA_IMG_SYSTEM_PIPE_WB_CTX_s *pWbCtx = NULL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((WbId == 0U) || (WbId >= (UINT32) WB_ID_TOTAL)) {
        RetCode = NG_UL;
    }

    if ((RetCode == OK_UL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* zone find */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((ZoneId >> i) > 0U); i++) {
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* pipe get */
                        if (PipeMode == AMBA_IK_PIPE_VIDEO) {
                            pPipe = &(pZone->Pipe[0]);
                        } else {
                            pPipe = &(pZone->Pipe[1]);
                        }
                        /* wb get */
                        pWb = &(pPipe->Wb);
                        /* wb ctx get */
                        switch (WbId) {
                            case (UINT32) WB_ID_FE:
                                pWbCtx = &(pWb->Fe[0]);
                                break;
                            case (UINT32) WB_ID_FE1:
                                pWbCtx = &(pWb->Fe[1]);
                                break;
                            case (UINT32) WB_ID_FE2:
                                pWbCtx = &(pWb->Fe[2]);
                                break;
                            case (UINT32) WB_ID_BE:
                                pWbCtx = &(pWb->Be);
                                break;
                            case (UINT32) WB_ID_BE_ALT:
                                pWbCtx = &(pWb->BeAlt);
                                break;
                            default:
                                /* */
                                break;
                        }
                        if (pWbCtx != NULL) {
                            /* pipe mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pPipe->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* wb mutex take */
                                FuncRetCode = AmbaKAL_MutexTake(&(pWbCtx->Mutex), AMBA_KAL_WAIT_FOREVER);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    switch (Type) {
                                        case (UINT32) WB_GAIN_OP_MIX:
                                            /* mix */
                                            pWbCtx->Op.Bits.Mix = (UINT8) (Enable & 0xFFU);
                                            EnableCount++;
                                            break;
                                        case (UINT32) WB_GAIN_OP_FINE:
                                            /* fine */
                                            pWbCtx->Op.Bits.Fine = (UINT8) (Enable & 0xFFU);
                                            EnableCount++;
                                            break;
                                        default:
                                            /* nothing to do */
                                            break;
                                    }
                                    /* wb mutex give */
                                    FuncRetCode = AmbaKAL_MutexGive(&(pWbCtx->Mutex));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                } else {
                                    /* wb mutex fail */
                                    RetCode = NG_UL;
                                }
                                /* pipe mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pPipe->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* pipe mutex fail */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* wb ctx null */
                            RetCode = NG_UL;
                        }
                    }
                }
            }

            if (EnableCount == 0U) {
                /* not found */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system white balance mix gain enable
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] Enable enable/disable
 *  @return error code
 */
UINT32 AmbaImgSystem_WbGainMix(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Enable)
{
    return AmbaImgSystem_WbGainAltEnable(ImageChanId, Mode, (UINT32) WB_GAIN_OP_MIX, Enable);
}

/**
 *  Amba image system white balance fine gain enable
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] Enable enable/disable
 *  @return error code
 */
UINT32 AmbaImgSystem_WbGainFine(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Enable)
{
    return AmbaImgSystem_WbGainAltEnable(ImageChanId, Mode, (UINT32) WB_GAIN_OP_FINE, Enable);
}

/**
 *  Amba image system gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] Type gain type
 *  @param[in] pGain pointer to the gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSystem_GainWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Type, const UINT32 *pGain)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;
    UINT32 WriteCount = 0U;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    UINT32 WbId = Mode >> 16U;
    UINT32 PipeMode = Mode % 0x10000U;

    AMBA_IMG_SYSTEM_PIPE_s *pPipe;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_PIPE_WB_s *pWb;

    AMBA_IMG_SYSTEM_PIPE_WB_CTX_s *pWbCtx = NULL;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;
    UINT32 *pSrcGain;

    AMBA_IMG_SYSTEM_GAIN_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    CmdMsg.Ctx.Data = Type;

    if ((WbId == 0U) || (WbId >= (UINT32) WB_ID_TOTAL)) {
        RetCode = NG_UL;
    }

    if ((RetCode == OK_UL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            /* zone find */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((ZoneId >> i) > 0U); i++) {
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* pipe get */
                        if (PipeMode == AMBA_IK_PIPE_VIDEO) {
                            pPipe = &(pZone->Pipe[0]);
                        } else {
                            pPipe = &(pZone->Pipe[1]);
                        }
                        /* wb get */
                        pWb = &(pPipe->Wb);
                        /* wb ctx get */
                        switch (WbId) {
                            case (UINT32) WB_ID_FE:
                                pWbCtx = &(pWb->Fe[0]);
                                break;
                            case (UINT32) WB_ID_FE1:
                                pWbCtx = &(pWb->Fe[1]);
                                break;
                            case (UINT32) WB_ID_FE2:
                                pWbCtx = &(pWb->Fe[2]);
                                break;
                            case (UINT32) WB_ID_BE:
                                pWbCtx = &(pWb->Be);
                                break;
                            case (UINT32) WB_ID_BE_ALT:
                                pWbCtx = &(pWb->BeAlt);
                                break;
                            default:
                                /* */
                                break;
                        }
                        /* zone id get */
                        ImageChanId2.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
                        if (pWbCtx != NULL) {
                            /* pipe mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pPipe->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* wb mutex take */
                                FuncRetCode = AmbaKAL_MutexTake(&(pWbCtx->Mutex), AMBA_KAL_WAIT_FOREVER);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    switch (CmdMsg.Ctx.Bits.Cmd) {
                                        case (UINT8) GAIN_CMD_WGC:
                                            /* wgc src get */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = pWbCtx->Wgc;
                                            } else {
                                                pSrcGain = pWbCtx->FineWgc;
                                            }
                                            /* wgc set */
                                            for(j = 0U; j < 3U; j++) {
                                                pSrcGain[j] = pGain[j];
                                            }
                                            break;
                                        case (UINT8) GAIN_CMD_IGC:
                                            /* igc src set */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = &(pWbCtx->Igc);
                                            } else {
                                                pSrcGain = &(pWbCtx->FineIgc);
                                            }
                                            /* igc set */
                                            *pSrcGain = *pGain;
                                            break;
                                        case (UINT8) GAIN_CMD_DGC:
                                            /* dgc src set */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = &(pWbCtx->Dgc);
                                            } else {
                                                pSrcGain = &(pWbCtx->FineDgc);
                                            }
                                            /* dgc set */
                                            *pSrcGain = *pGain;
                                            break;
                                        case (UINT8) GAIN_CMD_GGC:
                                            /* dgc src set */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = &(pWbCtx->Ggc);
                                            } else {
                                                pSrcGain = &(pWbCtx->FineGgc);
                                            }
                                            /* ggc set */
                                            *pSrcGain = *pGain;
                                            break;
                                        default:
                                            /* nothing to do */
                                            break;
                                    }
                                    /* wb write */
                                    FuncRetCode = AmbaImgSystem_WbGainWrite(ImageChanId2, Mode, pWbCtx, i);
                                    if (FuncRetCode != OK_UL) {
                                        /* write fail */
                                        RetCode = NG_UL;
                                    } else {
                                        /* write ok */
                                        WriteCount++;
                                    }
                                    /* wb mutex give */
                                    FuncRetCode = AmbaKAL_MutexGive(&(pWbCtx->Mutex));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                } else {
                                    /* wb mutex fail */
                                    RetCode = NG_UL;
                                }
                                /* pipe mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pPipe->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* pipe mutex fail */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* wb ctx null */
                            RetCode = NG_UL;
                        }
                    }
                }
            }

            if (WriteCount == 0U) {
                /* not found */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system white balance gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pWgc pointer to the white balance gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pWgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_WGC | (UINT32) GAIN_TYPE_NORMAL), pWgc);
}

/**
 *  Amba image system white balance fine gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pWgc pointer to the white balance fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineWgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pWgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_WGC | (UINT32) GAIN_TYPE_FINE), pWgc);
}

/**
 *  @private
 *  Amba image system HDR DSP digital gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pDgc pointer to the DSP digital gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_HdrDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    const AMBA_IMG_SYSTEM_INFO_s *pInfo;

    UINT32 ExposureNum;
    UINT32 WbId = Mode >> 16U;
    UINT32 PipeMode = Mode & 0xFFFFU;

    if (ImageChanId.Ctx.Bits.VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* info get */
        pInfo = AmbaImgSystem_Chan[ImageChanId.Ctx.Bits.VinId][ImageChanId.Ctx.Bits.ChainId].pInfo;
        /* fe? */
        if (WbId == (UINT32) WB_ID_FE) {
            if (pInfo->Pipe.Hdr.Bits.Enable == 1U/*dol-hdr*/) {
                ExposureNum = pInfo->Pipe.Hdr.Bits.ExposureNum;
            } else {
                ExposureNum = 1U;
            }
            /* fe hdr gain write */
            for (i = 0U; i < ExposureNum; i++) {
                FuncRetCode = AmbaImgSystem_GainWrite(ImageChanId, PipeMode | (WbId << 16U), ((UINT32) GAIN_DGC | (UINT32) GAIN_TYPE_NORMAL), &(pDgc[i]));
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
                WbId++;
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image system ir gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pIgc pointer to the ir gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_IgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pIgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_IGC | (UINT32) GAIN_TYPE_NORMAL), pIgc);
}

/**
 *  Amba image system ir fine gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pIgc pointer to the ir fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineIgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pIgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_IGC | (UINT32) GAIN_TYPE_FINE), pIgc);
}

/**
 *  Amba image system digital gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pDgc pointer to the digital gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_DGC | (UINT32) GAIN_TYPE_NORMAL), pDgc);
}

/**
 *  Amba image system digital fine gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pDgc pointer to the digital fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_DGC | (UINT32) GAIN_TYPE_FINE), pDgc);
}

/**
 *  Amba image system global gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pGgc pointer to the global gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_GgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pGgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_GGC | (UINT32) GAIN_TYPE_NORMAL), pGgc);
}

/**
 *  Amba image system golbal fine gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pGgc pointer to the global fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineGgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pGgc)
{
    return AmbaImgSystem_GainWrite(ImageChanId, Mode, ((UINT32) GAIN_GGC | (UINT32) GAIN_TYPE_FINE), pGgc);
}

/**
 *  Amba image system gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] Type gain type
 *  @param[out] pGain pointer to the gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSystem_GainRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Type, UINT32 *pGain)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;
    UINT32 ReadCount = 0U;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    UINT32 WbId = Mode >> 16U;
    UINT32 PipeMode = Mode % 0x10000U;

    AMBA_IMG_SYSTEM_PIPE_s *pPipe;
    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;
    AMBA_IMG_SYSTEM_PIPE_WB_s *pWb;

    AMBA_IMG_SYSTEM_PIPE_WB_CTX_s *pWbCtx = NULL;

    const UINT32 *pSrcGain;

    AMBA_IMG_SYSTEM_GAIN_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    CmdMsg.Ctx.Data = Type;

    if ((WbId == 0U) || (WbId >= (UINT32) WB_ID_TOTAL)) {
        RetCode = NG_UL;
    }

    if ((RetCode == OK_UL) &&
        (pGain != NULL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* zone find */
            for (i = 0U; (pCtx[i].Magic == 0xCafeU) && ((ZoneId >> i) > 0U); i++) {
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* pipe get */
                        if (PipeMode == AMBA_IK_PIPE_VIDEO) {
                            pPipe = &(pZone->Pipe[0]);
                        } else {
                            pPipe = &(pZone->Pipe[1]);
                        }
                        /* wb get */
                        pWb = &(pPipe->Wb);
                        /* wb ctx get */
                        switch (WbId) {
                            case (UINT32) WB_ID_FE:
                                pWbCtx = &(pWb->Fe[0]);
                                break;
                            case (UINT32) WB_ID_FE1:
                                pWbCtx = &(pWb->Fe[1]);
                                break;
                            case (UINT32) WB_ID_FE2:
                                pWbCtx = &(pWb->Fe[2]);
                                break;
                            case (UINT32) WB_ID_BE:
                                pWbCtx = &(pWb->Be);
                                break;
                            case (UINT32) WB_ID_BE_ALT:
                                pWbCtx = &(pWb->BeAlt);
                                break;
                            default:
                                /* */
                                break;
                        }
                        if (pWbCtx != NULL) {
                            /* pipe mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pPipe->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* wb mutex take */
                                FuncRetCode = AmbaKAL_MutexTake(&(pWbCtx->Mutex), AMBA_KAL_WAIT_FOREVER);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    switch (CmdMsg.Ctx.Bits.Cmd) {
                                        case (UINT8) GAIN_CMD_WGC:
                                            /* wgc src get */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = pWbCtx->Wgc;
                                            } else {
                                                pSrcGain = pWbCtx->FineWgc;
                                            }
                                            /* wgc get */
                                            for(j = 0U; j < 3U; j++) {
                                                pGain[j] = pSrcGain[j];
                                            }
                                            ReadCount++;
                                            break;
                                        case (UINT8) GAIN_CMD_IGC:
                                            /* igc src get */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = &(pWbCtx->Igc);
                                            } else {
                                                pSrcGain = &(pWbCtx->FineIgc);
                                            }
                                            /* igc read */
                                            *pGain = *pSrcGain;
                                            ReadCount++;
                                            break;
                                        case (UINT8) GAIN_CMD_DGC:
                                            /* dgc src get */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = &(pWbCtx->Dgc);
                                            } else {
                                                pSrcGain = &(pWbCtx->FineDgc);
                                            }
                                            /* dgc read */
                                            *pGain = *pSrcGain;
                                            ReadCount++;
                                            break;
                                        case (UINT8) GAIN_CMD_GGC:
                                            /* ggc src get */
                                            if (CmdMsg.Ctx.Bits.Type == (UINT8) GAIN_TYPE_NORMAL) {
                                                pSrcGain = &(pWbCtx->Ggc);
                                            } else {
                                                pSrcGain = &(pWbCtx->FineGgc);
                                            }
                                            /* ggc read */
                                            *pGain = *pSrcGain;
                                            ReadCount++;
                                            break;
                                        default:
                                            /* nothing to do */
                                            break;
                                    }
                                    /* wb mutex give */
                                    FuncRetCode = AmbaKAL_MutexGive(&(pWbCtx->Mutex));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                } else {
                                    /* wb mutex fail */
                                    RetCode = NG_UL;
                                }
                                /* pipe mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pPipe->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            } else {
                                /* pipe mutex fail */
                                RetCode = NG_UL;
                            }

                            if (ReadCount > 0U) {
                                break;
                            }
                        } else {
                            /* wb ctx null */
                            RetCode = NG_UL;
                        }
                    }
                }
            }

            if (ReadCount == 0U) {
                /* no write */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system white balance gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pWgc pointer to the while balance gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_WgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pWgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_WGC | (UINT32) GAIN_TYPE_NORMAL), pWgc);
}

/**
 *  Amba image system white balance fine gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pWgc pointer to the while balance fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineWgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pWgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_WGC | (UINT32) GAIN_TYPE_FINE), pWgc);
}

/**
 *  Amba image system ir gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pIgc pointer to the ir gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_IgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pIgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_IGC | (UINT32) GAIN_TYPE_NORMAL), pIgc);
}

/**
 *  Amba image system ir fine gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pIgc pointer to the ir fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineIgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pIgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_IGC | (UINT32) GAIN_TYPE_FINE), pIgc);
}

/**
 *  Amba image system digital gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pDgc pointer to the digital gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_DgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pDgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_DGC | (UINT32) GAIN_TYPE_NORMAL), pDgc);
}

/**
 *  Amba image system digital fine gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pDgc pointer to the digital fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineDgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pDgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_DGC | (UINT32) GAIN_TYPE_FINE), pDgc);
}

/**
 *  Amba image system global gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pGgc pointer to the global gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_GgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pGgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_GGC | (UINT32) GAIN_TYPE_NORMAL), pGgc);
}

/**
 *  Amba image system global fine gain read
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[out] pGgc pointer to the global fine gain data
 *  @return error code
 */
UINT32 AmbaImgSystem_FineGgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pGgc)
{
    return AmbaImgSystem_GainRead(ImageChanId, Mode, ((UINT32) GAIN_GGC | (UINT32) GAIN_TYPE_FINE), pGgc);
}

/**
 *  @private
 *  Amba image system aik process
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_AikProcess(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL) &&
        (pFrwImageChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pFrwImageChannel[VinId][ChainId].AikProc != NULL) {
            FuncRetCode = pFrwImageChannel[VinId][ChainId].AikProc(ImageChanId, Msg, User);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        } else {
            /* proc null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system efov process
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_EFovProcess(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL) &&
        (pFrwImageChannel[VinId][ChainId].Magic == 0xCafeU)) {
        if (pFrwImageChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AE] != NULL) {
            FuncRetCode = pFrwImageChannel[VinId][ChainId].EFovTxCb[IMG_CHAN_EFOV_CB_ID_AE](ImageChanId, Msg, User);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        } else {
            /* proc null */
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

