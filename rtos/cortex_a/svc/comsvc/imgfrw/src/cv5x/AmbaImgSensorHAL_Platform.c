/**
 *  @file AmbaImgSensorHAL_Platform.c
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
 *  @details Amba Image Sensor HAL for Common SOC (CV2FS)
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgChannel.h"
#include "AmbaImgChannel_Internal.h"
#include "AmbaImgSystem.h"
#include "AmbaImgSystem_Internal.h"
#include "AmbaImgSensorHAL.h"

#include "AmbaImgSensorHAL_Platform.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  @private
 *  Amba image sensor HAL timing configuration
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaImgSensorHAL_TimingConfig(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 i, k;

    UINT32 ChainId;
    UINT32 CaptureLine;

    UINT32 TotalZoneId;
    UINT32 TotalZoneCnt;

    UINT32 TotalSensorCnt;
#if 0
    UINT32 ZoneId;
    UINT32 ZoneCnt;
#endif
    UINT32 FrameTimeInNs;
    UINT32 FrameValidTimeInNs;
    UINT32 BlankingTimeInNs;

    UINT32 R2yBackgroup;
    //UINT32 Stitch;
    //UINT32 DeferredBlackLevel;
#if 0
    UINT32 LowDelay;
#endif
    UINT32 ForceAdvEof;

    UINT32 AikExecTimeInMs;
    UINT32 WriteLatencyInMs;

    UINT32 AdvDsp2NextSofInNs;
    UINT32 Sof2AdvDspInNs;
#if 0
    UINT32 AdvEof2NextSofInNs;
    UINT32 Sof2AdvEofInNs;
#endif
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_OP_CTRL_s *pCtrl;
    AMBA_IMG_SENSOR_HAL_OP_TIMING_s *pTiming;
    const AMBA_IMG_SENSOR_HAL_OP_USER_s *pUser;

    /* total zone id */
    TotalZoneId = 0U;
    for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
        TotalZoneId |= AmbaImgChannel_TotalZoneIdGet(i);
    }

    /* total zone count */
    TotalZoneCnt = 0U;
    for (i = 0U; (i < AMBA_IMGFRW_NUM_FOV_CHANNEL) && ((TotalZoneId >> i) > 0U); i++) {
        if ((TotalZoneId & (((UINT32) 1U) << i)) > 0U) {
            TotalZoneCnt++;
        }
    }
    if (TotalZoneCnt == 0U) {
        TotalZoneCnt++;
    }

    /* total sensor count */
    TotalSensorCnt = 0U;
    for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
        if ((pFrwImageChannel != NULL) && (pFrwImageChannel[i] != NULL)) {
            for (k = 0U; pFrwImageChannel[i][k].Magic == 0xCafeU; k++) {
                if (pFrwImageChannel[i][k].pCtx != NULL) {
                    TotalSensorCnt++;
                }
            }
        }
    }
    if (TotalSensorCnt == 0U) {
        TotalSensorCnt++;
    }

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if ((pFrwImageChannel != NULL) && (pFrwImageChannel[VinId] != NULL)) {
            for (k = 0U; pFrwImageChannel[VinId][k].Magic == 0xCafeU; k++) {
                if (pFrwImageChannel[VinId][k].pCtx != NULL) {
                    /* chain id get */
                    ChainId = (UINT32) pFrwImageChannel[VinId][k].pCtx->Id.Ctx.Bits.ChainId;
                    /* zone id get */
#if 0
                    ZoneId = (UINT32) pFrwImageChannel[VinId][k].pCtx->Id.Ctx.Bits.ZoneId;
#endif
                    /* info get */
                    pInfo = AmbaImgSensorHAL_Chan[VinId][ChainId].pCtx->pInfo;
                    if (pInfo != NULL) {
                        /* ctrl get */
                        pCtrl = &(pInfo->Op.Ctrl);
                        /* timing get */
                        pTiming = &(pInfo->Op.Timing);
                        /* user get */
                        pUser = &(pInfo->Op.User);

                        /* capture line get */
                        CaptureLine = AmbaImgSystem_VinCaptureLineGet(VinId, ChainId);
                        /* frame time */
                        FrameTimeInNs = pInfo->Frame.Timetick * 1000U;
                        /* read out time */
                        FrameValidTimeInNs = (pInfo->Frame.Timetick * CaptureLine * 1000U) / pInfo->Frame.TotalLine;
                        /* vertical blanking time */
                        BlankingTimeInNs = (pInfo->Frame.Timetick * 1000U) - FrameValidTimeInNs;

                        /* zone count */
#if 0
                        ZoneCnt = 0U;
                        for (i = 0U; (i < AMBA_IMGFRW_NUM_FOV_CHANNEL) && ((ZoneId >> i) > 0U); i++) {
                            if ((ZoneId & (((UINT32) 1U) << i)) > 0U) {
                                ZoneCnt++;
                            }
                        }
                        if (ZoneCnt == 0U) {
                            ZoneCnt++;
                        }
#endif
                        /* aik exec time */
                        AikExecTimeInMs = pUser->AikExecTime * TotalZoneCnt; /* worst case: multi-fov aik execute in same cpu */

                        /* mof enable */
                        pTiming->Mof.Bits.Enable = 1U;
                        /* mof timing */
                        if (FrameValidTimeInNs > (pUser->EofMoveMaxTime*1000U)) {
                            pTiming->Mof.Bits.Timetick = (UINT16) ((((FrameValidTimeInNs - (pUser->EofMoveMaxTime*1000U)) + 1000U) / 2000U) & 0xFFFFU);
                        } else {
                            pTiming->Mof.Bits.Timetick = (UINT16) (((FrameValidTimeInNs + 1000U) / 2000U) & 0xFFFFU);
                        }

                        /* post sof timing */
                        if (pUser->ForcePostSof == 0U) {
                            /* post sof disable */
                            pTiming->PostSof.Bits.Enable = 0U;
                            /* sof itself */
                            pTiming->PostSof.Bits.Timetick = 0U;
                        } else {
                            /* post sof enable */
                            pTiming->PostSof.Bits.Enable = 1U;
                            /* post sof */
                            if (pUser->ForcePostSof < pTiming->Mof.Bits.Timetick) {
                                pTiming->PostSof.Bits.Timetick = (UINT16) pUser->ForcePostSof;
                            } else {
                                pTiming->PostSof.Bits.Timetick = pTiming->Mof.Bits.Timetick;
                            }
                        }

                        /* write latency */
                        if (pUser->WriteLatency > 0U) {
                            /* write latency check */
                            if (pUser->WriteLatency > IMAGE_SENSOR_WRITE_LATENCY) {
                                WriteLatencyInMs = pUser->WriteLatency;
                            } else {
                                WriteLatencyInMs = IMAGE_SENSOR_WRITE_LATENCY;
                            }
                            /* force adv eof */
                            ForceAdvEof = 1U;
                        } else {
                            /* user force check */
                            if (pUser->ForceAdvEof > 0U) {
                                /* user force */
                                WriteLatencyInMs = IMAGE_SENSOR_WRITE_LATENCY;
                                /* force adv eof */
                                ForceAdvEof = 1U;
                            } else {
                                /* blanking check */
                                if (BlankingTimeInNs > ((IMAGE_SENSOR_WRITE_LATENCY*1000U)*TotalSensorCnt)) {
                                    /* engouh */
                                    WriteLatencyInMs = 0U;
                                    /* adv eof disabled */
                                    ForceAdvEof = 0U;
                                } else {
                                    /* too small */
                                    WriteLatencyInMs = IMAGE_SENSOR_WRITE_LATENCY;
                                    /* force adv eof */
                                    ForceAdvEof = 1U;
                                }
                            }
                        }
                        WriteLatencyInMs = WriteLatencyInMs * TotalSensorCnt; /* worst case: multi-sensor write in same cpu */

                        /* adv dsp 2 next sof */
                        AdvDsp2NextSofInNs = ((WriteLatencyInMs + AikExecTimeInMs + pUser->EofMoveMaxTime) * 1000U) + BlankingTimeInNs;
                        /* sof 2 timing */
                        if (FrameTimeInNs > AdvDsp2NextSofInNs) {
                            Sof2AdvDspInNs = FrameTimeInNs - AdvDsp2NextSofInNs;
                        } else {
                            Sof2AdvDspInNs = 0U;
                        }
#if 0
                        if (ForceAdvEof > 0U) {
                            /* adv eof 2 next sof */
                            AdvEof2NextSofInNs = ((WriteLatencyInMs + pUser->EofMoveMaxTime) * 1000U) + BlankingTimeInNs;
                            /* sof 2 timing */
                            if (FrameTimeInNs > AdvEof2NextSofInNs) {
                                Sof2AdvEofInNs = FrameTimeInNs - AdvEof2NextSofInNs;
                            } else {
                                Sof2AdvEofInNs = 0U;
                            }
                        }
#endif
                        /* adv dsp/aik enable */
                        pTiming->AdvDsp.Bits.Enable = 1U;
                        /* adv dsp/aik timing */
                        pTiming->AdvDsp.Bits.Timetick = (UINT16) ((Sof2AdvDspInNs / 1000U) & 0xFFFFU);
                        /* adv dsp/aik check */
                        if (pTiming->AdvDsp.Bits.Timetick < pTiming->PostSof.Bits.Timetick) {
                            pTiming->AdvDsp.Bits.Timetick = pTiming->PostSof.Bits.Timetick;
                        }

                        /* adv eof */
                        if (ForceAdvEof > 0U) {
                            /* adv eof enable */
                            pTiming->AdvEof.Bits.Enable = 1U;
                            /* adv eof timing before aik execute if enable (for some reason) */
                            pTiming->AdvEof.Bits.Timetick = (UINT16) ((Sof2AdvDspInNs / 1000U) & 0xFFFFU);
                            /* adv eof check */
                            if (pTiming->AdvEof.Bits.Timetick < pTiming->PostSof.Bits.Timetick) {
                                pTiming->AdvEof.Bits.Timetick = pTiming->PostSof.Bits.Timetick;
                            }
                        } else {
                            /* adv eof disable */
                            pTiming->AdvEof.Bits.Enable = 0U;
                        }

                        /* aik execute enable */
                        pTiming->AikExecute.Bits.Enable = 1U;
                        /* aik execute timing */
                        pTiming->AikExecute.Bits.Timetick = (UINT16) ((Sof2AdvDspInNs / 1000U) & 0xFFFFU);
                        /* aik execute check */
                        if (pTiming->AikExecute.Bits.Timetick < pTiming->PostSof.Bits.Timetick) {
                            pTiming->AikExecute.Bits.Timetick = pTiming->PostSof.Bits.Timetick;
                        }

                        /* r2y background */
                        R2yBackgroup = pInfo->pAux->Ctx.Bits.R2yBackground;
                        /* stitch */
                        //Stitch = pInfo->pAux->Ctx.Bits.Stitch;
                        /* deferred black level */
                        //DeferredBlackLevel = pInfo->pAux->Ctx.Bits.DeferredBlackLevel;
                        /* low delay */
                        //LowDelay = pInfo->pAux->Ctx.Bits.LowDelay;

                        /* r2y background */
                        pCtrl->R2yBackground = R2yBackgroup;

                        /* adv ssi cmd */
                        pCtrl->AdvSsiCmd = 2U;

                        /* adv svr upd */
                        if (pUser->SvrDelay > 2U) {
                            pCtrl->AdvSvrUpd = 2U;
                        } else {
                            pCtrl->AdvSvrUpd = 0U;
                        }
#if 0
                        /* adv rgb/aik */
                        if (R2yBackgroup > 0U) {
                            if (LowDelay == 0U) {
                                pCtrl->AdvRgbCmd = 1U;
                                pCtrl->AdvAikCmd = 1U;
                            } else {
                                pCtrl->AdvRgbCmd = 1U;
                                pCtrl->AdvAikCmd = 1U;
                            }
                        } else {
                            pCtrl->AdvRgbCmd = 1U;
                            pCtrl->AdvAikCmd = 1U;
                        }

                        /* pre aik cmd */
                        if (pCtrl->AdvAikCmd == 0U) {
                            pCtrl->PreAikCmd = 1U;
                        } else {
                            pCtrl->PreAikCmd = 0U;
                        }
#else
                        /* adv rgb/aik */
                        pCtrl->AdvRgbCmd = 1U;
                        pCtrl->AdvAikCmd = 1U;

                        /* pre aik cmd */
                        pCtrl->PreAikCmd = 0U;
#endif
                        /*  rgb/aik cmd if shr delay > 2 */
                        if (pUser->ShrDelay > 2U) {
                            pCtrl->AdvRgbCmd = 0U;
                            pCtrl->AdvAikCmd = 0U;
                        }
                    } else {
                        /* info null */
                        RetCode = NG_UL;
                    }
                }
            }
        } else {
            /* vin chan null */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}
