/**
 *  @file AmbaImgMainAe_Platform.c
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
 *  @details Amba Image Main Ae for Common SOC
 *
 */

#include "AmbaTypes.h"
#include "AmbaVIN_Def.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"

#include "AmbaKAL.h"
#include "AmbaSensor.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"

#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImg_External_CtrlFunc.h"
#ifdef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
#include "Amba_AeAwbAdj_Control.h"
#endif
#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"
#include "AmbaImgMainAe_Platform.h"
#ifdef CONFIG_BUILD_IMGFRW_EFOV
#include "AmbaImgMainEFov.h"
#endif
#define OK_UL    ((UINT32) 0U)
#define NG_UL    ((UINT32) 1U)

typedef union /*_AE_MEM_INFO_PF_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR          Data;
    void                            *pVoid;
    UINT16                          *pUint16;
    UINT32                          *pUint32;
    AMBA_IMG_MAIN_AE_USER_DATA_s    *pUser;
    PIPELINE_CONTROL_s              *pVideoCtrl;
    AMBA_IP_VIN_TONE_CURVE_s        *pVinToneCurve;
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    UINT64                          *pUint64;
    AMBA_IMG_MAIN_AE_EFOV_USER_s    *pEFovUser;
    AMBA_IMG_MAIN_AE_EFOV_TXRX_s    *pTxRx;
#endif
} AE_MEM_INFO_PF_u;

typedef struct /*_AE_MEM_INFO_PF_s_*/ {
    AE_MEM_INFO_PF_u    Ctx;
} AE_MEM_INFO_PF_s;

typedef union /*_AE_SNAP_MEM_INFO_PF_u_*/ {
    AMBA_IMG_MAIN_MEM_ADDR               Data;
    void                                 *pVoid;
    UINT16                               *pUint16;
    UINT32                               *pUint32;
    AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s    *pUser;
    PIPELINE_STILL_CONTROL_s             *pStillCtrl;
    AMBA_IP_VIN_TONE_CURVE_s             *pVinToneCurve;
} AE_SNAP_MEM_INFO_PF_u;

typedef struct /*_AE_SNAP_MEM_INFO_PF_s_*/ {
    AE_SNAP_MEM_INFO_PF_u    Ctx;
} AE_SNAP_MEM_INFO_PF_s;

