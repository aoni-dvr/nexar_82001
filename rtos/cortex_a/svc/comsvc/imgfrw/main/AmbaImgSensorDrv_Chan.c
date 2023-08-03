/**
 *  @file AmbaImgSensorDrv_Chan.c
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
 *  @details Amba Image Sensor Drv Channel
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"

#include "AmbaKAL.h"

#include "AmbaDSP.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaSensor.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgSensorDrv_Chan.h"

#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_SENSOR_DRV_MEM_INFO_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR    Data;
    AMBA_IMG_SENSOR_WB_s      *pWgc;
} SENSOR_DRV_MEM_INFO_u;

typedef struct /*_SENSOR_DRV_MEM_INFO_s_*/ {
    SENSOR_DRV_MEM_INFO_u    Ctx;
} SENSOR_DRV_MEM_INFO_s;

//#define AMBA_IMG_SENSOR_DRV_TIMING_DEBUG
#ifndef CONFIG_BUILD_IMGFRW_SLAVE_SS
#define AmbaSensor_SetMasterSyncCtrl AmbaSensor_MasterSyncCtrlSet
static UINT32 AmbaSensor_MasterSyncCtrlSet(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 Msc)
{
    (void) pChan;
    (void) Msc;
    return SENSOR_ERR_INVALID_API;
}
#endif
/**
 *  image sensor drv 0 register write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pBuf pointer to the buffer memory
 *  @param[in] Size buffer length
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_RegWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT8 *pBuf, UINT8 Size)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 SensorId;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
    UINT16 Addr, Data;

    VinId = ImageChanId.Ctx.Bits.VinId;

    if (Size == 4U) {
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
            }
        }

        Addr = (((UINT16) pBuf[0]) << 8U) | pBuf[1];
        Data = (((UINT16) pBuf[2]) << 8U) | pBuf[3];

        AmbaImgPrintEx2(PRINT_FLAG_DBG, "reg", Addr, 16U, Data, 16U);

        FuncRetCode = AmbaSensor_RegisterWrite(&SensorChanId, Addr, Data);
        if (FuncRetCode != SENSOR_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  image sensor drv 0 agc write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pAgc pointer to the analog gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_AgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pAgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 SensorId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
    }

    if (VrId == 0U) {
        /* debug */
#ifdef AMBA_IMG_SENSOR_DRV_TIMING_DEBUG
        {
            char str[11];
            str[0] = ' ';str[1] = ' ';
            for (i = 0U; i < 4U; i++) {
                if ((((UINT32) ImageChanId.Ctx.Bits.HdrId) & (((UINT32) 1U) << i)) > 0U) {
                    var_utoa(pAgc[i], &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            }
        }
#endif
        /* non-vr */
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
            }
        }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                /* td batch counter */
                SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
