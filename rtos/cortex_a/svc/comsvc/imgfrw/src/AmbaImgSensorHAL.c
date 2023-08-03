/**
 *  @file AmbaImgSensorHAL.c
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
 *  @details Amba Image Sensor HAL (FIFO, Set, Register and Inforamation)
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgVar.h"
#include "AmbaImgChannel.h"
#include "AmbaImgChannel_Internal.h"
//#include "AmbaImgEvent.h"
#include "AmbaImgEvent_Internal.h"
#include "AmbaImgSystem.h"
#include "AmbaImgSystem_Internal.h"
#include "AmbaImgSystem_Platform.h"
#include "AmbaImgSensorDrv.h"
#include "AmbaImgSensorHAL_Reg.h"
#include "AmbaImgSensorHAL.h"
#include "AmbaImgSensorHAL_Internal.h"
#include "AmbaImgSensorHAL_Platform.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static UINT32 AmbaImgSensorHAL_FifoReset(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 AmbaImgSensorHAL_HdrFifoReset(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 AmbaImgSensorHAL_UserBufferRst(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 AmbaImgSensorHAL_RingReset(AMBA_IMG_CHANNEL_ID_s ImageChanId);

static UINT32 AmbaImgSensorHAL_SsiWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);

static UINT32 AmbaImgSensorHAL_LastSvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 AmbaImgSensorHAL_LastShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 AmbaImgSensorHAL_LastAgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 AmbaImgSensorHAL_LastDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 AmbaImgSensorHAL_LastWgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId);

/**
 *  Amba image sensor HAL init
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    AMBA_IMG_SENSOR_HAL_VIN_s *pVin;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_RING_s *pRing;
    const AMBA_IMG_SENSOR_HAL_USER_CONTEXT_s *pUserCtx;
    const AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    /* vin check */
    for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
        /* chan check */
        if (AmbaImgSensorHAL_Chan[i] == NULL) {
            continue;
        }

        /* get vin & info */
        pVin = &(AmbaImgSensorHAL_Vin[i]);

        /* op */
        pVin->Op.Bits.Out2Dsp = 0U;
        pVin->Op.Bits.Out2Sensor = 0U;

        /* counter */
        pVin->Counter.Raw = 0U;

        /* timing mark mutex */
        FuncRetCode = AmbaKAL_MutexCreate(&(pVin->Timing.Mutex), NULL);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* mutex success */
            pVin->Timing.Op.Bits.Rdy = 1U;
        } else {
            /* mutex fail */
            pVin->Timing.Op.Bits.Rdy = 0U;
            pVin->Timing.Op.Bits.Enable = 0U;
            RetCode = NG_UL;
        }

        /* chain check */
        for (j = 0U; AmbaImgSensorHAL_Chan[i][j].Magic == 0xCafeU; j++) {
            /* ctx check */
            if (AmbaImgSensorHAL_Chan[i][j].pCtx == NULL) {
                continue;
            }

            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[i][j].pCtx->pInfo;

            /* user setting */
            pInfo->Op.User.DgcType = (UINT32) IMAGE_DGC_BOTH;
            pInfo->Op.User.SsType = (UINT32) IMAGE_SS_TYPE_0;
            pInfo->Op.User.SvrDelay = 2U;
            pInfo->Op.User.ShrDelay = 2U;
            pInfo->Op.User.AgcDelay = 1U;
            pInfo->Op.User.DgcDelay = 1U;
            pInfo->Op.User.WgcDelay = 1U;
            pInfo->Op.User.AuxDelay = 0U;
            pInfo->Op.User.StatDelay = 1U;
            pInfo->Op.User.ForcePostSof = 0U;
            pInfo->Op.User.ForceAdvEof = 0U;
            pInfo->Op.User.AikExecTime = 4U;
            pInfo->Op.User.EofMoveMaxTime = 0U;
            pInfo->Op.User.WriteLatency = 0U;
            pInfo->Op.User.ForceSsInVideo = 0U;

            /* ctrl */
            pInfo->Op.Ctrl.R2yBackground = 1U;
            pInfo->Op.Ctrl.AdvSvrUpd = 0U;
            pInfo->Op.Ctrl.AdvSsiCmd = 1U;
            pInfo->Op.Ctrl.AdvRgbCmd = 0U;
            pInfo->Op.Ctrl.AdvAikCmd = 0U;
            pInfo->Op.Ctrl.PreAikCmd = 1U;

            /* status param */
            pInfo->Op.Status.SsIndex = 0U;
            pInfo->Op.Status.SsState = 0U;
            pInfo->Op.Status.LastSsIndex = 0U;
            pInfo->Op.Status.LastSvr = 1U;

            /* image channel id */
            ImageChanId.Ctx.Bits.VinId = (UINT8) i;
            ImageChanId.Ctx.Bits.ChainId = (UINT8) j;

            /* fifo reset */
            FuncRetCode = AmbaImgSensorHAL_FifoReset(ImageChanId);
            if (FuncRetCode != OK_UL) {
                /* reset fail */
                RetCode = NG_UL;
            }

            /* ring get */
            pRing = &(AmbaImgSensorHAL_Chan[i][j].pCtx->Ring);
            /* ring mutex create */
            FuncRetCode = AmbaKAL_MutexCreate(&(pRing->Mutex), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            /* ring reset */
            FuncRetCode = AmbaImgSensorHAL_RingReset(ImageChanId);
            if (FuncRetCode != OK_UL) {
                /* reset fail */
                RetCode = NG_UL;
            }

            /* user ctx get */
            pUserCtx = AmbaImgSensorHAL_Chan[i][j].pUserCtx;
            /* ctx check */
            if (pUserCtx != NULL) {
                /* ring reset */
                FuncRetCode = AmbaImgSensorHAL_UserBufferRst(ImageChanId);
                if (FuncRetCode != OK_UL) {
                    /* reset fail */
                    RetCode = NG_UL;
                }
            }

            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[i][j].pHdrCtx;
            /* ctx check */
            if (pHdrCtx != NULL) {
                /* fifo reset */
                FuncRetCode = AmbaImgSensorHAL_HdrFifoReset(ImageChanId);
                if (FuncRetCode != OK_UL) {
                    /* reset fail */
                    RetCode = NG_UL;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL debug
 *  @param[in] VinId vin id
 *  @param[in] Flag debug flag
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_Debug(UINT32 VinId, UINT32 Flag)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AmbaImgSensorHAL_Vin[VinId].Debug.Data = Flag;
    } else {
        /* id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL reset
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_Reset(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_IMG_SENSOR_HAL_VIN_s *pVin;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    const AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_USER_CONTEXT_s *pUserCtx;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if (AmbaImgSensorHAL_Chan[VinId] != NULL) {
        /* timing mark clear */
        FuncRetCode = AmbaImgSensorHAL_TimingMarkClr(VinId);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }

        /* get vin */
        pVin = &(AmbaImgSensorHAL_Vin[VinId]);

        /* sensor vin op */
        pVin->Op.Bits.Out2Dsp = 1U;
        pVin->Op.Bits.Out2Sensor = 1U;

        /* counter */
        pVin->Counter.Raw = 0U;

        /* chan check */
        for(i = 0U; AmbaImgSensorHAL_Chan[VinId][i].Magic == 0xCafeU; i++) {
            /* exist? */
            if (AmbaImgSensorHAL_Chan[VinId][i].pCtx == NULL) {
                continue;
            }

            /* assign id */
            ImageChanId.Ctx.Bits.VinId = (UINT8) VinId;
            ImageChanId.Ctx.Bits.ChainId = (UINT8) i;
            /* get context */
            pCtx = AmbaImgSensorHAL_Chan[VinId][i].pCtx;

            /* info status reset */
            pCtx->pInfo->Op.Status.SsIndex = 0U;
            pCtx->pInfo->Op.Status.SsState = 0U;
            pCtx->pInfo->Op.Status.LastSsIndex = 0U;
            pCtx->pInfo->Op.Status.LastSvr = 1U;

            /* fifo reset */
            FuncRetCode = AmbaImgSensorHAL_FifoReset(ImageChanId);
            if (FuncRetCode != OK_UL) {
                /* fifo reset fail */
                RetCode = NG_UL;
            }

            /* ring reset */
            FuncRetCode = AmbaImgSensorHAL_RingReset(ImageChanId);
            if (FuncRetCode != OK_UL) {
                /* ring reset fail */
                RetCode = NG_UL;
            }

            /* get user context */
            pUserCtx = AmbaImgSensorHAL_Chan[VinId][i].pUserCtx;
            if (pUserCtx != NULL) {
                /* user buffer reset */
                FuncRetCode = AmbaImgSensorHAL_UserBufferRst(ImageChanId);
                if (FuncRetCode != OK_UL) {
                    /* user buffer reset fail */
                    RetCode = NG_UL;
                }
            }

            /* get hdr context */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][i].pHdrCtx;
            if (pHdrCtx != NULL) {
                /* hdr fifo reset */
                FuncRetCode = AmbaImgSensorHAL_HdrFifoReset(ImageChanId);
                if (FuncRetCode != OK_UL) {
                    /* hdr fifo reset fail */
                    RetCode = NG_UL;
                }
            }
        }

        /* advance timing check */
        FuncRetCode = AmbaImgSensorHAL_TimingConfig(VinId);
        if (FuncRetCode != OK_UL) {
            /* timing schedule fail */
            RetCode = NG_UL;
        }
    } else {
        /* vin or chain null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL stop
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_Stop(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgSensorHAL_Vin[VinId]);
        /* diable */
        pVin->Op.Bits.Out2Dsp = 0U;
        pVin->Op.Bits.Out2Sensor = 0U;
    } else {
        /* vin exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL raw capture sequence put
 *  @param[in] VinId vin id
 *  @param[in] RawCapSeq raw capture sequence number
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_RawCapSeqPut(UINT32 VinId, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AmbaImgSensorHAL_Vin[VinId].Counter.Raw = RawCapSeq;
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL info get
 *  @param[in] ImageChanId image channel id
 *  @param[out] pInfo pointer to the HAL information pointer
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_InfoGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_INFO_s **pInfo)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if((pInfo != NULL) &&
       (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
       (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            *pInfo = pCtx->pInfo;
            /* info check */
            if (*pInfo == NULL) {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL slow shutter status get
 *  @param[in] ImageChanId image channel id
 *  @param[out] pSsStatus pointer to the slow shutter status pointer
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_SsStatusGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_SS_STATUS_s **pSsStatus)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if((pSsStatus != NULL) &&
       (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
       (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* info check */
            if (pInfo != NULL) {
                /* ss status get */
                *pSsStatus = &(pInfo->Op.Status);
            } else {
                /* info null */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* ssstatus/chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL slow shutter check
 *  @param[in] ImageChanId image channel id
 *  @param[in] Shr shutter unit
 *  @param[out] pSvr pointer to the slow shutter
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_SvrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Shr, UINT32 *pSvr)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    UINT64 Var_ULL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pSvr != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            if (pInfo != NULL) {
                /* svr get */
                Var_ULL = ((Shr - 1ULL) / pInfo->Frame.TotalShutterLine) + 1ULL;
                *pSvr = (UINT32) (Var_ULL & 0xFFFFFFFFULL);
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL hdr slow shutter check
 *  @param[in] ImageChanId image channel id
 *  @param[in] pShr pointer to the shutter uint
 *  @param[out] pSvr pointer to the slow shutter
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrSvrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pShr, UINT32 *pSvr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 HdrId;

    UINT32 HdrShr[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};
    UINT32 Svr;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    HdrId = ImageChanId.Ctx.Bits.HdrId;

    if ((pSvr != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            if (pInfo != NULL) {
                if (HdrId > 0U) {
                    /* hdr shr get */
                    for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                        if ((HdrId & (((UINT32) 1U) << i)) > 0U) {
                            HdrShr[i] = (UINT32) (pShr[i] & 0xFFFFFFFFULL);
                        }
                    }
                    /* hdr svr convert */
                    FuncRetCode = AmbaImgSensorDrv_SvrConvert(ImageChanId, HdrShr, &Svr);
                    if (FuncRetCode == OK_UL) {
                        /* svr put */
                        *pSvr = Svr;
                        /* svr debug */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Conv");
                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                            char str[11];
                            str[0] = ' ';str[1] = ' ';
                            var_utoa(Svr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        }
                    } else {
                        /* svr fail */
                        RetCode = NG_UL;
                        /* svr debug */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Conv!");
                    }
                } else {
                    /* hdr id invalid */
                    RetCode = NG_UL;
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL shutter check
 *  @param[in] ImageChanId image channel id
 *  @param[in] Shr shutter value
 *  @param[out] pSsIndex pointer to the slow shutter index
 *  @param[out] pSsState pointer to the slow shutter state
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_ShrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Shr, UINT32 *pSsIndex, UINT32 *pSsState)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 Svr;
    UINT32 LastSsIndex;

    UINT32 ForceSsIndex;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    UINT64 Var_ULL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pSsIndex != NULL) &&
        (pSsState != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            if (pInfo != NULL) {
                /* svr get */
                Var_ULL = ((Shr - 1ULL) / pInfo->Frame.TotalShutterLine) + 1ULL;
                Svr = (UINT32) (Var_ULL & 0xFFFFFFFFULL);
                /* ss index find */
                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_SS_LEVEL; i++) {
                    if (Svr <= (((UINT32) 1U) << i)) {
                        break;
                    }
                }
                /* ss index get */
                *pSsIndex = i;

                /* slow motion check */
                ForceSsIndex = pCtx->pInfo->Op.User.ForceSsInVideo & 0xFFU;
                if (ForceSsIndex > i) {
                    /* force ss */
                    *pSsIndex = ForceSsIndex;
                }

                /* last ss index get */
                LastSsIndex = pInfo->Op.Status.SsIndex;
                /* ss index update */
                pInfo->Op.Status.SsIndex = *pSsIndex;

                if (*pSsIndex == 0U) {
                    if (LastSsIndex == 0U) {
                        pInfo->Op.Status.SsState = (UINT32) SS_STATE_NONE;
                    } else {
                        pInfo->Op.Status.SsState = (UINT32) SS_STATE_LEAVE;
                    }
                } else {
                    if (LastSsIndex == 0U) {
                        pInfo->Op.Status.SsState = (UINT32) SS_STATE_ENTER;
                    } else {
                        if (*pSsIndex < LastSsIndex) {
                            pInfo->Op.Status.SsState = (UINT32) SS_STATE_LESS;
                        } else if (*pSsIndex > LastSsIndex) {
                            pInfo->Op.Status.SsState = (UINT32) SS_STATE_MORE;
                        } else {
                            pInfo->Op.Status.SsState = (UINT32) SS_STATE_STILL;
                        }
                    }
                }

                /* state get */
                *pSsState = pInfo->Op.Status.SsState;
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL hdr shutter check
 *  @param[in] ImageChanId image channel id
 *  @param[in] pShr pointer to the shutter value
 *  @param[out] pSsIndex pointer to the slow shutter index
 *  @param[out] pSsState pointer to the slow shutter state
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrShrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pShr, UINT32 *pSsIndex, UINT32 *pSsState)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 HdrId;

    UINT32 HdrShr[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};
    UINT32 Svr;
    UINT32 LastSsIndex;

    UINT32 ForceSsIndex;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    HdrId = ImageChanId.Ctx.Bits.HdrId;

    if ((pSsIndex != NULL) &&
        (pSsState != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            if (pInfo != NULL) {
                if (HdrId > 0U) {
                    /* hdr shr get */
                    for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                        if ((HdrId & (((UINT32) 1U) << i)) > 0U) {
                            HdrShr[i] = (UINT32) (pShr[i] & 0xFFFFFFFFULL);
                        }
                    }
                    /* hdr svr convert */
                    FuncRetCode = AmbaImgSensorDrv_SvrConvert(ImageChanId, HdrShr, &Svr);
                    if (FuncRetCode == OK_UL) {
                        /* ss index find */
                        for (i = 0U; i < AMBA_IMG_SENSOR_HAL_SS_LEVEL; i++) {
                            if (Svr <= (((UINT32) 1U) << i)) {
                                break;
                            }
                        }
                        /* ss index get */
                        *pSsIndex = i;

                        /* slow motion check */
                        ForceSsIndex = pCtx->pInfo->Op.User.ForceSsInVideo & 0xFFU;
                        if (ForceSsIndex > i) {
                            /* force ss */
                            *pSsIndex = ForceSsIndex;
                        }

                        /* last ss index get */
                        LastSsIndex = pInfo->Op.Status.SsIndex;
                        /* ss index update */
                        pInfo->Op.Status.SsIndex = *pSsIndex;

                        if (*pSsIndex == 0U) {
                            if (LastSsIndex == 0U) {
                                pInfo->Op.Status.SsState = (UINT32) SS_STATE_NONE;
                            } else {
                                pInfo->Op.Status.SsState = (UINT32) SS_STATE_LEAVE;
                            }
                        } else {
                            if (LastSsIndex == 0U) {
                                pInfo->Op.Status.SsState = (UINT32) SS_STATE_ENTER;
                            } else {
                                if (*pSsIndex < LastSsIndex) {
                                    pInfo->Op.Status.SsState = (UINT32) SS_STATE_LESS;
                                } else if (*pSsIndex > LastSsIndex) {
                                    pInfo->Op.Status.SsState = (UINT32) SS_STATE_MORE;
                                } else {
                                    pInfo->Op.Status.SsState = (UINT32) SS_STATE_STILL;
                                }
                            }
                        }

                        /* state get */
                        *pSsState = pInfo->Op.Status.SsState;

                        /* svr debug */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Conv");
                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                            char str[11];
                            str[0] = ' ';str[1] = ' ';
                            var_utoa(Svr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            var_utoa(*pSsIndex, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        }
                    } else {
                        /* svr fail */
                        RetCode = NG_UL;
                        /* svr debug */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Conv!");
                    }

                } else {
                    /* hdr id invalid */
                    RetCode = NG_UL;
                }
            } else {
                /* info null */
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
 *  Amba image sensor HAL user buffer reset
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_UserBufferRst(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_USER_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL)  &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pUserCtx != NULL) {
            /* user ring get */
            pRing = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pUserCtx->Ring);
            /* user buffer reset */
            pRing->Index = 0U;
            pRing->Count = 0U;
            pRing->Size = 0U;
            pRing->pMem = NULL;
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
 *  @private
 *  Amba image sensor HAL user buffer configuration
 *  @param[in] ImageChanId image channel id
 *  @param[in] Size buffer size
 *  @param[in] Count buffer count
 *  @param[in] pMem potiner to the buffer memory
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_UserBufferCfg(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pMem)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_USER_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pMem != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pUserCtx != NULL) {
            /* user ring get */
            pRing = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pUserCtx->Ring);
            /* user ring register */
            pRing->Index = 0U;
            pRing->Size = Size;
            pRing->Count = Count;
            pRing->pMem = pMem;
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* mem/chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL user buffer get
 *  @param[in] ImageChanId image channel id
 *  @return pointer to the buffer memory
 *  @note this function is intended for internal use only
 */
void *AmbaImgSensorHAL_UserBufferGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT8 *pRetMem = NULL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_USER_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pUserCtx != NULL) {
            /* user ring get */
            pRing = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pUserCtx->Ring);
            /* user buffer get */
            if (pRing->pMem != NULL) {
                pRetMem = &(pRing->pMem[pRing->Size * pRing->Index]);
                pRing->Index = (pRing->Index + 1U) % pRing->Count;
            }
        }
    }

    return pRetMem;
}

/**
 *  Amba image sensor HAL ring reset
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_RingReset(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* ring get */
            pRing = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Ring);
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* reset */
                pRing->Count = 0U;
                pRing->Index.Wr = 0U;
                pRing->Index.Rd = 0U;
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
 *  @private
 *  Amba image sensor HAL ring put
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData pointer to the HAL Data
 *  @param[in] RawCapSeq raw capture sequence number
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_RingPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get buf */
            pRing = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Ring);
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* raw cap seq */
                pRing->RawCapSeq[pRing->Index.Wr] = RawCapSeq;
                /* data put */
                pRing->Data[pRing->Index.Wr].Agc = pData->Agc;
                pRing->Data[pRing->Index.Wr].Dgc = pData->Dgc;
                pRing->Data[pRing->Index.Wr].Wgc = pData->Wgc;
                pRing->Data[pRing->Index.Wr].Shr = pData->Shr;
                pRing->Data[pRing->Index.Wr].DDgc = pData->DDgc;
                pRing->Data[pRing->Index.Wr].User = pData->User;
                /* index update */
                pRing->Index.Wr = (pRing->Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_RING_SIZE;
                if (pRing->Index.Rd == pRing->Index.Wr) {
                    pRing->Index.Rd = (pRing->Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_RING_SIZE;
                } else {
                   pRing->Count++;
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
 *  Amba image sensor HAL ring get
 *  @param[in] ImageChanId image channel id
 *  @param[out] pData pointer to the HAL data
 *  @param[in] RawCapSeq raw capture sequence number
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_RingGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 Wr;

    AMBA_IMG_SENSOR_HAL_RING_s *pRing;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ring */
            pRing = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Ring);
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pRing->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* wr get */
                Wr = pRing->Index.Wr;
                /* find raw cap seq */
                for (i = 0U; i < pRing->Count; i++) {
                    Wr = (Wr + AMBA_IMG_SENSOR_HAL_RING_SIZE - 1U) % AMBA_IMG_SENSOR_HAL_RING_SIZE;
                    if (pRing->RawCapSeq[Wr] == RawCapSeq) {
                        break;
                    }
                }

                if (i < pRing->Count) {
                    /* data get */
                    pData->Agc = pRing->Data[Wr].Agc;
                    pData->Dgc = pRing->Data[Wr].Dgc;
                    pData->Wgc = pRing->Data[Wr].Wgc;
                    pData->Shr = pRing->Data[Wr].Shr;
                    pData->DDgc = pRing->Data[Wr].DDgc;
                    pData->User = pRing->Data[Wr].User;
                } else {
                    RetCode = NG_UL;
                }
                FuncRetCode = AmbaKAL_MutexGive(&(pRing->Mutex));
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
 *  @private
 *  Amba image sensor HAL data in
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData pointer to the HAL data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DataIn(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get ctx */
        pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
        /* ctx check */
        if (pCtx != NULL) {
            /* input reg */
            pCtx->Status.In.Agc = pData->Agc;
            pCtx->Status.In.Dgc = pData->Dgc;
            pCtx->Status.In.Wgc = pData->Wgc;
            pCtx->Status.In.Shr = pData->Shr;
            pCtx->Status.In.DDgc = pData->DDgc;
            pCtx->Status.In.User = pData->User;
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
 *  @private
 *  Amba image sensor HAL register get
 *  @param[in] ImageChanId image channel id
 *  @param[out] pData pointer to the HAL data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DataGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get ctx */
        pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
        /* ctx check */
        if (pCtx != NULL) {
            /* input reg */
            pData->Agc = pCtx->Status.In.Agc;
            pData->Dgc = pCtx->Status.In.Dgc;
            pData->Wgc = pCtx->Status.In.Wgc;
            pData->Shr = pCtx->Status.In.Shr;
            pData->DDgc = pCtx->Status.In.DDgc;
            pData->User = pCtx->Status.In.User;
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
 *  @private
 *  Amba image sensor HAL active register in
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData pointer to the HAL data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_ActDataIn(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get ctx */
        pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
        /* ctx check */
        if (pCtx != NULL) {
            /* input reg */
            pCtx->Status.Act.Agc = pData->Agc;
            pCtx->Status.Act.Dgc = pData->Dgc;
            pCtx->Status.Act.Wgc = pData->Wgc;
            pCtx->Status.Act.Shr = pData->Shr;
            pCtx->Status.Act.DDgc = pData->DDgc;
            pCtx->Status.Act.User = pData->User;
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
 *  @private
 *  Amba image sensor HAL active register get
 *  @param[in] ImageChanId image channel id
 *  @param[out] pData pointer to the HAL active data
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_ActDataGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get ctx */
        pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
        /* ctx check */
        if (pCtx != NULL) {
            /* input reg */
            pData->Agc = pCtx->Status.Act.Agc;
            pData->Dgc = pCtx->Status.Act.Dgc;
            pData->Wgc = pCtx->Status.Act.Wgc;
            pData->Shr = pCtx->Status.Act.Shr;
            pData->DDgc = pCtx->Status.Act.DDgc;
            pData->User = pCtx->Status.Act.User;
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
 *  @private
 *  Amba image sensor HAL HDR shutter load
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData piointer to the HDR shutter
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrShrLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get hdr ctx */
        pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;
        /* ctx check */
        if (pHdrCtx != NULL) {
            /* hdr shr load */
            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                DataMem.Ctx.pUint64 = &(pData[i]);
                pHdrCtx->Status.Load.Shr[i] = (UINT32) (DataMem.Ctx.pShr->Shr & 0xFFFFFFFFULL);
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
 *  @private
 *  Amba image sensor HAL HDR analog gain load
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData piointer to the HDR analog gain
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrAgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get hdr ctx */
        pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;
        /* ctx check */
        if (pHdrCtx != NULL) {
            /* hdr agc load */
            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                DataMem.Ctx.pUint64 = &(pData[i]);
                pHdrCtx->Status.Load.Agc[i] = (UINT32) (DataMem.Ctx.pAgc->Agc & 0xFFFFFFFFULL);
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
 *  @private
 *  Amba image sensor HAL HDR digital gain load
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData piointer to the HDR digital gain
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrDgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get hdr ctx */
        pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;
        /* ctx check */
        if (pHdrCtx != NULL) {
            /* hdr dgc load */
            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                DataMem.Ctx.pUint64 = &(pData[i]);
                pHdrCtx->Status.Load.Dgc[i] = (UINT32) (DataMem.Ctx.pDgc->Dgc & 0xFFFFFFFFULL);
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
 *  @private
 *  Amba image sensor HAL HDR white balance gain load
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData piointer to the HDR white balance gain
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrWgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get hdr ctx */
        pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;
        /* ctx check */
        if (pHdrCtx != NULL) {
            /* hdr dgc load */
            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                DataMem.Ctx.pUint64 = &(pData[i]);
                pHdrCtx->Status.Load.Wgc[i] = DataMem.Ctx.pWgc->Wgc;
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
 *  @private
 *  Amba image sensor HAL HDR DSP digital gain load
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData piointer to the HDR DSP digital gain
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrDDgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* get hdr ctx */
        pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;
        /* ctx check */
        if (pHdrCtx != NULL) {
            /* hdr ddgc load */
            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                DataMem.Ctx.pUint64 = &(pData[i]);
                pHdrCtx->Status.Load.DDgc[i] = (UINT32) (DataMem.Ctx.pDDgc->Dgc & 0xFFFFFFFFULL);
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
 *  Amba image sensor HAL HDR FIFO reset
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_HdrFifoReset(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_HDR_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx != NULL) {
            /* get hdr fifo */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx->Fifo);

            /* agc reset */
            pFifo->Agc.Count = 0U;
            pFifo->Agc.Index.Wr = 0U;
            pFifo->Agc.Index.Rd = 0U;

            /* dgc reset */
            pFifo->Dgc.Count = 0U;
            pFifo->Dgc.Index.Wr = 0U;
            pFifo->Dgc.Index.Rd = 0U;

            /* wgc reset */
            pFifo->Wgc.Count = 0U;
            pFifo->Wgc.Index.Wr = 0U;
            pFifo->Wgc.Index.Rd = 0U;

            /* shr reset */
            pFifo->Shr.Count = 0U;
            pFifo->Shr.Index.Wr = 0U;
            pFifo->Shr.Index.Rd = 0U;

            /* ddgc reset */
            pFifo->DDgc.Count = 0U;
            pFifo->DDgc.Index.Wr = 0U;
            pFifo->DDgc.Index.Rd = 0U;
        } else {
            /* hdr ctx null */
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
 *  Amba image sensor HAL HDR FIFO put
 *  @param[in] ImageChanId image channel id
 *  @param[in] pData piointer to the HAL HDR data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_HdrFifoPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_HDR_DATA_s *pData)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_HAL_HDR_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if ((AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) &&
            (AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx != NULL)) {
            /* get hdr fifo */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx->Fifo);
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            if (pInfo != NULL) {
                /* agc put */
                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                    pFifo->Agc.Data[pFifo->Agc.Index.Wr][i] = pData->Agc[i];
                }
                pFifo->Agc.Index.Wr = (pFifo->Agc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Agc.Index.Wr == pFifo->Agc.Index.Rd) {
                    pFifo->Agc.Index.Rd = (pFifo->Agc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Agc.Count++;
                }

                /* dgc put */
                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                    pFifo->Dgc.Data[pFifo->Dgc.Index.Wr][i] = pData->Dgc[i];
                }
                pFifo->Dgc.Index.Wr = (pFifo->Dgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Dgc.Index.Wr == pFifo->Dgc.Index.Rd) {
                    pFifo->Dgc.Index.Rd = (pFifo->Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Dgc.Count++;
                }

                /* wgc put */
                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                    pFifo->Wgc.Data[pFifo->Wgc.Index.Wr][i] = pData->Wgc[i];
                }
                pFifo->Wgc.Index.Wr = (pFifo->Wgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Wgc.Index.Wr == pFifo->Wgc.Index.Rd) {
                    pFifo->Wgc.Index.Rd = (pFifo->Wgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Wgc.Count++;
                }

                /* shr put */
                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                    pFifo->Shr.Data[pFifo->Shr.Index.Wr][i] = pData->Shr[i];
                }
                pFifo->Shr.Index.Wr = (pFifo->Shr.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Shr.Index.Wr == pFifo->Shr.Index.Rd) {
                    pFifo->Shr.Index.Rd = (pFifo->Shr.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Shr.Count++;
                }

                /* ddgc put */
                if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                    for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                        pFifo->DDgc.Data[pFifo->DDgc.Index.Wr][i] = pData->DDgc[i];
                    }
                    pFifo->DDgc.Index.Wr = (pFifo->DDgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                    if (pFifo->DDgc.Index.Wr == pFifo->DDgc.Index.Rd) {
                        pFifo->DDgc.Index.Rd = (pFifo->DDgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                    } else {
                        pFifo->DDgc.Count++;
                    }
                }
            } else {
                /* info null */
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
 *  Amba image sensor HAL FIFO reset
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_FifoReset(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get fifo */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);

            /* agc reset */
            pFifo->Agc.Count = 0U;
            pFifo->Agc.Index.Wr = 0U;
            pFifo->Agc.Index.Rd = 0U;

            /* dgc reset */
            pFifo->Dgc.Count = 0U;
            pFifo->Dgc.Index.Wr = 0U;
            pFifo->Dgc.Index.Rd = 0U;

            /* wgc reset */
            pFifo->Wgc.Count = 0U;
            pFifo->Wgc.Index.Wr = 0U;
            pFifo->Wgc.Index.Rd = 0U;

            /* shr reset */
            pFifo->Shr.Count = 0U;
            pFifo->Shr.Index.Wr = 0U;
            pFifo->Shr.Index.Rd = 0U;

            /* ddgc reset */
            pFifo->DDgc.Count = 0U;
            pFifo->DDgc.Index.Wr = 0U;
            pFifo->DDgc.Index.Rd = 0U;

            /* user reset */
            pFifo->User.Count = 0U;
            pFifo->User.Index.Wr = 0U;
            pFifo->User.Index.Rd = 0U;
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
 *  @private
 *  Amba image sensor HAL FIFO put
 *  @param[in] ImageChanId image channel id
 *  @param[in] pCtrl pointer to the HAL control
 *  @param[in] pData pointer to the HAL data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_FifoPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_CTRL_REG_s *pCtrl, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((pCtrl != NULL) &&
        (pData != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get fifo */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;

#ifndef CONFIG_BUILD_IMGFRW_FIFO_PROT
            /* agc put */
            pFifo->Agc.Ctrl[pFifo->Agc.Index.Wr] = pCtrl->Agc;
            pFifo->Agc.Data[pFifo->Agc.Index.Wr] = pData->Agc;
            pFifo->Agc.Index.Wr = (pFifo->Agc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            if (pFifo->Agc.Index.Wr == pFifo->Agc.Index.Rd) {
                pFifo->Agc.Index.Rd = (pFifo->Agc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            } else {
                pFifo->Agc.Count++;
            }

            /* dgc put */
            pFifo->Dgc.Ctrl[pFifo->Dgc.Index.Wr] = pCtrl->Dgc;
            pFifo->Dgc.Data[pFifo->Dgc.Index.Wr] = pData->Dgc;
            pFifo->Dgc.Index.Wr = (pFifo->Dgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            if (pFifo->Dgc.Index.Wr == pFifo->Dgc.Index.Rd) {
                pFifo->Dgc.Index.Rd = (pFifo->Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            } else {
                pFifo->Dgc.Count++;
            }

            /* wgc put */
            pFifo->Wgc.Ctrl[pFifo->Wgc.Index.Wr] = pCtrl->Wgc;
            pFifo->Wgc.Data[pFifo->Wgc.Index.Wr] = pData->Wgc;
            pFifo->Wgc.Index.Wr = (pFifo->Wgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            if (pFifo->Wgc.Index.Wr == pFifo->Wgc.Index.Rd) {
                pFifo->Wgc.Index.Rd = (pFifo->Wgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            } else {
                pFifo->Wgc.Count++;
            }

            /* shr put */
            pFifo->Shr.Ctrl[pFifo->Shr.Index.Wr] = pCtrl->Shr;
            pFifo->Shr.Data[pFifo->Shr.Index.Wr] = pData->Shr;
            pFifo->Shr.Index.Wr = (pFifo->Shr.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            if (pFifo->Shr.Index.Wr == pFifo->Shr.Index.Rd) {
                pFifo->Shr.Index.Rd = (pFifo->Shr.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            } else {
                pFifo->Shr.Count++;
            }

            /* ddgc put */
            if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                pFifo->DDgc.Ctrl[pFifo->DDgc.Index.Wr] = pCtrl->DDgc;
                pFifo->DDgc.Data[pFifo->DDgc.Index.Wr] = pData->DDgc;
                pFifo->DDgc.Index.Wr = (pFifo->DDgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->DDgc.Index.Wr == pFifo->DDgc.Index.Rd) {
                    pFifo->DDgc.Index.Rd = (pFifo->DDgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->DDgc.Count++;
                }
            }

            /* user put */
            pFifo->User.Ctrl[pFifo->User.Index.Wr] = pCtrl->User;
            pFifo->User.Data[pFifo->User.Index.Wr] = pData->User;
            pFifo->User.Index.Wr = (pFifo->User.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            if (pFifo->User.Index.Wr == pFifo->User.Index.Rd) {
                pFifo->User.Index.Rd = (pFifo->User.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
            } else {
                pFifo->User.Count++;
            }
#else
            /* count check (overflow protect mechanism) */
            if ((pFifo->Agc.Count < AMBA_IMG_SENSOR_HAL_FIFO_LEVEL) &&
                (pFifo->Dgc.Count < AMBA_IMG_SENSOR_HAL_FIFO_LEVEL) &&
                (pFifo->Wgc.Count < AMBA_IMG_SENSOR_HAL_FIFO_LEVEL) &&
                (pFifo->Shr.Count < AMBA_IMG_SENSOR_HAL_FIFO_LEVEL) &&
                (pFifo->DDgc.Count < AMBA_IMG_SENSOR_HAL_FIFO_LEVEL)) {
                /* agc put */
                pFifo->Agc.Ctrl[pFifo->Agc.Index.Wr] = pCtrl->Agc;
                pFifo->Agc.Data[pFifo->Agc.Index.Wr] = pData->Agc;
                pFifo->Agc.Index.Wr = (pFifo->Agc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Agc.Index.Wr == pFifo->Agc.Index.Rd) {
                    pFifo->Agc.Index.Rd = (pFifo->Agc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Agc.Count++;
                }

                /* dgc put */
                pFifo->Dgc.Ctrl[pFifo->Dgc.Index.Wr] = pCtrl->Dgc;
                pFifo->Dgc.Data[pFifo->Dgc.Index.Wr] = pData->Dgc;
                pFifo->Dgc.Index.Wr = (pFifo->Dgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Dgc.Index.Wr == pFifo->Dgc.Index.Rd) {
                    pFifo->Dgc.Index.Rd = (pFifo->Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Dgc.Count++;
                }

                /* wgc put */
                pFifo->Wgc.Ctrl[pFifo->Wgc.Index.Wr] = pCtrl->Wgc;
                pFifo->Wgc.Data[pFifo->Wgc.Index.Wr] = pData->Wgc;
                pFifo->Wgc.Index.Wr = (pFifo->Wgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Wgc.Index.Wr == pFifo->Wgc.Index.Rd) {
                    pFifo->Wgc.Index.Rd = (pFifo->Wgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Wgc.Count++;
                }

                /* shr put */
                pFifo->Shr.Ctrl[pFifo->Shr.Index.Wr] = pCtrl->Shr;
                pFifo->Shr.Data[pFifo->Shr.Index.Wr] = pData->Shr;
                pFifo->Shr.Index.Wr = (pFifo->Shr.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->Shr.Index.Wr == pFifo->Shr.Index.Rd) {
                    pFifo->Shr.Index.Rd = (pFifo->Shr.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->Shr.Count++;
                }

                /* ddgc put */
                if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                    pFifo->DDgc.Ctrl[pFifo->DDgc.Index.Wr] = pCtrl->DDgc;
                    pFifo->DDgc.Data[pFifo->DDgc.Index.Wr] = pData->DDgc;
                    pFifo->DDgc.Index.Wr = (pFifo->DDgc.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                    if (pFifo->DDgc.Index.Wr == pFifo->DDgc.Index.Rd) {
                        pFifo->DDgc.Index.Rd = (pFifo->DDgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                    } else {
                        pFifo->DDgc.Count++;
                    }
                }

                /* user put */
                pFifo->User.Ctrl[pFifo->User.Index.Wr] = pCtrl->User;
                pFifo->User.Data[pFifo->User.Index.Wr] = pData->User;
                pFifo->User.Index.Wr = (pFifo->User.Index.Wr + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                if (pFifo->User.Index.Wr == pFifo->User.Index.Rd) {
                    pFifo->User.Index.Rd = (pFifo->User.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                } else {
                    pFifo->User.Count++;
                }
            } else {
                /* fifo accumulated */
                RetCode = NG_UL;
            }
#endif
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
 *  @private
 *  Amba image sensor HAL FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_FifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* cnt check */
            if ((pFifo->Shr.Count == 0U) &&
                (pFifo->Agc.Count == 0U) &&
                (pFifo->Dgc.Count == 0U) &&
                (pFifo->Wgc.Count == 0U)) {
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
#ifdef CONFIG_BUILD_IMGFRW_FIFO_PROT
/**
 *  @private
 *  Amba image sensor HAL FIFO put check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_FifoPutCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* cnt check */
            if ((pFifo->Shr.Count >= AMBA_IMG_SENSOR_HAL_FIFO_LEVEL) ||
                (pFifo->Agc.Count >= AMBA_IMG_SENSOR_HAL_FIFO_LEVEL) ||
                (pFifo->Dgc.Count >= AMBA_IMG_SENSOR_HAL_FIFO_LEVEL) ||
                (pFifo->Wgc.Count >= AMBA_IMG_SENSOR_HAL_FIFO_LEVEL)) {
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
#endif
/**
 *  @private
 *  Amba image sensor HAL slow shutter index FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_SsiFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem2;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;

            /* info check */
            if (pInfo != NULL) {
                /* shr count? */
                if (pFifo->Shr.Count > 0U) {
                    /* get shr ctrl in adv dsp */
                    CtrlMem.Ctx.pUint64 = &(pFifo->Shr.Ctrl[pFifo->Shr.Index.Rd]);
                    /* get shr data in adv dsp */
                    DataMem.Ctx.pUint64 = &(pFifo->Shr.Data[pFifo->Shr.Index.Rd]);

                    /* svr is advanced than ssi? */
                    if ((pInfo->Op.Ctrl.AdvSsiCmd <= 1U) &&
                        (pInfo->Op.Ctrl.AdvSvrUpd > 1U)) {
                        /* svr precondition in shr */
                        if (CtrlMem.Ctx.pShr->Fll != (UINT8) SHR_SVR_NON_ADV) {
                            /* svr precondition is still */
                            ChkCode = NG_UL;
                        }
                    }

                    /* inter check (dsp) */
                    if (ChkCode == OK_UL) {
                        /* inter get */
                        pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                        /* is inter? */
                        if (pInter->Ctx.Bits.Num > 1U) {
                            /* counter get */
                            pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                            /* effect raw get */
                            if (pInfo->Op.Ctrl.AdvSsiCmd <= 1U) {
                                /* same frame with shr */
                                EffectRaw = pCounter->Raw + 4U;
                            } else {
                                /* one advanced frame than shr */
                                EffectRaw = pCounter->Raw + 5U;
                            }
                            /* frame id get */
                            FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                            /* is blong to inter id? */
                            if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                /* not belong */
                                ChkCode = NG_UL;
                            } else {
                                /* batch counter (TBD) */
                                pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                            }
                        }
                    }

                    if (ChkCode == OK_UL) {
                        /* ssi write */
                        FuncRetCode = AmbaImgSensorHAL_SsiWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }

                        /* ssi precondtion in shr */
                        if (pInfo->Op.Ctrl.AdvSsiCmd > 1U) {
                            if (pInfo->Op.User.ShrDelay <= 2U) {
                                /* ssi precondition update (one frame delay) */
                               CtrlMem.Ctx.pShr->Ssi = (UINT8) SHR_SSI_CHK;
                            } else {
                                /* ssi precondition clear */
                                CtrlMem.Ctx.pShr->Ssi = (UINT8) SHR_SSI_NONE;
                            }
                        } else {
                            /* ssi precondition clear */
                            CtrlMem.Ctx.pShr->Ssi = (UINT8) SHR_SSI_NONE;
                        }

                        /* ssi precondition in dgc/ddgc (dsp)*/
                        if (pInfo->Op.User.DgcType != (UINT32) IMAGE_DGC_SENSOR) {
                            /* dsp dgc ctrl get */
                            if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                                /* dsp dgc ctrl in dgc */
                                CtrlMem2.Ctx.pUint64 = &(pFifo->Dgc.Ctrl[pFifo->Shr.Index.Rd]);
                            } else {
                                /* dsp dgc ctrl in ddgc */
                                CtrlMem2.Ctx.pUint64 = &(pFifo->DDgc.Ctrl[pFifo->Shr.Index.Rd]);
                            }
                            /* ssi precondition in dsp dgc */
                            if (CtrlMem2.Ctx.pDgc->Ssi == (UINT8) DGC_SSI_ADV) {
                                /* ssi precondition clear */
                                CtrlMem2.Ctx.pDgc->Ssi = (UINT8) DGC_SSI_NON_ADV;
                            }
                        }
                    }
                }
            } else {
               /* info null */
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
 *  @private
 *  Amba image sensor HAL slow shutter FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_SvrFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem2;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;
    UINT32 FirstId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;

            /* info check & one frame advane than shr */
            if ((pInfo != NULL) &&
                (pInfo->Op.Ctrl.AdvSvrUpd > 1U)) {
                /* shr count? */
                if (pFifo->Shr.Count > 0U) {
                    /* shr ctrl get in adv eof */
                    CtrlMem.Ctx.pUint64 = &(pFifo->Shr.Ctrl[pFifo->Shr.Index.Rd]);
                    /* shr data get in adv eof */
                    DataMem.Ctx.pUint64 = &(pFifo->Shr.Data[pFifo->Shr.Index.Rd]);

                    /* svr update check */
                    if (CtrlMem.Ctx.pShr->Fll == (UINT8) SHR_SVR_ADV) {
                        /* ssi is advanced than svr? */
                        if (pInfo->Op.Ctrl.AdvSsiCmd > 1U) {
                            /* ssi precondition in shr */
                            if (CtrlMem.Ctx.pShr->Ssi == (UINT8) SHR_SSI_ADV) {
                                /* adv ssi precondition is still */
                                ChkCode = NG_UL;
                            }
                        }
                    } else {
                        /* no update */
                        ChkCode = NG_UL;
                    }

                    /* inter get (sensor) */
                    pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                    /* is inter? */
                    if (pInter->Ctx.Bits.Num > 1U) {
                        /* counter get */
                        pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                        /* one frame advance than shr, effect raw get */
                        EffectRaw = pCounter->Raw + 5U;
                        /* frame id get */
                        FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                        /* is blong to inter id? */
                        if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) > 0U) {
                            /* batch counter (TBD) */
                            pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                            /* belong but no update? */
                            if (ChkCode != OK_UL) {
                                /* first id get */
                                FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                /* is belong to first id? */
                                if (FrameId == FirstId) {
                                    /* last svr write */
                                    FuncRetCode = AmbaImgSensorHAL_LastSvrWrite(ImageChanId);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                }
                            }
                        } else {
                            /* not belong */
                            ChkCode = NG_UL;
                        }
                    }

                    if (ChkCode == OK_UL) {
                        /* svr write */
                        FuncRetCode = AmbaImgSensorHAL_SvrWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }

                        /* svr precondtion in shr */
                        if (CtrlMem.Ctx.pShr->Fll == (UINT8) SHR_SVR_ADV) {
                            if (pInfo->Op.User.ShrDelay <= 2U) {
                               /* svr precondition update (one frame delay) */
                               CtrlMem.Ctx.pShr->Fll = (UINT8) SHR_SVR_CHK;
                            } else {
                                /* svr precondition clear */
                                CtrlMem.Ctx.pShr->Fll = (UINT8) SHR_SVR_NON_ADV;
                            }
                        }

                        /* svr precondition in dgc/ddgc (dsp)*/
                        if (pInfo->Op.User.DgcType != (UINT32) IMAGE_DGC_SENSOR) {
                            /* dsp dgc ctrl get */
                            if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                                /* dsp dgc ctrl in dgc */
                                CtrlMem2.Ctx.pUint64 = &(pFifo->Dgc.Ctrl[pFifo->Shr.Index.Rd]);
                            } else {
                                /* dsp dgc ctrl in ddgc */
                                CtrlMem2.Ctx.pUint64 = &(pFifo->DDgc.Ctrl[pFifo->Shr.Index.Rd]);
                            }
                            /* svr precondition in dsp dgc */
                            if (CtrlMem2.Ctx.pDgc->Svr == (UINT8) DGC_SVR_ADV) {
                                /* svr precondition clear */
                                CtrlMem2.Ctx.pDgc->Svr = (UINT8) DGC_SVR_NON_ADV;
                            }
                        }
                    }
                } else {
                    /* fifo empty, inter get (sensor) */
                    pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                    /* is inter? */
                    if (pInter->Ctx.Bits.Num > 1U) {
                        /* counter get */
                        pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                        /* one frame advance than shr, effect raw get */
                        EffectRaw = pCounter->Raw + 5U;
                        /* frame id get */
                        FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                        /* first id get */
                        FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                        /* is blong to first id? */
                        if (FrameId == FirstId) {
                            /* batch counter (TBD) */
                            pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                            /* last svr write */
                            FuncRetCode = AmbaImgSensorHAL_LastSvrWrite(ImageChanId);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                    }
                }
            } else {
               /* info null or not one frame advance */
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
 *  @private
 *  Amba image sensor HAL shutter FIFO timing
 *  @param[in] ImageChanId image channel id
 *  @param[out] pCtrl pointer to the shutter control
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_ShrFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* shr check */
            if (pCtrl != NULL) {
                if (pFifo->Shr.Count > 0U) {
                    /* shr ctrl get */
                    *pCtrl = pFifo->Shr.Ctrl[pFifo->Shr.Index.Rd];
                } else {
                    /* fifo empty */
                    RetCode = NG_UL;
                }
            } else {
                /* shr null */
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
 *  @private
 *  Amba image sensor HAL shutter FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_ShrFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem2;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem2;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;
    UINT32 FirstId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* get fifo */
            pFifo = &(pCtx->Fifo);
            /* get info */
            pInfo = pCtx->pInfo;
            /* get hdr ctx */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            /* info check */
            if (pInfo != NULL) {
                /* shr count? */
                if (pFifo->Shr.Count > 0U) {
                    /* shr ctrl get in adv eof */
                    CtrlMem.Ctx.pUint64 = &(pFifo->Shr.Ctrl[pFifo->Shr.Index.Rd]);
                    /* shr data get in adv eof */
                    DataMem.Ctx.pUint64 = &(pFifo->Shr.Data[pFifo->Shr.Index.Rd]);

                    /* shr precondition check */
                    if ((CtrlMem.Ctx.pShr->Ssi != (UINT8) SHR_SSI_ADV) &&
                        (CtrlMem.Ctx.pShr->Rgb != (UINT8) SHR_RGB_ADV) &&
                        (CtrlMem.Ctx.pShr->Aik < (UINT8) SHR_AIK_ADV) &&
                        (CtrlMem.Ctx.pShr->Fll != (UINT8) SHR_SVR_ADV)) {
                        /* shr precondition check */
                        if ((CtrlMem.Ctx.pShr->Ssi < (UINT8) SHR_SSI_ADV) &&
                            (CtrlMem.Ctx.pShr->Fll == (UINT8) SHR_SVR_NON_ADV)) {
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw get */
                                EffectRaw = pCounter->Raw + 4U;
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* is blong to inter id */
                                if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                    /* not belong */
                                    ChkCode = NG_UL;
                                } else {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                }
                            }

                            if (ChkCode == OK_UL) {
                                /* shr hdr check */
                                if (CtrlMem.Ctx.pShr->Hdr > 0U) {
                                    /* ctx check */
                                    if (pHdrCtx != NULL) {
                                        if (pHdrCtx->Fifo.Shr.Count > 0U) {
                                            /* hdr shr load */
                                            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                                DataMem2.Ctx.pUint64 = &(pHdrCtx->Fifo.Shr.Data[pHdrCtx->Fifo.Shr.Index.Rd][i]);
                                                pHdrCtx->Status.Load.Shr[i] = (UINT32) (DataMem2.Ctx.pShr->Shr & 0xFFFFFFFFULL);
                                            }
                                            /* hdr fifo update */
                                            pHdrCtx->Fifo.Shr.Index.Rd = (pHdrCtx->Fifo.Shr.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                            pHdrCtx->Fifo.Shr.Count--;
                                        }
                                    }
                                }

                                /* shr write */
                                FuncRetCode = AmbaImgSensorHAL_ShrWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }

                                /* act data (shr) */
                                pCtx->Status.Act.Shr = DataMem.Ctx.pShr->Shr;

                                /* shr precondition in user */
                                if (pFifo->User.Count > 0U) {
                                    /* user ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->User.Ctrl[pFifo->Shr.Index.Rd]);
                                    /* user data get */
                                    DataMem2.Ctx.pUint64 = &(pFifo->User.Data[pFifo->Shr.Index.Rd]);
                                    if (CtrlMem2.Ctx.pUser->Shr == (UINT8) USER_SHR_ADV) {
                                        /* precondition clear */
                                        CtrlMem2.Ctx.pUser->Shr = (UINT8) USER_SHR_NON_ADV;
                                    }
                                    /* act data (user) */
                                    pCtx->Status.Act.User = DataMem2.Ctx.pUser->User;
                                }

                                /* msc */
                                if (CtrlMem.Ctx.pShr->Msc > 0U) {
                                    if (CtrlMem.Ctx.pShr->Msc == (UINT8) SHR_MSC_UPD) {
                                        /* msc write */
                                        FuncRetCode = AmbaImgSensorHAL_MscWrite(ImageChanId, pInfo->Op.Status.LastSvr);
                                        if (FuncRetCode != OK_UL) {
                                            /* */
                                        }
                                    } else {
                                        /* msc in sof */
                                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_MSC_SOF);
                                        if (FuncRetCode == OK_UL) {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof");
                                        } else {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof!");
                                        }
                                    }
                                }

                                /* sls */
                                if (CtrlMem.Ctx.pShr->Sls > 0U) {
                                    /* sls write */
                                    FuncRetCode = AmbaImgSensorHAL_SlsWrite(ImageChanId, pCtx->Status.Act.User);
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                }

                                /* shr precondition in agc */
                                if (pFifo->Agc.Count > 0U) {
                                    /* agc ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->Agc.Ctrl[pFifo->Shr.Index.Rd]);
                                    /* agc data get */
                                    DataMem2.Ctx.pUint64 = &(pFifo->Agc.Data[pFifo->Shr.Index.Rd]);
                                    if (CtrlMem2.Ctx.pAgc->Shr == (UINT8) AGC_SHR_ADV) {
                                        if (pInfo->Op.User.AgcDelay <= 1U) {
                                            /* shr precondition update (one frame wait) */
                                            CtrlMem2.Ctx.pAgc->Shr = (UINT8) AGC_SHR_CHK;
                                        } else {
                                            /* shr precondition clear */
                                            CtrlMem2.Ctx.pAgc->Shr = (UINT8) AGC_SHR_NON_ADV;
                                        }
                                    }
                                    /* act data (agc) */
                                    pCtx->Status.Act.Agc = DataMem2.Ctx.pAgc->Agc;
                                }

                                /* shr precondition in dgc */
                                if (pFifo->Dgc.Count > 0U) {
                                    /* dgc ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->Dgc.Ctrl[pFifo->Shr.Index.Rd]);
                                    /* dgc data get */
                                    DataMem2.Ctx.pUint64 = &(pFifo->Dgc.Data[pFifo->Shr.Index.Rd]);
                                    if ((pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_SENSOR) ||
                                        (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH)) {
                                        /* dgc (sensor) */
                                        if (CtrlMem2.Ctx.pDgc->Shr == (UINT8) DGC_SHR_ADV) {
                                            if (pInfo->Op.User.DgcDelay <= 1U) {
                                                /* shr precondition update (one frame wait) */
                                                CtrlMem2.Ctx.pDgc->Shr = (UINT8) DGC_SHR_CHK;
                                            } else {
                                                /* shr precondition clear */
                                                CtrlMem2.Ctx.pDgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                                            }
                                        }
                                    } else {
                                        /* dgc (dsp) */
                                        if (CtrlMem2.Ctx.pDgc->Shr == (UINT8) DGC_SHR_ADV) {
                                            /* shr precondition clear */
                                            CtrlMem2.Ctx.pDgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                                        }
                                    }
                                    /* act data (dgc) */
                                    pCtx->Status.Act.Dgc = DataMem2.Ctx.pDgc->Dgc;
                                }

                                /* shr precondition in wgc */
                                if (pFifo->Wgc.Count > 0U) {
                                    /* wgc ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->Wgc.Ctrl[pFifo->Shr.Index.Rd]);
                                    /* wgc data get */
                                    DataMem2.Ctx.pUint64 = &(pFifo->Wgc.Data[pFifo->Shr.Index.Rd]);
                                    if (CtrlMem2.Ctx.pWgc->Shr == (UINT8) DGC_SHR_ADV) {
                                        if (pInfo->Op.User.WgcDelay <= 1U) {
                                            /* shr precondition update (one frame wait) */
                                            CtrlMem2.Ctx.pWgc->Shr = (UINT8) DGC_SHR_CHK;
                                        } else {
                                            /* shr precondition clear */
                                            CtrlMem2.Ctx.pWgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                                        }
                                    }
                                    /* act data (wgc) */
                                    pCtx->Status.Act.Wgc = DataMem2.Ctx.pWgc->Wgc;
                                }

                                /* shr precondition in ddgc */
                                if (pFifo->DDgc.Count > 0U) {
                                    /* ddgc ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->DDgc.Ctrl[pFifo->Shr.Index.Rd]);
                                    /* ddgc data get */
                                    DataMem2.Ctx.pUint64 = &(pFifo->DDgc.Data[pFifo->Shr.Index.Rd]);
                                    /* dsp dgc */
                                    if (CtrlMem2.Ctx.pDDgc->Shr == (UINT8) DGC_SHR_ADV) {
                                        /* shr precondition clear */
                                        CtrlMem2.Ctx.pDDgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                                    }
                                    /* act data (ddgc) */
                                    pCtx->Status.Act.DDgc = DataMem2.Ctx.pDDgc->Dgc;
                                }

                                /* shr fifo update */
                                pFifo->Shr.Index.Rd = (pFifo->Shr.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                pFifo->Shr.Count--;
                            }
                        } else {
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                EffectRaw = pCounter->Raw + 4U;
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* first id get */
                                FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                /* is blong to first id */
                                if (FrameId == FirstId) {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                    /*last shr write */
                                    FuncRetCode = AmbaImgSensorHAL_LastShrWrite(ImageChanId);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                    /* last msc */
                                    if (pInfo->Op.User.ShrDelay <= 2U) {
                                        /* last msc write */
                                        FuncRetCode = AmbaImgSensorHAL_MscWrite(ImageChanId, pInfo->Op.Status.LastSvr);
                                        if (FuncRetCode != OK_UL) {
                                            /* */
                                        }
                                    } else {
                                        /* msc in sof */
                                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_MSC_SOF);
                                        if (FuncRetCode == OK_UL) {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof");
                                        } else {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof!");
                                        }
                                    }
                                    /* last sls write */
                                    FuncRetCode = AmbaImgSensorHAL_SlsWrite(ImageChanId, pCtx->Status.Last.User);
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                }
                            }

                            /* ssi precondition in shr */
                            if (CtrlMem.Ctx.pShr->Ssi == (UINT8) SHR_SSI_CHK) {
                                /* ssi precodintion clear */
                                CtrlMem.Ctx.pShr->Ssi = (UINT8) SHR_SSI_NONE;
                                /* ssi/svr ack in adv eof */
                                if (CtrlMem.Ctx.pShr->Ack == (UINT8) SHR_ACK_SSI_SVR) {
                                    /* ack event put */
                                    FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SSI_SVR_ACK);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E!");
                                        RetCode = NG_UL;
                                    }
                                    /* ack condition clear */
                                    CtrlMem.Ctx.pShr->Ack = (UINT8) SHR_ACK_NONE;
                                }
                            }

                            /* svr precondition in shr */
                            if (CtrlMem.Ctx.pShr->Fll == (UINT8) SHR_SVR_CHK) {
                                /* svr precondition clr */
                                CtrlMem.Ctx.pShr->Fll = (UINT8) SHR_SVR_NON_ADV;
                                /* ssi/svr ack in adv eof */
                                if (CtrlMem.Ctx.pShr->Ack == (UINT8) SHR_ACK_SSI_SVR) {
                                    /* ack event put */
                                    FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SSI_SVR_ACK);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E!");
                                        RetCode = NG_UL;
                                    }
                                    /* ack condition clear */
                                    CtrlMem.Ctx.pShr->Ack = (UINT8) SHR_ACK_NONE;
                                }
                            }
                        }
                    } else {
                        /* inter get */
                        pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                        /* is inter? */
                        if (pInter->Ctx.Bits.Num > 1U) {
                            /* counter get */
                            pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                            /* effect raw */
                            EffectRaw = pCounter->Raw + 4U;
                            /* aux delay */
                            EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                            /* frame id get */
                            FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                            /* first id get */
                            FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                            /* is blong to first id */
                            if (FrameId == FirstId) {
                                /* batch counter (TBD) */
                                pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                /* last shr write */
                                FuncRetCode = AmbaImgSensorHAL_LastShrWrite(ImageChanId);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                                /* last msc */
                                if (pInfo->Op.User.ShrDelay <= 2U) {
                                    /* last msc write */
                                    FuncRetCode = AmbaImgSensorHAL_MscWrite(ImageChanId, pInfo->Op.Status.LastSvr);
                                    if (FuncRetCode != OK_UL) {
                                        /* */
                                    }
                                } else {
                                    /* msc in sof */
                                    FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_MSC_SOF);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof!");
                                    }
                                }
                                /* last sls write */
                                FuncRetCode = AmbaImgSensorHAL_SlsWrite(ImageChanId, pCtx->Status.Last.User);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            }
                        }

                        /* ssi precondition in shr */
                        if (CtrlMem.Ctx.pShr->Ssi == (UINT8) SHR_SSI_CHK) {
                            /* ssi precodintion clear */
                            CtrlMem.Ctx.pShr->Ssi = (UINT8) SHR_SSI_NONE;
                            /* ssi/svr ack in adv eof */
                            if (CtrlMem.Ctx.pShr->Ack == (UINT8) SHR_ACK_SSI_SVR) {
                                /* ack event put */
                                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SSI_SVR_ACK);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E!");
                                    RetCode = NG_UL;
                                }
                                /* ack condition clear */
                                CtrlMem.Ctx.pShr->Ack = (UINT8) SHR_ACK_NONE;
                            }
                        }

                        /* svr precondition in shr */
                        if (CtrlMem.Ctx.pShr->Fll == (UINT8) SHR_SVR_CHK) {
                            /* svr precondition clear */
                            CtrlMem.Ctx.pShr->Fll = (UINT8) SHR_SVR_NON_ADV;
                            /* ssi/svr ack in adv eof */
                            if (CtrlMem.Ctx.pShr->Ack == (UINT8) SHR_ACK_SSI_SVR) {
                                /* ack event put */
                                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SSI_SVR_ACK);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ss_Ack_E!");
                                    RetCode = NG_UL;
                                }
                                /* ack precondition clear */
                                CtrlMem.Ctx.pShr->Ack = (UINT8) SHR_ACK_NONE;
                            }
                        }
                    }
                } else {
                    /* fifo empty, inter get (sensor) */
                    pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                    /* is inter? */
                    if (pInter->Ctx.Bits.Num > 1U) {
                        /* counter get */
                        pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                        /* effect raw get */
                        EffectRaw = pCounter->Raw + 4U;
                        /* aux delay */
                        EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                        /* frame id get */
                        FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                        /* first id get */
                        FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                        /* is belong to first id? */
                        if (FrameId == FirstId) {
                            /* batch counter (TBD) */
                            pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                            /* last shr write */
                            FuncRetCode = AmbaImgSensorHAL_LastShrWrite(ImageChanId);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* last msc */
                            if (pInfo->Op.User.ShrDelay <= 2U) {
                                /* last msc write */
                                FuncRetCode = AmbaImgSensorHAL_MscWrite(ImageChanId, pInfo->Op.Status.LastSvr);
                                if (FuncRetCode != OK_UL) {
                                    /* */
                                }
                            } else {
                                /* msc in sof */
                                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_MSC_SOF);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_Sof!");
                                }
                            }
                            /* last sls write */
                            FuncRetCode = AmbaImgSensorHAL_SlsWrite(ImageChanId, pCtx->Status.Last.User);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL user FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_UserFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get fifo */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);

            /* user count? */
            if (pFifo->User.Count > 0U) {
                /* user ctrl get */
                CtrlMem.Ctx.pUint64 = &(pFifo->User.Ctrl[pFifo->User.Index.Rd]);
                /* user data get */
                DataMem.Ctx.pUint64 = &(pFifo->User.Data[pFifo->User.Index.Rd]);

                /* shr precondition check in user */
                if (CtrlMem.Ctx.pUser->Shr == (UINT8) USER_SHR_NON_ADV) {
                    /* user write */
                    FuncRetCode = AmbaImgSensorHAL_UserWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* user fifo udpate */
                    pFifo->User.Index.Rd = (pFifo->User.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                    pFifo->User.Count--;
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

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL analog gain FIFO timing
 *  @param[in] ImageChanId image channel id
 *  @param[out] pCtrl pointer to the analog gain control
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_AgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* agc check */
            if (pCtrl != NULL) {
                if (pFifo->Agc.Count > 0U) {
                    /* agc ctrl get */
                    *pCtrl = pFifo->Agc.Ctrl[pFifo->Agc.Index.Rd];
                } else {
                    /* fifo empty */
                    RetCode = NG_UL;
                }
            } else {
                /* agc null */
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
 *  @private
 *  Amba image sensor HAL analog gain FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_AgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem2;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem2;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;
    UINT32 FirstId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* get fifo */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* get info */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            /* get hdr ctx */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            /* info check */
            if (pInfo != NULL) {
                /* agc count? */
                if (pFifo->Agc.Count > 0U) {
                    /* agc ctrl get */
                    CtrlMem.Ctx.pUint64 = &(pFifo->Agc.Ctrl[pFifo->Agc.Index.Rd]);
                    /* agc data get */
                    DataMem.Ctx.pUint64 = &(pFifo->Agc.Data[pFifo->Agc.Index.Rd]);

                    /* ssr/ssg check */
                    if (pInfo->Cfg.ShutterMode == (UINT32) IMAGE_SHR_SSR) {
                        /* ssr: shr precondition in agc */
                        if (CtrlMem.Ctx.pAgc->Shr == (UINT8) AGC_SHR_NON_ADV) {
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                if (pInfo->Op.User.AgcDelay <= 1U) {
                                    /* following frame to shr */
                                    EffectRaw = pCounter->Raw + 3U;
                                } else {
                                    /* same frame with shr */
                                    EffectRaw = pCounter->Raw + 4U;
                                }
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* is belong to inter id? */
                                if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                    /* not belong */
                                    ChkCode = NG_UL;
                                } else {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                }
                            }

                            if (ChkCode == OK_UL) {
                                /* hdr agc */
                                if (CtrlMem.Ctx.pAgc->Hdr > 0U) {
                                    if (pHdrCtx != NULL) {
                                        if (pHdrCtx->Fifo.Agc.Count > 0U) {
                                            /* hdr agc load */
                                            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                                DataMem2.Ctx.pUint64 = &(pHdrCtx->Fifo.Agc.Data[pHdrCtx->Fifo.Agc.Index.Rd][i]);
                                                pHdrCtx->Status.Load.Agc[i] = (UINT32) (DataMem2.Ctx.pAgc->Agc & 0xFFFFFFFFULL);
                                            }
                                            /* hdr age fifo update */
                                            pHdrCtx->Fifo.Agc.Index.Rd = (pHdrCtx->Fifo.Agc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                            pHdrCtx->Fifo.Agc.Count--;
                                        }
                                    }
                                }

                                /* agc write */
                                FuncRetCode = AmbaImgSensorHAL_AgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                                /* agc fifo update */
                                pFifo->Agc.Index.Rd = (pFifo->Agc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                pFifo->Agc.Count--;

                                /* shr precondition check for next agc */
                                if (pFifo->Agc.Count > 0U) {
                                    /* agc ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->Agc.Ctrl[pFifo->Agc.Index.Rd]);
                                    /* shr precondition check */
                                    if (CtrlMem2.Ctx.pAgc->Shr == (UINT8) AGC_SHR_CHK) {
                                        /* shr precondition clear for next frame */
                                        CtrlMem2.Ctx.pAgc->Shr = (UINT8) AGC_SHR_NON_ADV;
                                    }
                                }
                            }
                        } else if (CtrlMem.Ctx.pAgc->Shr == (UINT8) AGC_SHR_CHK) {
                            /* shr precondition clear for next frame */
                            CtrlMem.Ctx.pAgc->Shr = (UINT8) AGC_SHR_NON_ADV;
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                if (pInfo->Op.User.AgcDelay <= 1U) {
                                    /* following frame to shr */
                                    EffectRaw = pCounter->Raw + 3U;
                                } else {
                                    /* same frame with shr */
                                    EffectRaw = pCounter->Raw + 4U;
                                }
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* first id get */
                                FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                /* is belong to first id? */
                                if (FrameId == FirstId) {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                    /* last agc write */
                                    FuncRetCode = AmbaImgSensorHAL_LastAgcWrite(ImageChanId);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                }
                            }
                        } else {
                            /* precondition is stil, inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                if (pInfo->Op.User.AgcDelay <= 1U) {
                                    /* following frame to shr */
                                    EffectRaw = pCounter->Raw + 3U;
                                } else {
                                    /* same frame with shr */
                                    EffectRaw = pCounter->Raw + 4U;
                                }
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* first id get */
                                FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                /* is belong to first id? */
                                if (FrameId == FirstId) {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                    /* last agc write */
                                    FuncRetCode = AmbaImgSensorHAL_LastAgcWrite(ImageChanId);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                }
                            }
                        }
                    } else {
                        /* ssg: precondition don't care */
                        FuncRetCode = AmbaImgSensorHAL_AgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                        pFifo->Agc.Index.Rd = (pFifo->Agc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                        pFifo->Agc.Count--;
                    }
                } else {
                    /* fifo empty, inter get */
                    pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                    /* is inter? */
                    if (pInter->Ctx.Bits.Num > 1U) {
                        /* counter get */
                        pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                        /* effect raw */
                        if (pInfo->Op.User.AgcDelay <= 1U) {
                            /* following frame to shr */
                            EffectRaw = pCounter->Raw + 3U;
                        } else {
                            /* same frame with shr */
                            EffectRaw = pCounter->Raw + 4U;
                        }
                        /* aux delay */
                        EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                        /* frame id get */
                        FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                        /* first id get */
                        FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                        /* is belong to first id? */
                        if (FrameId == FirstId) {
                            /* batch counter (TBD) */
                            pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                            /* last agc write */
                            FuncRetCode = AmbaImgSensorHAL_LastAgcWrite(ImageChanId);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL digital gain FIFO timing
 *  @param[in] ImageChanId image channel id
 *  @param[out] pCtrl pointer to the digital gain control
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* dgc check */
            if (pCtrl != NULL) {
                if (pFifo->Dgc.Count > 0U) {
                    /* dgc ctrl get */
                    *pCtrl = pFifo->Dgc.Ctrl[pFifo->Dgc.Index.Rd];
                } else {
                    /* fifo empty */
                    RetCode = NG_UL;
                }
            } else {
                /* dgc null */
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
 *  @private
 *  Amba image sensor HAL digital gain FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem2;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem2;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;
    UINT32 FirstId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            /* info check */
            if (pInfo != NULL) {
                /* dgc count? */
                if (pFifo->Dgc.Count > 0U) {
                    /* dgc ctrl get */
                    CtrlMem.Ctx.pUint64 = &(pFifo->Dgc.Ctrl[pFifo->Dgc.Index.Rd]);
                    /* dgc data get */
                    DataMem.Ctx.pUint64 = &(pFifo->Dgc.Data[pFifo->Dgc.Index.Rd]);
                    /* dgc type? */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                        /* dgc (dsp) */
                        switch ((UINT8) CtrlMem.Ctx.pDgc->Put) {
                            case (UINT8) DGC_PUT_ADV:
                                /* ssi/svr precondition check in dgc (dsp) */
                                if ((CtrlMem.Ctx.pDgc->Ssi == (UINT8) DGC_SSI_NON_ADV) &&
                                    (CtrlMem.Ctx.pDgc->Svr == (UINT8) DGC_SVR_NON_ADV)) {
                                    /* inter get */
                                    pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                                    /* is inter? */
                                    if (pInter->Ctx.Bits.Num > 1U) {
                                        /* counter get */
                                        pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                        /* effect raw */
                                        EffectRaw = pCounter->Raw + 4U;
                                        /* frame id get */
                                        FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                        /* is belong to inter id? */
                                        if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                            /* not belong */
                                            ChkCode = NG_UL;
                                        } else {
                                            /* batch counter (TBD) */
                                            pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                        }
                                    }

                                    if (ChkCode == OK_UL) {
                                        /* dgc (dsp) hdr check */
                                        if (CtrlMem.Ctx.pDgc->Hdr > 0U) {
                                            if (pHdrCtx != NULL) {
                                                if (pHdrCtx->Fifo.Dgc.Count > 0U) {
                                                    /* hdr dgc load */
                                                    for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                                        DataMem2.Ctx.pUint64 = &(pHdrCtx->Fifo.Dgc.Data[pHdrCtx->Fifo.Dgc.Index.Rd][i]);
                                                        pHdrCtx->Status.Load.Dgc[i] = (UINT32) (DataMem2.Ctx.pDgc->Dgc & 0xFFFFFFFFULL);
                                                    }
                                                    /* hdr dgc fifo update */
                                                    pHdrCtx->Fifo.Dgc.Index.Rd = (pHdrCtx->Fifo.Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                                    pHdrCtx->Fifo.Dgc.Count--;
                                                }
                                            }
                                        }
                                        /* dgc (dsp) write */
                                        FuncRetCode = AmbaImgSensorHAL_DgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                                        if (FuncRetCode != OK_UL) {
                                            RetCode = NG_UL;
                                        }
                                        /* rgb precondition udpate in shr */
                                        if (pFifo->Shr.Count > 0U) {
                                            /* shr ctrl get */
                                            CtrlMem2.Ctx.pUint64 = &(pFifo->Shr.Ctrl[pFifo->Dgc.Index.Rd]);
                                            /* rgb precondition clear in shr */
                                            CtrlMem2.Ctx.pShr->Rgb = (UINT8) SHR_RGB_NON_ADV;
                                        }
                                        /* dgc (dsp) fifo update */
                                        pFifo->Dgc.Index.Rd = (pFifo->Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                        pFifo->Dgc.Count--;
                                    }
                                }
                                break;
                            case (UINT8) DGC_PUT_SOF:
                                /* shr precondition in dgc */
                                if (CtrlMem.Ctx.pDgc->Shr == (UINT8) DGC_SHR_NON_ADV) {
                                    /* dgc (dsp) write */
                                    FuncRetCode = AmbaImgSensorHAL_DgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                    /* dgc (dsp) fifo update */
                                    pFifo->Dgc.Index.Rd = (pFifo->Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                    pFifo->Dgc.Count--;
                                }
                                break;
                            default:
                                /* nothing to do */
                                break;
                        }
                    } else {
                        /* dgc (sensor) */
                        if (pInfo->Cfg.ShutterMode == (UINT32) IMAGE_SHR_SSR) {
                            /* ssr: shr precondition in dgc (sensor) */
                            if (CtrlMem.Ctx.pDgc->Shr == (UINT8) DGC_SHR_NON_ADV) {
                                /* inter get */
                                pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                                /* is inter? */
                                if (pInter->Ctx.Bits.Num > 1U) {
                                    /* counter get */
                                    pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                    /* effect raw */
                                    if (pInfo->Op.User.DgcDelay <= 1U) {
                                        /* following frame to shr */
                                        EffectRaw = pCounter->Raw + 3U;
                                    } else {
                                        /* same frame with shr */
                                        EffectRaw = pCounter->Raw + 4U;
                                    }
                                    /* aux delay */
                                    EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                    /* frame id get */
                                    FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                    /* is belong to inter id? */
                                    if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                        /* not belong */
                                        ChkCode = NG_UL;
                                    } else {
                                        /* batch counter (TBD) */
                                        pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                    }
                                }

                                if (ChkCode == OK_UL) {
                                    /* hdr check */
                                    if (CtrlMem.Ctx.pDgc->Hdr > 0U) {
                                        if (pHdrCtx != NULL) {
                                            if (pHdrCtx->Fifo.Dgc.Count > 0U) {
                                                /* hdr dgc load */
                                                for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                                    DataMem2.Ctx.pUint64 = &(pHdrCtx->Fifo.Dgc.Data[pHdrCtx->Fifo.Dgc.Index.Rd][i]);
                                                    pHdrCtx->Status.Load.Dgc[i] = (UINT32) (DataMem2.Ctx.pDgc->Dgc & 0xFFFFFFFFULL);
                                                }
                                                /* hdr dgc fifo update */
                                                pHdrCtx->Fifo.Dgc.Index.Rd = (pHdrCtx->Fifo.Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                                pHdrCtx->Fifo.Dgc.Count--;
                                            }
                                        }
                                    }
                                    /* dgc write */
                                    FuncRetCode =  AmbaImgSensorHAL_DgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                    /* dgc fifo update */
                                    pFifo->Dgc.Index.Rd = (pFifo->Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                    pFifo->Dgc.Count--;
                                    /* shr precondition check for next dgc */
                                    if (pFifo->Dgc.Count > 0U) {
                                        /* dgc ctrl get */
                                        CtrlMem2.Ctx.pUint64 = &(pFifo->Dgc.Ctrl[pFifo->Dgc.Index.Rd]);
                                        /* shr precondition check */
                                        if (CtrlMem2.Ctx.pDgc->Shr == (UINT8) DGC_SHR_CHK) {
                                            /* shr precondition clear for next frame */
                                            CtrlMem2.Ctx.pDgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                                        }
                                    }
                                }
                            } else if (CtrlMem.Ctx.pDgc->Shr == (UINT8) DGC_SHR_CHK) {
                                /* shr precondition clear for next frame */
                                CtrlMem.Ctx.pDgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                                /* inter get */
                                pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                                /* is inter? */
                                if (pInter->Ctx.Bits.Num > 1U) {
                                    /* counter get */
                                    pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                    /* effect raw */
                                    if (pInfo->Op.User.DgcDelay <= 1U) {
                                        /* following frame to shr */
                                        EffectRaw = pCounter->Raw + 3U;
                                    } else {
                                        /* same frame with shr */
                                        EffectRaw = pCounter->Raw + 4U;
                                    }
                                    /* aux delay */
                                    EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                    /* frame id get */
                                    FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                    /* first id get */
                                    FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                    /* is belong to first id? */
                                    if (FrameId == FirstId) {
                                        /* batch counter (TBD) */
                                        pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                        /* last dgc write */
                                        FuncRetCode = AmbaImgSensorHAL_LastDgcWrite(ImageChanId);
                                        if (FuncRetCode != OK_UL) {
                                            RetCode = NG_UL;
                                        }
                                    }
                                }
                            } else {
                                /* precondition is still, inter get */
                                pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                                /* is inter? */
                                if (pInter->Ctx.Bits.Num > 1U) {
                                    /* counter get */
                                    pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                    /* effect raw */
                                    if (pInfo->Op.User.DgcDelay <= 1U) {
                                        /* following frame to shr */
                                        EffectRaw = pCounter->Raw + 3U;
                                    } else {
                                        /* same frame with shr */
                                        EffectRaw = pCounter->Raw + 4U;
                                    }
                                    /* aux delay */
                                    EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                    /* frame id get */
                                    FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                    /* first id get */
                                    FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                    /* is belong to first id? */
                                    if (FrameId == FirstId) {
                                        /* batch counter (TBD) */
                                        pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                        /* last dgc write */
                                        FuncRetCode = AmbaImgSensorHAL_LastDgcWrite(ImageChanId);
                                        if (FuncRetCode != OK_UL) {
                                            RetCode = NG_UL;
                                        }
                                    }
                                }
                            }
                        } else {
                            /* ssg: precondition don't care */
                            FuncRetCode = AmbaImgSensorHAL_DgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* dgc fifo update */
                            pFifo->Dgc.Index.Rd = (pFifo->Dgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                            pFifo->Dgc.Count--;
                        }
                    }
                } else {
                    /* fifo empty */
                    if (pInfo->Op.User.DgcType != (UINT32) IMAGE_DGC_DSP) {
                        /* inter get */
                        pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                        /* is inter? */
                        if (pInter->Ctx.Bits.Num > 1U) {
                            /* counter get */
                            pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                            /* effect raw */
                            if (pInfo->Op.User.DgcDelay <= 1U) {
                                /* following frame to shr */
                                EffectRaw = pCounter->Raw + 3U;
                            } else {
                                /* same frame with shr */
                                EffectRaw = pCounter->Raw + 4U;
                            }
                            /* aux delay */
                            EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                            /* frame id get */
                            FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                            /* first id get */
                            FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                            /* is belong to first id? */
                            if (FrameId == FirstId) {
                                /* batch counter (TBD) */
                                pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                /* last dgc write */
                                FuncRetCode = AmbaImgSensorHAL_LastDgcWrite(ImageChanId);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL white balance gain FIFO timing
 *  @param[in] ImageChanId image channel id
 *  @param[out] pCtrl pointer to the white balance gain control
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_WgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* wgc check */
            if (pCtrl != NULL) {
                if (pFifo->Wgc.Count > 0U) {
                    /* wgc ctrl get */
                    *pCtrl = pFifo->Wgc.Ctrl[pFifo->Wgc.Index.Rd];
                } else {
                    /* fifo empty */
                    RetCode = NG_UL;
                }
            } else {
                /* wgc null */
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
 *  @private
 *  Amba image sensor HAL white balance gain FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_WgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem2;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem2;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;
    UINT32 FirstId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            /* info check */
            if (pInfo != NULL) {
                /* wgc count? */
                if (pFifo->Wgc.Count > 0U) {
                    /* wgc ctrl get */
                    CtrlMem.Ctx.pUint64 = &(pFifo->Wgc.Ctrl[pFifo->Wgc.Index.Rd]);
                    /* wgc data get */
                    DataMem.Ctx.pUint64 = &(pFifo->Wgc.Data[pFifo->Wgc.Index.Rd]);
                    /* wgc (sensor) */
                    if (pInfo->Cfg.ShutterMode == (UINT32) IMAGE_SHR_SSR) {
                        /* ssr: shr precondition in wgc (sensor) */
                        if (CtrlMem.Ctx.pWgc->Shr == (UINT8) DGC_SHR_NON_ADV) {
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                if (pInfo->Op.User.WgcDelay <= 1U) {
                                    /* following frame to shr */
                                    EffectRaw = pCounter->Raw + 3U;
                                } else {
                                    /* same frame with shr */
                                    EffectRaw = pCounter->Raw + 4U;
                                }
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* is belong to inter id? */
                                if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                    /* not belong */
                                    ChkCode = NG_UL;
                                } else {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                }
                            }

                            if (ChkCode == OK_UL) {
                                /* hdr check */
                                if (CtrlMem.Ctx.pWgc->Hdr > 0U) {
                                    if (pHdrCtx != NULL) {
                                        if (pHdrCtx->Fifo.Wgc.Count > 0U) {
                                            /* hdr wgc load */
                                            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                                DataMem2.Ctx.pUint64 = &(pHdrCtx->Fifo.Wgc.Data[pHdrCtx->Fifo.Wgc.Index.Rd][i]);
                                                pHdrCtx->Status.Load.Wgc[i] = DataMem2.Ctx.pWgc->Wgc;
                                            }
                                            /* hdr wgc fifo update */
                                            pHdrCtx->Fifo.Wgc.Index.Rd = (pHdrCtx->Fifo.Wgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                            pHdrCtx->Fifo.Wgc.Count--;
                                        }
                                    }
                                }
                                /* wgc write */
                                FuncRetCode =  AmbaImgSensorHAL_WgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                                /* wgc fifo update */
                                pFifo->Wgc.Index.Rd = (pFifo->Wgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                pFifo->Wgc.Count--;
                                /* shr precondition check for next wgc */
                                if (pFifo->Wgc.Count > 0U) {
                                    /* wgc ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->Wgc.Ctrl[pFifo->Wgc.Index.Rd]);
                                    /* shr precondition check */
                                    if (CtrlMem2.Ctx.pWgc->Shr == (UINT8) DGC_SHR_CHK) {
                                        /* shr precondition clear for next frame */
                                        CtrlMem2.Ctx.pWgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                                    }
                                }
                            }
                        } else if (CtrlMem.Ctx.pWgc->Shr == (UINT8) DGC_SHR_CHK) {
                            /* shr precondition clear for next frame */
                            CtrlMem.Ctx.pWgc->Shr = (UINT8) DGC_SHR_NON_ADV;
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                if (pInfo->Op.User.WgcDelay <= 1U) {
                                    /* following frame to shr */
                                    EffectRaw = pCounter->Raw + 3U;
                                } else {
                                    /* same frame with shr */
                                    EffectRaw = pCounter->Raw + 4U;
                                }
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* first id get */
                                FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                /* is belong to first id? */
                                if (FrameId == FirstId) {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                    /* last wgc write */
                                    FuncRetCode = AmbaImgSensorHAL_LastWgcWrite(ImageChanId);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                }
                            }
                        } else {
                            /* precondition is still, inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                if (pInfo->Op.User.WgcDelay <= 1U) {
                                    /* following frame to shr */
                                    EffectRaw = pCounter->Raw + 3U;
                                } else {
                                    /* same frame with shr */
                                    EffectRaw = pCounter->Raw + 4U;
                                }
                                /* aux delay */
                                EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* first id get */
                                FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                                /* is belong to first id? */
                                if (FrameId == FirstId) {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                    /* last wgc write */
                                    FuncRetCode = AmbaImgSensorHAL_LastWgcWrite(ImageChanId);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                }
                            }
                        }
                    } else {
                        /* ssg: precondition don't care */
                        FuncRetCode = AmbaImgSensorHAL_WgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                        /* wgc fifo update */
                        pFifo->Wgc.Index.Rd = (pFifo->Wgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                        pFifo->Wgc.Count--;
                    }
                } else {
                    /* fifo empty, inter get */
                    pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                    /* is inter? */
                    if (pInter->Ctx.Bits.Num > 1U) {
                        /* counter get */
                        pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                        /* effect raw */
                        if (pInfo->Op.User.WgcDelay <= 1U) {
                            /* following frame after shr */
                            EffectRaw = pCounter->Raw + 3U;
                        } else {
                            /* same frame with shr */
                            EffectRaw = pCounter->Raw + 4U;
                        }
                        /* aux delay */
                        EffectRaw = EffectRaw + pInfo->Op.User.AuxDelay;
                        /* frame id get */
                        FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                        /* first id get */
                        FirstId = (UINT32) pInter->Ctx.Bits.FirstId;
                        /* is belong to first id? */
                        if (FrameId == FirstId) {
                            /* batch counter (TBD) */
                            pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                            /* last wgc write */
                            FuncRetCode = AmbaImgSensorHAL_LastWgcWrite(ImageChanId);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL DSP digital gain FIFO timing
 *  @param[in] ImageChanId image channel id
 *  @param[out] pCtrl pointer to the DSP digital gain control
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DDgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* ddgc check */
            if (pCtrl != NULL) {
                if (pFifo->DDgc.Count > 0U) {
                    /* ddgc ctrl get */
                    *pCtrl = pFifo->DDgc.Ctrl[pFifo->DDgc.Index.Rd];
                } else {
                    /* fifo empty */
                    RetCode = NG_UL;
                }
            } else {
                /* ddgc null */
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
 *  @private
 *  Amba image sensor HAL DSP digital gain FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DDgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem2;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem2;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* fifo get */
            pFifo = &(pCtx->Fifo);
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            /* ddgc count? */
            if (pFifo->DDgc.Count > 0U) {
                /* dgc ctrl get */
                CtrlMem.Ctx.pUint64 = &(pFifo->DDgc.Ctrl[pFifo->DDgc.Index.Rd]);
                /* dgc data get */
                DataMem.Ctx.pUint64 = &(pFifo->DDgc.Data[pFifo->DDgc.Index.Rd]);

                /* dsp dgc timing check */
                switch ((UINT8) CtrlMem.Ctx.pDDgc->Put) {
                    case (UINT8) DGC_PUT_ADV:
                        /* ssi/svr precondition in ddgc */
                        if ((CtrlMem.Ctx.pDDgc->Ssi == (UINT8) DGC_SSI_NON_ADV) &&
                            (CtrlMem.Ctx.pDDgc->Svr == (UINT8) DGC_SVR_NON_ADV)) {
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                EffectRaw = pCounter->Raw + 4U;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* is belong to inter id? */
                                if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                    /* not belong */
                                    ChkCode = NG_UL;
                                } else {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                }
                            }

                            if (ChkCode == OK_UL) {
                                /* dgc (dsp) hdr check */
                                if (CtrlMem.Ctx.pDDgc->Hdr > 0U) {
                                    if (pHdrCtx != NULL) {
                                        if (pHdrCtx->Fifo.DDgc.Count > 0U) {
                                            /* hdr ddgc load */
                                            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                                DataMem2.Ctx.pUint64 = &(pHdrCtx->Fifo.DDgc.Data[pHdrCtx->Fifo.DDgc.Index.Rd][i]);
                                                pHdrCtx->Status.Load.DDgc[i] = (UINT32) (DataMem2.Ctx.pDDgc->Dgc & 0xFFFFFFFFULL);
                                            }
                                            /* hdr ddgc fifo update */
                                            pHdrCtx->Fifo.DDgc.Index.Rd = (pHdrCtx->Fifo.DDgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                            pHdrCtx->Fifo.DDgc.Count--;
                                        }
                                    }
                                }
                                /* dsp dgc write */
                                FuncRetCode = AmbaImgSensorHAL_DDgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }

                                /* rgb precondition in shr */
                                if (pFifo->Shr.Count > 0U) {
                                    /* shr ctrl get */
                                    CtrlMem2.Ctx.pUint64 = &(pFifo->Shr.Ctrl[pFifo->DDgc.Index.Rd]);
                                    /* rgb precondition clear in shr */
                                    CtrlMem2.Ctx.pShr->Rgb = (UINT8) SHR_RGB_NON_ADV;
                                }
                                /* dsp dgc fifo update */
                                pFifo->DDgc.Index.Rd = (pFifo->DDgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                                pFifo->DDgc.Count--;
                            }
                        }
                        break;
                    case (UINT8) DGC_PUT_SOF:
                        /* shr precondition in ddgc */
                        if (CtrlMem.Ctx.pDDgc->Shr == (UINT8) DGC_SHR_NON_ADV) {
                            /* dsp dgc write */
                            FuncRetCode = AmbaImgSensorHAL_DDgcWrite(ImageChanId, *(CtrlMem.Ctx.pUint64), *(DataMem.Ctx.pUint64));
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* dsp dgc fifo upate */
                            pFifo->DDgc.Index.Rd = (pFifo->DDgc.Index.Rd + 1U) % AMBA_IMG_SENSOR_HAL_FIFO_SIZE;
                            pFifo->DDgc.Count--;
                        }
                        break;
                    default:
                        /* nothing to do */
                        break;
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

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL aik FIFO check
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_AikFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_FIFO_s *pFifo;
    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    AMBA_IMG_CHANNEL_INTER_s *pInter;
    const AMBA_IMG_SENSOR_HAL_COUNTER_s *pCounter;

    UINT32 EffectRaw;
    UINT32 FrameId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* fifo get */
            pFifo = &(AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->Fifo);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;

            /* info check */
            if (pInfo != NULL) {
                /* shr count? */
                if (pFifo->Shr.Count > 0U) {
                    /* shr ctrl get */
                    CtrlMem.Ctx.pUint64 = &(pFifo->Shr.Ctrl[pFifo->Shr.Index.Rd]);
                    /* aik precondition in adv aik */
                    if ((CtrlMem.Ctx.pShr->Aik >= (UINT8) SHR_AIK_ADV) &&
                        (CtrlMem.Ctx.pShr->Ssi < (UINT8) SHR_SSI_ADV) &&
                        (CtrlMem.Ctx.pShr->Fll == (UINT8) SHR_SVR_NON_ADV)) {
                        /* rgb precondition in adv aik */
                        if (CtrlMem.Ctx.pShr->Rgb == (UINT8) SHR_RGB_NON_ADV) {
                            /* inter get */
                            pInter = &(pFrwImageChannel[VinId][ChainId].pCtx->Inter);
                            /* is inter? */
                            if (pInter->Ctx.Bits.Num > 1U) {
                                /* counter get */
                                pCounter = &(AmbaImgSensorHAL_Vin[VinId].Counter);
                                /* effect raw */
                                EffectRaw = pCounter->Raw + 4U;
                                /* frame id get */
                                FrameId = EffectRaw % ((UINT32) pInter->Ctx.Bits.Num);
                                /* is belong to inter id? */
                                if ((((UINT32) pInter->Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                    /* not belong */
                                    ChkCode = NG_UL;
                                } else {
                                    /* batch counter (TBD) */
                                    pInter->Ctx.Bits.Batch = (UINT8) (pCounter->Raw & 0xFFU);
                                }
                            }

                            if (ChkCode == OK_UL) {
                                AMBA_IMG_CHANNEL_AIK_PROC_MSG_s AikMsg;
                                AikMsg.Ctx.Data = 0ULL;
                                /* user ctrl get */
                                DataMem.Ctx.pUint64 = &(pFifo->User.Data[pFifo->Shr.Index.Rd]);

                                /* aik timing */
                                switch ((UINT8) CtrlMem.Ctx.pShr->Aik) {
                                    case (UINT8) SHR_AIK_ADV_SOF:
                                        /* pre aik & aik */
                                        CtrlMem.Ctx.pShr->Aik = (UINT8) SHR_AIK_SOF;
                                        if (pInfo->Op.Ctrl.AdvAikCmd == 0U) {
                                            AikMsg.Ctx.Bits.PreAik = 1U;
                                        }
                                        break;
                                    case (UINT8) SHR_AIK_ADV:
                                    default:
                                        /* adv aik */
                                        CtrlMem.Ctx.pShr->Aik = (UINT8) SHR_AIK_NONE;
                                        break;
                                }

                                /* aik write */
                                FuncRetCode = AmbaImgSensorHAL_AikWrite(ImageChanId, AikMsg.Ctx.Data, DataMem.Ctx.pUser->User);
                                if (FuncRetCode == OK_UL) {
                                    if (CtrlMem.Ctx.pShr->Aik == (UINT8) SHR_AIK_NONE) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Adv_Aik");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_Aik");
                                    }
                                } else {
                                    if (CtrlMem.Ctx.pShr->Aik == (UINT8) SHR_AIK_NONE) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Adv_Aik!");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_Aik!");
                                    }
                                    RetCode = NG_UL;
                                }

                                /* efov write */
                                if (pFrwImageChannel[VinId][ChainId].pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                                    AikMsg.Ctx.Bits2.Ae = 1U;
                                    FuncRetCode = AmbaImgSensorHAL_EFovWrite(ImageChanId, AikMsg.Ctx.Data, DataMem.Ctx.pUser->User);
                                    if (FuncRetCode == OK_UL) {
                                        if (CtrlMem.Ctx.pShr->Aik == (UINT8) SHR_AIK_NONE) {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Adv_EFov");
                                        } else {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_EFov");
                                        }
                                    } else {
                                        if (CtrlMem.Ctx.pShr->Aik == (UINT8) SHR_AIK_NONE) {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Adv_EFov!");
                                        } else {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_EFov!");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  Amba image sensor HAL slow shutter index write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl shutter control
 *  @param[in] Data shutter data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_SsiWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;

    (void) Data;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;

            /* info? */
            if (pInfo != NULL) {
                /* shr ctrl get */
                CtrlMem.Ctx.pUint64 = &Ctrl;

                /* parse shr2 ctrl bits */
                switch ((UINT8) CtrlMem.Ctx.pShr->Sig) {
                    case (UINT8) SHR_SIG_BOTH:
                    case (UINT8) SHR_SIG_SVR:
                        if (pVin->Op.Bits.Out2Dsp > 0U) {
                            /* slow shutter change in video */
                            FuncRetCode = AmbaImgSensorHAL_SlowShutter(ImageChanId, CtrlMem.Ctx.pShr->Svr);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_Adv");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_Adv!");
                                RetCode = NG_UL;
                            }

                            if (pVin->Timing.Op.Bits.Enable > 0U) {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                var_utoa(CtrlMem.Ctx.pShr->Svr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }
                        }
                        break;
                    case (UINT8) SHR_SIG_SHR:
                    case (UINT8) SHR_SIG_SSR:
                    case (UINT8) SHR_SIG_SSG:
                    default:
                        /* nothing to do */
                        break;
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL slow shutter write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl shutter control
 *  @param[in] Data shutter data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_SvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;

    UINT32 Svr_UL;

    (void) Data;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            /* info? */
            if (pInfo != NULL) {
                /* shr ctrl get */
                CtrlMem.Ctx.pUint64 = &Ctrl;

                /* parse shr2 ctrl bits */
                switch ((UINT8) CtrlMem.Ctx.pShr->Sig) {
                    case (UINT8) SHR_SIG_BOTH:
                    case (UINT8) SHR_SIG_SVR:
                        /* adv svr in here */
                        if (pVin->Op.Bits.Out2Sensor > 0U) {
                            /* sensor svr write in video */
                            Svr_UL = ((UINT32) 1U) << (CtrlMem.Ctx.pShr->Svr & 0x1FU);
                            FuncRetCode = AmbaImgSensorDrv_SvrWrite(ImageChanId, &Svr_UL);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Adv");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Adv!");
                                RetCode = NG_UL;
                            }
                            if (pVin->Timing.Op.Bits.Enable > 0U) {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                var_utoa(CtrlMem.Ctx.pShr->Svr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }
                        }
                        break;
                    case (UINT8) SHR_SIG_SHR:
                    case (UINT8) SHR_SIG_SSR:
                    case (UINT8) SHR_SIG_SSG:
                    default:
                        /* nothing to do */
                        break;
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

    return RetCode;
}

/**
 *  Amba image sensor HAL last slow shutter write
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_LastSvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            /* info? */
            if (pInfo != NULL) {
                /* out to sensor? */
                if (pVin->Op.Bits.Out2Sensor > 0U) {
                    /* last svr write */
                    FuncRetCode = AmbaImgSensorDrv_SvrWrite(ImageChanId, &(pInfo->Op.Status.LastSvr));
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_L");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_L!");
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

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL shutter write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl shutter control
 *  @param[in] Data shutter data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_ShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 i;
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    UINT32 Var_UL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            /* info? */
            if (pInfo != NULL) {
                /* shr ctrl get */
                CtrlMem.Ctx.pUint64 = &Ctrl;
                /* shr data get */
                DataMem.Ctx.pUint64 = &Data;

                /* parse shr2 ctrl bits */
                switch ((UINT8) CtrlMem.Ctx.pShr->Sig) {
                    case (UINT8) SHR_SIG_BOTH:
                        /* svr/shr write */
                        if (pVin->Op.Bits.Out2Sensor > 0U) {
                            /* svr */
                            if (pInfo->Op.Ctrl.AdvSvrUpd <= 1U) {
                                /* non adv svr in here */
                                Var_UL = ((UINT32) 1U) << (CtrlMem.Ctx.pShr->Svr & 0x1FU);
                                FuncRetCode = AmbaImgSensorDrv_SvrWrite(ImageChanId, &Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr!");
                                    RetCode = NG_UL;
                                }
                            }

                            /* hdr? */
                            if (CtrlMem.Ctx.pShr->Hdr == 0U) {
                                /* shr */
                                Var_UL = (UINT32) (DataMem.Ctx.pShr->Shr & 0xFFFFFFFFULL);
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, &Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr!");
                                    RetCode = NG_UL;
                                }
                            } else {
                                /* hdr shr */
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, pHdrCtx->Status.Load.Shr);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Hdr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Hdr!");
                                    RetCode = NG_UL;
                                }
                            }

                            /* last shr status */
                            pCtx->Status.Last.Shr = DataMem.Ctx.pShr->Shr;
                            /* hdr? */
                            if (CtrlMem.Ctx.pShr->Hdr > 0U) {
                                /* last hdr shr status */
                                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                    pHdrCtx->Status.Last.Shr[i] = pHdrCtx->Status.Load.Shr[i];
                                }
                            }
                            /* last ssi status */
                            pInfo->Op.Status.LastSsIndex = CtrlMem.Ctx.pShr->Svr;
                            /* last svr status */
                            pInfo->Op.Status.LastSvr = ((UINT32) 1U) << (CtrlMem.Ctx.pShr->Svr & 0x1FU);

                            /* svr event */
                            FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SVR_UPD);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Upd");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Upd!");
                                RetCode = NG_UL;
                            }

                            if (pVin->Timing.Op.Bits.Enable > 0U) {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                var_utoa((UINT32) (DataMem.Ctx.pShr->Shr & 0xFFFFFFFFULL), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                var_utoa(CtrlMem.Ctx.pShr->Svr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }
                        }

                        /* ssi */
                        switch ((UINT8) CtrlMem.Ctx.pShr->Ssi) {
                            case (UINT8) SHR_SSI_SOF:
                                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SSI_SOF);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_Sof");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_Sof!");
                                    RetCode = NG_UL;
                                }
                                break;
                            default:
                                /* nothing to do */
                                break;
                        }

                        /* aik */
                        switch ((UINT8) CtrlMem.Ctx.pShr->Aik) {
                            case (UINT8) SHR_AIK_SOF:
                                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_AIK_SOF);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Sof");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Sof!");
                                    RetCode = NG_UL;
                                }
                                break;
                            default:
                                /* nothing to do */
                                break;
                        }

                        /* shr ack */
                        if (CtrlMem.Ctx.pShr->Ack == (UINT8) SHR_ACK_SHR) {
                            FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SHR_ACK);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack_E");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack_E!");
                                RetCode = NG_UL;
                            }
                        }
                        break;
                    case (UINT8) SHR_SIG_SHR:
                        /* shr write */
                        if (pVin->Op.Bits.Out2Sensor > 0U) {
                            /* hdr? */
                            if (CtrlMem.Ctx.pShr->Hdr == 0U) {
                                /* shr */
                                Var_UL = (UINT32) (DataMem.Ctx.pShr->Shr & 0xFFFFFFFFULL);
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, &Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr!");
                                    RetCode = NG_UL;
                                }
                            } else {
                                /* hdr shr */
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, pHdrCtx->Status.Load.Shr);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Hdr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Hdr!");
                                    RetCode = NG_UL;
                                }
                            }

                            if (pVin->Timing.Op.Bits.Enable > 0U) {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                var_utoa((UINT32) (DataMem.Ctx.pShr->Shr & 0xFFFFFFFFULL), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                str[1] = 'c';
                                var_utoa(pCtx->Fifo.Shr.Count, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }

                            /* last data status */
                            pCtx->Status.Last.Shr = DataMem.Ctx.pShr->Shr;
                            /* hdr? */
                            if (CtrlMem.Ctx.pShr->Hdr > 0U) {
                                /* last hdr reg status */
                                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                    pHdrCtx->Status.Last.Shr[i] = pHdrCtx->Status.Load.Shr[i];
                                }
                            }
                        }

                        /* aik */
                        switch ((UINT8) CtrlMem.Ctx.pShr->Aik) {
                            case (UINT8) SHR_AIK_SOF:
                                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_AIK_SOF);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Sof");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Sof!");
                                    RetCode = NG_UL;
                                }
                                break;
                            default:
                                /* nothing to do */
                                break;
                        }

                        /* shr ack */
                        if (CtrlMem.Ctx.pShr->Ack == (UINT8) SHR_ACK_SHR) {
                            FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SHR_ACK);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack_E");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack_E!");
                                RetCode = NG_UL;
                            }
                        }
                        break;
                    case (UINT8) SHR_SIG_SVR:
                        /* svr write/shr refrest */
                        if (pVin->Op.Bits.Out2Sensor > 0U) {
                            /* svr write */
                            if (pInfo->Op.Ctrl.AdvSvrUpd <= 1U) {
                                /* non adv in here */
                                Var_UL = ((UINT32) 1U) << (CtrlMem.Ctx.pShr->Svr & 0x1FU);
                                FuncRetCode = AmbaImgSensorDrv_SvrWrite(ImageChanId, &Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr!");
                                    RetCode = NG_UL;
                                }
                            }

                            /* hdr? */
                            if (CtrlMem.Ctx.pShr->Hdr == 0U) {
                                /* shr refresh */
                                Var_UL = (UINT32) (pCtx->Status.Last.Shr & 0xFFFFFFFFULL);
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, &Var_UL);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            } else {
                                /* hdr shr refresh */
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, pHdrCtx->Status.Last.Shr);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            }

                            /* last ssi status */
                            pInfo->Op.Status.LastSsIndex = CtrlMem.Ctx.pShr->Svr;
                            /* last svr status */
                            pInfo->Op.Status.LastSvr = ((UINT32) 1U) << (CtrlMem.Ctx.pShr->Svr & 0x1FU);

                            /* svr event */
                            FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SVR_UPD);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Upd");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Upd!");
                                RetCode = NG_UL;
                            }

                            if (pInfo->Op.Ctrl.AdvSvrUpd <= 1U) {
                                if (pVin->Timing.Op.Bits.Enable > 0U) {
                                    char str[11];
                                    str[0] = ' ';str[1] = ' ';
                                    var_utoa(CtrlMem.Ctx.pShr->Svr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                }
                            }
                        }

                        /* ssi */
                        switch ((UINT8) CtrlMem.Ctx.pShr->Ssi) {
                            case (UINT8) SHR_SSI_SOF:
                                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SSI_SOF);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_Sof");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_Sof!");
                                    RetCode = NG_UL;
                                }
                                break;
                            default:
                                /* nothing to do */
                                break;
                        }

                        /* shr ack */
                        if (CtrlMem.Ctx.pShr->Ack == (UINT8) SHR_ACK_SHR) {
                            FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_SHR_ACK);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack_E");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack_E!");
                                RetCode = NG_UL;
                            }
                        }
                        break;
                    case (UINT8) SHR_SIG_SSR:
                        /* svr/shr write */
                        if (pVin->Op.Bits.Out2Sensor > 0U) {
                            /*
                             *  still capture:
                             *  automatically fll need be enabled if adv_svr_upd > 1.
                             */
                            if (pInfo->Op.Ctrl.AdvSvrUpd <= 1U) {
                                Var_UL = CtrlMem.Ctx.pShr->Svr;
                                FuncRetCode = AmbaImgSensorDrv_SvrWrite(ImageChanId, &Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Ssr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_Ssr!");
                                    RetCode = NG_UL;
                                }
                            }

                            /* shr write */
                            if (CtrlMem.Ctx.pShr->Hdr == 0U) {
                                Var_UL = (UINT32) (DataMem.Ctx.pShr->Shr & 0xFFFFFFFFULL);
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, &Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ssr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ssr!");
                                    RetCode = NG_UL;
                                }
                            } else {
                                FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, pHdrCtx->Status.Load.Shr);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_H_Ssr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_H_Ssr!");
                                    RetCode = NG_UL;
                                }
                            }

                            if (pVin->Timing.Op.Bits.Enable > 0U) {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                var_utoa((UINT32) (DataMem.Ctx.pShr->Shr & 0xFFFFFFFFULL), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }

                            /* last reg status */
                            pCtx->Status.Last.Shr = DataMem.Ctx.pShr->Shr;
                            /* hdr? */
                            if (CtrlMem.Ctx.pShr->Hdr > 0U) {
                                /* last hdr reg status */
                                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                    pHdrCtx->Status.Last.Shr[i] = pHdrCtx->Status.Load.Shr[i];
                                }
                            }
                            /* last svr status */
                            pInfo->Op.Status.LastSvr = CtrlMem.Ctx.pShr->Svr;
                        }
                        break;
                    case (UINT8) SHR_SIG_SSG:
                        /* not yet */
                        break;
                    default:
                        /* nothing to do */
                        break;
                }
            } else {
                /* info null */
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
 *  Amba image sensor HAL last shutter write
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_LastShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 HdrId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    const AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    UINT32 Var_UL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    HdrId = ImageChanId.Ctx.Bits.HdrId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            /* info? */
            if (pInfo != NULL) {
                if (pVin->Op.Bits.Out2Sensor > 0U) {
                    /* non-adv svr? */
                    if (pInfo->Op.Ctrl.AdvSvrUpd <= 1U) {
                        /* last svr write */
                        FuncRetCode = AmbaImgSensorDrv_SvrWrite(ImageChanId, &(pInfo->Op.Status.LastSvr));
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_L");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_L!");
                            RetCode = NG_UL;
                        }
                    }
                    /* last shr write */
                    if (HdrId == 0U) {
                        /* non-hdr */
                        Var_UL = (UINT32) (pCtx->Status.Last.Shr & 0xFFFFFFFFULL);
                        FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, &Var_UL);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_L");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_L!");
                            RetCode = NG_UL;
                        }
                    } else {
                        /* hdr */
                        FuncRetCode = AmbaImgSensorDrv_ShrWrite(ImageChanId, pHdrCtx->Status.Last.Shr);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Hdr_L");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Hdr_L!");
                            RetCode = NG_UL;
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL master sync write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Data master sync data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_MscWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            /* info? */
            if (pInfo != NULL) {
                /* out to sensor? */
                if (pVin->Op.Bits.Out2Sensor > 0U) {
                    /* msc write */
                    FuncRetCode = AmbaImgSensorDrv_MscWrite(ImageChanId, &Data);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc!");
                        RetCode = NG_UL;
                    }

                    if (pVin->Timing.Op.Bits.Enable > 0U) {
                        char str[11];
                        str[0] = ' ';str[1] = ' ';
                        var_utoa(Data, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
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

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL sync light source write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Data user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_SlsWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* info get */
            pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
            /* info? */
            if (pInfo != NULL) {
                /* out to sensor? */
                if (pVin->Op.Bits.Out2Sensor > 0U) {
                    /* sls cb */
                    if (pFrwImageChannel[VinId][ChainId].SlsCb != NULL) {
                        FuncRetCode = pFrwImageChannel[VinId][ChainId].SlsCb(ImageChanId, Data);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sls");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sls!");
                            RetCode = NG_UL;
                        }
                    } else {
                        /* cb null */
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

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL analog gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl analog gain control
 *  @param[in] Data analog gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_AgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    UINT32 Var_UL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            if (pInfo != NULL) {
                /* agc ctrl get */
                CtrlMem.Ctx.pUint64 = &Ctrl;
                /* agc data get */
                DataMem.Ctx.pUint64 = &Data;

                if (CtrlMem.Ctx.pAgc->Sig != (UINT8) AGC_SIG_SKIP) {
                    /* agc write */
                    if (pVin->Op.Bits.Out2Sensor > 0U) {
                        /* hdr? */
                        if (CtrlMem.Ctx.pAgc->Hdr > 0U) {
                            FuncRetCode  = AmbaImgSensorDrv_AgcWrite(ImageChanId, pHdrCtx->Status.Load.Agc);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Hdr");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Hdr!");
                                RetCode = NG_UL;
                            }
                        } else {
                            Var_UL = (UINT32) (DataMem.Ctx.pAgc->Agc & 0xFFFFFFFFULL);
                            FuncRetCode = AmbaImgSensorDrv_AgcWrite(ImageChanId, &Var_UL);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc!");
                                RetCode = NG_UL;
                            }
                        }

                        if (pVin->Timing.Op.Bits.Enable > 0U) {
                            char str[11];
                            str[0] = ' ';str[1] = ' ';
                            var_utoa((UINT32) (DataMem.Ctx.pAgc->Agc & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'c';
                            var_utoa(pCtx->Fifo.Agc.Count, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        }

                        /* last reg update */
                        pCtx->Status.Last.Agc = DataMem.Ctx.pAgc->Agc;
                        /* last hdr reg udpate */
                        if (CtrlMem.Ctx.pAgc->Hdr > 0U) {
                            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                pHdrCtx->Status.Last.Agc[i] = pHdrCtx->Status.Load.Agc[i];
                            }
                        }
                    }

                    /* agc ack */
                    if (CtrlMem.Ctx.pAgc->Ack > 0U) {
                        FuncRetCode =  AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_AGC_ACK);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_E");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_E!");
                            RetCode = NG_UL;
                        }
                    }
                }
            } else {
                /* info null */
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
 *  Amba image sensor HAL last analog gain write
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_LastAgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 HdrId;

    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    const AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    UINT32 Var_UL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    HdrId = ImageChanId.Ctx.Bits.HdrId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
           /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            if (pVin->Op.Bits.Out2Sensor > 0U) {
                /* last agc */
                if (HdrId == 0U) {
                    /* non-hdr */
                    Var_UL = (UINT32) (pCtx->Status.Last.Agc & 0xFFFFFFFFULL);
                    FuncRetCode = AmbaImgSensorDrv_AgcWrite(ImageChanId, &Var_UL);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_L");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_L!");
                        RetCode = NG_UL;
                    }
                } else {
                    /* hdr */
                    FuncRetCode = AmbaImgSensorDrv_AgcWrite(ImageChanId, pHdrCtx->Status.Last.Agc);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Hdr_L");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Hdr_L!");
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

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL digital gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl digital gain control
 *  @param[in] Data digital gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    UINT32 Var_UL[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            if (pInfo != NULL) {
                /* dgc ctrl get */
                CtrlMem.Ctx.pUint64 = &Ctrl;
                /* dgc data get */
                DataMem.Ctx.pUint64 = &Data;

                if (CtrlMem.Ctx.pDgc->Sig != (UINT8) DGC_SIG_SKIP) {
                    if ((pInfo->Op.User.DgcType == (UINT8) IMAGE_DGC_SENSOR) ||
                        (pInfo->Op.User.DgcType == (UINT8) IMAGE_DGC_BOTH)) {
                        /* dgc (sensor) write */
                        if (pVin->Op.Bits.Out2Sensor > 0U) {
                            /* hdr? */
                            if (CtrlMem.Ctx.pDgc->Hdr > 0U) {
                                FuncRetCode = AmbaImgSensorDrv_DgcWrite(ImageChanId, pHdrCtx->Status.Load.Dgc);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Hdr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Hdr!");
                                    RetCode = NG_UL;
                                }
                            } else {
                                Var_UL[0] = (UINT32) (DataMem.Ctx.pDgc->Dgc & 0xFFFFFFFFULL);
                                FuncRetCode = AmbaImgSensorDrv_DgcWrite(ImageChanId, Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc!");
                                    RetCode = NG_UL;
                                }
                            }

                            if (pVin->Timing.Op.Bits.Enable > 0U) {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                var_utoa((UINT32) (DataMem.Ctx.pDgc->Dgc & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                str[1] = 'c';
                                var_utoa(pCtx->Fifo.Dgc.Count, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }

                            /* last reg update */
                            pCtx->Status.Last.Dgc = DataMem.Ctx.pDgc->Dgc;
                            /* last hdr reg update */
                            if (CtrlMem.Ctx.pDgc->Hdr > 0U) {
                                for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                    pHdrCtx->Status.Last.Dgc[i] = pHdrCtx->Status.Load.Dgc[i];
                                }
                            }
                        }

                        /* dgc ack */
                        if (CtrlMem.Ctx.pDgc->Ack > 0U) {
                            FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, (UINT64) IMAGE_EVENT_CHAN_FLAG_DGC_ACK);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_E");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_E!");
                                RetCode = NG_UL;
                            }
                        }
                    } else {
                        /* dgc (dsp) write */
                        if (CtrlMem.Ctx.pDgc->Img == (UINT8) DGC_IMG_VIDEO) {
                            if (pVin->Op.Bits.Out2Dsp > 0U) {
                                UINT32 Mode = AMBA_IK_PIPE_VIDEO;
                                UINT32 WbId = (UINT32) CtrlMem.Ctx.pDgc->WbId;
                                /* hdr? */
                                if (CtrlMem.Ctx.pDgc->Hdr > 0U) {
                                    FuncRetCode = AmbaImgSystem_HdrDgcWrite(ImageChanId, Mode | (WbId << 16U), pHdrCtx->Status.Load.Dgc);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Hdr");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Hdr!");
                                        RetCode = NG_UL;
                                    }
                                } else {
                                    Var_UL[0] = (UINT32) (DataMem.Ctx.pDgc->Dgc & 0xFFFFFFFFULL);
                                    FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId, Mode, Var_UL);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc!");
                                        RetCode = NG_UL;
                                    }
                                }

                                if (pVin->Timing.Op.Bits.Enable > 0U) {
                                    char str[11];
                                    str[0] = ' ';str[1] = ' ';
                                    var_utoa((UINT32) (DataMem.Ctx.pDgc->Dgc & 0xFFFFFFFFULL), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                }

                                /* last reg update */
                                pCtx->Status.Last.Dgc = DataMem.Ctx.pDgc->Dgc;
                                /* last hdr reg update */
                                if (CtrlMem.Ctx.pDgc->Hdr > 0U) {
                                    for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                        pHdrCtx->Status.Last.Dgc[i] = pHdrCtx->Status.Load.Dgc[i];
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  Amba image sensor HAL last digital gain write
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_LastDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 HdrId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    const AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    UINT32 Var_UL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    HdrId = ImageChanId.Ctx.Bits.HdrId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            if (pInfo != NULL) {
                /* dgc (sensor) */
                if (pInfo->Op.User.DgcType != (UINT8) IMAGE_DGC_DSP) {
                    if (pVin->Op.Bits.Out2Sensor > 0U) {
                        /* last dgc (sensor) */
                        if (HdrId == 0U) {
                            /* non-hdr */
                            Var_UL = (UINT32) (pCtx->Status.Last.Dgc & 0xFFFFFFFFULL);
                            FuncRetCode = AmbaImgSensorDrv_DgcWrite(ImageChanId, &Var_UL);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_L");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_L!");
                                RetCode = NG_UL;
                            }
                        } else {
                            /* hdr */
                            FuncRetCode = AmbaImgSensorDrv_DgcWrite(ImageChanId, pHdrCtx->Status.Last.Dgc);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Hdr_L");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Hdr_L!");
                                RetCode = NG_UL;
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL white balance gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl white balance gain control
 *  @param[in] Data white balance gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    UINT64 Var_ULL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            if (pInfo != NULL) {
                /* wgc ctrl get */
                CtrlMem.Ctx.pUint64 = &Ctrl;
                /* wgc data get */
                DataMem.Ctx.pUint64 = &Data;

                if (CtrlMem.Ctx.pWgc->Sig != (UINT8) DGC_SIG_SKIP) {
                    /* wgc (sensor) write */
                    if (pVin->Op.Bits.Out2Sensor > 0U) {
                        /* hdr? */
                        if (CtrlMem.Ctx.pWgc->Hdr > 0U) {
                            FuncRetCode = AmbaImgSensorDrv_WgcWrite(ImageChanId, pHdrCtx->Status.Load.Wgc);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_Hdr");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_Hdr!");
                                RetCode = NG_UL;
                            }
                        } else {
                            Var_ULL = (UINT64) DataMem.Ctx.pWgc->Wgc;
                            FuncRetCode = AmbaImgSensorDrv_WgcWrite(ImageChanId, &Var_ULL);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc!");
                                RetCode = NG_UL;
                            }
                        }

                        if (pVin->Timing.Op.Bits.Enable > 0U) {
                            char str[11];
                            str[0] = ' ';str[1] = ' ';
                            var_utoa((UINT32) (DataMem.Ctx.pWgc->Wgc & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[1] = 'c';
                            var_utoa(pCtx->Fifo.Wgc.Count, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        }

                        /* last reg update */
                        pCtx->Status.Last.Wgc = DataMem.Ctx.pWgc->Wgc;
                        /* last hdr reg update */
                        if (CtrlMem.Ctx.pWgc->Hdr > 0U) {
                            for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                pHdrCtx->Status.Last.Wgc[i] = pHdrCtx->Status.Load.Wgc[i];
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  Amba image sensor HAL last white balance gain write
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_LastWgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 HdrId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    const AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    UINT64 Var_ULL;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    HdrId = ImageChanId.Ctx.Bits.HdrId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            if (pInfo != NULL) {
                /* wgc (sensor) */
                if (pVin->Op.Bits.Out2Sensor > 0U) {
                    if (HdrId == 0U) {
                        /* non-hdr */
                        Var_ULL = (UINT64) pCtx->Status.Last.Wgc;
                        FuncRetCode = AmbaImgSensorDrv_WgcWrite(ImageChanId, &Var_ULL);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_L");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_L!");
                            RetCode = NG_UL;
                        }
                    } else {
                        /* hdr */
                        FuncRetCode = AmbaImgSensorDrv_WgcWrite(ImageChanId, pHdrCtx->Status.Last.Wgc);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_Hdr_L");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_Hdr_L!");
                            RetCode = NG_UL;
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL DSP digital gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl DSP digital gain control
 *  @param[in] Data DSP digital gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_DDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    UINT32 Var_UL[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;
            /* hdr ctx get */
            pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;

            if (pInfo != NULL) {
                /* dgc ctrl get */
                CtrlMem.Ctx.pUint64 = &Ctrl;
                /* dgc data get */
                DataMem.Ctx.pUint64 = &Data;

                if (CtrlMem.Ctx.pDDgc->Sig != (UINT8) DGC_SIG_SKIP) {
                    /* video? */
                    if (CtrlMem.Ctx.pDDgc->Img == (UINT8) DGC_IMG_VIDEO) {
                        /* out2dsp? */
                        if (pVin->Op.Bits.Out2Dsp > 0U) {
                            UINT32 Mode = AMBA_IK_PIPE_VIDEO;
                            UINT32 WbId = (UINT32) CtrlMem.Ctx.pDDgc->WbId;
                            /* hdr check */
                            if (CtrlMem.Ctx.pDDgc->Hdr > 0U) {
                                /* hdr */
                                FuncRetCode = AmbaImgSystem_HdrDgcWrite(ImageChanId, Mode | (WbId << 16U), pHdrCtx->Status.Load.DDgc);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc_Hdr");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc_Hdr!");
                                    RetCode = NG_UL;
                                }
                            } else {
                                /* non-hdr */
                                Var_UL[0] = (UINT32) (DataMem.Ctx.pDDgc->Dgc & 0xFFFFFFFFULL);
                                FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId, Mode | (WbId << 16U), Var_UL);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc!");
                                    RetCode = NG_UL;
                                }
                            }

                            if (pVin->Timing.Op.Bits.Enable > 0U) {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                var_utoa((UINT32) (DataMem.Ctx.pDDgc->Dgc & 0xFFFFFFFFULL), &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            }

                            /* last reg update */
                            pCtx->Status.Last.DDgc = DataMem.Ctx.pDDgc->Dgc;
                            /* last hdr reg update */
                            if (CtrlMem.Ctx.pDDgc->Hdr > 0U) {
                                for (UINT32 i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                    pHdrCtx->Status.Last.DDgc[i] = pHdrCtx->Status.Load.DDgc[i];
                                }
                            }
                        }
                    }
                }
            } else {
                /* info null */
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
 *  @private
 *  Amba image sensor HAL AIK write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg user control data
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_AikWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;

    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    VinId = ImageChanId.Ctx.Bits.VinId;

    pVin = &(AmbaImgSensorHAL_Vin[VinId]);

    if (pVin->Op.Bits.Out2Dsp > 0U) {
        /* image system aik process */
        FuncRetCode = AmbaImgSystem_AikProcess(ImageChanId, Msg, User);
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik!");
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL EFov write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg user control data
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_EFovWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;

    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    VinId = ImageChanId.Ctx.Bits.VinId;

    pVin = &(AmbaImgSensorHAL_Vin[VinId]);

    if (pVin->Op.Bits.Out2Dsp > 0U) {
        /* image system aik process */
        FuncRetCode = AmbaImgSystem_EFovProcess(ImageChanId, Msg, User);
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov!");
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor HAL user write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Ctrl user control
 *  @param[in] Data user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_UserWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    AMBA_IMG_SENSOR_HAL_CTRL_MEM_s CtrlMem;
    AMBA_IMG_SENSOR_HAL_DATA_MEM_s DataMem;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* user ctrl get */
            CtrlMem.Ctx.pUint64 = &Ctrl;
            /* user data get */
            DataMem.Ctx.pUint64 = &Data;

            if ((CtrlMem.Ctx.pUser->Sig == (UINT8) USER_SIG_USER) ||
                (CtrlMem.Ctx.pUser->Sig == (UINT8) USER_SIG_NONE)) {
                /* last reg update */
                pCtx->Status.Last.User = DataMem.Ctx.pUser->User;
                /* timing debug */
                AmbaImgSensorHAL_TimingMarkPut(VinId, "User");
                if (pVin->Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';
                    var_utoa((UINT32) (DataMem.Ctx.pUser->User & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            }
        } else {
            /* ctx null*/
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
 *  Amba image sensor HAL slow shutter to DSP
 *  @param[in] ImageChanId image channel id
 *  @param[in] SsIndex slow shutter index
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_SlowShutter(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 SsIndex)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_CONTEXT_s *pCtx;
    const AMBA_IMG_SENSOR_HAL_VIN_s *pVin;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* chan check */
    if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
        (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx != NULL) {
            /* vin get */
            pVin = &(AmbaImgSensorHAL_Vin[VinId]);
            /* ctx get */
            pCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx;
            /* info get */
            pInfo = pCtx->pInfo;

            if (pInfo != NULL) {
                if(pVin->Op.Bits.Out2Dsp > 0U) {
                    if ((pInfo->Op.User.ForceSsInVideo & (((UINT32) 1U) << 31U)) == 0U) {
                        /* slow shutter change */
                        FuncRetCode = AmbaImgSystem_SlowShutterChange(ImageChanId, SsIndex, (UINT32) SS_SYNC_BOTH);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi!");
                            RetCode = NG_UL;
                        }
                    } else {
                        /* slow motion change */
                        FuncRetCode = AmbaImgSystem_SlowShutterChange(ImageChanId, SsIndex, (UINT32) SS_SYNC_DEC_ONLY);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dec");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dec!");
                            RetCode = NG_UL;
                        }
                    }
                }
            } else {
                /* info null */
                RetCode = NG_UL;
            }
        } else {
            /* ctx null*/
            RetCode = NG_UL;
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL timing mark enable
 *  @param[in] VinId vin id
 *  @param[in] Enable enable/disable
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_TimingMarkEn(UINT32 VinId, UINT32 Enable)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AMBA_IMG_SENSOR_HAL_MARK_s *pTiming = &(AmbaImgSensorHAL_Vin[VinId].Timing);
        if (pTiming->Op.Bits.Rdy > 0U) {
            pTiming->Op.Bits.Enable = (UINT8) (Enable & 0xFFU);
        } else {
            /* not ready */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL timing mark check
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_TimingMarkChk(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        const AMBA_IMG_SENSOR_HAL_MARK_s *pTiming = &(AmbaImgSensorHAL_Vin[VinId].Timing);
        if ((pTiming->Op.Bits.Rdy == 0U) ||
            (pTiming->Op.Bits.Enable == 0U)) {
            /* not ready or enable */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL timing mark clear
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_TimingMarkClr(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IMG_SENSOR_HAL_MARK_s *pTiming;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* timing obj get */
        pTiming = &(AmbaImgSensorHAL_Vin[VinId].Timing);
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(pTiming->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* reset */
            pTiming->Index.Wr = 0U;
            pTiming->Index.Rd = 0U;
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(pTiming->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        } else {
            /* mutex fail */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL timing mark put ex
 *  @param[in] VinId vin id
 *  @param[in] pName pointer to the string
 *  @return error code
 */
UINT32 AmbaImgSensorHAL_TimingMarkPutE(UINT32 VinId, const char *pName)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;
    UINT32 FuncRetCode;

    UINT32 *pWr;
    UINT32 *pRd;

    char *pTimingName;

    AMBA_IMG_SENSOR_HAL_MARK_s *pTiming;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if (pName != NULL) {
            /* timing obj get */
            pTiming = &(AmbaImgSensorHAL_Vin[VinId].Timing);

            if ((pTiming->Op.Bits.Rdy > 0U) &&
                (pTiming->Op.Bits.Enable > 0U)) {
                /* mutex get */
                FuncRetCode = AmbaKAL_MutexTake(&(pTiming->Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* wr & rd get */
                    pWr = &(pTiming->Index.Wr);
                    pRd = &(pTiming->Index.Rd);

                    /* time tick put */
                    FuncRetCode = AmbaKAL_GetSysTickCount(&(pTiming->Msg.Data[*pWr].Timetick));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }

                    /* msg string put */
                    pTimingName = pTiming->Msg.Data[*pWr].Name;
                    for (i = 0U; i < AMBA_IMG_SENSOR_HAL_MARK_STRING_LENGTH; i++) {
                        if (pName[i] == '\0') {
                            break;
                        }
                        pTimingName[i] = pName[i];
                    }
                    pTimingName[i] = '\0';

                    /* wr udpate */
                    *pWr = (*pWr + 1U) %  pTiming->Msg.Num;
                    if (*pWr == *pRd) {
                        *pRd = (*pRd + 1U) %  pTiming->Msg.Num;
                    }

                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pTiming->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* mutex fail */
                    RetCode = NG_UL;
                }
            }
        } else {
            /* vin or name null */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL timing mark put
 *  @param[in] VinId vin id
 *  @param[in] pName pointer to the string
 */
void AmbaImgSensorHAL_TimingMarkPut(UINT32 VinId, const char *pName)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaImgSensorHAL_TimingMarkPutE(VinId, pName);
    if (FuncRetCode != OK_UL) {
        /* */
    }
}

/**
 *  Amba image sensor HAL timing mark get
 *  @param[in] VinId vin id
 *  @param[out] pName pointer to the string buffer
 *  @param[in] Size string length
 *  @param[out] pTimetick pointer to the timetick data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorHAL_TimingMarkGet(UINT32 VinId, char *pName, UINT32 Size, UINT32 *pTimetick)
{
    UINT32 RetCode = OK_UL;

    UINT32 i;
    UINT32 FuncRetCode;

    const UINT32 *pWr;
    UINT32 *pRd;

    const char *pTimingName;

    AMBA_IMG_SENSOR_HAL_MARK_s *pTiming;

    if (pName != NULL) {
        /* timing obj get */
        pTiming = &(AmbaImgSensorHAL_Vin[VinId].Timing);

        if ((pTiming->Op.Bits.Rdy > 0U) /*&&
            (pTiming->Op.Bits.Enable > 0U)*/) {
            /* mutex get */
            FuncRetCode = AmbaKAL_MutexTake(&(pTiming->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* wr & rd get */
                pWr = &(pTiming->Index.Wr);
                pRd = &(pTiming->Index.Rd);

                if (*pWr == *pRd) {
                    RetCode = NG_UL;
                } else {
                    /* name pointer */
                    pTimingName = pTiming->Msg.Data[*pRd].Name;

                    /* msg string get */
                    for (i = 0U; (i < AMBA_IMG_SENSOR_HAL_MARK_STRING_LENGTH) && (i < (Size - 1U)); i++) {
                        if (pTimingName[i] == '\0') {
                            break;
                        }
                        pName[i] = pTimingName[i];
                    }
                    pName[i] = '\0';

                    /* time tick get */
                    *pTimetick = pTiming->Msg.Data[*pRd].Timetick;

                    /* rd update */
                    *pRd = (*pRd + 1U) % pTiming->Msg.Num;
                }

                /* mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(pTiming->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            } else {
                /* mutex fail */
                RetCode = NG_UL;
            }
        } else {
            /* not rdy or diabled */
            RetCode = NG_UL;
        }
    } else {
        /* vin null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor HAL timing mark print
 *  @param[in] VinId vin id
 */
void AmbaImgSensorHAL_TimingMarkPrt(UINT32 VinId)
{
    UINT32 FuncRetCode;

    UINT32 Timetick;

    char TimetickString[11];
    char NameString[AMBA_IMG_SENSOR_HAL_MARK_STRING_LENGTH + 1U];

    UINT32 Argc;
    const char *Argv[2];

    /* time tick & msg get */
    FuncRetCode = AmbaImgSensorHAL_TimingMarkGet(VinId, NameString, AMBA_IMG_SENSOR_HAL_MARK_STRING_LENGTH + 1U, &Timetick);

    while (FuncRetCode == OK_UL) {
        /* time tick to string */
        var_utoa(Timetick, TimetickString, 10U, 9U, (UINT32) VAR_LEADING_ZERO);

        /* print */
        Argc = 2U; Argv[0] = TimetickString; Argv[1] = NameString;
        var_print("[%s] %s", &Argc, Argv);

        /* time tick & msg get */
        FuncRetCode = AmbaImgSensorHAL_TimingMarkGet(VinId, NameString, AMBA_IMG_SENSOR_HAL_MARK_STRING_LENGTH + 1U, &Timetick);
    }
}