/* vin tone curve flag */
UINT32 AmbaImgMainAe_VinToneCurveFlag[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

/**
 *  @private
 *  Amba image main ae user gain put
 *  @param[in] ImageChanId image channel id
 *  @param[in,out] pUserData pointer to the user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_UserGainPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, void *pUserData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 AlgoId;

    const AMBA_AE_INFO_s    *pAeInfo;
    AMBA_DGAIN_INFO_s       *pAuxAeInfo;
    const AMBA_IK_WB_GAIN_s *pWbGain;
    AMBA_IMG_SENSOR_WB_s    *pSensorWb;

    UINT32 *pDspWgc;

    UINT32 *pFeDgc;
    UINT32 *pBeDgc;

    AE_MEM_INFO_PF_s MemInfo;

    UINT32 ExposureNum;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    AlgoId = ImageChanId.Ctx.Bits.AlgoId;

    ExposureNum = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

    if (pUserData != NULL) {
        /* user data get */
        MemInfo.Ctx.pVoid = pUserData;
        /* user ae info */
        pAeInfo = MemInfo.Ctx.pUser->AeInfo.Ctx;
        /* user aux ae info */
        pAuxAeInfo = MemInfo.Ctx.pUser->AeInfo.AuxCtx;
        /* user dgain get */
        FuncRetCode = AmbaImgProc_GetIdspDgInfo(AlgoId, pAeInfo, pAuxAeInfo);
        if (FuncRetCode != OK_UL) {
            /* */
        }
#ifdef IMG_MAIN_FE_WB_IR_GAIN
#if 1
        /* user ir gain reset, will be removed in the future */
        for (i = 0U; i < ExposureNum; i++) {
            MemInfo.Ctx.pUser->AeInfo.Dsp.FeIgc[i] = 4096U;
        }
#endif
        /* user ir gain get */
        FuncRetCode = AmbaImgProc_AWBGetIrGain(AlgoId, MemInfo.Ctx.pUser->AeInfo.Dsp.FeIgc);
        if (FuncRetCode != OK_UL) {
            /* */
        }
#endif
        /* user flash gain (TBD) */
        {
            UINT32 SlsFactor;
            /* sls factor get */
            SlsFactor = pAeInfo[0].Flash;
            /* sls reset */
            MemInfo.Ctx.pUser->AeInfo.Sensor.Sls = 0U;
            /* sls get */
            FuncRetCode = AmbaImgSensorDrv_SlsConvert(ImageChanId, &SlsFactor, &(MemInfo.Ctx.pUser->AeInfo.Sensor.Sls));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }

        /* user fe gain */
        pFeDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.FeDgc;
        for (i = 0U; i < ExposureNum; i++) {
            pFeDgc[i] = pAuxAeInfo[i].DgainInfo[1];
        }

        /* user be gain */
        for (i = 0U; i < 2U; i++) {
            pBeDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[i];
            for (j = 0U; j < ExposureNum; j++) {
                pBeDgc[j] = pAuxAeInfo[j].DgainInfo[2U+i];
            }
        }

        /* user algo wb info */
        pWbGain = MemInfo.Ctx.pUser->WbInfo.Ctx;
        /* user sensor wb gain */
        pSensorWb = MemInfo.Ctx.pUser->WbInfo.Sensor.Wgc;
        /* sensor wgc? */
        if (MemInfo.Ctx.pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_SENSOR) {
            /* sensor wgc */
            for (i = 0U; i < ExposureNum; i++) {
                pSensorWb[i].Gain[0] = (pWbGain[i].GainR*pAuxAeInfo[i].DgainInfo[0])/4096U;
                pSensorWb[i].Gain[1] = (pWbGain[i].GainG*pAuxAeInfo[i].DgainInfo[0])/4096U;
                pSensorWb[i].Gain[2] = (pWbGain[i].GainG*pAuxAeInfo[i].DgainInfo[0])/4096U;
                pSensorWb[i].Gain[3] = (pWbGain[i].GainB*pAuxAeInfo[i].DgainInfo[0])/4096U;
            }
        } else {
            /* sensor wb reset */
            for (i = 0U; i < ExposureNum; i++) {
                for (j = 0U; j < AMBA_NUM_SWB_CHANNEL; j++) {
                    pSensorWb[i].Gain[j] = 4096U;
                }
            }
        }
        /* dsp wgc? */
        if (MemInfo.Ctx.pUser->WbInfo.Dsp.WbId != (UINT32) WB_ID_SENSOR) {
            /* wgc info */
            for (i = 0U; i < ExposureNum; i++) {
                /* user dsp wb gain */
                pDspWgc = MemInfo.Ctx.pUser->WbInfo.Dsp.Wgc[i];
                /* wb gain */
                pDspWgc[0] = pWbGain[i].GainR;
                pDspWgc[1] = pWbGain[i].GainG;
                pDspWgc[2] = pWbGain[i].GainB;
            }
        } else {
            for (i = 0U; i < ExposureNum; i++) {
                /* user dsp wb gain */
                pDspWgc = MemInfo.Ctx.pUser->WbInfo.Dsp.Wgc[i];
                /* wgc info for carry */
                pDspWgc[0] = pWbGain[i].GainR;
                pDspWgc[1] = pWbGain[i].GainG;
                pDspWgc[2] = pWbGain[i].GainB;
            }
        }
    } else {
        /* data null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae user data put
 *  @param[in] ImageChanId image channel id
 *  @param[in,out] pUserData pointer to the user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_UserDataPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, void *pUserData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 AlgoId;

    AE_MEM_INFO_PF_s MemInfo;

    AMBA_IMG_MAIN_AE_USER_DATA_s *pUser;
    const PIPELINE_CONTROL_s *pVideoCtrl;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
    AMBA_SENSOR_HDR_INFO_s SensorHdrInfo;

    UINT32 ExposureNum;
    UINT32 HdrEnable;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    AlgoId = ImageChanId.Ctx.Bits.AlgoId;

    ExposureNum = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;
    HdrEnable = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;

    AmbaImgSensorHAL_TimingMarkPut(VinId, "User_Put");

    if (ImageChanId.Ctx.Bits.HdrId == 0U) {
        /* non-hdr */
        if (pUserData != NULL) {
            /* user get */
            MemInfo.Ctx.pVoid = pUserData;
            pUser = MemInfo.Ctx.pUser;
            /* hdr info reset */
            pUser->HdrInfo.Op.Data = 0ULL;
            /* linear info carry */
            if (AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.LCeEnable > 0U) {
                FuncRetCode = AmbaImgProc_ADJGetVideoAddr(AlgoId, &(MemInfo.Ctx.Data));
                if (FuncRetCode == OK) {
                    pVideoCtrl = MemInfo.Ctx.pVideoCtrl;
                    /* ce */
#if 0 /* in adj */
                    if (pVideoCtrl->CEInfoUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.Ce), &(pVideoCtrl->CEInfo), sizeof(AMBA_IK_CE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.Ce = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce");
                    }
#endif
                    /* ce input */
                    if (pVideoCtrl->CEInputTableUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.CeInput), &(pVideoCtrl->CEInputTable), sizeof(AMBA_IK_CE_INPUT_TABLE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.CeInput = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_In");
                    }
                    /* ce output */
                    if (pVideoCtrl->CEOutputTableUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.CeOutput), &(pVideoCtrl->CEOutputTable), sizeof(AMBA_IK_CE_OUT_TABLE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.CeOutput = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_Out");
                    }
                    /* linear ce flags reset */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                    FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 2U/*ce*/);
#else
                    FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 2U/*ce*/);
#endif
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }
        }
    } else {
        /* hdr */
        if (pUserData != NULL) {
            /* user get */
            MemInfo.Ctx.pVoid = pUserData;
            pUser = MemInfo.Ctx.pUser;
#if defined(CONFIG_BUILD_IMGFRW_RAW_ENC) || defined(CONFIG_BUILD_IMGFRW_EFOV) || defined(CONFIG_BUILD_IMGFRW_RAW_PIV)
            /* aaa info carry */
            FuncRetCode = AmbaImgProc_GetOfflineAAAInfo(AlgoId, &(pUser->AaaInfo));
            if (FuncRetCode != OK_UL) {
                /* */
            }
#endif
            /* hdr info reset */
            pUser->HdrInfo.Op.Data = 0ULL;
            /* hdr info carry */
            FuncRetCode = AmbaImgProc_ADJGetVideoAddr(AlgoId, &(MemInfo.Ctx.Data));
            if (FuncRetCode == OK) {
                pVideoCtrl = MemInfo.Ctx.pVideoCtrl;
                if (pVideoCtrl != NULL) {
                    /* vin tone curve */
                    if (pVideoCtrl->HDSToneCurveUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(pUser->HdrInfo.pVinToneCurve, &(pVideoCtrl->HDSToneCurve), sizeof(AMBA_IP_VIN_TONE_CURVE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.VinToneCurve = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Vin_TC");
                    }
                    /* fe tone curve */
                    if (pVideoCtrl->FEToneCurveUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.FeToneCurve), &(pVideoCtrl->FEToneCurve), sizeof(AMBA_IK_FE_TONE_CURVE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.FeToneCurve = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_TC");
                    }

                    /* blend */
                    if (pVideoCtrl->HdrBlendUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.Blend[0]), &(pVideoCtrl->HdrBlend), sizeof(AMBA_IK_HDR_BLEND_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.Blend0 = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_Bld0");
                    }
                    //if (pVideoCtrl->HdrBlend1Update) > 0U) {
                        //FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.Blend[1]), &(pVideoCtrl->HdrBlend1), sizeof(AMBA_IK_HDR_BLEND_s));
                        //if (FuncRetCode != OK_UL) {
                            /* */
                        //}
                        //pUser->HdrInfo.Op.Bits.Blend1 = 1U;
                        //AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_Bld1");
                    //}
                    /* blc */
                    if (pVideoCtrl->HdrBlackCorr0Update > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.FeBlc[0]), &(pVideoCtrl->HdrBlackCorr0), sizeof(AMBA_IK_STATIC_BLC_LVL_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.FeBlc0 = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc0");
                    }
                    /* blc1 */
                    if (pVideoCtrl->HdrBlackCorr1Update > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.FeBlc[1]), &(pVideoCtrl->HdrBlackCorr1), sizeof(AMBA_IK_STATIC_BLC_LVL_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.FeBlc1 = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc1");
                    }
                    /* blc2 */
                    if (pVideoCtrl->HdrBlackCorr2Update > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.FeBlc[2]), &(pVideoCtrl->HdrBlackCorr2), sizeof(AMBA_IK_STATIC_BLC_LVL_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.FeBlc2 = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc2");
                    }
                    /* wb gain */
                    if (HdrEnable == 1U) {
                        /* dol hdr */
                        pUser->HdrInfo.Op.Bits.FeWbGain0 = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb0");
                        pUser->HdrInfo.Op.Bits.FeWbGain1 = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb1");
                        if (ExposureNum > 2U) {
                            pUser->HdrInfo.Op.Bits.FeWbGain2 = 1U;
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb2");
                        }
                    } else {
                        /* sensor hdr */
                        pUser->HdrInfo.Op.Bits.FeWbGain0 = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_SWb0");
                    }
                    /* ce */
#if 0 /* in adj */
                    if (pVideoCtrl->CEInfoUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.Ce), &(pVideoCtrl->CEInfo), sizeof(AMBA_IK_CE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.Ce = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce");
                    }
#endif
                    /* ce input */
                    if (pVideoCtrl->CEInputTableUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.CeInput), &(pVideoCtrl->CEInputTable), sizeof(AMBA_IK_CE_INPUT_TABLE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.CeInput = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_In");
                    }
                    /* ce output */
                    if (pVideoCtrl->CEOutputTableUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(&(pUser->HdrInfo.CeOutput), &(pVideoCtrl->CEOutputTable), sizeof(AMBA_IK_CE_OUT_TABLE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.CeOutput = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_Out");
                    }

                    if (HdrEnable == 1U) {
                        /* dol hdr */
                        SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                        SensorChanId.SensorID = ImageChanId.Ctx.Bits.SensorId;
                        /* raw offset clr (needed for ssp) */
                        FuncRetCode = AmbaWrap_memset(&(pUser->HdrInfo.RawOffset), 0, sizeof(AMBA_IK_HDR_RAW_INFO_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* raw offset */
                        FuncRetCode = AmbaSensor_GetHdrInfo(&SensorChanId, pUser->AeInfo.Sensor.Shr, &SensorHdrInfo);
                        if (FuncRetCode == SENSOR_ERR_NONE) {
                            for( i = 0U; i < ExposureNum; i++) {
                                pUser->HdrInfo.RawOffset.XOffset[i] = SensorHdrInfo.ChannelInfo[i].EffectiveArea.StartX;
                                pUser->HdrInfo.RawOffset.YOffset[i] = SensorHdrInfo.ChannelInfo[i].EffectiveArea.StartY * ExposureNum;
                                pUser->HdrInfo.RawOffset.YOffset[i] += i;
                                {
                                    char str[11];
                                    str[0] = 'x';str[1] = ' ';
                                    var_utoa(pUser->HdrInfo.RawOffset.XOffset[i], &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                    str[0] = 'y';
                                    var_utoa(pUser->HdrInfo.RawOffset.YOffset[i], &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                }
                            }
                            pUser->HdrInfo.Op.Bits.RawOffset = 1U;
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Raw_Ofs");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Raw_Ofs!");
                        }
                    } else {
                        /* sensor hdr */
                        pUser->HdrInfo.Op.Bits.RawOffset = 0U;
                    }

                    /* hdr flags reset */
#ifndef CONFIG_BUILD_IMGFRW_AAA_ADJ_EXT
                    FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 1U/*hdr*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = AmbaImgProc_ADJResetVideoFlags(AlgoId, 2U/*ce*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#else
                    FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 1U/*hdr*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    FuncRetCode = Amba_Adj_ResetVideoFlags(AlgoId, 2U/*ce*/);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
#endif
                    /* hdr filter enable */
                    pUser->HdrInfo.Op.Bits.Enable = 1U;

                    /* debug */
                    {
                        char str[11];
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_Op");
                        str[0] = ' ';str[1] = ' ';
                        var_utoa((UINT32) (pUser->HdrInfo.Op.Data >> 32ULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_ZERO);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        var_utoa((UINT32) pUser->HdrInfo.Op.Data, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_ZERO);
                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae aik processing
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_AikProc(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    AE_MEM_INFO_PF_s MemInfo;

    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_MODE_CFG_s *pImgMode;

    const AMBA_IMG_MAIN_AE_USER_DATA_s *pUser;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    AMBA_DSP_VIN_SUB_CHAN_s SubChan;
    AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s VinCfgCtrl;
    UINT64 RawSeqNo;

    UINT32 Var_U[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};

    UINT32 Mode = AMBA_IK_PIPE_VIDEO;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;

    Msg2.Ctx.Data = Msg;

    SubChan.IsVirtual = 0U;
    SubChan.Index = (UINT16) VinId;

    if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
        char str[11];
        str[0] = 'm';str[1] = ' ';
        AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Proc");
        var_utoa((UINT32) Msg2.Ctx.Bits.PreAik, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        str[0] = 'u';
        var_utoa((UINT32) (User & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
    }

    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) User;
    pUser = MemInfo.Ctx.pUser;

    for (i = 0U; (i < AMBA_IMG_NUM_FOV_CHANNEL) && ((ZoneId >> i) > 0U); i++) {
        if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
            /* img mode get */
            ImageChanId2.Ctx.Bits.ZoneId = ((UINT32) 1U) << i;
            FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_VIDEO, &pImgMode);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaWrap_memcpy(&ImgMode, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                if (FuncRetCode == OK_UL) {
                    /* context id get */
                    ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                    /* hdr? */
                    if (pUser->HdrInfo.Op.Bits.Enable > 0U) {
                        /* vin tone curve */
                        if ((pUser->HdrInfo.Op.Bits.VinToneCurve > 0U) && (AmbaImgMainAe_VinToneCurveFlag[VinId][ChainId] > 0U)) {
                            VinCfgCtrl.EnaVinCompand = 1U;
                            MemInfo.Ctx.pUint16 = pUser->HdrInfo.pVinToneCurve->CompandTable;
                            VinCfgCtrl.VinCompandTableAddr = MemInfo.Ctx.Data;
                            MemInfo.Ctx.pUint32 = pUser->HdrInfo.pVinToneCurve->DecompandTable;
                            VinCfgCtrl.VinDeCompandTableAddr = MemInfo.Ctx.Data;
                            MemInfo.Ctx.pVinToneCurve = pUser->HdrInfo.pVinToneCurve;
                            FuncRetCode = AmbaCache_DataClean(MemInfo.Ctx.Data, sizeof(AMBA_IP_VIN_TONE_CURVE_s));
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                            FuncRetCode = AmbaDSP_LiveviewUpdateVinCfg((UINT16) VinId, 1U, &SubChan, &VinCfgCtrl, &RawSeqNo);
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Vin_TC");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Vin_TC!");
                            }
                        }
                        /* fe tone curve */
                        if (pUser->HdrInfo.Op.Bits.FeToneCurve > 0U) {
                            FuncRetCode = AmbaIK_SetFeToneCurve(&ImgMode, &(pUser->HdrInfo.FeToneCurve));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_TC");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_TC!");
                            }
                        }

                        /* fe blc 0 */
                        if (pUser->HdrInfo.Op.Bits.FeBlc0 > 0U) {
                            FuncRetCode = AmbaIK_SetFeStaticBlc(&ImgMode, &(pUser->HdrInfo.FeBlc[0]), 0U);
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc0");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc0!");
                            }
                        }
                        /* fe blc 1 */
                        if (pUser->HdrInfo.Op.Bits.FeBlc1 > 0U) {
                            FuncRetCode = AmbaIK_SetFeStaticBlc(&ImgMode, &(pUser->HdrInfo.FeBlc[1]), 1U);
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc1");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc1!");
                            }
                        }
                        /* fe blc 2 */
                        if (pUser->HdrInfo.Op.Bits.FeBlc2 > 0U) {
                            FuncRetCode = AmbaIK_SetFeStaticBlc(&ImgMode, &(pUser->HdrInfo.FeBlc[2]), 2U);
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc2");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Blc2!");
                            }
                        }

                        /* wb gain 0 */
                        if (pUser->HdrInfo.Op.Bits.FeWbGain0 > 0U) {
                            /* dsp pipe dgc? */
                            if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_FE) {
                                /* dgc (fe) */
                                FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeDgc[0]));
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc0");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc0!");
                                }
                            }
#ifdef IMG_MAIN_FE_WB_IR_GAIN
                            /* igc (fe) */
                            {
                                FuncRetCode = AmbaImgSystem_IgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeIgc[0]));
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc0");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc0!");
                                }
                            }