#endif
        /* sensor agc ctrl */
        FuncRetCode = AmbaSensor_SetAnalogGainCtrl(&SensorChanId, pAgc);
        if (FuncRetCode != SENSOR_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* aux info */
            if (pInfo->pAux != NULL) {
                /* op mode */
                if (pInfo->pAux->Ctx.Bits.OpMode < (UINT8) STILL_OP_CAPTURE) {
                    /* video */
                    if (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* vr master map get */
                        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
                        /* vin search */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            /* vin check */
                            if (pVrMap->Flag[i] > 0U) {
                                SensorChanId.VinID = i;
                                /* sensor search */
                                SensorId = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    /* last? */
                                    if ((pVrMap->Flag[i] >> j) == 0U) {
                                        break;
                                    }
                                    /* sensor check */
                                    if ((pVrMap->Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                                        SensorId |= (UINT32) pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Bits.SensorId;
                                    }
                                }
                                /* sensor id convert */
                                SensorChanId.SensorID = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                        SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[i][j];
                                    }
                                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                                if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
                                    (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
                                    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
                                    /* ctx get */
                                    pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
                                    /* td? */
                                    if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                                        /* td sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        /* td batch counter */
                                        SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                                        /* debug message */
                                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                                            char str[11];
                                            str[0] = 'm';
                                            var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            str[5] = ' ';
                                            var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }
                                    } else {
                                        /* sub sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        SensorChanId.Reserved[1] = 0U;
                                    }
                                }
#endif
                                /* vin agc ctrl */
                                FuncRetCode = AmbaSensor_SetAnalogGainCtrl(&SensorChanId, pAgc);
                                if (FuncRetCode != SENSOR_ERR_NONE) {
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* non-master */
                        RetCode = NG_UL;
                    }
                } else {
                    /* still */
                    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                    SensorId = ImageChanId.Ctx.Bits.SensorId;
                    /* sensor id convert */
                    SensorChanId.SensorID = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
                        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                            SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                        }
                    }
                    /* sensor agc ctrl */
                    FuncRetCode = AmbaSensor_SetAnalogGainCtrl(&SensorChanId, pAgc);
                    if (FuncRetCode != SENSOR_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            } else {
               /* aux null */
               RetCode = NG_UL;
            }
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 dgc write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pDgc pointer to the digital gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pDgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 SensorId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
    }

    if (VrId == 0U) {
        /* debug */
#ifdef AMBA_IMG_SENSOR_DRV_TIMING_DEBUG
        {
            char str[11];
            str[0] = ' ';str[1] = ' ';
            for (i = 0U; i < 4U; i++) {
                if ((((UINT32) ImageChanId.Ctx.Bits.HdrId) & (((UINT32) 1U) << i)) > 0U) {
                    var_utoa(pDgc[i], &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            }
        }
#endif
        /* non-vr */
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
            }
        }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                /* td batch counter */
                SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
#endif
        /* sensor dgc ctrl */
        FuncRetCode = AmbaSensor_SetDigitalGainCtrl(&SensorChanId, pDgc);
        if (FuncRetCode != SENSOR_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* aux info */
            if (pInfo->pAux != NULL) {
                /* op mode */
                if (pInfo->pAux->Ctx.Bits.OpMode < (UINT8) STILL_OP_CAPTURE) {
                    /* video */
                    if (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* vr master map get */
                        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
                        /* vin search */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            /* vin check */
                            if (pVrMap->Flag[i] > 0U) {
                                SensorChanId.VinID = i;
                                /* sensor search */
                                SensorId = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    /* last? */
                                    if ((pVrMap->Flag[i] >> j) == 0U) {
                                        break;
                                    }
                                    /* sensor check */
                                    if ((pVrMap->Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                                        SensorId |= (UINT32) pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Bits.SensorId;
                                    }
                                }
                                /* sensor id convert */
                                SensorChanId.SensorID = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                        SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[i][j];
                                    }
                                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                                if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
                                    (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
                                    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
                                    /* ctx get */
                                    pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
                                    /* td? */
                                    if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                                        /* td sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        /* td batch counter */
                                        SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                                        /* debug message */
                                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                                            char str[11];
                                            str[0] = 'm';
                                            var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            str[5] = ' ';
                                            var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }
                                    } else {
                                        /* sub sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        SensorChanId.Reserved[1] = 0U;
                                    }
                                }
#endif
                                /* vin dgc ctrl */
                                FuncRetCode = AmbaSensor_SetDigitalGainCtrl(&SensorChanId, pDgc);
                                if (FuncRetCode != SENSOR_ERR_NONE) {
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* non-master */
                        RetCode = NG_UL;
                    }
                } else {
                    /* still */
                    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                    SensorId = ImageChanId.Ctx.Bits.SensorId;
                    /* sensor id convert */
                    SensorChanId.SensorID = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
                        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                            SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                        }
                    }
                    /* sensor dgc ctrl */
                    FuncRetCode = AmbaSensor_SetDigitalGainCtrl(&SensorChanId, pDgc);
                    if (FuncRetCode != SENSOR_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            } else {
                /* aux null */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 shr write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pShr pointer to the shutter data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_ShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 SensorId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
    }

    if (VrId == 0U) {
        /* debug */
#ifdef AMBA_IMG_SENSOR_DRV_TIMING_DEBUG
        {
            char str[11];
            str[0] = ' ';str[1] = ' ';
            for (i = 0U; i < 4U; i++) {
                if ((((UINT32) ImageChanId.Ctx.Bits.HdrId) & (((UINT32) 1U) << i)) > 0U) {
                    var_utoa(pShr[i], &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            }
        }
#endif
        /* non-vr */
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
            }
        }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                /* td batch counter */
                SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
#endif
        /* sensor shutter ctrl */
        FuncRetCode = AmbaSensor_SetShutterCtrl(&SensorChanId, pShr);
        if (FuncRetCode != SENSOR_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* aux info */
            if (pInfo->pAux != NULL) {
                /* op mode */
                if (pInfo->pAux->Ctx.Bits.OpMode < (UINT8) STILL_OP_CAPTURE) {
                    /* video */
                    if (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* vr master map get */
                        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
                        /* vin search */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            /* vin check */
                            if (pVrMap->Flag[i] > 0U) {
                                SensorChanId.VinID = i;
                                /* sensor search */
                                SensorId = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    /* last? */
                                    if ((pVrMap->Flag[i] >> j) == 0U) {
                                        break;
                                    }
                                    /* sensor check */
                                    if ((pVrMap->Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                                        SensorId |= (UINT32) pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Bits.SensorId;
                                    }
                                }
                                /* sensor id convert */
                                SensorChanId.SensorID = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                        SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[i][j];
                                    }
                                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                                if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
                                    (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
                                    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
                                    /* ctx get */
                                    pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
                                    /* td? */
                                    if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                                        /* td sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        /* td batch counter */
                                        SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                                        /* debug message */
                                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                                            char str[11];
                                            str[0] = 'm';
                                            var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            str[5] = ' ';
                                            var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }
                                    } else {
                                        /* sub sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        SensorChanId.Reserved[1] = 0U;
                                    }
                                }
#endif
                                /* vin shutter ctrl */
                                FuncRetCode = AmbaSensor_SetShutterCtrl(&SensorChanId, pShr);
                                if (FuncRetCode != SENSOR_ERR_NONE) {
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* non-master */
                        RetCode = NG_UL;
                    }
                } else {
                    /* still */
                    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                    SensorId = ImageChanId.Ctx.Bits.SensorId;
                    /* sensor id convert */
                    SensorChanId.SensorID = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
                        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                            SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                        }
                    }
                    /* sensor shutter ctrl */
                    FuncRetCode = AmbaSensor_SetShutterCtrl(&SensorChanId, pShr);
                    if (FuncRetCode != SENSOR_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            } else {
                /* aux null */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 svr write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSvr pointer to the slow shutter data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_SvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSvr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 SensorId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
    }

    if (VrId == 0U) {
        /* non-vr */
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
            }
        }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                /* td batch counter */
                SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
#endif
        /* sensor slow shutter ctrl */
        FuncRetCode = AmbaSensor_SetSlowShutterCtrl(&SensorChanId, *pSvr);
        if (FuncRetCode != SENSOR_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* aux info */
            if (pInfo->pAux != NULL) {
                /* op mode */
                if (pInfo->pAux->Ctx.Bits.OpMode < (UINT8) STILL_OP_CAPTURE) {
                    /* video */
                    if (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* vr master map get */
                        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
                        /* vin search */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            /* vin check */
                            if (pVrMap->Flag[i] > 0U) {
                                SensorChanId.VinID = i;
                                /* sensor search */
                                SensorId = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    /* last? */
                                    if ((pVrMap->Flag[i] >> j) == 0U) {
                                        break;
                                    }
                                    /* sensor check */
                                    if ((pVrMap->Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                                        SensorId |= (UINT32) pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Bits.SensorId;
                                    }
                                }
                                /* sensor id convert */
                                SensorChanId.SensorID = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                        SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[i][j];
                                    }
                                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                                if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
                                    (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
                                    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
                                    /* ctx get */
                                    pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
                                    /* td? */
                                    if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                                        /* td sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        /* td batch counter */
                                        SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                                        /* debug message */
                                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                                            char str[11];
                                            str[0] = 'm';
                                            var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            str[5] = ' ';
                                            var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }
                                    } else {
                                        /* sub sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        SensorChanId.Reserved[1] = 0U;
                                    }
                                }
#endif
                                /* vin slow shutter ctrl */
                                FuncRetCode = AmbaSensor_SetSlowShutterCtrl(&SensorChanId, *pSvr);
                                if (FuncRetCode != SENSOR_ERR_NONE) {
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* non-master */
                        RetCode = NG_UL;
                    }
                } else {
                    /* still */
                    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                    SensorId = ImageChanId.Ctx.Bits.SensorId;
                    /* sensor id convert */
                    SensorChanId.SensorID = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
                        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                            SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                        }
                    }
                    /* sensor slow shutter ctrl */
                    FuncRetCode = AmbaSensor_SetSlowShutterCtrl(&SensorChanId, *pSvr);
                    if (FuncRetCode != SENSOR_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            } else {
                /* aux null */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 master sync write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pMsc pointer to the master sync data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_MscWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pMsc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 SensorId;
    UINT32 SensorUpdateFlag;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
    }

    if (VrId == 0U) {
        /* non-vr */
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* update flag reset */
        SensorUpdateFlag = 0U;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                /* sensor id accumulated */
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                /* update? */
                if (AmbaImgMain_LastMasterSyncTable[VinId][i] != *pMsc) {
                    AmbaImgMain_LastMasterSyncTable[VinId][i] = *pMsc;
                    SensorUpdateFlag = 1U;
                }
            }
        }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                /* td batch counter */
                SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
#endif
        /* sensor master sync ctrl (TBD) */
        if (SensorUpdateFlag > 0U) {
            FuncRetCode = AmbaSensor_SetMasterSyncCtrl(&SensorChanId, *pMsc);
            if (FuncRetCode != SENSOR_ERR_NONE) {
                RetCode = NG_UL;
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_S!");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_S");
            }
        } else {
            /* */
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_L");
        }
    } else {
        AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* aux info */
            if (pInfo->pAux != NULL) {
                /* op mode */
                if (pInfo->pAux->Ctx.Bits.OpMode < (UINT8) STILL_OP_CAPTURE) {
                    /* video */
                    if (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* vr master map get */
                        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
                        /* vin search */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            /* vin check */
                            if (pVrMap->Flag[i] > 0U) {
                                SensorChanId.VinID = i;
                                /* sensor search */
                                SensorId = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    /* last? */
                                    if ((pVrMap->Flag[i] >> j) == 0U) {
                                        break;
                                    }
                                    /* sensor check */
                                    if ((pVrMap->Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                                        SensorId |= (UINT32) pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Bits.SensorId;
                                    }
                                }
                                /* update flag reset */
                                SensorUpdateFlag = 0U;
                                /* sensor id convert */
                                SensorChanId.SensorID = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                        /* sensor id accumulated */
                                        SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[i][j];
                                        /* update? */
                                        if (AmbaImgMain_LastMasterSyncTable[i][j] != *pMsc) {
                                            AmbaImgMain_LastMasterSyncTable[i][j] = *pMsc;
                                            SensorUpdateFlag = 1U;
                                        }
                                    }
                                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                                if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
                                    (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
                                    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
                                    /* ctx get */
                                    pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
                                    /* td? */
                                    if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                                        /* td sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        /* td batch counter */
                                        SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                                        /* debug message */
                                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                                            char str[11];
                                            str[0] = 'm';
                                            var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            str[5] = ' ';
                                            var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }
                                    } else {
                                        /* sub sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        SensorChanId.Reserved[1] = 0U;
                                    }
                                }
#endif
                                /* vin master sync ctrl (TBD) */
                                if (SensorUpdateFlag > 0U) {
                                    FuncRetCode = AmbaSensor_SetMasterSyncCtrl(&SensorChanId, *pMsc);
                                    if (FuncRetCode != SENSOR_ERR_NONE) {
                                        RetCode = NG_UL;
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_S!");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_S");
                                    }
                                } else {
                                    /* */
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_L");
                                }
                            }
                        }
                    } else {
                        /* non-master */
                        RetCode = NG_UL;
                    }
                } else {
                    /* still */
                    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                    SensorId = ImageChanId.Ctx.Bits.SensorId;
                    /* update flag reset */
                    SensorUpdateFlag = 0U;
                    /* sensor id convert */
                    SensorChanId.SensorID = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
                        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                            SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                            /* update? */
                            if (AmbaImgMain_LastMasterSyncTable[VinId][i] != *pMsc) {
                                AmbaImgMain_LastMasterSyncTable[VinId][i] = *pMsc;
                                SensorUpdateFlag = 1U;
                            }
                        }
                    }
                    /* sensor master sync ctrl (TBD) */
                    if (SensorUpdateFlag > 0U) {
                        FuncRetCode = AmbaSensor_SetMasterSyncCtrl(&SensorChanId, *pMsc);
                        if (FuncRetCode != SENSOR_ERR_NONE) {
                            RetCode = NG_UL;
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_S!");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_S");
                        }
                    } else {
                        /* */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_L");
                    }
                }
            } else {
                /* aux null */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 sls write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSls pointer to the sync light source data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_SlsWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSls)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 SensorId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
    }

    if (VrId == 0U) {
        /* non-vr */
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
            }
        }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                /* td batch counter */
                SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
