/**
 *  @file AmbaImgChannel.c
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
 *  @details Amba Image Channel
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"

#include "AmbaImgChannel.h"
#include "AmbaImgChannel_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

AMBA_IMG_CHANNEL_s **pFrwImageChannel = NULL;

/**
 *  Amba image channel register
 *  @param[in] pImageChannel pointer to the image channel pointer
 */
void AmbaImgChannel_Register(AMBA_IMG_CHANNEL_s **pImageChannel)
{
    pFrwImageChannel = pImageChannel;
}

/**
 *  Amba image channel total zone id get by vin
 *  @param[in] VinId vin id
 *  @return zone id (bits)
 */
UINT32 AmbaImgChannel_TotalZoneIdGet(UINT32 VinId)
{
    UINT32 TotalZoneId = 0U;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    /* chan check */
    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL)) {
        /* chan find */
        for (UINT32 i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* ctx check */
            if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                UINT32 ChainId;
                UINT32 ZoneId;
                /* get image channel id */
                ImageChanId.Ctx.Data = pFrwImageChannel[VinId][i].pCtx->Id.Ctx.Data;
                /* vr map get */
                pVrMap = &(pFrwImageChannel[VinId][i].pCtx->VrMap);
                /* chain id get */
                ChainId = ImageChanId.Ctx.Bits.ChainId;
                /* vr check */
                if (pVrMap->Id.Ctx.Bits.VrId > 0U) {
                    /* vr master check */
                    if ((((UINT32) pVrMap->Id.Ctx.Bits.VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                        (((UINT32) pVrMap->Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* master */
                        ZoneId = ImageChanId.Ctx.Bits.ZoneId;
                    } else {
                        /* slave */
                        ZoneId = 0U;
                    }
                } else {
                    /* non vr */
                    ZoneId = ImageChanId.Ctx.Bits.ZoneId;
                }
                /* accumulate zone id */
                TotalZoneId |= ZoneId;
            }
        }
    }

    return TotalZoneId;
}

/**
 *  Amba image channel vr map set
 *  @param[in] ImageChanId image channel id
 *  @param[in] pImageChannel pointer to the image channel pointer
 *  @return error code
 *  @note the function is intended for internal use only
 */
static UINT32 AmbaImgChannel_VrMapSet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_CHANNEL_s * const *pImageChannel)
{
    UINT32 RetCode = OK_UL;
    UINT32 m, n;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 mVinId;
    UINT32 mChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    mVinId = VinId;
    mChainId = ChainId;

    if ((pImageChannel != NULL) &&
        (pImageChannel[VinId] != NULL) &&
        (pImageChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pImageChannel[VinId][ChainId].pCtx != NULL)) {
        /* is vr */
        if (pImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId > 0U) {
            /* is vr master */
            if (((pImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                ((pImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                /* vr master */
            } else {
                /* vr slave */
                UINT32 i;

                /* find master vin id */
                for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
                    if ((pImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId & (((UINT32) 1U) << i)) > 0U) {
                        mVinId = i;
                        break;
                    }
                }
                /* is found? */
                if (i == AMBA_IMGFRW_NUM_VIN_CHANNEL) {
                    RetCode = NG_UL;
                }

                /* find master chain id */
                for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_ALGO; i++) {
                    if ((pImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << i)) > 0U) {
                        mChainId = i;
                        break;
                    }
                }
                /* is found? */
                if (i == AMBA_IMGFRW_NUM_VIN_ALGO) {
                    RetCode = NG_UL;
                }
            }

            if (RetCode == OK_UL) {
                /* vr cnt */
                pImageChannel[VinId][ChainId].pCtx->VrMap.Cnt = 0U;
                /* vr map */
                for (m = 0U; m < AMBA_IMGFRW_NUM_VIN_CHANNEL; m++) {
                    /* clear chain flag of vr map */
                    pImageChannel[VinId][ChainId].pCtx->VrMap.Flag[m] = 0U;
                    /* is vin null */
                    if (pImageChannel[m] == NULL) {
                        continue;
                    }
                    /* find families */
                    for (n = 0U; pImageChannel[m][n].Magic == 0xCafeU; n++) {
                        /* is ctx null */
                        if (pImageChannel[m][n].pCtx == NULL) {
                            continue;
                        }
                        /* is families */
                        if (((pImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrId & (((UINT32) 1U) << mVinId)) > 0U) &&
                            ((pImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << mChainId)) > 0U)) {
                            /* chain in families */
                             pImageChannel[VinId][ChainId].pCtx->VrMap.Flag[m] |= (((UINT32) 1U) << n);
                             pImageChannel[VinId][ChainId].pCtx->VrMap.Cnt++;
                        }
                    }
                }
            } else {
                pImageChannel[VinId][ChainId].pCtx->VrMap.Cnt = 0U;
                for (m = 0U; m < AMBA_IMGFRW_NUM_VIN_CHANNEL; m++) {
                    pImageChannel[VinId][ChainId].pCtx->VrMap.Flag[m] = 0U;
                }
            }
        } else {
            pImageChannel[VinId][ChainId].pCtx->VrMap.Cnt = 0U;
            for (m = 0U; m < AMBA_IMGFRW_NUM_VIN_CHANNEL; m++) {
                pImageChannel[VinId][ChainId].pCtx->VrMap.Flag[m] = 0U;
            }
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image channel vr map
 *  @param[in] VinId vin id
 *  @param[in] pImageChannel pointer to the image channel pointer
 *  @return error code
 */
UINT32 AmbaImgChannel_VrMap(UINT32 VinId, AMBA_IMG_CHANNEL_s * const *pImageChannel)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if ((pImageChannel != NULL) &&
        (pImageChannel[VinId] != NULL)) {
        for (UINT32 i = 0U; pImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* is ctx null */
            if (pImageChannel[VinId][i].pCtx == NULL) {
                continue;
            }
            /* get image channel id */
            ImageChanId.Ctx.Data = pImageChannel[VinId][i].pCtx->Id.Ctx.Data;
            /* get vr map */
            FuncRetCode = AmbaImgChannel_VrMapSet(ImageChanId, pImageChannel);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}