#endif
                            if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_BE) {
                                /* dgc (be) */
                                FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), &(pUser->AeInfo.Dsp.BeDgc[0][0]));
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Dgc");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Dgc!");
                                }
                            }
                            if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_BE_ALT) {
                                /* dgc (alt) */
                                FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), &(pUser->AeInfo.Dsp.BeDgc[1][0]));
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Dgc");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Dgc!");
                                }
                            }
                            /* dsp pipe wb? */
                            if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_FE) {
                                /* wb (fe) */
                                FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), pUser->WbInfo.Dsp.Wgc[0]);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb0");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb0!");
                                }
                            } else {
                                if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_BE) {
                                    /* wb (be) */
                                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), pUser->WbInfo.Dsp.Wgc[0]);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Wb");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Wb!");
                                    }
                                } else if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_BE_ALT) {
                                    /* wb (alt) */
                                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), pUser->WbInfo.Dsp.Wgc[0]);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Wb");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Wb!");
                                    }
                                } else {
                                    /* */
                                }
                            }
                        }
                        /* wb gain 1 */
                        if (pUser->HdrInfo.Op.Bits.FeWbGain1 > 0U) {
                            /* dsp pipe dgc? */
                            if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_FE) {
                                /* dgc (fe) */
                                FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE1), &(pUser->AeInfo.Dsp.FeDgc[1]));
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc1");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc1!");
                                }
                            }
#ifdef IMG_MAIN_FE_WB_IR_GAIN
                            /* igc (fe) */
                            {
                                FuncRetCode = AmbaImgSystem_IgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE1), &(pUser->AeInfo.Dsp.FeIgc[1]));
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc1");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc1!");
                                }
                            }
#endif
                            /* dsp pipe wb? */
                            if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_FE) {
                                /* wb (fe) */
                                FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE1), pUser->WbInfo.Dsp.Wgc[1]);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb1");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb1!");
                                }
                            }
                        }
                        /* wb gain 2 */
                        if (pUser->HdrInfo.Op.Bits.FeWbGain2 > 0U) {
                            /* dsp pipe dgc? */
                            if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_FE) {
                                /* dgc (fe) */
                                Var_U[0] = pUser->AeInfo.Dsp.FeDgc[2];
                                FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE2), Var_U);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc2");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc2!");
                                }
                            }
#ifdef IMG_MAIN_FE_WB_IR_GAIN
                            /* igc (fe) */
                            {
                                Var_U[0] = pUser->AeInfo.Dsp.FeIgc[2];
                                FuncRetCode = AmbaImgSystem_IgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE2), Var_U);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc2");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc2!");
                                }
                            }