#endif
        /* sensor slow shutter ctrl */
        FuncRetCode = AmbaSensor_SetStrobeCtrl(&SensorChanId, pSls);
        if (FuncRetCode != SENSOR_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* aux info */
            if (pInfo->pAux != NULL) {
                /* op mode */
                if (pInfo->pAux->Ctx.Bits.OpMode < (UINT8) STILL_OP_CAPTURE) {
                    /* video */
                    if (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* vr master map get */
                        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
                        /* vin search */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            /* vin check */
                            if (pVrMap->Flag[i] > 0U) {
                                SensorChanId.VinID = i;
                                /* sensor search */
                                SensorId = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    /* last? */
                                    if ((pVrMap->Flag[i] >> j) == 0U) {
                                        break;
                                    }
                                    /* sensor check */
                                    if ((pVrMap->Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                                        SensorId |= (UINT32) pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Bits.SensorId;
                                    }
                                }
                                /* sensor id convert */
                                SensorChanId.SensorID = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                        SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[i][j];
                                    }
                                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                                if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
                                    (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
                                    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
                                    /* ctx get */
                                    pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
                                    /* td? */
                                    if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                                        /* td sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        /* td batch counter */
                                        SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                                        /* debug message */
                                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                                            char str[11];
                                            str[0] = 'm';
                                            var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            str[5] = ' ';
                                            var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }
                                    } else {
                                        /* sub sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        SensorChanId.Reserved[1] = 0U;
                                    }
                                }
#endif
                                /* vin slow shutter ctrl */
                                FuncRetCode = AmbaSensor_SetStrobeCtrl(&SensorChanId, pSls);
                                if (FuncRetCode != SENSOR_ERR_NONE) {
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* non-master */
                        RetCode = NG_UL;
                    }
                } else {
                    /* still */
                    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                    SensorId = ImageChanId.Ctx.Bits.SensorId;
                    /* sensor id convert */
                    SensorChanId.SensorID = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
                        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                            SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                        }
                    }
                    /* sensor slow shutter ctrl */
                    FuncRetCode = AmbaSensor_SetStrobeCtrl(&SensorChanId, pSls);
                    if (FuncRetCode != SENSOR_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            } else {
                /* aux null */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 wgc write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pWgc pointer to the white balance gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pWgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    UINT32 SensorId;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;

    AMBA_SENSOR_WB_CTRL_s WbCtrl[AMBA_SENSOR_NUM_MAX_HDR_CHAN] = {0};
    AMBA_SENSOR_CHANNEL_s SensorChanId;

    SENSOR_DRV_MEM_INFO_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
    }

    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) *pWgc;

    if (ImageChanId.Ctx.Bits.HdrId == 0U) {
        /* non-hdr */
        WbCtrl[0].R  = MemInfo.Ctx.pWgc[0].Gain[0];
        WbCtrl[0].Gr = MemInfo.Ctx.pWgc[0].Gain[1];
        WbCtrl[0].Gb = MemInfo.Ctx.pWgc[0].Gain[2];
        WbCtrl[0].B  = MemInfo.Ctx.pWgc[0].Gain[3];
        /* debug */
#ifdef AMBA_IMG_SENSOR_DRV_TIMING_DEBUG
        {
            char str[11];
            str[0] = 'r';str[1] = ' ';
            var_utoa(WbCtrl[0].R, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            str[0] = 'g';
            var_utoa(WbCtrl[0].Gr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            str[0] = 'g';
            var_utoa(WbCtrl[0].Gb, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            str[0] = 'b';
            var_utoa(WbCtrl[0].B, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        }
#endif
    } else {
        /* hdr */
        for (i = 0U; i < AMBA_SENSOR_NUM_MAX_HDR_CHAN; i++) {
            if (((UINT32) ImageChanId.Ctx.Bits.HdrId & (((UINT32) 1U) << i)) > 0U) {
                WbCtrl[i].R  = MemInfo.Ctx.pWgc[i].Gain[0];
                WbCtrl[i].Gr = MemInfo.Ctx.pWgc[i].Gain[1];
                WbCtrl[i].Gb = MemInfo.Ctx.pWgc[i].Gain[2];
                WbCtrl[i].B  = MemInfo.Ctx.pWgc[i].Gain[3];
                /* debug */
#ifdef AMBA_IMG_SENSOR_DRV_TIMING_DEBUG
                {
                    char str[11];
                    str[0] = 'r';str[1] = ' ';
                    var_utoa(WbCtrl[i].R, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    str[0] = 'g';
                    var_utoa(WbCtrl[i].Gr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    str[0] = 'g';
                    var_utoa(WbCtrl[i].Gb, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    str[0] = 'b';
                    var_utoa(WbCtrl[i].B, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
#endif
            }
        }
    }

    if (VrId == 0U) {
        /* non-vr */
        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
        SensorId = ImageChanId.Ctx.Bits.SensorId;
        /* sensor id convert */
        SensorChanId.SensorID = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
            if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
            }
        }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                /* td batch counter */
                SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
#endif
        /* sensor wgc ctrl */
        FuncRetCode = AmbaSensor_SetWbGainCtrl(&SensorChanId, WbCtrl);
        if (FuncRetCode != SENSOR_ERR_NONE) {
            RetCode = NG_UL;
        }
    } else {
        AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* aux info */
            if (pInfo->pAux != NULL) {
                /* op mode */
                if (pInfo->pAux->Ctx.Bits.OpMode < (UINT8) STILL_OP_CAPTURE) {
                    /* video */
                    if (((VrId & (((UINT32) 1U) << VinId)) > 0U) && ((VrAltId & (((UINT32) 1U) << ChainId)) > 0U)) {
                        /* vr master map get */
                        pVrMap = &(pAmbaImgMainChannel[VinId][ChainId].pCtx->VrMap);
                        /* vin search */
                        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
                            /* vin check */
                            if (pVrMap->Flag[i] > 0U) {
                                SensorChanId.VinID = i;
                                /* sensor search */
                                SensorId = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    /* last? */
                                    if ((pVrMap->Flag[i] >> j) == 0U) {
                                        break;
                                    }
                                    /* sensor check */
                                    if ((pVrMap->Flag[i] & (((UINT32) 1U) << j)) > 0U) {
                                        SensorId |= (UINT32) pAmbaImgMainChannel[i][j].pCtx->Id.Ctx.Bits.SensorId;
                                    }
                                }
                                /* sensor id convert */
                                SensorChanId.SensorID = 0U;
                                for (j = 0U; j < AMBA_IMG_NUM_VIN_SENSOR; j++) {
                                    if ((SensorId & (((UINT32) 1U) << j)) > 0U) {
                                        SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[i][j];
                                    }
                                }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
                                if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
                                    (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
                                    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
                                    /* ctx get */
                                    pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
                                    /* td? */
                                    if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                                        /* td sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        /* td batch counter */
                                        SensorChanId.Reserved[1] = ((UINT32) pCtx->Inter.Ctx.Bits.Batch) | 0x80000000U;
                                        /* debug message */
                                        if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                                            char str[11];
                                            str[0] = 'm';
                                            var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            str[5] = ' ';
                                            var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }
                                    } else {
                                        /* sub sensor mode */
                                        SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                                        SensorChanId.Reserved[1] = 0U;
                                    }
                                }
#endif
                                /* vin wgc ctrl */
                                FuncRetCode = AmbaSensor_SetWbGainCtrl(&SensorChanId, WbCtrl);
                                if (FuncRetCode != SENSOR_ERR_NONE) {
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* non-master */
                        RetCode = NG_UL;
                    }
                } else {
                    /* still */
                    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                    SensorId = ImageChanId.Ctx.Bits.SensorId;
                    /* sensor id convert */
                    SensorChanId.SensorID = 0U;
                    for (i = 0U; i < AMBA_IMG_NUM_VIN_SENSOR; i++) {
                        if ((SensorId & (((UINT32) 1U) << i)) > 0U) {
                            SensorChanId.SensorID |=  AmbaImgMain_SensorIdTable[VinId][i];
                        }
                    }
                    /* sensor wgc ctrl */
                    FuncRetCode = AmbaSensor_SetWbGainCtrl(&SensorChanId, WbCtrl);
                    if (FuncRetCode != SENSOR_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
            } else {
                /* aux null */
                RetCode = NG_UL;
            }
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 shutter convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pFactor pointer to the shutter factor
 *  @param[out] pShr pointer to the shutter data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_ShutterConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pShr)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    FLOAT TimeSec[4] = {0.f};
    FLOAT ActualTimeSec[4];

    FLOAT RecursiveCompen = 0.f; /* no recursive -> 0.f, recursive -> 1.0f */

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
    SensorChanId.SensorID = 0U;
    for (i = 0U; (i < AMBA_IMG_NUM_VIN_SENSOR) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> i) > 0U); i++) {
        if ((((UINT32)ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << i)) > 0U) {
            SensorChanId.SensorID = AmbaImgMain_SensorIdTable[SensorChanId.VinID][i];
            break;
        }
    }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
    {
        UINT32 VinId;
        UINT32 ChainId;

        VinId = ImageChanId.Ctx.Bits.VinId;
        ChainId = ImageChanId.Ctx.Bits.ChainId;

        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
    }
#endif
    if (ImageChanId.Ctx.Bits.HdrId == 0U) {
        TimeSec[0] = (((FLOAT) pFactor[0]) + RecursiveCompen) / (1024.0f * 1024.0f);
    } else {
        for (i = 0U; i < AMBA_SENSOR_NUM_MAX_HDR_CHAN; i++) {
            if (((UINT32) ImageChanId.Ctx.Bits.HdrId & (((UINT32) 1U) << i)) > 0U) {
                TimeSec[i] = (((FLOAT) pFactor[i]) + RecursiveCompen) / (1024.0f * 1024.0f);
            }
        }
    }

    FuncRetCode = AmbaSensor_ConvertShutterSpeed(&SensorChanId, TimeSec, ActualTimeSec, pShr);
    if (FuncRetCode != SENSOR_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  image sensor drv 0 slow shutter convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pShr pointer to the shutter
 *  @param[out] pSvr pointer to the slow shutter
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_SvrConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr, UINT32 *pSvr)
{
    UINT32 RetCode = OK_UL;
#ifdef CONFIG_BUILD_IMGFRW_HDR_SS
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
#endif
    UINT32 VinId;
    UINT32 ChainId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
#ifdef CONFIG_BUILD_IMGFRW_HDR_SS
    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
    SensorChanId.SensorID = 0U;
    for (i = 0U; (i < AMBA_IMG_NUM_VIN_SENSOR) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> i) > 0U); i++) {
        if ((((UINT32)ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << i)) > 0U) {
            SensorChanId.SensorID = AmbaImgMain_SensorIdTable[SensorChanId.VinID][i];
            break;
        }
    }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
    {
        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
    }
#endif
    if (ImageChanId.Ctx.Bits.HdrId == 0U) {
        /* non-hdr (internal supported)  */
    } else {
        /* hdr */
        UINT32 TotalShutterLine = 0U;
        UINT32 TotalShutterLineSs = 0U;
        UINT32 ChannelSsType = AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.ChannelSsType;

        AMBA_SENSOR_HDR_INFO_s SensorHdrInfo;

        *pSvr = 1U;

        switch (ChannelSsType) {
            case AMBA_IMG_SENSOR_HAL_HDR_SS_ALL:
                /* TBD */
                *pSvr = ((pShr[0] - 1U) / AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.TotalShutterLine[0]) + 1U;
                break;
            case AMBA_IMG_SENSOR_HAL_HDR_SS_LONG:
                /* TBD */
                if (pShr[0] < AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.TotalShutterLine[0]) {
                    *pSvr = 1U;
                } else {
                    *pSvr = ((pShr[0] - AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.TotalShutterLine[0]) / AmbaImgSensorHAL_Info[VinId][ChainId].Frame.TotalShutterLine) + 2U;
                }
                break;
            default:
                /* TBD */
                FuncRetCode = AmbaSensor_GetHdrInfo(&SensorChanId, pShr, &SensorHdrInfo);
                if (FuncRetCode == SENSOR_ERR_NONE) {
                    if (SensorHdrInfo.ActiveChannels > 0U) {
                        /* accumulated hdr shutter line */
                        for (i = 0U; i < SensorHdrInfo.ActiveChannels; i++) {
                            TotalShutterLine += AmbaImgSensorHAL_HdrInfo[VinId][ChainId].Frame.TotalShutterLine[i];
                            TotalShutterLineSs += SensorHdrInfo.ChannelInfo[i].NumExposureStepPerFrame;
                        }
                        /* svr get */
                        if ((TotalShutterLine > 0U) &&
                            (TotalShutterLineSs > 0U)) {
                            *pSvr = ((TotalShutterLineSs - 1U) / TotalShutterLine) + 1U;
                        }
                    } else {
                        /* channel null */
                        RetCode = NG_UL;
                    }
                } else {
                    /* hdr info null */
                    RetCode = NG_UL;
                }
                break;
        }
    }
#else
    /* compatible */
    *pSvr = ((pShr[0] - 1U) / AmbaImgSensorHAL_Info[VinId][ChainId].Frame.TotalShutterLine) + 1U;
#endif

    return RetCode;
}

/**
 *  image sensor drv 0 gain convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pFactor pointer to the gain factor
 *  @param[out] pAgc pointer to the analog gain data
 *  @param[out] pDgc pointer to the digital gain data
 *  @param[in,out] pWgc pointer to the white balance gain data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_GainConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pAgc, UINT32 *pDgc, AMBA_IMG_SENSOR_WB_s *pWgc)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;
    UINT32 Count = 0U;

    AMBA_SENSOR_GAIN_FACTOR_s Factor;
    AMBA_SENSOR_GAIN_FACTOR_s ActualFactor;

    AMBA_SENSOR_GAIN_CTRL_s GainCtrl = {0};
    AMBA_SENSOR_CHANNEL_s SensorChanId;

    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
    SensorChanId.SensorID = 0U;
    for (i = 0U; (i < AMBA_IMG_NUM_VIN_SENSOR) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> i) > 0U); i++) {
        if ((((UINT32)ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << i)) > 0U) {
            SensorChanId.SensorID = AmbaImgMain_SensorIdTable[SensorChanId.VinID][i];
            break;
        }
    }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
    {
        UINT32 VinId;
        UINT32 ChainId;

        VinId = ImageChanId.Ctx.Bits.VinId;
        ChainId = ImageChanId.Ctx.Bits.ChainId;

        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
    }
#endif
    if (ImageChanId.Ctx.Bits.HdrId == 0U) {
        /* ae gain */
        Factor.Gain[0] = ((FLOAT) pFactor[0])/4096.0f;
        /* wb gain */
        Factor.WbGain[0].R  = ((FLOAT) pWgc[0].Gain[0])/4096.0f;
        Factor.WbGain[0].Gr = ((FLOAT) pWgc[0].Gain[1])/4096.0f;
        Factor.WbGain[0].Gb = ((FLOAT) pWgc[0].Gain[2])/4096.0f;
        Factor.WbGain[0].B  = ((FLOAT) pWgc[0].Gain[3])/4096.0f;
    } else {
        for (i = 0U; i < AMBA_SENSOR_NUM_MAX_HDR_CHAN; i++) {
            if ((((UINT32) ImageChanId.Ctx.Bits.HdrId) & (((UINT32) 1U) << i)) > 0U) {
                /* ae gain */
                Factor.Gain[i] = ((FLOAT) pFactor[i])/4096.0f;
                /* wb gain */
                Factor.WbGain[i].R  = ((FLOAT) pWgc[i].Gain[0])/4096.0f;
                Factor.WbGain[i].Gr = ((FLOAT) pWgc[i].Gain[1])/4096.0f;
                Factor.WbGain[i].Gb = ((FLOAT) pWgc[i].Gain[2])/4096.0f;
                Factor.WbGain[i].B  = ((FLOAT) pWgc[i].Gain[3])/4096.0f;
                Count++;
            } else {
                /* ae gain */
                Factor.Gain[i] = 1.0f;
                /* wb gain */
                Factor.WbGain[i].R  = 1.0f;
                Factor.WbGain[i].Gr = 1.0f;
                Factor.WbGain[i].Gb = 1.0f;
                Factor.WbGain[i].B  = 1.0f;
            }
        }
        if (Count == 0U) {
            RetCode = NG_UL;
        }
    }

    if (RetCode == OK_UL) {
        FuncRetCode = AmbaSensor_ConvertGainFactor(&SensorChanId, &Factor, &ActualFactor, &GainCtrl);
        if (FuncRetCode == SENSOR_ERR_NONE) {
            if (ImageChanId.Ctx.Bits.HdrId == 0U) {
                /* agc & dgc */
                pAgc[0] = GainCtrl.AnalogGain[0];
                pDgc[0] = GainCtrl.DigitalGain[0];
                pWgc[0].Gain[0] = GainCtrl.WbGain[0].R;
                pWgc[0].Gain[1] = GainCtrl.WbGain[0].Gr;
                pWgc[0].Gain[2] = GainCtrl.WbGain[0].Gb;
                pWgc[0].Gain[3] = GainCtrl.WbGain[0].B;
            } else {
                /* hdr agc & hdr dgc */
                for (i = 0U; i < AMBA_SENSOR_NUM_MAX_HDR_CHAN; i++) {
                    if (((UINT32) ImageChanId.Ctx.Bits.HdrId & (((UINT32) 1U) << i)) > 0U) {
                        pAgc[i] = GainCtrl.AnalogGain[i];
                        pDgc[i] = GainCtrl.DigitalGain[i];
                        pWgc[i].Gain[0] = GainCtrl.WbGain[i].R;
                        pWgc[i].Gain[1] = GainCtrl.WbGain[i].Gr;
                        pWgc[i].Gain[2] = GainCtrl.WbGain[i].Gb;
                        pWgc[i].Gain[3] = GainCtrl.WbGain[i].B;
                    }
                }
            }
        } else {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  image sensor drv 0 sls convert
 *  @param[in] ImageChanId image channel id
 *  @param[in] pFactor pointer to the sync light source factor
 *  @param[out] pSls pointer to the sync light source data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 ImageSensorDrv0_SlsConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pSls)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    FLOAT DesiredFactor;
    FLOAT ActualFactor;

    AMBA_SENSOR_CHANNEL_s SensorChanId;

    SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
    SensorChanId.SensorID = 0U;
    for (i = 0U; (i < AMBA_IMG_NUM_VIN_SENSOR) && ((((UINT32) ImageChanId.Ctx.Bits.SensorId) >> i) > 0U); i++) {
        if ((((UINT32)ImageChanId.Ctx.Bits.SensorId) & (((UINT32) 1U) << i)) > 0U) {
            SensorChanId.SensorID = AmbaImgMain_SensorIdTable[SensorChanId.VinID][i];
            break;
        }
    }
#ifdef CONFIG_BUILD_IMGFRW_TD_SM_BCNT
    {
        UINT32 VinId;
        UINT32 ChainId;

        VinId = ImageChanId.Ctx.Bits.VinId;
        ChainId = ImageChanId.Ctx.Bits.ChainId;

        if ((pAmbaImgMainChannel[VinId][ChainId].Magic == 0xCafeU) &&
            (pAmbaImgMainChannel[VinId][ChainId].pCtx != NULL)) {
            const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;
            /* ctx get */
            pCtx = pAmbaImgMainChannel[VinId][ChainId].pCtx;
            /* td? */
            if (pCtx->Inter.Ctx.Bits.Num > 1U) {
                /* td sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
                /* debug message */
                if (AmbaImgSensorHAL_Vin[VinId].Timing.Op.Bits.Enable > 0U) {
                    char str[11];
                    str[0] = 'm';
                    var_utoa(SensorChanId.Reserved[0], &(str[1]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    str[5] = ' ';
                    var_utoa(SensorChanId.Reserved[1] & 0x7FFFFFFFU, &(str[6]), 16U, 4U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            } else {
                /* sub sensor mode */
                SensorChanId.Reserved[0] = pCtx->Inter.Ctx.Bits.Mode;
                SensorChanId.Reserved[1] = 0U;
            }
        }
    }
#endif
    /* sls gain */
    DesiredFactor = ((FLOAT) *pFactor) / 1000000.0f;

    FuncRetCode = AmbaSensor_ConvertStrobeWidth(&SensorChanId, &DesiredFactor, &ActualFactor, pSls);
    if (FuncRetCode != SENSOR_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

AMBA_IMG_SENSOR_DRV_FUNC_s AmbaImgSensorDrv_ChanFunc = {
    .RegWrite       = ImageSensorDrv0_RegWrite,
    .AgcWrite       = ImageSensorDrv0_AgcWrite,
    .DgcWrite       = ImageSensorDrv0_DgcWrite,
    .ShrWrite       = ImageSensorDrv0_ShrWrite,
    .SvrWrite       = ImageSensorDrv0_SvrWrite,
    .MscWrite       = ImageSensorDrv0_MscWrite,
    .SlsWrite       = ImageSensorDrv0_SlsWrite,
    .WgcWrite       = ImageSensorDrv0_WgcWrite,
    .GainConvert    = ImageSensorDrv0_GainConvert,
    .ShutterConvert = ImageSensorDrv0_ShutterConvert,
    .SvrConvert     = ImageSensorDrv0_SvrConvert,
    .SlsConvert     = ImageSensorDrv0_SlsConvert
};
