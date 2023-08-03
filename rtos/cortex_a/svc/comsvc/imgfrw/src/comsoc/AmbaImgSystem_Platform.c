/**
 *  @file AmbaImgSystem_Platform.c
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
 *  @details Amba Image System for Common SOC
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaWrap.h"

#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"

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

typedef union /*_AMBA_IMG_SYSTEM_PF_MEM_u_*/ {
    AMBA_IMG_MEM_ADDR     Data;
    void                  *pVoid;
    AMBA_IK_MODE_CFG_s    *pImgMode;
} AMBA_IMG_SYSTEM_PF_MEM_u;

typedef struct /*_AMBA_IMG_SYSTEM_PF_MEM_s*/ {
    AMBA_IMG_SYSTEM_PF_MEM_u    Ctx;
} AMBA_IMG_SYSTEM_PF_MEM_s;

/**
 *  @private
 *  Amba image system platform reset
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_PlatformReset(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;

    const AMBA_IMG_CHANNEL_VR_MAP_s *pVrMap;
    AMBA_IMG_CHANNEL_ID_s ImageChanId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    UINT32 ChainId;
    UINT64 RawSeqNum = 0ULL;

    UINT32 VrId;
    UINT32 VrAltId;

    UINT32 Mode = AMBA_IK_PIPE_VIDEO;

    AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s IsoCfgCtrl;

    /* vin check */
    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL)) {
        /* chain check */
        for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                /* image channel id get */
                ImageChanId.Ctx.Data = pFrwImageChannel[VinId][i].pCtx->Id.Ctx.Data;
                /* vr map get */
                pVrMap = &(pFrwImageChannel[VinId][i].pCtx->VrMap);
                /* chain id get */
                ChainId = ImageChanId.Ctx.Bits.ChainId;
                /* vr id get */
                VrId = pVrMap->Id.Ctx.Bits.VrId;
                /* vr alt id get */
                VrAltId = pVrMap->Id.Ctx.Bits.VrAltId;
                /* vr check */
                if ((VrId > 0U/*vr*/) &&
                    (((VrId & (((UINT32) 1U) << VinId)) == 0U) || ((VrAltId & (((UINT32) 1U) << ChainId)) == 0U))) {
                    /* vr salve */
                    continue;
                }
                /* image channel get */
                ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
                /* valid check */
                if ((AmbaImgSystem_Chan[VinId] != NULL) &&
                    (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                    /* ctx check */
                    if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
                        /* ctx get */
                        pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
                        for (j = 0U; pCtx[j].Magic == 0xCafeU; j++) {
                            /* zone find */
                            if ((ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << j)) > 0U) {
                                /* zone check */
                                if (pCtx[j].pZone != NULL) {
                                    /* iso cfg update */
                                    IsoCfgCtrl.ViewZoneId = (UINT16) j;
                                    IsoCfgCtrl.CtxIndex = j;
                                    IsoCfgCtrl.CfgIndex = 0U;
                                    IsoCfgCtrl.CfgAddress = 0UL;
                                    IsoCfgCtrl.HdrCfgIndex = 0U;
                                    FuncRetCode = AmbaDSP_LiveviewUpdateIsoCfg(1U, &IsoCfgCtrl, &RawSeqNum);
                                    if (FuncRetCode != OK) {
                                        RetCode = NG_UL;
                                    }
                                    /* image channel get */
                                    ImageChanId2.Ctx.Bits.ZoneId = ((UINT32) 1U) << j;
                                    /* zone get */
                                    pZone = pCtx[j].pZone;
                                    /* pipe mutex take */
                                    FuncRetCode = AmbaKAL_MutexTake(&(pZone->Pipe[0].Mutex), AMBA_KAL_WAIT_FOREVER);
                                    if (FuncRetCode == KAL_ERR_NONE) {
                                        /* ik wb (fe0) reset */
                                        FuncRetCode = AmbaImgSystem_WbGainWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_FE), &(pZone->Pipe[0].Wb.Fe[0]), j);
                                        if (FuncRetCode != OK_UL) {
                                            RetCode = NG_UL;
                                        }
                                        /* ik wb (before ce) reset */
                                        FuncRetCode = AmbaImgSystem_WbGainWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE), &(pZone->Pipe[0].Wb.Be), j);
                                        if (FuncRetCode != OK_UL) {
                                            RetCode = NG_UL;
                                        }
                                        /* ik wb (after ce) reset */
                                        FuncRetCode = AmbaImgSystem_WbGainWrite(ImageChanId2, Mode | ((UINT32) WB_PIPE_BE_ALT), &(pZone->Pipe[0].Wb.BeAlt), j);
                                        if (FuncRetCode != OK_UL) {
                                            RetCode = NG_UL;
                                        }
                                        /* pipe mutex give */
                                        FuncRetCode = AmbaKAL_MutexGive(&(pZone->Pipe[0].Mutex));
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

    return RetCode;
}

/**
 *  @private
 *  Amba image system ik context id get
 *  @param[in] ZoneIndex zone index
 *  @return ik context id
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_IkContextIdGet(UINT32 ZoneIndex)
{
    /* zone index to ik context id */
    return ZoneIndex;
}