#endif
                            /* dsp pipe wb? */
                            if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_FE) {
                                /* wb (fe) */
                                FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE2), pUser->WbInfo.Dsp.Wgc[2]);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb2");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb2!");
                                }
                            }
                        }

                        /* blend 0 */
                        if (pUser->HdrInfo.Op.Bits.Blend0 > 0U) {
                            FuncRetCode = AmbaIK_SetHdrBlend(&ImgMode, &(pUser->HdrInfo.Blend[0]));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_Bld0");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_Bld0!");
                            }
                        }
                        /* blend 1 */
                        if (pUser->HdrInfo.Op.Bits.Blend1 > 0U) {
                            FuncRetCode = AmbaIK_SetHdrBlend(&ImgMode, &(pUser->HdrInfo.Blend[1]));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_Bld1");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_Bld1!");
                            }
                        }

                        /* ce */
                        if (pUser->HdrInfo.Op.Bits.Ce > 0U) {
                            FuncRetCode = AmbaIK_SetCe(&ImgMode, &(pUser->HdrInfo.Ce));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce!");
                            }
                        }
                        /* ce input */
                        if (pUser->HdrInfo.Op.Bits.CeInput > 0U) {
                            FuncRetCode = AmbaIK_SetCeInputTable(&ImgMode, &(pUser->HdrInfo.CeInput));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_In");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_In!");
                            }
                        }
                        /* ce output */
                        if (pUser->HdrInfo.Op.Bits.CeOutput > 0U) {
                            FuncRetCode = AmbaIK_SetCeOutputTable(&ImgMode, &(pUser->HdrInfo.CeOutput));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_Out");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_Out!");
                            }
                        }

                        /* offset */
                        if (pUser->HdrInfo.Op.Bits.RawOffset > 0U) {
                            FuncRetCode = AmbaIK_SetHdrRawOffset(&ImgMode, &(pUser->HdrInfo.RawOffset));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Raw_Ofs");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Raw_Ofs!");
                            }
                        }

                    } else if (AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.LCeEnable > 0U) {
                        /* dsp pipe dgc? */
                        if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_FE) {
                            /* dgc (fe) */
                            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeDgc[0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc0");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc0!");
                            }
                        }
#ifdef IMG_MAIN_FE_WB_IR_GAIN
                        /* igc (fe) */
                        {
                            FuncRetCode = AmbaImgSystem_IgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeIgc[0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc0");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc0!");
                            }
                        }
#endif
                        if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_BE) {
                            /* dgc (be) */
                            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), &(pUser->AeInfo.Dsp.BeDgc[0][0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Dgc");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Dgc!");
                            }
                        }
                        if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_BE_ALT) {
                            /* dgc (alt) */
                            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), &(pUser->AeInfo.Dsp.BeDgc[1][0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Dgc");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Dgc!");
                            }
                        }
                        /* ce */
                        if (pUser->HdrInfo.Op.Bits.Ce > 0U) {
                            FuncRetCode = AmbaIK_SetCe(&ImgMode, &(pUser->HdrInfo.Ce));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce!");
                            }
                        }
                        /* ce input */
                        if (pUser->HdrInfo.Op.Bits.CeInput > 0U) {
                            FuncRetCode = AmbaIK_SetCeInputTable(&ImgMode, &(pUser->HdrInfo.CeInput));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_In");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_In!");
                            }
                        }
                        /* ce output */
                        if (pUser->HdrInfo.Op.Bits.CeOutput > 0U) {
                            FuncRetCode = AmbaIK_SetCeOutputTable(&ImgMode, &(pUser->HdrInfo.CeOutput));
                            if (FuncRetCode == OK) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_Out");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ce_Out!");
                            }
                        }
                    } else {
                        /* dsp pipe dgc? */
                        if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_FE) {
                            /* dgc (fe) */
                            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeDgc[0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc0");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Dgc0!");
                            }
                        }
#ifdef IMG_MAIN_FE_WB_IR_GAIN
                        /* igc (fe) */
                        {
                            FuncRetCode = AmbaImgSystem_IgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeIgc[0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc0");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Igc0!");
                            }
                        }
