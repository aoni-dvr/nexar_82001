/**
 *  @file AmbaImgStatistics.c
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
 *  @details Amba Image Statistics
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"

#include "AmbaImgVar.h"
#include "AmbaImgChannel.h"
#include "AmbaImgChannel_Internal.h"
#include "AmbaImgSensorHAL.h"
#include "AmbaImgStatistics.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba image statistics entry
 *  @param[in] VinId vin id
 *  @param[in] ZoneId zone id (bits)
 *  @param[in] pCfa pointer to the CFA statistics pointer
 *  @param[in] pRgb pointer to the RGB statistics pointer
 *  @param[in] RawCapSeq raw capture sequence number
 *  @return error code
 */
UINT32 AmbaImgStatistics_Entry(UINT32 VinId, UINT32 ZoneId, const void **pCfa, const void **pRgb, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pChanCtx;
    AMBA_IMG_STATISTICS_CONTEXT_s *pCtx;

    AMBA_IMG_STATISTICS_PORT_s *pList;

    UINT32 FrameId;

    FuncRetCode = AmbaImgSensorHAL_TimingMarkChk(VinId);
    if (FuncRetCode == OK_UL) {
        char str[11];
        /* stat sig */
        AmbaImgSensorHAL_TimingMarkPut(VinId, "Stat_Sig");
        /* zone id */
        str[0] = ' ';str[1] = ' ';
        var_utoa(ZoneId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_ZERO);
        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        var_utoa(RawCapSeq, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
    }

    /* image channel check */
    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL) &&
        (AmbaImgStatistics_Chan[VinId] != NULL)) {
        /* image channel find */
        for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* image channel ctx check */
            if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                /* image channel ctx get */
                pChanCtx = pFrwImageChannel[VinId][i].pCtx;
                /* image chain id get */
                ChainId = pChanCtx->Id.Ctx.Bits.ChainId;
                /* chk code reset */
                ChkCode = OK_UL;
                /* image chain inter? */
                if ((AmbaImgStatistics_Vin[VinId].Op.Bits.Inter > 0U) &&
                    (pChanCtx->Inter.Ctx.Bits.Num > 1U)) {
                    FrameId = RawCapSeq % ((UINT32) pChanCtx->Inter.Ctx.Bits.Num);
                    if ((((UINT32) pChanCtx->Inter.Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                        /* not belong */
                        ChkCode = NG_UL;
                    }
                }
                /* belong? */
                if (ChkCode == OK_UL) {
                    /* chan check */
                    if ((AmbaImgStatistics_Chan[VinId] != NULL) &&
                        (AmbaImgStatistics_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                        /* ctx check */
                        if (AmbaImgStatistics_Chan[VinId][ChainId].pCtx != NULL) {
                            /* ctx get */
                            pCtx = AmbaImgStatistics_Chan[VinId][ChainId].pCtx;
                            /* mutext take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == KAL_ERR_NONE) {
                                /* port list get */
                                pList = pCtx->pList;
                                /* port check */
                                while ((pList != NULL) && (pList->Magic == 0xCafeU)) {
                                    /* acc get */
                                    pList->ZoneId.Acc |= (ZoneId & pList->ZoneId.Req);
                                    /* last one? */
                                    if ((pList->ZoneId.Acc << 1U) > pList->ZoneId.Req) {
                                        /* rdy check */
                                        if (pList->ZoneId.Acc == pList->ZoneId.Req) {
                                            /* counter add */
                                            pList->Counter.Bits.Num++;
                                            /* req check */
                                            FuncRetCode = AmbaKAL_SemaphoreTake(&(pList->Sem.Req), AMBA_KAL_NO_WAIT);
                                            if (FuncRetCode == KAL_ERR_NONE) {
                                                /* cfa get */
                                                pList->Data.pCfa = pCfa;
                                                /* rgc get */
                                                pList->Data.pRgb = pRgb;
                                                /* reg get */
                                                FuncRetCode = AmbaImgSensorHAL_RingGet(pList->ImageChanId, &(pList->Reg), RawCapSeq);
                                                if (FuncRetCode == OK_UL) {
                                                    char str[11];
                                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Raw_F");
                                                    str[0] = ' ';str[1] = ' ';
                                                    var_utoa((UINT32) pList->Reg.User, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                                }
                                                /* call back? */
                                                if (pList->Proc.pFunc != NULL) {
                                                    pList->Proc.pFunc(pList->ImageChanId, pList->Data.pCfa, pList->Data.pRgb, pList->Counter.Bits.Num);
                                                }
                                                /* rdy ack */
                                                FuncRetCode = AmbaKAL_SemaphoreGive(&(pList->Sem.Rdy));
                                                if (FuncRetCode != KAL_ERR_NONE) {
                                                    /* */
                                                }
                                            }
                                        }
                                        /* acc clr */
                                        pList->ZoneId.Acc = 0U;
                                    }
                                    /* next get */
                                    pList = pList->Link.Down;
                                }
                                /* mutex give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                        } else {
                            /* ctx null */
                            RetCode = NG_UL;
                        }
                    } else {
                        /* chan null*/
                        RetCode = NG_UL;
                    }
                }
            }
        }
    } else {
        /* channel null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image statistics push
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgStatistics_Push(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ChainId;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pChanCtx;
    AMBA_IMG_STATISTICS_CONTEXT_s *pCtx;

    AMBA_IMG_STATISTICS_PORT_s *pList;

    /* image channel check */
    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL) &&
        (AmbaImgStatistics_Chan[VinId] != NULL)) {
        /* image channel find */
        for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* image channel ctx check */
            if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                /* image channel ctx get */
                pChanCtx = pFrwImageChannel[VinId][i].pCtx;
                /* image chain id get */
                ChainId = pChanCtx->Id.Ctx.Bits.ChainId;
                /* chan check */
                if ((AmbaImgStatistics_Chan[VinId] != NULL) &&
                    (AmbaImgStatistics_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                    /* ctx check */
                    if (AmbaImgStatistics_Chan[VinId][ChainId].pCtx != NULL) {
                        /* ctx get */
                        pCtx = AmbaImgStatistics_Chan[VinId][ChainId].pCtx;
                        /* mutext take */
                        FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* port list get */
                            pList = pCtx->pList;
                            /* port check */
                            while ((pList != NULL) && (pList->Magic == 0xCafeU)) {
                                /* req check */
                                FuncRetCode = AmbaKAL_SemaphoreTake(&(pList->Sem.Req), AMBA_KAL_NO_WAIT);
                                if (FuncRetCode == KAL_ERR_NONE) {
                                    /* rdy ack */
                                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pList->Sem.Rdy));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                }
                                /* next get */
                                pList = pList->Link.Down;
                            }
                            /* mutex give */
                            FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    } else {
                        /* ctx null */
                        RetCode = NG_UL;
                    }
                } else {
                    /* chan null*/
                    RetCode = NG_UL;
                }
            }
        }
    } else {
        /* channel null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image statistics init
 *  @return error code
 */
UINT32 AmbaImgStatistics_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    AMBA_IMG_STATISTICS_VIN_s *pVin;
    AMBA_IMG_STATISTICS_CONTEXT_s *pCtx;

    for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
        /* vin get */
        pVin = &(AmbaImgStatistics_Vin[i]);
        /* debug */
        pVin->Debug.Data = 0U;
        /* op */
        pVin->Op.Data = 0U;
        /* inter will be enabled in the future */
        pVin->Op.Bits.Inter = 0U;
        /* status */
        pVin->Status.Counter = 0U;
        pVin->Status.ZoneId = 0U;
        /* chan check */
        if (AmbaImgStatistics_Chan[i] != NULL) {
            /* chan find */
            for (j = 0U; AmbaImgStatistics_Chan[i][j].Magic == 0xCafeU; j++) {
                /* ctx check */
                if (AmbaImgStatistics_Chan[i][j].pCtx != NULL) {
                    /* ctx get */
                    pCtx = AmbaImgStatistics_Chan[i][j].pCtx;
                    /* mutex */
                    FuncRetCode = AmbaKAL_MutexCreate(&(pCtx->Mutex), NULL);
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* mutex fail */
                        RetCode = NG_UL;
                    }
                    /* op */
                    pCtx->Op.Bits.Enable = 1U;
                    /* list */
                    pCtx->pList = NULL;
                }
            }
        } else {
            /* chan null */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  Amba image statistics debug
 *  @param[in] VinId vin id
 *  @param[in] Flag debug flag (bits)
 *  @return error code
 */
UINT32 AmbaImgStatistics_Debug(UINT32 VinId, UINT32 Flag)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AmbaImgStatistics_Vin[VinId].Debug.Data = Flag;
    } else {
        /* id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image statistics reset
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgStatistics_Reset(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AMBA_IMG_STATISTICS_VIN_s *pVin;
        /* vin get */
        pVin = &(AmbaImgStatistics_Vin[VinId]);
        /* reset */
        pVin->Op.Data = 0U;
        pVin->Status.Counter = 0U;
        pVin->Status.ZoneId = 0U;
    } else {
        /* id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image statistics inter
 *  @param[in] VinId vin id
 *  @param[in] Enable enable/disable
 *  @return error code
 */
UINT32 AmbaImgStatistics_Inter(UINT32 VinId, UINT32 Enable)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AMBA_IMG_STATISTICS_VIN_s *pVin;
        /* vin get */
        pVin = &(AmbaImgStatistics_Vin[VinId]);
        /* reset */
        pVin->Op.Bits.Inter = (UINT8) (Enable & 0x1U);
    } else {
        /* id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image statistics zone id get for global AAA algorithm
 *  @param[in] ImageChanId image channel id
 *  @return statistics zone id (bits)
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgStatistics_GZoneIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 m, n;
     INT32 k;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 ExclusiveZoneId;
    UINT32 InclusiveZoneId;
    UINT32 TempZoneId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    InclusiveZoneId = ZoneId;

    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL)) {
        /* vr id get */
        if ((pFrwImageChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pFrwImageChannel[VinId][ChainId].pCtx != NULL)) {
            VrId = pFrwImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
            VrAltId = pFrwImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
        }
        /* vr check */
        if (VrId > 0U) {
            /* vr master check  */
            if ((((UINT32) VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                (((UINT32) VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                /* vr master */
                ExclusiveZoneId = 0U;
                InclusiveZoneId = 0U;
                /* find slave */
                for (m = 0U; m < AMBA_IMGFRW_NUM_VIN_CHANNEL; m++) {
                    /* vin check */
                    if (pFrwImageChannel[m] == NULL) {
                        continue;
                    }

                    if (m == VinId) {
                        /* same vin */
                        for (n = 0U; pFrwImageChannel[m][n].Magic == 0xCafeU; n++) {
                            /* is ctx null */
                            if (pFrwImageChannel[m][n].pCtx == NULL) {
                                continue;
                            }
                            /* get the image channel id */
                            ImageChanId2.Ctx.Data = pFrwImageChannel[m][n].pCtx->Id.Ctx.Data;
                            /* is not itself */
                            if (ChainId != ImageChanId2.Ctx.Bits.ChainId) {
                                /* is families */
                                if ((((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                                    (((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                                    /* slave chain of families */
                                    TempZoneId = (UINT32) ImageChanId2.Ctx.Bits.ZoneId;
                                    /* exclusive zone id */
                                    ExclusiveZoneId |= TempZoneId;
                                    /* inclusive zone id */
                                    if (ImageChanId2.Ctx.Bits.ZoneMsb == 0U) {
                                        for (k = 0; k < (INT32) AMBA_IMGFRW_NUM_FOV_CHANNEL; k++) {
                                            if ((TempZoneId & (((UINT32) 1U) << ((UINT32) k))) > 0U) {
                                                InclusiveZoneId |= (((UINT32) 1U) << ((UINT32) k));
                                                break;
                                            }
                                        }
                                    } else {
                                        for (k = (((INT32) AMBA_IMGFRW_NUM_FOV_CHANNEL) - 1); k >= 0; k--) {
                                            if ((TempZoneId & (((UINT32) 1U) << ((UINT32) k))) > 0U) {
                                                InclusiveZoneId |= (((UINT32) 1U) << ((UINT32) k));
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        /* other vin */
                        if ((VrId & (((UINT32) 1U) << m)) > 0U) {
                            /* vin family */
                            for (n = 0U; pFrwImageChannel[m][n].Magic == 0xCafeU; n++) {
                                /* is ctx null */
                                if (pFrwImageChannel[m][n].pCtx == NULL) {
                                    continue;
                                }
                                /* get the image channel id */
                                ImageChanId2.Ctx.Data = pFrwImageChannel[m][n].pCtx->Id.Ctx.Data;
                                /* is families */
                                if ((((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                                    (((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                                    /* slave chain of families */
                                    TempZoneId = (UINT32) ImageChanId2.Ctx.Bits.ZoneId;
                                    /* exclusive zone id */
                                    ExclusiveZoneId |= TempZoneId;
                                    /* inclusive zone id */
                                    if (ImageChanId2.Ctx.Bits.ZoneMsb == 0U) {
                                        for (k = 0; k < (INT32) AMBA_IMGFRW_NUM_FOV_CHANNEL; k++) {
                                            if ((TempZoneId & (((UINT32) 1U) << ((UINT32) k))) > 0U) {
                                                InclusiveZoneId |= (((UINT32) 1U) << ((UINT32) k));
                                                break;
                                            }
                                        }
                                    } else {
                                        for (k = (((INT32) AMBA_IMGFRW_NUM_FOV_CHANNEL) - 1); k >= 0; k--) {
                                            if ((TempZoneId & (((UINT32) 1U) << ((UINT32) k))) > 0U) {
                                                InclusiveZoneId |= (((UINT32) 1U) << ((UINT32) k));
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                /* remove the salve zone id */
                ZoneId &= ~ExclusiveZoneId;
                /* inclusive zone id */
                if (ImageChanId.Ctx.Bits.ZoneMsb == 0U) {
                    for (k = 0; k < (INT32) AMBA_IMGFRW_NUM_FOV_CHANNEL; k++) {
                        if ((ZoneId & (((UINT32) 1U) << ((UINT32) k))) > 0U) {
                            InclusiveZoneId |= (((UINT32) 1U) << ((UINT32) k));
                            break;
                        }
                    }
                } else {
                    for (k = (((INT32) AMBA_IMGFRW_NUM_FOV_CHANNEL) - 1); k >= 0; k--) {
                        if ((ZoneId & (((UINT32) 1U) << ((UINT32) k))) > 0U) {
                            InclusiveZoneId |= (((UINT32) 1U) << ((UINT32) k));
                            break;
                        }
                    }
                }
            }
        }
    }

    return InclusiveZoneId;
}

/**
 *  Amba image statistics zone id get by image channel
 *  @param[in] ImageChanId image channel id
 *  @return statistics zone id (bits)
 */
UINT32 AmbaImgStatistics_ZoneIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 m, n;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 ExclusiveZoneId;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL)) {
        /* vr id get */
        if ((pFrwImageChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pFrwImageChannel[VinId][ChainId].pCtx != NULL)) {
            VrId = pFrwImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
            VrAltId = pFrwImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
        }
        /* vr check */
        if (VrId > 0U) {
            /* vr master check  */
            if ((((UINT32) VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                (((UINT32) VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                /* vr master */
                ExclusiveZoneId = 0U;
                /* find slave */
                for (m = 0U; m < AMBA_IMGFRW_NUM_VIN_CHANNEL; m++) {
                    /* vin check */
                    if (pFrwImageChannel[m] == NULL) {
                        continue;
                    }

                    if (m == VinId) {
                        /* same vin */
                        for (n = 0U; pFrwImageChannel[m][n].Magic == 0xCafeU; n++) {
                            /* is ctx null */
                            if (pFrwImageChannel[m][n].pCtx == NULL) {
                                continue;
                            }
                            /* get the image channel id */
                            ImageChanId2.Ctx.Data = pFrwImageChannel[m][n].pCtx->Id.Ctx.Data;
                            /* is not itself */
                            if (ChainId != ImageChanId2.Ctx.Bits.ChainId) {
                                /* is families */
                                if ((((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                                    (((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                                    /* slave chain of families */
                                    ExclusiveZoneId |= ImageChanId2.Ctx.Bits.ZoneId;
                                }
                            }
                        }
                    } else {
                        /* other vin */
                        if ((VrId & (((UINT32) 1U) << m)) > 0U) {
                            for (n = 0U; pFrwImageChannel[m][n].Magic == 0xCafeU; n++) {
                                /* is ctx null */
                                if (pFrwImageChannel[m][n].pCtx == NULL) {
                                    continue;
                                }
                                /* get the image channel id */
                                ImageChanId2.Ctx.Data = pFrwImageChannel[m][n].pCtx->Id.Ctx.Data;
                                /* is families */
                                if ((((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrId & (((UINT32) 1U) << VinId)) > 0U) &&
                                    (((UINT32) pFrwImageChannel[m][n].pCtx->VrMap.Id.Ctx.Bits.VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                                    /* slave chain of families */
                                    ExclusiveZoneId |= ImageChanId2.Ctx.Bits.ZoneId;
                                }
                            }
                        }
                    }
                }
                /* remove the salve zone id */
                ZoneId &= ~ExclusiveZoneId;
            }
        }
    }

    return ZoneId;
}

/**
 *  Amba image statistics total zone id get by vin
 *  @param[in] VinId vin id
 *  @return statistics zone id (bits)
 */
UINT32 AmbaImgStatistics_TZoneIdGet(UINT32 VinId)
{
    UINT32 TotalZoneId = 0U;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    /* chan check */
    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL)) {
        /* chan find */
        for (UINT32 i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* ctx check */
            if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                /* get image channel id */
                ImageChanId.Ctx.Data = pFrwImageChannel[VinId][i].pCtx->Id.Ctx.Data;
                /* accumulate zone id */
                TotalZoneId |= AmbaImgStatistics_ZoneIdGet(ImageChanId);
            }
        }
    }

    return TotalZoneId;
}

/**
 *  Amba image statistics port open (extend)
 *  @param[in] ImageChanId image channel id
 *  @param[in] pPort pointer to the statistics port
 *  @param[in] pFunc pointer to the statistics callback function
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgStatistics_OpenEx(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_STATISTICS_PORT_s *pPort, AMBA_IMG_STATISTICS_FUNC_f pFunc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_STATISTICS_CONTEXT_s *pCtx;
    AMBA_IMG_STATISTICS_PORT_s *pList;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pPort != NULL) &&
        (pPort->Magic != 0xCafeU) &&
        (AmbaImgStatistics_Chan[VinId] != NULL) &&
        (AmbaImgStatistics_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgStatistics_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgStatistics_Chan[VinId][ChainId].pCtx;
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                pPort->Magic = 0xCafeU;
                pPort->ImageChanId.Ctx.Data = ImageChanId.Ctx.Data;
                pPort->Counter.Data = 0ULL;
                pPort->ZoneId.Req = AmbaImgStatistics_GZoneIdGet(ImageChanId);
                pPort->ZoneId.Acc = 0U;
                pPort->Proc.pFunc = pFunc;
                pPort->Data.pCfa = NULL;
                pPort->Data.pRgb = NULL;
                pPort->Reg.Agc = 0ULL;
                pPort->Reg.Dgc = 0ULL;
                pPort->Reg.Wgc = 0ULL;
                pPort->Reg.Shr = 0ULL;
                pPort->Reg.DDgc = 0ULL;
                pPort->Reg.User = 0ULL;
                pPort->Link.Up = NULL;
                pPort->Link.Down = NULL;
                /* semaphore create */
                FuncRetCode = AmbaKAL_SemaphoreCreate(&(pPort->Sem.Req), NULL, 0U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_SemaphoreCreate(&(pPort->Sem.Rdy), NULL, 1U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    RetCode = NG_UL;
                }
                /* hook to list */
                pList = pCtx->pList;
                if (pList == NULL) {
                    /* root */
                    pCtx->pList = pPort;
                } else {
                    /* last find */
                    while (pList->Link.Down != NULL) {
                        pList = pList->Link.Down;
                    }
                    pList->Link.Down = pPort;
                    pPort->Link.Up = pList;
                }
                /* mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            } else {
                /* mutex fail */
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
 *  Amba image statistics port open
 *  @param[in] ImageChanId image channel id
 *  @param[in] pPort pointer to the statistics port
 *  @return error code
 */
UINT32 AmbaImgStatistics_Open(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_STATISTICS_PORT_s *pPort)
{
    return AmbaImgStatistics_OpenEx(ImageChanId, pPort, NULL);
}

/**
 *  Amba image statistics port close
 *  @param[in] pPort pointer to the statistics port
 *  @return error code
 */
UINT32 AmbaImgStatistics_Close(AMBA_IMG_STATISTICS_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_STATISTICS_CONTEXT_s *pCtx;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        VinId = pPort->ImageChanId.Ctx.Bits.VinId;
        ChainId = pPort->ImageChanId.Ctx.Bits.ChainId;
        if ((AmbaImgStatistics_Chan[VinId] != NULL) &&
            (AmbaImgStatistics_Chan[VinId][ChainId].Magic == 0xCafeU)) {
            if (AmbaImgStatistics_Chan[VinId][ChainId].pCtx != NULL) {
                /* get ctx */
                pCtx = AmbaImgStatistics_Chan[VinId][ChainId].pCtx;
                /* mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* remove from list */
                    if (pPort->Link.Up == NULL) {
                        /* root */
                        if (pPort->Link.Down == NULL) {
                            /* null */
                            pCtx->pList = NULL;
                        } else {
                            /* linker */
                            pPort->Link.Down->Link.Up = NULL;
                            pCtx->pList = pPort->Link.Down;
                        }
                    } else {
                        /* succeed */
                        if (pPort->Link.Down == NULL) {
                            /* tail */
                            pPort->Link.Up->Link.Down = NULL;
                        } else {
                            /* linker */
                            pPort->Link.Up->Link.Down = pPort->Link.Down;
                            pPort->Link.Down->Link.Up = pPort->Link.Up;
                        }
                    }
                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* magic clr */
                    pPort->Magic = 0U;
                    /* semaphore delete */
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&(pPort->Sem.Req));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    FuncRetCode = AmbaKAL_SemaphoreDelete(&(pPort->Sem.Rdy));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* */
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
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image statistics request
 *  @param[in] pPort pointer to the statistics port
 *  @param[in] Flag timeout flag
 *  @return error code
 */
UINT32 AmbaImgStatistics_Request(AMBA_IMG_STATISTICS_PORT_s *pPort, UINT32 Flag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IMG_STATISTICS_CMD_MSG_s CmdMsg;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        FuncRetCode = AmbaKAL_SemaphoreTake(&(pPort->Sem.Rdy), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* req put */
            FuncRetCode = AmbaKAL_SemaphoreGive(&(pPort->Sem.Req));
            if (FuncRetCode == OK_UL) {
                /* cmd get */
                CmdMsg.Ctx.Data = Flag;
                if (CmdMsg.Ctx.Bits.Cmd == (UINT8) STAT_CMD_WAIT_FOREVER) {
                    /* wait */
                    FuncRetCode = AmbaKAL_SemaphoreTake(&(pPort->Sem.Rdy), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(pPort->Sem.Rdy));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    } else {
                        /* sem fail */
                        RetCode = NG_UL;
                    }
                }
            } else {
                /* rdy give */
                FuncRetCode = AmbaKAL_SemaphoreGive(&(pPort->Sem.Rdy));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        } else {
            /* rdy fail */
            RetCode = NG_UL;
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}