/**
 *  @private
 *  Amba image system static black level write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode
 *  @param[in] pBlc pointer to the black level data
 *  @param[in] ZoneIndex zone index
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_StaticBlcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const AMBA_IMG_SYSTEM_PIPE_BLC_s *pBlc, UINT32 ZoneIndex)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode = {0};

    AMBA_IK_STATIC_BLC_LVL_s ImgBlc;

    AMBA_IMG_SYSTEM_PF_MEM_s MemInfo;

    /* assign blc */
    ImgBlc.BlackR  = pBlc->Offset[0];
    ImgBlc.BlackGr = pBlc->Offset[1];
    ImgBlc.BlackGb = pBlc->Offset[2];
    ImgBlc.BlackB  = pBlc->Offset[3];

    /* get img mode */
    FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId, Mode, &pImgMode);
    if (FuncRetCode == OK_UL) {
        /* assign img mode */
        MemInfo.Ctx.pImgMode = &ImgMode;
        FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
        if (FuncRetCode == OK) {
            if (Mode == AMBA_IK_PIPE_VIDEO) {
                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(ZoneIndex);
            }
            /* ik blc set */
            FuncRetCode = AmbaIK_SetFeStaticBlc(&ImgMode, &ImgBlc, 0U);
            if (FuncRetCode != IK_OK) {
                RetCode = NG_UL;
            }
        } else {
            /* memcpy fail */
            RetCode = NG_UL;
        }
    } else {
        /* img mode fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system white balance gain write
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode pipe mode with the white balance gain id
 *  @param[in] pWbCtx pointer to the white balance gain context
 *  @param[in] ZoneIndex zone index
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_WbGainWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const AMBA_IMG_SYSTEM_PIPE_WB_CTX_s *pWbCtx, UINT32 ZoneIndex)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;

    UINT32 Scaler;

    const UINT32 *pWgc;
    UINT32 Dgc;
    UINT32 Ggc;

    UINT32 WbId = Mode >> 16U;
    UINT32 PipeMode = Mode % 0x10000U;

    const AMBA_IMG_SYSTEM_VIN_s *pVin = AmbaImgSystem_Vin;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode = {0};

    AMBA_IK_FE_WB_GAIN_s ImgFeWbGain;
    AMBA_IK_WB_GAIN_s ImgWbGain;

    AMBA_IMG_SYSTEM_PF_MEM_s MemInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;

    if ((WbId > 0U) &&
        (WbId < (UINT32) WB_ID_TOTAL)) {
        Scaler = pWbCtx->Op.Bits.Scaler;

        pWgc = pWbCtx->Wgc;
        Dgc = pWbCtx->Dgc;
        Ggc = pWbCtx->Ggc;

        if (pWbCtx->Op.Bits.Mix > 0U) {
            Dgc = (UINT32) ((((UINT64) Dgc)*Ggc/Scaler) & 0xFFFFFFFFULL);

            ImgWbGain.GainR = (UINT32) ((((UINT64) pWgc[0])*Dgc/Scaler) & 0xFFFFFFFFULL);
            ImgWbGain.GainG = (UINT32) ((((UINT64) pWgc[1])*Dgc/Scaler) & 0xFFFFFFFFULL);
            ImgWbGain.GainB = (UINT32) ((((UINT64) pWgc[2])*Dgc/Scaler) & 0xFFFFFFFFULL);

            //ImgWbGain.AeGain = 4096;
            //ImgWbGain.GlobalDGain = 4096;
        } else {
            /* support not yet */
            ImgWbGain.GainR = pWgc[0];
            ImgWbGain.GainG = pWgc[1];
            ImgWbGain.GainB = pWgc[2];

            //ImgWbGain.AeGain = Dgc;
            //ImgWbGain.GlobalDGain = Ggc;
        }

        if (pWbCtx->Op.Bits.Fine > 0U) {
            pWgc = pWbCtx->FineWgc;
            Dgc = pWbCtx->FineDgc;
            Ggc = pWbCtx->FineGgc;

            if (pWbCtx->Op.Bits.Mix > 0U) {
                ImgWbGain.GainR = (UINT32) ((((UINT64) ImgWbGain.GainR)*pWgc[0]/Scaler) & 0xFFFFFFFFULL);
                ImgWbGain.GainG = (UINT32) ((((UINT64) ImgWbGain.GainG)*pWgc[1]/Scaler) & 0xFFFFFFFFULL);
                ImgWbGain.GainB = (UINT32) ((((UINT64) ImgWbGain.GainB)*pWgc[2]/Scaler) & 0xFFFFFFFFULL);

                Dgc = (UINT32) ((((UINT64) Dgc)*Ggc/Scaler) & 0xFFFFFFFFULL);

                ImgWbGain.GainR = (UINT32) ((((UINT64) ImgWbGain.GainR)*Dgc/Scaler) & 0xFFFFFFFFULL);
                ImgWbGain.GainG = (UINT32) ((((UINT64) ImgWbGain.GainG)*Dgc/Scaler) & 0xFFFFFFFFULL);
                ImgWbGain.GainB = (UINT32) ((((UINT64) ImgWbGain.GainB)*Dgc/Scaler) & 0xFFFFFFFFULL);
            } else {
                /* support not yet */
                ImgWbGain.GainR = (UINT32) ((((UINT64) ImgWbGain.GainR)*pWgc[0]/Scaler) & 0xFFFFFFFFULL);
                ImgWbGain.GainG = (UINT32) ((((UINT64) ImgWbGain.GainG)*pWgc[1]/Scaler) & 0xFFFFFFFFULL);
                ImgWbGain.GainB = (UINT32) ((((UINT64) ImgWbGain.GainB)*pWgc[2]/Scaler) & 0xFFFFFFFFULL);

                //ImgWbGain.AeGain = (UINT32) (((UINT64) ImgWbGain.AeGain)*Dgc/Scaler);
                //ImgWbGain.GlobalDGain = (UINT32) (((UINT64) ImgWbGain.GlobalDGain)*Ggc/Scaler);
            }
        }

        FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId, PipeMode, &pImgMode);
        if (FuncRetCode == OK_UL) {
            MemInfo.Ctx.pImgMode = &ImgMode;
            FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
            if (FuncRetCode == OK) {
                if (PipeMode == AMBA_IK_PIPE_VIDEO) {
                    ImgMode.ContextId = AmbaImgSystem_ContextIdGet(ZoneIndex);
                }
                switch (WbId) {
                    case (UINT32) WB_ID_FE:
                        /* front end (before cfa) */
                        ImgFeWbGain.Bgain = ImgWbGain.GainB;
                        ImgFeWbGain.Ggain = ImgWbGain.GainG;
                        ImgFeWbGain.Rgain = ImgWbGain.GainR;
                        ImgFeWbGain.ShutterRatio = 1U;
                        FuncRetCode = AmbaIK_SetFeWbGain(&ImgMode, &ImgFeWbGain, 0U);
                        if (FuncRetCode != IK_OK) {
                            RetCode = NG_UL;
                        }
                        /* debug msg */
                        if (pVin[VinId].Debug.Bits.Timing > 0U) {
                            if (RetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Fe");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Fe!");
                            }
                        }
                        break;
                    case (UINT32) WB_ID_FE1:
                        /* front end (before cfa) */
                        ImgFeWbGain.Bgain = ImgWbGain.GainB;
                        ImgFeWbGain.Ggain = ImgWbGain.GainG;
                        ImgFeWbGain.Rgain = ImgWbGain.GainR;
                        ImgFeWbGain.ShutterRatio = 1U;
                        FuncRetCode = AmbaIK_SetFeWbGain(&ImgMode, &ImgFeWbGain, 1U);
                        if (FuncRetCode != IK_OK) {
                            RetCode = NG_UL;
                        }
                        /* debug msg */
                        if (pVin[VinId].Debug.Bits.Timing > 0U) {
                            if (RetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Fe1");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Fe1!");
                            }
                        }
                        break;
                    case (UINT32) WB_ID_FE2:
                        /* front end (before cfa) */
                        ImgFeWbGain.Bgain = ImgWbGain.GainB;
                        ImgFeWbGain.Ggain = ImgWbGain.GainG;
                        ImgFeWbGain.Rgain = ImgWbGain.GainR;
                        ImgFeWbGain.ShutterRatio = 1U;
                        FuncRetCode = AmbaIK_SetFeWbGain(&ImgMode, &ImgFeWbGain, 2U);
                        if (FuncRetCode != IK_OK) {
                            RetCode = NG_UL;
                        }
                        /* debug msg */
                        if (pVin[VinId].Debug.Bits.Timing > 0U) {
                            if (RetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Fe2");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Fe2!");
                            }
                        }
                        break;
                    case (UINT32) WB_ID_BE:
                        /* before ce (after cfa) */
                        FuncRetCode = AmbaIK_SetBeforeCeWbGain(&ImgMode, &ImgWbGain);
                        if (FuncRetCode != IK_OK) {
                            RetCode = NG_UL;
                        }
                        /* debug msg */
                        if (pVin[VinId].Debug.Bits.Timing > 0U) {
                            if (RetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Be");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_Be!");
                            }
                        }
                        break;
                    case (UINT32) WB_ID_BE_ALT:
                    default:
                        /* after ce (after cfa) */
                        FuncRetCode = AmbaIK_SetAfterCeWbGain(&ImgMode, &ImgWbGain);
                        if (FuncRetCode != IK_OK) {
                            RetCode = NG_UL;
                        }
                        /* debug msg */
                        if (pVin[VinId].Debug.Bits.Timing > 0U) {
                            if (RetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_BeA");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wb_Id_BeA!");
                            }
                        }
                        break;
                    /* dead code: wb id > 0, wb id < WB_ID_TOTAL */
                    //default:
                        //RetCode = NG_UL;
                        //break;
                }
            } else {
                /* memcpy fail */
                RetCode = NG_UL;
            }
        } else {
            /* img fail */
            RetCode = NG_UL;
        }
    } else {
        /* wb id null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image system ring pipe put
 *  @param[in] ImageChanId image channel id
 *  @param[in] pImgMode pointer to the image mode
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSystem_RingPipePut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IK_MODE_CFG_s *pImgMode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    UINT32 RawCapSeq;
    UINT32 FeChanNum;

    AMBA_IMG_SYSTEM_RING_PIPE_s RingPipe;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU) &&
        (AmbaImgSystem_Chan[VinId][ChainId].pInfo != NULL)) {
        /* raw cap seq */
        RawCapSeq = AmbaImgSystem_Vin[VinId].Counter.Raw + 4U;
        /* fe channel num */
        if (AmbaImgSystem_Chan[VinId][ChainId].pInfo->Pipe.Hdr.Bits.Enable == 1U) {
            FeChanNum = AmbaImgSystem_Chan[VinId][ChainId].pInfo->Pipe.Hdr.Bits.ExposureNum;
        } else {
            FeChanNum = 1U;
        }

        /* fe get */
        for (i = 0U; i < FeChanNum; i++) {
            FuncRetCode = AmbaIK_GetFeWbGain(pImgMode, &(RingPipe.Wb.Fe[i]), i);
            if (FuncRetCode != IK_OK) {
                /* */
            }
        }
        for (; i < 3U; i++) {
            RingPipe.Wb.Fe[i].Rgain = 4096U;
            RingPipe.Wb.Fe[i].Ggain = 4096U;
            RingPipe.Wb.Fe[i].Bgain = 4096U;
            RingPipe.Wb.Fe[i].ShutterRatio = 1U;
        }
        /* be get */
        FuncRetCode = AmbaIK_GetBeforeCeWbGain(pImgMode, &(RingPipe.Wb.Be));
        if (FuncRetCode != IK_OK) {
            /* */
        }
        /* be alt get */
        FuncRetCode = AmbaIK_GetAfterCeWbGain(pImgMode, &(RingPipe.Wb.BeAlt));
        if (FuncRetCode != IK_OK) {
            /* */
        }

        /* ring put */
        FuncRetCode = AmbaImgSystem_RingPut(ImageChanId, &RingPipe, RawCapSeq);
        if (FuncRetCode != OK_UL) {
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
 *  Amba image system aik execute
 *  @param[in] ImageChanId image channel id
 *  @param[in] Mode execute mode
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_AikExecute(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 FuncRetCode2;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    UINT32 ZoneIsoDisId = 0U;

    UINT32 VrId = 0U;
    UINT32 VrAltId = 0U;

    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    AMBA_IMG_SYSTEM_ZONE_s *pZone;
    const AMBA_IMG_SYSTEM_CONTEXT_s *pCtx;

    const AMBA_IMG_SYSTEM_VIN_s *pVin = AmbaImgSystem_Vin;

    AMBA_IMG_SYSTEM_AIK_CTX_s AikCtx;
    AMBA_IMG_SYSTEM_AIK_CTX_s *pActAikCtx;

    AMBA_IK_EXECUTE_CONTAINER_s IkExecResult = {0};
    AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s IsoCfgCtrl;

    UINT64 RawSeqNum;

    AMBA_IMG_SYSTEM_PF_MEM_s MemInfo;
    AMBA_IMG_SYSTEM_AIK_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL) &&
        (pFrwImageChannel[VinId][ChainId].Magic == 0xCafeU) &&
        (pFrwImageChannel[VinId][ChainId].pCtx != NULL)) {
        VrId = pFrwImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrId;
        VrAltId = pFrwImageChannel[VinId][ChainId].pCtx->VrMap.Id.Ctx.Bits.VrAltId;
        ZoneIsoDisId = pFrwImageChannel[VinId][ChainId].pCtx->Aik.Ctx.Bits.IsoDisId & ZoneId;
    }

    CmdMsg.Ctx.Data = Mode;

    if ((VrId > 0U/*vr*/) &&
        (((VrId & (((UINT32) 1U) << VinId)) == 0U) || ((VrAltId & (((UINT32) 1U) << ChainId)) == 0U))) {
        /* vr salve */
        RetCode = NG_UL;
    }

    if ((RetCode == OK_UL) &&
        (AmbaImgSystem_Chan[VinId] != NULL) &&
        (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        /* ctx check */
        if (AmbaImgSystem_Chan[VinId][ChainId].pCtx != NULL) {
            /* get ctx */
            pCtx = AmbaImgSystem_Chan[VinId][ChainId].pCtx;
            /* get image channel id */
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            /* zone valid check */
            for (i = 0U; pCtx[i].Magic == 0xCafeU; i++) {
                /* zone find */
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone check */
                    if (pCtx[i].pZone != NULL) {
                        /* zone get */
                        pZone = pCtx[i].pZone;
                        /* act aik ctx get */
                        pActAikCtx = &(pZone->ActAikCtx);
                        /* pipe mutex take */
                        FuncRetCode = AmbaKAL_MutexTake(&(pZone->Pipe[0].Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* image channel id assign */
                            ImageChanId2.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
                            FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId2, CmdMsg.Ctx.Bits.Mode ,&pImgMode);
                            if (FuncRetCode == OK_UL) {
                                /* img mode get */
                                MemInfo.Ctx.pImgMode = &ImgMode;
                                FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                                if (FuncRetCode == OK) {
                                    /* context id get */
                                    ImgMode.ContextId = AmbaImgSystem_ContextIdGet(i);
                                    /* ik execute check */
                                    if (CmdMsg.Ctx.Bits.Cmd == (UINT8) AIK_CMD_EXECUTE_WARP_ONLY) {
                                        /* ctx get */
                                        FuncRetCode = AmbaImgSystem_AikStatusGet(pZone, &AikCtx);
                                        if (FuncRetCode == OK_UL) {
                                            /* ik execute warp only */
                                            FuncRetCode = AmbaIK_ExecuteConfigUpdateWarp(&ImgMode, AikCtx.Id, &IkExecResult);
                                            if (FuncRetCode == IK_OK) {
                                                /* ctx ik id get */
                                                pActAikCtx->Id = IkExecResult.IkId;
                                                /* ctx ik iso cfg get */
                                                pActAikCtx->pIsoCfg = IkExecResult.pIsoCfg;
                                                /* debug message */
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Exe_W");
                                            } else {
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Exe_W!");
                                                RetCode = NG_UL;
                                            }
                                            /* debug message */
                                            if (pVin[VinId].Debug.Bits.Timing > 0U) {
                                                char str[11];
                                                str[0] = ' ';str[1] = ' ';
                                                var_utoa(AikCtx.Id, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                                if (FuncRetCode != IK_OK) {
                                                    str[0] = 'e';
                                                    var_utoa(FuncRetCode, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                                }
                                                str[0] = 'c';
                                                var_utoa(ImgMode.ContextId, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                                str[0] = ' ';
                                                var_utoa(IkExecResult.IkId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                                MemInfo.Ctx.pVoid = IkExecResult.pIsoCfg;
                                                var_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                            }
                                        } else {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Get!");
                                            RetCode = NG_UL;
                                        }
                                    } else {
                                        if (CmdMsg.Ctx.Bits.Cmd == (UINT8) AIK_CMD_EXECUTE_NO_WARP_ATTACH) {
                                            /* ik execute exculding warp */
                                            FuncRetCode = AmbaIK_ExecuteConfigNoWarp(&ImgMode, &IkExecResult);
                                            if (FuncRetCode == IK_OK) {
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Exe_B");
                                                /* ctx ik id assign */
                                                AikCtx.Id = IkExecResult.IkId;
                                                /* ctx ik iso cfg assign */
                                                AikCtx.pIsoCfg = IkExecResult.pIsoCfg;
                                                /* ctx put */
                                                FuncRetCode2 = AmbaImgSystem_AikStatusPut(pZone, &AikCtx);
                                                if (FuncRetCode2 != OK_UL) {
                                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Put!");
                                                }
                                            } else {
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Exe_B!");
                                            }
                                        } else {
                                            /* ik execute including warp */
                                            FuncRetCode = AmbaIK_ExecuteConfig(&ImgMode, &IkExecResult);
                                            if (FuncRetCode == IK_OK) {
                                                /* ctx ik id get */
                                                pActAikCtx->Id = IkExecResult.IkId;
                                                /* ctx ik iso cfg get */
                                                pActAikCtx->pIsoCfg = IkExecResult.pIsoCfg;
                                                /* debug message */
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Exe_A");
                                            } else {
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_Exe_A!");
                                            }
                                        }

                                        /* debug message */
                                        if (pVin[VinId].Debug.Bits.Timing > 0U) {
                                            char str[11];
                                            str[0] = ' ';str[1] = ' ';
                                            if (FuncRetCode != IK_OK) {
                                                str[0] = 'e';
                                                var_utoa(FuncRetCode, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                            }
                                            str[0] = 'c';
                                            var_utoa(ImgMode.ContextId, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                            str[0] = ' ';
                                            var_utoa(IkExecResult.IkId, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                            MemInfo.Ctx.pVoid = IkExecResult.pIsoCfg;
                                            var_utoa((UINT32) MemInfo.Ctx.Data, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                        }

                                        /* attach */
                                        if (FuncRetCode == IK_OK) {
                                            /* iso cfg update */
                                            IsoCfgCtrl.ViewZoneId = (UINT16) i;
                                            IsoCfgCtrl.CtxIndex = ImgMode.ContextId;
                                            IsoCfgCtrl.CfgIndex = IkExecResult.IkId;
                                            IsoCfgCtrl.HdrCfgIndex = IkExecResult.IkId;
                                            MemInfo.Ctx.pVoid = IkExecResult.pIsoCfg;
                                            IsoCfgCtrl.CfgAddress = MemInfo.Ctx.Data;
                                            if ((ZoneIsoDisId & (((UINT32) 1U) << i)) == 0U) {
                                                FuncRetCode = AmbaDSP_LiveviewUpdateIsoCfg(1U, &IsoCfgCtrl, &RawSeqNum);
                                                if (FuncRetCode == OK) {
                                                    /* debug message */
                                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Iso_Upd");
                                                    if (pVin[VinId].Debug.Bits.Timing > 0U) {
                                                        char str[11];
                                                        str[0] = ' ';str[1] = ' ';
                                                        var_utoa((UINT32) (RawSeqNum & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                                                        AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                                                    }
                                                } else {
                                                    /* iso cfg update fail */
                                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Iso_Upd!");
                                                    RetCode = NG_UL;
                                                }
                                            } else {
                                                /* iso cfg update disable */
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Iso_Dis");
                                            }
                                            /* ring put */
                                            FuncRetCode = AmbaImgSystem_RingPipePut(ImageChanId2, &ImgMode);
                                            if (FuncRetCode == OK_UL) {
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put");
                                            } else {
                                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put!");
                                            }
                                        } else {
                                            /* ik execute fail */
                                            RetCode = NG_UL;
                                        }
                                    }
                                } else {
                                    /* memcpy fail */
                                    RetCode = NG_UL;
                                }
                            } else {
                                /* img mode fail */
                                RetCode = NG_UL;
                            }
                            /* pipe mutex give */
                            FuncRetCode = AmbaKAL_MutexGive(&(pZone->Pipe[0].Mutex));
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
 *  Amba image system command message ahead get
 *  @param[in] VinId vin id
 *  @param[out] pCmdMsgAhead pointer to the command message ahead data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_CmdMsgAheadGet(UINT32 VinId, UINT32 *pCmdMsgAhead)
{
    UINT32 RetCode = OK_UL;

    *pCmdMsgAhead = 0U;

    if (VinId >= AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin id invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system command message decimation rate get
 *  @param[in] VinId vin id
 *  @param[out] pCmdMsgDec pointer to the command message decimation rate data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_CmdMsgDecRateGet(UINT32 VinId, UINT32 *pCmdMsgDec)
{
    UINT32 RetCode = OK_UL;

    *pCmdMsgDec = 1U;

    if (VinId >= AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin id invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system defer balck level get
 *  @param[in] VinId vin id
 *  @param[out] pEnable pointer to the enable data
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_DeferBlackLvlGet(UINT32 VinId, UINT32 *pEnable)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;
    AMBA_IK_MODE_CFG_s *pImgMode;
    AMBA_IK_MODE_CFG_s ImgMode;

    AMBA_IMG_SYSTEM_PF_MEM_s MemInfo;

    /* chan check */
    if ((pFrwImageChannel != NULL) &&
        (pFrwImageChannel[VinId] != NULL)) {
        /* chain find */
        for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
            /* ctx check */
            if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                /* get ctx */
                pCtx = pFrwImageChannel[VinId][i].pCtx;
                /* get image channel id */
                ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
                /* get img mode */
                FuncRetCode = AmbaImgSystem_ImageModeGet(ImageChanId, AMBA_IK_PIPE_VIDEO, &pImgMode);
                if (FuncRetCode == OK_UL) {
                    /* img mode cpy */
                    MemInfo.Ctx.pImgMode = &ImgMode;
                    FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pImgMode, sizeof(AMBA_IK_MODE_CFG_s));
                    if (FuncRetCode == OK) {
                        /*get context id */
                        for (j = 0U; j < AMBA_IMGFRW_NUM_FOV_CHANNEL; j++) {
                            if ((ImageChanId.Ctx.Bits.ZoneId & (((UINT32) 1U) << j)) > 0U) {
                                ImgMode.ContextId = AmbaImgSystem_ContextIdGet(j);
                                /* temporary used */
                                *pEnable = 1U;
                                break;
                            }
                        }
                        /* break if got one */
                        if (j < AMBA_IMGFRW_NUM_FOV_CHANNEL) {
                            break;
                        }
                    }
                }
            }
        }

        if (pFrwImageChannel[VinId][i].Magic != 0xCafeU) {
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
 *  Amba image system slow shutter change
 *  @param[in] ImageChanId image channel id
 *  @param[in] SsIndex slow shutter index
 *  @param[in] Mode change mode
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_SlowShutterChange(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 SsIndex, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 UpsamplingRate;
    //UINT32 DecRate;
    UINT32 CmdMsgDecRate = 0U;

    FuncRetCode = AmbaImgSystem_CmdMsgDecRateGet(ImageChanId.Ctx.Bits.VinId, &CmdMsgDecRate);
    if (FuncRetCode == OK_UL) {
        UpsamplingRate = ((UINT32) 1U) << (SsIndex & 0x1FU);
        //DecRate = CmdMsgDecRate/UpsamplingRate;

        //if (DecRate < 1U) {
            //DecRate = 1U;
        //}

        if (Mode == (UINT32) SS_SYNC_BOTH) {
            UINT32 i;
            UINT32 ZoneId;

            UINT16 FovNum = 0U;
            UINT16 FovIdx[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
            UINT32 FrameRatio[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

            /* zone id get */
            ZoneId = (UINT32) ImageChanId.Ctx.Bits.ZoneId;
            /* zone num & idx get */
            for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                if ((ZoneId >> i) == 0U) {
                    break;
                }
                if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                    FovIdx[FovNum] = (UINT16) i;
                    FrameRatio[FovNum] = UpsamplingRate;
                    FovNum++;
                }
            }
            /* raw upsampling rate ctrl */
            if (FovNum > 0U) {
                FuncRetCode = AmbaDSP_LiveviewSlowShutterCtrl(FovNum, FovIdx, FrameRatio);
                if (FuncRetCode == OK) {
                    /* support not yet (dec) */
                } else {
                    RetCode = NG_UL;
                }
            }
        } else {
            /* supported not yet (dec) */
        }
    } else {
        /* dec fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image system r2y delay
 *  @param[in] ImageChanId image channel id
 *  @param[in] R2yDelay ry2 delay
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSystem_R2yDelay(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 R2yDelay)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 PostZoneId;

    AMBA_DSP_LV_VZ_POSTPONE_CFG_s PostponeConfig = {0};

    PostZoneId = ImageChanId.Ctx.Bits.ZoneId;

    PostponeConfig.StageId = AMBA_DSP_VZ_POSTPONE_STAGE_PRE_R2Y;
    PostponeConfig.DelayTimeIn10xMs = R2yDelay * 10U;

    for (i = 0U; (i < AMBA_IMGFRW_NUM_FOV_CHANNEL) && ((PostZoneId >> i) > 0U); i++) {
        if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaDSP_LiveviewVZPostponeCfg((UINT16) i, &PostponeConfig);
            if (FuncRetCode != OK) {
                RetCode = NG_UL;
            }
        }
    }

    return RetCode;
}