#endif
                        if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_BE) {
                            /* dgc (be) */
                            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), &(pUser->AeInfo.Dsp.BeDgc[0][0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Dgc");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Be_Dgc!");
                            }
                        }
                        if (pUser->AeInfo.Dsp.SyncDgId != SYNC_WB_ID_BE_ALT) {
                            /* dgc (alt) */
                            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), &(pUser->AeInfo.Dsp.BeDgc[1][0]));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Dgc");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Alt_Dgc!");
                            }
                        }
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae digital gain position get
 *  @param[out] pSyncDgId pointer to the digital gain id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_DgcPositionGet(UINT32 *pSyncDgId)
{
    UINT32 RetCode = OK_UL;
    *pSyncDgId = AMBA_IMG_MAIN_AE_SYNC_DGCID;
    return RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_EFOV
/**
 *  @private
 *  Amba image main ae external fov tx cb
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] TxData tx data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_EFovTxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 TxData)
{
    static AMBA_IMG_MAIN_AE_EFOV_TXRX_s GNU_SECTION_NOZEROINIT AmbaImgMainAe_EFovTxRxData[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    AMBA_IMG_MAIN_AE_EFOV_TXRX_s *pEFovTxRx;

    AE_MEM_INFO_PF_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* msg get */
    Msg2.Ctx.Data = Msg;

    if (Msg2.Ctx.Bits2.AikExec > 0U) {
        /* tx get */
        MemInfo.Ctx.pUint64 = &(Msg2.Ctx.Data);
        /* efov tx aik exec */
        FuncRetCode = AmbaImgMainEFov_Tx(ImageChanId, (UINT64) MemInfo.Ctx.Data, (UINT32) sizeof(UINT64));
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
        /* debug msg */
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, "EFov_Tx_A");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, "EFov_Tx_A!");
        }
    } else if (Msg2.Ctx.Bits2.Ae > 0U) {
        /* tx get */
        pEFovTxRx = &(AmbaImgMainAe_EFovTxRxData[VinId][ChainId]);
        /* user get */
        MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) TxData;
        /* msg put */
        pEFovTxRx->Msg = Msg2.Ctx.Data;

        /* efov tx ae put */
        FuncRetCode = AmbaWrap_memcpy(&(pEFovTxRx->User.AaaInfo), &(MemInfo.Ctx.pUser->AaaInfo), sizeof(AMBA_IMGPROC_OFFLINE_AAA_INFO_s));
        if (FuncRetCode != OK_UL) {
            /* */
        }
        FuncRetCode = AmbaWrap_memcpy(pEFovTxRx->User.AeInfo.Ctx, MemInfo.Ctx.pUser->AeInfo.Ctx, sizeof(AMBA_AE_INFO_s)*AMBA_IMG_MAX_EXPOSURE_NUM);
        if (FuncRetCode != OK_UL) {
            /* */
        }
        FuncRetCode = AmbaWrap_memcpy(pEFovTxRx->User.WbInfo.Ctx, MemInfo.Ctx.pUser->WbInfo.Ctx, sizeof(AMBA_IK_WB_GAIN_s)*AMBA_IMG_MAX_EXPOSURE_NUM);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* efov tx ae write */
        MemInfo.Ctx.pTxRx = pEFovTxRx;
        FuncRetCode = AmbaImgMainEFov_Tx(ImageChanId, (UINT64) MemInfo.Ctx.Data, (UINT32) sizeof(AMBA_IMG_MAIN_AE_EFOV_TXRX_s));
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }

        /* debug msg */
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_E");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_E!");
        }
    } else if (Msg2.Ctx.Bits2.Raw > 0U) {
        /* tx get */
        MemInfo.Ctx.pUint64 = &(Msg2.Ctx.Data);
        /* efov tx raw write in mof */
        FuncRetCode = AmbaImgMainEFov_Tx(ImageChanId, (UINT64) MemInfo.Ctx.Data, (UINT32) sizeof(UINT64));
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
        /* debug msg */
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_R");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Tx_R!");
        }
        {
            char str[11];
            /* raw cap seq */
            str[0] = 'm';str[1] = ' ';
            var_utoa(Msg2.Ctx.Bits2.Var, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, str);
        }
    } else if (Msg2.Ctx.Bits2.SofEnd > 0U) {
        /* tx get */
        MemInfo.Ctx.pUint64 = &(Msg2.Ctx.Data);
        /* efov tx sof end */
        FuncRetCode = AmbaImgMainEFov_Tx(ImageChanId, (UINT64) MemInfo.Ctx.Data, (UINT32) sizeof(UINT64));
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
        /* debug msg */
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, "EFov_Tx_S");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, "EFov_Tx_S!");
        }
    } else {
        /* */
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae external fov rx cb
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] RxData rx data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_EFovRxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 RxData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 AlgoId;

    UINT32 HdrEnable;
    UINT32 ExposureNum;

    AMBA_AE_INFO_s    *pAeInfo;
    AMBA_IK_WB_GAIN_s *pWbGain;
    AMBA_IMG_SENSOR_WB_s *pSensorWb;

    FLOAT VarF;
    UINT32 Factor[AMBA_IMG_MAX_EXPOSURE_NUM]={0};

    UINT32 *pAgc;
    UINT32 *pDgc;
    const UINT32 *pDDgc;
    UINT32 *pShr;

    UINT32 WbPosition = 0U;
    UINT32 WbId = (UINT32) WB_ID_FE;
    UINT32 SyncDgId = AMBA_IMG_MAIN_AE_SYNC_DGCID;
    UINT32 Mode = AMBA_IK_PIPE_VIDEO;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    AE_MEM_INFO_PF_s MemInfo;
    AE_MEM_INFO_PF_s MemInfo2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* msg get */
    Msg2.Ctx.Data = Msg;
    /* rx data get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) RxData;

    if (Msg2.Ctx.Bits2.AikExec > 0U) {
        /* efov rx aik exec */
        FuncRetCode = AmbaImgSystem_AikExec(ImageChanId);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
        /* debug msg */
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Rx_A");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Rx_A!");
        }
    } else if (Msg2.Ctx.Bits2.Ae > 0U) {
        /* algo id get */
        AlgoId = ImageChanId.Ctx.Bits.AlgoId;
        /* user get */
        MemInfo.Ctx.pEFovUser = &(MemInfo.Ctx.pTxRx->User);
        /* hdr info get */
        HdrEnable = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;
        ExposureNum = (UINT32) AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;
        /* wb position get */
        FuncRetCode = AmbaImgProc_AWBGetWBPosition(AlgoId, &WbPosition);
        if (FuncRetCode == OK) {
            switch (WbPosition) {
                case 16U:
                    WbId = (UINT32) WB_ID_SENSOR;
                    break;
                case 32U:
                    WbId = (UINT32) WB_ID_FE;
                    break;
                case 48U:
                    WbId = (UINT32) WB_ID_BE;
                    break;
                case 64U:
                    WbId = (UINT32) WB_ID_BE_ALT;
                    break;
                default:
                    /* */
                    break;
            }
        }

        /* ae/awb info put */
        for (i = 0U; i < ExposureNum; i++) {
            FuncRetCode = AmbaImgProc_AESetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(MemInfo.Ctx.pEFovUser->AeInfo.Ctx[i]));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaImgProc_AWBSetWBGain(AlgoId, i, IP_MODE_VIDEO, &(MemInfo.Ctx.pEFovUser->WbInfo.Ctx[i]));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
        /* aaa info put (TBD) */
        if (HdrEnable > 0U) {
            FuncRetCode = AmbaImgProc_SetOfflineAAAInfo(AlgoId, MemInfo.Ctx.pEFovUser->AaaInfo);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }

        /* user ae buf get */
        MemInfo2.Ctx.pVoid = AmbaImgSensorSync_UserBufferGet(ImageChanId);

        /* user ae info */
        pAeInfo = MemInfo2.Ctx.pUser->AeInfo.Ctx;
        /* user ae shr/agc/dgc */
        pShr = MemInfo2.Ctx.pUser->AeInfo.Sensor.Shr;
        pAgc = MemInfo2.Ctx.pUser->AeInfo.Sensor.Agc;
        pDgc = MemInfo2.Ctx.pUser->AeInfo.Sensor.Dgc;
        /* dg id put */
        FuncRetCode = AmbaImgMainAe_DgcPositionGet(&SyncDgId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* dg id put */
        MemInfo2.Ctx.pUser->AeInfo.Dsp.SyncDgId = SyncDgId;
        /* user ae ddgc */
        switch (SyncDgId) {
            case SYNC_WB_ID_FE:
                pDDgc = MemInfo2.Ctx.pUser->AeInfo.Dsp.FeDgc;
                break;
            case SYNC_WB_ID_BE:
                pDDgc = MemInfo2.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
                break;
            case SYNC_WB_ID_BE_ALT:
                pDDgc = MemInfo2.Ctx.pUser->AeInfo.Dsp.BeDgc[1];
                break;
            default:
                pDDgc = MemInfo2.Ctx.pUser->AeInfo.Dsp.BeDgc[0];
                break;
        }

        /* user wb info */
        pWbGain = MemInfo2.Ctx.pUser->WbInfo.Ctx;
        /* wb id put */
        MemInfo2.Ctx.pUser->WbInfo.Dsp.WbId = WbId;
        /* user sensor wgc */
        pSensorWb = MemInfo2.Ctx.pUser->WbInfo.Sensor.Wgc;

        /* ae/awb info get */
        for (i = 0U; i < ExposureNum; i++) {
            /* ae info get */
            FuncRetCode = AmbaImgProc_AEGetExpInfo(AlgoId, i, IP_MODE_VIDEO, &(pAeInfo[i]));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* hdr? */
            if (HdrEnable > 0U) {
                /* awb info get */
                FuncRetCode = AmbaImgProc_AWBGetWBGain(AlgoId, i, IP_MODE_VIDEO, &(pWbGain[i]));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
            }
        }

        /* hdr calculate (TBD) */
        if (HdrEnable > 0U) {
            const AMBA_IMGPROC_OFFLINE_AAA_INFO_s *pAaaInfo = &(MemInfo.Ctx.pEFovUser->AaaInfo);
            /* shift ae gain */
            FuncRetCode = AmbaImgProc_AdjHdrSetShiftAeGain(AlgoId, pAaaInfo->ShiftAeGain);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            /* offline hdr control */
            FuncRetCode = AmbaImgProc_OfflineHdrVideoControl(AlgoId, pAaaInfo->BlendRatio, pAaaInfo->MaxBlendRatio, pAaaInfo->MinBlendRatio);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }

        /* gain rebuild for algo */
        FuncRetCode = AmbaImgMainAe_UserGainPut(ImageChanId, MemInfo2.Ctx.pVoid);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* agc/dgc/wgc transfer */
        for (i = 0U; i < ExposureNum; i++) {
            VarF = pAeInfo[i].AgcGain * 4096.0f;
            Factor[i] = (VarF > 0.0f) ? (UINT32) VarF : 0U;
        }
        FuncRetCode = AmbaImgSensorDrv_GainConvert(ImageChanId, Factor, pAgc, pDgc, pSensorWb);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* shr transfer */
        for (i = 0U; i < ExposureNum; i++) {
            VarF = pAeInfo[i].ShutterTime * 1024.0f * 1024.0f;
            Factor[i] = (VarF > 0.0f) ? (UINT32) VarF : 0U;
        }
        FuncRetCode = AmbaImgSensorDrv_ShutterConvert(ImageChanId, Factor, pShr);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* user data put */
        FuncRetCode = AmbaImgMainAe_UserDataPut(ImageChanId, MemInfo2.Ctx.pVoid);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* adv dsp */
        FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId, Mode | SyncDgId, pDDgc);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* aik proc */
        FuncRetCode = AmbaImgMainAe_AikProc(ImageChanId, 0ULL, (UINT64) MemInfo2.Ctx.Data);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* debug msg */
        AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Rx_E");
    } else if (Msg2.Ctx.Bits2.Raw > 0U) {
        /* efov rx raw in mof */

        /* debug msg */
        AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Rx_R");
        {
            char str[11];
            /* raw cap seq */
            str[0] = 'm';str[1] = ' ';
            var_utoa(Msg2.Ctx.Bits2.Var, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, str);
        }
    } else {
        /* */
    }

    return RetCode;
}
#endif
/**
 *  @private
 *  Amba image main ae sls callback
 *  @param[in] ImageChanId image channel id
 *  @param[in] User the user data address
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_SlsCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AE_MEM_INFO_PF_s MemInfo;

    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) User;
    FuncRetCode = AmbaImgSensorDrv_SlsWrite(ImageChanId, &(MemInfo.Ctx.pUser->AeInfo.Sensor.Sls));
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    if (AmbaImgSensorHAL_TimingMarkChk(ImageChanId.Ctx.Bits.VinId) == OK_UL) {
        char str[11];
        str[0] = ' ';str[1] = ' ';
        var_utoa(MemInfo.Ctx.pUser->AeInfo.Sensor.Sls, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
        AmbaImgSensorHAL_TimingMarkPut(ImageChanId.Ctx.Bits.VinId, str);
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae user snap gain put
 *  @param[in] ImageChanId image channel id
 *  @param[in] AlgoId algorithm id
 *  @param[in] AebIndex aeb index
 *  @param[in,out] pUserData pointer to the user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_UserSnapGainPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, UINT32 AebIndex, void *pUserData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_AE_INFO_s    *pAeInfo;
    AMBA_DGAIN_INFO_s       *pAuxAeInfo;
    const AMBA_IK_WB_GAIN_s *pWbGain;
    AMBA_IMG_SENSOR_WB_s    *pSensorWb;

    UINT32 *pDspWgc;

    UINT32 *pFeDgc;
    UINT32 *pBeDgc;

    AE_SNAP_MEM_INFO_PF_s MemInfo;

    UINT32 ExposureNum;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    ExposureNum = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;

    if (pUserData != NULL) {
        /* user data get */
        MemInfo.Ctx.pVoid = pUserData;

        /* user ae info */
        pAeInfo = MemInfo.Ctx.pUser->AeInfo.Ctx;
        /* user aux ae info */
        pAuxAeInfo = MemInfo.Ctx.pUser->AeInfo.AuxCtx;
        /* user dgain get */
        FuncRetCode = AmbaImgProc_GetStillIdspDgInfo(AlgoId, AebIndex, pAeInfo, pAuxAeInfo);
        if (FuncRetCode != OK_UL) {
            /* */
        }

        /* user flash gain (TBD) */
        {
            UINT32 SlsFactor;
            /* sls factor get */
            SlsFactor = pAeInfo[0].Flash;
            /* sls reset */
            MemInfo.Ctx.pUser->AeInfo.Sensor.Sls = 0U;
            /* sls get */
            FuncRetCode = AmbaImgSensorDrv_SlsConvert(ImageChanId, &SlsFactor, &(MemInfo.Ctx.pUser->AeInfo.Sensor.Sls));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }

        /* user fe gain */
        pFeDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.FeDgc;
        for (i = 0U; i < ExposureNum; i++) {
            pFeDgc[i] = pAuxAeInfo[i].DgainInfo[1];
        }

        /* user be gain */
        for (i = 0U; i < 2U; i++) {
            pBeDgc = MemInfo.Ctx.pUser->AeInfo.Dsp.BeDgc[i];
            for (j = 0U; j < ExposureNum; j++) {
                pBeDgc[j] = pAuxAeInfo[j].DgainInfo[2U+i];
            }
        }

        /* user algo wb info */
        pWbGain = MemInfo.Ctx.pUser->WbInfo.Ctx;
        /* user sensor wb gain */
        pSensorWb = MemInfo.Ctx.pUser->WbInfo.Sensor.Wgc;
        /* sensor wgc? */
        if (MemInfo.Ctx.pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_SENSOR) {
            /* sensor wgc */
            for (i = 0U; i < ExposureNum; i++) {
                pSensorWb[i].Gain[0] = (pWbGain[i].GainR*pAuxAeInfo[i].DgainInfo[0])/4096U;
                pSensorWb[i].Gain[1] = (pWbGain[i].GainG*pAuxAeInfo[i].DgainInfo[0])/4096U;
                pSensorWb[i].Gain[2] = (pWbGain[i].GainG*pAuxAeInfo[i].DgainInfo[0])/4096U;
                pSensorWb[i].Gain[3] = (pWbGain[i].GainB*pAuxAeInfo[i].DgainInfo[0])/4096U;
            }
        } else {
            /* sensor wb reset */
            for (i = 0U; i < ExposureNum; i++) {
                for (j = 0U; j < AMBA_NUM_SWB_CHANNEL; j++) {
                    pSensorWb[i].Gain[j] = 4096U;
                }
            }
        }
        /* dsp wgc? */
        if (MemInfo.Ctx.pUser->WbInfo.Dsp.WbId != (UINT32) WB_ID_SENSOR) {
            /* wgc info */
            for (i = 0U; i < ExposureNum; i++) {
                /* user dsp wb gain */
                pDspWgc = MemInfo.Ctx.pUser->WbInfo.Dsp.Wgc[i];
                /* wb gain */
                pDspWgc[0] = pWbGain[i].GainR;
                pDspWgc[1] = pWbGain[i].GainG;
                pDspWgc[2] = pWbGain[i].GainB;
            }
        } else {
            for (i = 0U; i < ExposureNum; i++) {
                /* user dsp wb gain */
                pDspWgc = MemInfo.Ctx.pUser->WbInfo.Dsp.Wgc[i];
                /* wgc info for carry */
                pDspWgc[0] = pWbGain[i].GainR;
                pDspWgc[1] = pWbGain[i].GainG;
                pDspWgc[2] = pWbGain[i].GainB;
            }
        }
    } else {
        /* data null */
        RetCode = NG_UL;
    }

    return RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
/**
 *  @private
 *  Amba image main ae user piv gain put (TBD)
 *  @param[in] ImageChanId image channel id
 *  @param[in] AlgoId algorithm id
 *  @param[in,out] pUserData pointer to the user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_UserPivGainPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, void *pUserData)
{
    return AmbaImgMainAe_UserSnapGainPut(ImageChanId, AlgoId, 0U, pUserData);
}
#endif
/**
 *  @private
 *  Amba image main ae user snap data put
 *  @param[in] ImageChanId image channel id
 *  @param[in] AlgoId algorithm id
 *  @param[in,out] pUserData pointer to the user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_UserSnapDataPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, void *pUserData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    AE_SNAP_MEM_INFO_PF_s MemInfo;

    AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s *pUser;

    const PIPELINE_STILL_CONTROL_s *pStillCtrl;

    AMBA_SENSOR_CHANNEL_s SensorChanId;
    AMBA_SENSOR_HDR_INFO_s SensorHdrInfo;

    UINT32 ExposureNum;
    UINT32 HdrEnable;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    ExposureNum = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;
    HdrEnable = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;

    AmbaImgSensorHAL_TimingMarkPut(VinId, "User_Put");

    if (ImageChanId.Ctx.Bits.HdrId == 0U) {
        /* non-hdr */
        if (pUserData != NULL) {
            /* user get */
            MemInfo.Ctx.pVoid = pUserData;
            pUser = MemInfo.Ctx.pUser;
            /* hdr info reset */
            pUser->HdrInfo.Op.Data = 0ULL;
        }
    } else {
        /* hdr */
        if (pUserData != NULL) {
            /* user get */
            MemInfo.Ctx.pVoid = pUserData;
            pUser = MemInfo.Ctx.pUser;
            /* hdr info reset */
            pUser->HdrInfo.Op.Data = 0ULL;
            /* hdr info carry */
            FuncRetCode = AmbaImgProc_ADJGetStillAddr(AlgoId, &(MemInfo.Ctx.Data));
            if (FuncRetCode == OK) {
                /* still ctrl get */
                pStillCtrl = MemInfo.Ctx.pStillCtrl;
                if (pStillCtrl != NULL) {
                    /* vin tone curve */
                    if (pStillCtrl->HDSToneCurveUpdate > 0U) {
                        FuncRetCode = AmbaWrap_memcpy(pUser->HdrInfo.pVinToneCurve, &(pStillCtrl->HDSToneCurve), sizeof(AMBA_IP_VIN_TONE_CURVE_s));
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        pUser->HdrInfo.Op.Bits.VinToneCurve = 1U;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Vin_TC");
                    }
                }
            }
            /* wb gain */
            if (HdrEnable == 1U) {
                /* dol hdr */
                pUser->HdrInfo.Op.Bits.FeWbGain0 = 1U;
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb0");
                pUser->HdrInfo.Op.Bits.FeWbGain1 = 1U;
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb1");
                if (ExposureNum > 2U) {
                    pUser->HdrInfo.Op.Bits.FeWbGain2 = 1U;
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb2");
                }
            } else {
                /* sensor hdr */
                pUser->HdrInfo.Op.Bits.FeWbGain0 = 1U;
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_SWb0");
            }
            /* raw offset */
            if (HdrEnable == 1U) {
                /* dol hdr */
                SensorChanId.VinID = ImageChanId.Ctx.Bits.VinId;
                SensorChanId.SensorID = ImageChanId.Ctx.Bits.SensorId;
                /* raw offset clr (needed for ssp) */
                FuncRetCode = AmbaWrap_memset(&(pUser->HdrInfo.RawOffset), 0, sizeof(AMBA_IK_HDR_RAW_INFO_s));
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                /* raw offset */
                FuncRetCode = AmbaSensor_GetHdrInfo(&SensorChanId, pUser->AeInfo.Sensor.Shr, &SensorHdrInfo);
                if (FuncRetCode == SENSOR_ERR_NONE) {
                    for( i = 0U; i < ExposureNum; i++) {
                        pUser->HdrInfo.RawOffset.XOffset[i] = SensorHdrInfo.ChannelInfo[i].EffectiveArea.StartX;
                        pUser->HdrInfo.RawOffset.YOffset[i] = SensorHdrInfo.ChannelInfo[i].EffectiveArea.StartY * ExposureNum;
                        pUser->HdrInfo.RawOffset.YOffset[i] += i;
                        {
                            char str[11];
                            str[0] = 'x';str[1] = ' ';
                            var_utoa(pUser->HdrInfo.RawOffset.XOffset[i], &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                            str[0] = 'y';
                            var_utoa(pUser->HdrInfo.RawOffset.YOffset[i], &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                        }
                    }
                    pUser->HdrInfo.Op.Bits.RawOffset = 1U;
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SRaw_Ofs");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SRaw_Ofs!");
                }
            } else {
                /* sensor hdr */
                pUser->HdrInfo.Op.Bits.RawOffset = 0U;
            }

            /* hdr filter enable */
            pUser->HdrInfo.Op.Bits.Enable = 1U;
        }
    }

    return RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
/**
 *  @private
 *  Amba image main ae user piv data put (TBD)
 *  @param[in] ImageChanId image channel id
 *  @param[in] AlgoId algorithm id
 *  @param[in,out] pUserData pointer to the user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_UserPivDataPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, void *pUserData)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AE_SNAP_MEM_INFO_PF_s MemInfo;

    AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s *pUser;

    UINT32 ExposureNum;
    UINT32 HdrEnable;

    (void) AlgoId;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    ExposureNum = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.ExposureNum;
    HdrEnable = AmbaImgSystem_Info[VinId][ChainId].Pipe.Hdr.Bits.Enable;

    AmbaImgSensorHAL_TimingMarkPut(VinId, "User_Put");

    if (ImageChanId.Ctx.Bits.HdrId == 0U) {
        /* non-hdr */
        if (pUserData != NULL) {
            /* user get */
            MemInfo.Ctx.pVoid = pUserData;
            pUser = MemInfo.Ctx.pUser;
            /* hdr info reset */
            pUser->HdrInfo.Op.Data = 0ULL;
        }
    } else {
        /* hdr */
        if (pUserData != NULL) {
            /* user get */
            MemInfo.Ctx.pVoid = pUserData;
            pUser = MemInfo.Ctx.pUser;
            /* hdr info reset */
            pUser->HdrInfo.Op.Data = 0ULL;
            /* wb gain */
            if (HdrEnable == 1U) {
                /* dol hdr */
                pUser->HdrInfo.Op.Bits.FeWbGain0 = 1U;
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb0");
                pUser->HdrInfo.Op.Bits.FeWbGain1 = 1U;
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb1");
                if (ExposureNum > 2U) {
                    pUser->HdrInfo.Op.Bits.FeWbGain2 = 1U;
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_Wb2");
                }
            } else {
                /* sensor hdr */
                pUser->HdrInfo.Op.Bits.FeWbGain0 = 1U;
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Fe_SWb0");
            }

            /* hdr filter enable */
            pUser->HdrInfo.Op.Bits.Enable = 1U;
        }
    }

    return RetCode;
}
#endif
/**
 *  @private
 *  Amba image main ae aik snap processing
 *  @param[in] ImageChanId image channel id
 *  @param[in] Msg message
 *  @param[in] User user data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_AikSnapProc(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AE_SNAP_MEM_INFO_PF_s MemInfo;

    const AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s *pUser;

    AMBA_DSP_VIN_SUB_CHAN_s SubChan;
    AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s VinCfgCtrl;
    UINT64 RawSeqNo;

    AMBA_IMG_CHANNEL_AIK_PROC_MSG_s Msg2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.VinId;

    SubChan.IsVirtual = 0U;
    SubChan.Index = (UINT16) VinId;

    Msg2.Ctx.Data = Msg;

    if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
        char str[11];
        str[0] = 'm';str[1] = ' ';
        AmbaImgSensorHAL_TimingMarkPut(VinId, "SAik_Proc");
        var_utoa((UINT32) Msg2.Ctx.Bits.PreAik, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        str[0] = 'u';
        var_utoa((UINT32) (User & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
    }

    /* user get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) User;
    pUser = MemInfo.Ctx.pUser;

    /* hdr? */
    if (pUser->HdrInfo.Op.Bits.Enable > 0U) {
        /* vin tone curve */
        if ((pUser->HdrInfo.Op.Bits.VinToneCurve > 0U)  && (AmbaImgMainAe_VinToneCurveFlag[VinId][ChainId] > 0U)) {
            VinCfgCtrl.EnaVinCompand = 1U;
            MemInfo.Ctx.pUint16 = pUser->HdrInfo.pVinToneCurve->CompandTable;
            VinCfgCtrl.VinCompandTableAddr = MemInfo.Ctx.Data;
            MemInfo.Ctx.pUint32 = pUser->HdrInfo.pVinToneCurve->DecompandTable;
            VinCfgCtrl.VinDeCompandTableAddr = MemInfo.Ctx.Data;
            MemInfo.Ctx.pVinToneCurve = pUser->HdrInfo.pVinToneCurve;
            FuncRetCode = AmbaCache_DataClean(MemInfo.Ctx.Data, sizeof(AMBA_IP_VIN_TONE_CURVE_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaDSP_LiveviewUpdateVinCfg((UINT16) VinId, 1U, &SubChan, &VinCfgCtrl, &RawSeqNo);
            if (FuncRetCode == OK) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Vin_TC");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Vin_TC!");
            }
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae sls snap callback
 *  @param[in] ImageChanId image channel id
 *  @param[in] User the user data address
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_SlsSnapCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AE_SNAP_MEM_INFO_PF_s MemInfo;

    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) User;
    FuncRetCode = AmbaImgSensorDrv_SlsWrite(ImageChanId, &(MemInfo.Ctx.pUser->AeInfo.Sensor.Sls));
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image main ae aik snap iso
 *  @param[in] ImageChanId image channel id
 *  @param[in] User the user data address
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_AikSnapIso(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;

    AE_SNAP_MEM_INFO_PF_s MemInfo;

    AMBA_IK_MODE_CFG_s *pImgMode = NULL;

    const AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s *pUser;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    UINT32 Var_U[AMBA_IMG_SENSOR_HAL_HDR_SIZE] = {0};

    UINT32 Mode = AMBA_IK_PIPE_STILL;

    const UINT32 Wgc[3] = {4096U, 4096U, 4096U};

    VinId = ImageChanId.Ctx.Bits.VinId;

    if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
        AmbaImgSensorHAL_TimingMarkPut(VinId, "SAik_Iso");
    }
    /* image channel id get */
    ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
    /* user get */
    MemInfo.Ctx.Data = (AMBA_IMG_MAIN_MEM_ADDR) User;
    pUser = MemInfo.Ctx.pUser;
    /* zone id get */
    ImageChanId2.Ctx.Bits.ZoneId = AmbaImgStatistics_ZoneIdGet(ImageChanId2);
    /* still img mode get */
    FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, AMBA_IK_PIPE_STILL, &pImgMode);
    if (FuncRetCode == OK_UL) {
        /* debug msg */
        AmbaImgPrint(PRINT_FLAG_DBG, "snap proc context id", pImgMode->ContextId);
    }
    /* wb (FE/BE/BE_ALT) reset */
    AmbaImgPrintStr(PRINT_FLAG_DBG, "wb reset");
    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), Wgc);
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), Wgc);
    if (FuncRetCode != OK_UL) {
        /* */
    }
    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), Wgc);
    if (FuncRetCode != OK_UL) {
        /* */
    }
    /* hdr? */
    if (pUser->HdrInfo.Op.Bits.Enable > 0U) {
        /* wb gain 0 */
        if (pUser->HdrInfo.Op.Bits.FeWbGain0 > 0U) {
            /* dgc (fe) */
            AmbaImgPrint(PRINT_FLAG_DBG, "dgc fe", pUser->AeInfo.Dsp.FeDgc[0]);
            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeDgc[0]));
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc0");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc0!");
            }
            /* dgc (be) */
            AmbaImgPrint(PRINT_FLAG_DBG, "dgc be", pUser->AeInfo.Dsp.BeDgc[0][0]);
            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), &(pUser->AeInfo.Dsp.BeDgc[0][0]));
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SBe_Dgc");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SBe_Dgc!");
            }
            /* dgc (alt) */
            AmbaImgPrint(PRINT_FLAG_DBG, "dgc alt", pUser->AeInfo.Dsp.BeDgc[1][0]);
            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), &(pUser->AeInfo.Dsp.BeDgc[1][0]));
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SAlt_Dgc");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SAlt_Dgc!");
            }
            /* dsp pipe wb? */
            if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_FE) {
                /* wb (fe) */
                AmbaImgPrintStr(PRINT_FLAG_DBG, "wb fe");
                AmbaImgPrint(PRINT_FLAG_DBG, "r", pUser->WbInfo.Dsp.Wgc[0][0]);
                AmbaImgPrint(PRINT_FLAG_DBG, "g", pUser->WbInfo.Dsp.Wgc[0][1]);
                AmbaImgPrint(PRINT_FLAG_DBG, "b", pUser->WbInfo.Dsp.Wgc[0][2]);
                FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), pUser->WbInfo.Dsp.Wgc[0]);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Wb0");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Wb0!");
                }
            } else {
                if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_BE) {
                    /* wb (be) */
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "wb be");
                    AmbaImgPrint(PRINT_FLAG_DBG, "r", pUser->WbInfo.Dsp.Wgc[0][0]);
                    AmbaImgPrint(PRINT_FLAG_DBG, "g", pUser->WbInfo.Dsp.Wgc[0][1]);
                    AmbaImgPrint(PRINT_FLAG_DBG, "b", pUser->WbInfo.Dsp.Wgc[0][2]);
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), pUser->WbInfo.Dsp.Wgc[0]);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "SBe_Wb");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "SBe_Wb!");
                    }
                } else if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_BE_ALT) {
                    /* wb (alt) */
                    AmbaImgPrintStr(PRINT_FLAG_DBG, "wb alt");
                    AmbaImgPrint(PRINT_FLAG_DBG, "r", pUser->WbInfo.Dsp.Wgc[0][0]);
                    AmbaImgPrint(PRINT_FLAG_DBG, "g", pUser->WbInfo.Dsp.Wgc[0][1]);
                    AmbaImgPrint(PRINT_FLAG_DBG, "b", pUser->WbInfo.Dsp.Wgc[0][2]);
                    FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), pUser->WbInfo.Dsp.Wgc[0]);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "SAlt_Wb");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "SAlt_Wb!");
                    }
                } else {
                    /* */
                }
            }
        }
        /* wb gain 1 */
        if (pUser->HdrInfo.Op.Bits.FeWbGain1 > 0U) {
            /* dgc (fe) */
            AmbaImgPrint(PRINT_FLAG_DBG, "dgc fe1", pUser->AeInfo.Dsp.FeDgc[1]);
            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE1), &(pUser->AeInfo.Dsp.FeDgc[1]));
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc1");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc1!");
            }
            /* dsp pipe wb? */
            if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_FE) {
                /* wb (fe) */
                AmbaImgPrintStr(PRINT_FLAG_DBG, "wb fe1");
                AmbaImgPrint(PRINT_FLAG_DBG, "r", pUser->WbInfo.Dsp.Wgc[1][0]);
                AmbaImgPrint(PRINT_FLAG_DBG, "g", pUser->WbInfo.Dsp.Wgc[1][1]);
                AmbaImgPrint(PRINT_FLAG_DBG, "b", pUser->WbInfo.Dsp.Wgc[1][2]);
                FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE1), pUser->WbInfo.Dsp.Wgc[1]);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Wb1");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Wb1!");
                }
            }
        }
        /* wb gain 2 */
        if (pUser->HdrInfo.Op.Bits.FeWbGain2 > 0U) {
            /* dgc (fe) */
            AmbaImgPrint(PRINT_FLAG_DBG, "dgc fe2", pUser->AeInfo.Dsp.FeDgc[2]);
            Var_U[0] = pUser->AeInfo.Dsp.FeDgc[2];
            FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE2), Var_U);
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc2");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc2!");
            }
            /* dsp pipe wb? */
            if (pUser->WbInfo.Dsp.WbId == (UINT32) WB_ID_FE) {
                /* wb (fe) */
                AmbaImgPrintStr(PRINT_FLAG_DBG, "wb fe2");
                AmbaImgPrint(PRINT_FLAG_DBG, "r", pUser->WbInfo.Dsp.Wgc[2][0]);
                AmbaImgPrint(PRINT_FLAG_DBG, "g", pUser->WbInfo.Dsp.Wgc[2][1]);
                AmbaImgPrint(PRINT_FLAG_DBG, "b", pUser->WbInfo.Dsp.Wgc[2][2]);
                FuncRetCode = AmbaImgSystem_WgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE2), pUser->WbInfo.Dsp.Wgc[2]);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Wb2");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Wb2!");
                }
            }
        }

        /* offset */
        if (pUser->HdrInfo.Op.Bits.RawOffset > 0U) {
            AmbaImgPrintStr(PRINT_FLAG_DBG, "raw offset");
            if (pImgMode != NULL) {
                FuncRetCode = AmbaIK_SetHdrRawOffset(pImgMode, &(pUser->HdrInfo.RawOffset));
                if (FuncRetCode == OK) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SRaw_Ofs");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "SRaw_Ofs!");
                }
            }
        }
    } else {
        /* dgc (fe) */
        AmbaImgPrint(PRINT_FLAG_DBG, "dgc fe", pUser->AeInfo.Dsp.FeDgc[0]);
        FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pUser->AeInfo.Dsp.FeDgc[0]));
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc0");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "SFe_Dgc0!");
        }
        /* dgc (be) */
        AmbaImgPrint(PRINT_FLAG_DBG, "dgc be", pUser->AeInfo.Dsp.BeDgc[0][0]);
        FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), &(pUser->AeInfo.Dsp.BeDgc[0][0]));
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "SBe_Dgc");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "SBe_Dgc!");
        }
        /* dgc (alt) */
        AmbaImgPrint(PRINT_FLAG_DBG, "dgc alt", pUser->AeInfo.Dsp.BeDgc[1][0]);
        FuncRetCode = AmbaImgSystem_DgcWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), &(pUser->AeInfo.Dsp.BeDgc[1][0]));
        if (FuncRetCode == OK_UL) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "SAlt_Dgc");
        } else {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "SAlt_Dgc!");
        }
    }

    return RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
/**
 *  @private
 *  Amba image main ae aik piv iso (TBD)
 *  @param[in] ImageChanId image channel id
 *  @param[in] User the user data address
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgMainAe_AikPivIso(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User)
{
    return AmbaImgMainAe_AikSnapIso(ImageChanId, User);
}
#endif
