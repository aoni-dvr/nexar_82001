/**
 *  @file AmbaImgSensorSync.c
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
 *  @details Amba Image Sensor Sync
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN_Def.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgMem.h"
#include "AmbaImgVar.h"
#include "AmbaImgChannel.h"
#include "AmbaImgChannel_Internal.h"
//#include "AmbaImgEvent.h"
#include "AmbaImgEvent_Internal.h"
#include "AmbaImgMessage.h"
#include "AmbaImgMessage_Internal.h"
#include "AmbaImgSystem.h"
#include "AmbaImgSystem_Internal.h"
#include "AmbaImgSystem_Platform.h"
#include "AmbaImgSensorHAL_Reg.h"
#include "AmbaImgSensorHAL.h"
#include "AmbaImgSensorHAL_Internal.h"
#include "AmbaImgSensorHAL_Platform.h"
#include "AmbaImgSensorSync.h"
#include "AmbaImgSensorSync_Internal.h"
#include "AmbaImgSensorSync_Platform.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_SENSOR_SYNC_MEM_INFO_u_*/ {
    AMBA_IMG_MEM_ADDR              Data;
    AMBA_IMG_CHANNEL_s             *pImageChannel;
    AMBA_IMG_SYSTEM_PAIK_INFO_s    *pPostAikInfo;
    void                           *pVoid;
} SENSOR_SYNC_MEM_INFO_u;

typedef struct /*_SENSOR_SYNC_MEM_INFO_s_*/ {
    SENSOR_SYNC_MEM_INFO_u  Ctx;
} SENSOR_SYNC_MEM_INFO_s;

/**
 *  Amba image sensor sync init
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j, k;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;
    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;

    /* vin check */
    for (i = 0U; i < AMBA_IMGFRW_NUM_VIN_CHANNEL; i++) {
        /* chan check */
        if (AmbaImgSensorSync_Chan[i] == NULL) {
            continue;
        }

        /* get vin & info */
        pVin = &(AmbaImgSensorSync_Vin[i]);

        /* sem timeout timer */
        FuncRetCode = AmbaKAL_SemaphoreCreate(&(pVin->Sem.Timer), NULL, 0U);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* sof mutex fail */
            RetCode = NG_UL;
        }

        /* sof mutex */
        FuncRetCode = AmbaKAL_MutexCreate(&(pVin->Mutex.Sof), NULL);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* sof mutex fail */
            RetCode = NG_UL;
        }

        /* eof mutex */
        FuncRetCode = AmbaKAL_MutexCreate(&(pVin->Mutex.Eof), NULL);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* eof mutex fail */
            RetCode = NG_UL;
        }

        /* wait mutex */
        for (j = 0U; j < (UINT32) SYNC_WAIT_TOTAL; j++) {
            FuncRetCode = AmbaKAL_MutexCreate(&(pVin->Wait[j].Mutex), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* mutex fail */
                RetCode = NG_UL;
            }
            /* list reset */
            pVin->Wait[j].pList = NULL;
        }

        /* vin op */
        pVin->Op.Data = 0U;

        /* timing mutex */
        FuncRetCode = AmbaKAL_MutexCreate(&(pVin->Timing.Mutex), NULL);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* mutex fail */
            RetCode = NG_UL;
        }

        /* timing cb reset */
        pVin->Timing.pFunc = NULL;

        /* timing port list reset */
        pVin->Timing.pSofList = NULL;
        pVin->Timing.pEofList = NULL;

        /* chain check */
        for (j = 0U; AmbaImgSensorSync_Chan[i][j].Magic == 0xCafeU; j++) {
            /* ctx check */
            if (AmbaImgSensorSync_Chan[i][j].pCtx == NULL) {
                continue;
            }
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[i][j].pCtx;

            /* fifo mutex */
            FuncRetCode = AmbaKAL_MutexCreate(&(pCtx->Fifo.Mutex), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* mutex fail */
                RetCode = NG_UL;
            }

            /* fifo sem */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pCtx->Fifo.Rdy), NULL, 1U);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* sem fail */
                RetCode = NG_UL;
            }

            /* fifo ack svr sem */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pCtx->Fifo.Ack.SsiSvr), NULL, 0U);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* sem fail */
                RetCode = NG_UL;
            }

            /* fifo ack shr sem */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pCtx->Fifo.Ack.Shr), NULL, 0U);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* sem fail */
                RetCode = NG_UL;
            }

            /* fifo ack agc sem */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pCtx->Fifo.Ack.Agc), NULL, 0U);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* sem fail */
                RetCode = NG_UL;
            }

            /* fifo ack dgc sem */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pCtx->Fifo.Ack.Dgc), NULL, 0U);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* sem fail */
                RetCode = NG_UL;
            }

            /* snap mutex */
            FuncRetCode = AmbaKAL_MutexCreate(&(pCtx->Snap.Mutex), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* mutex fail */
                RetCode = NG_UL;
            }

            /* snap sem */
            FuncRetCode = AmbaKAL_SemaphoreCreate(&(pCtx->Snap.Req), NULL, 0U);
            if (FuncRetCode != KAL_ERR_NONE) {
                /*sem fail */
                RetCode = NG_UL;
            }

            /* user mutex */
            FuncRetCode = AmbaKAL_MutexCreate(&(pCtx->User.Mutex), NULL);
            if (FuncRetCode != KAL_ERR_NONE) {
                /* mutex fail */
                RetCode = NG_UL;
            }

            /* snap op reset */
            pCtx->Snap.Op.Data = 0U;
            /* snap list reset */
            pCtx->Snap.pList = NULL;

            /* hdr preload reset */
            for (k = 0U; k < AMBA_IMG_SENSOR_HAL_HDR_SIZE; k++) {
                pCtx->HdrPreload.Agc[k] = 0ULL;
                pCtx->HdrPreload.Dgc[k] = 0ULL;
                pCtx->HdrPreload.Wgc[k] = 0ULL;
                pCtx->HdrPreload.Shr[k] = 0ULL;
                pCtx->HdrPreload.DDgc[k] = 0ULL;
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image sensor sync debug
 *  @param[in] VinId vin id
 *  @param[in] Flag debug flag (bits)
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Debug(UINT32 VinId, UINT32 Flag)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AmbaImgSensorSync_Vin[VinId].Debug.Data = Flag;
    } else {
        /* id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync time wait to
 *  @param[in] VinId
 *  @param[in] Timetick system timetick to wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_TimeWaitTo(UINT32 VinId, UINT32 Timetick)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick_;
    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* time tick get */
        FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick_);
        if (FuncRetCode == KAL_ERR_NONE) {
            if ((Timetick > Timetick_) &&
                (pVin->Op.Bits.Reset == 0U)) {
                /* timeout timer */
                FuncRetCode = AmbaKAL_SemaphoreTake(&(pVin->Sem.Timer), Timetick - Timetick_);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* sem get */
                    RetCode = NG_UL;
                }
            }
        } else {
            /* timetick fail */
            RetCode = NG_UL;
        }
    } else {
        /* vin exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync timing port add
 *  @param[in] ImageChanId image channel id
 *  @param[in] pPort pointer to the timing port
 *  @param[in] Mode sync mode (SYNC_SOF/SYNC_EOF)
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_TimingPortAdd(UINT32 VinId, AMBA_IMG_CHANNEL_TIMING_PORT_s *pPort, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;
    AMBA_IMG_CHANNEL_TIMING_PORT_s *pList;

    pVin = &(AmbaImgSensorSync_Vin[VinId]);

    /* mutex get */
    FuncRetCode = AmbaKAL_MutexTake(&(pVin->Timing.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* list get */
        switch (Mode) {
            case SYNC_SOF:
                pList = pVin->Timing.pSofList;
                break;
            case SYNC_EOF:
                pList = pVin->Timing.pEofList;
                break;
            default:
                pList = NULL;
                break;
        }
        /* root null? */
        if (pList == NULL) {
            /* port link reset */
            pPort->Link.Up = NULL;
            pPort->Link.Down = NULL;
            /* root */
            switch (Mode) {
                case SYNC_SOF:
                    //var_print("root (null) add", NULL, NULL);
                    //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                    pVin->Timing.pSofList = pPort;
                    break;
                case SYNC_EOF:
                    pVin->Timing.pEofList = pPort;
                    break;
                default:
                    /* */
                    break;
            }
        } else {
            /* list find */
            while (pList != NULL) {
                if (pList == pPort) {
                    //var_print("repeat?", NULL, NULL);
                    //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                    /* already exist? */
                    RetCode = NG_UL;
                    /* end */
                    break;
                } else {
                    /* timetick check */
                    if (pList->Op.Bits.Timetick <= pPort->Op.Bits.Timetick) {
                        /* same? */
                        if (pList->Op.Bits.Timetick == pPort->Op.Bits.Timetick) {
                            /* id priority? */
                            if (pList->Op.Bits.Id > pPort->Op.Bits.Id) {
                                /* link add */
                                if (pList->Link.Up != NULL) {
                                    //var_print("insert add", NULL, NULL);
                                    //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                                    /* insert */
                                    pList->Link.Up->Link.Down = pPort;
                                } else {
                                    /* root */
                                    switch (Mode) {
                                        case SYNC_SOF:
                                            //var_print("root add", NULL, NULL);
                                            //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                                            pVin->Timing.pSofList = pPort;
                                            break;
                                        case SYNC_EOF:
                                        default:
                                            pVin->Timing.pEofList = pPort;
                                            break;
                                        /* dead code: pList != NULL */
                                        //default:
                                            /* */
                                            //break;
                                    }
                                }
                                pPort->Link.Up = pList->Link.Up;
                                pPort->Link.Down = pList;
                                pList->Link.Up = pPort;
                                /* end */
                                pList = NULL;
                            } else {
                                /* tail? */
                                if (pList->Link.Down == NULL) {
                                    //var_print("tail add", NULL, NULL);
                                    //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                                    /* tail add */
                                    pList->Link.Down = pPort;
                                    pPort->Link.Up = pList;
                                    pPort->Link.Down = NULL;
                                    /* end */
                                    pList = NULL;
                                } else {
                                    //var_print("next", NULL, NULL);
                                    /* next */
                                    pList = pList->Link.Down;
                                }
                            }
                        } else {
                            /* tail? */
                            if (pList->Link.Down == NULL) {
                                //var_print("tail add", NULL, NULL);
                                //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                                /* tail add */
                                pList->Link.Down = pPort;
                                pPort->Link.Up = pList;
                                pPort->Link.Down = NULL;
                                /* end */
                                pList = NULL;
                            } else {
                                //var_print("next", NULL, NULL);
                                /* next */
                                pList = pList->Link.Down;
                            }
                        }
                    } else {
                        /* link add */
                        if (pList->Link.Up != NULL) {
                            //var_print("insert add", NULL, NULL);
                            //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                            /* insert */
                            pList->Link.Up->Link.Down = pPort;
                        } else {
                            //var_print("root add", NULL, NULL);
                            //var_print_ex(pPort->pName, (UINT32) pPort->Op.Bits.Timetick, 10U);
                            /* root */
                            switch (Mode) {
                                case SYNC_SOF:
                                    pVin->Timing.pSofList = pPort;
                                    break;
                                case SYNC_EOF:
                                default:
                                    pVin->Timing.pEofList = pPort;
                                    break;
                                /* dead code: pList != NULL */
                                //default:
                                    /* */
                                    //break;
                            }
                        }
                        pPort->Link.Up = pList->Link.Up;
                        pPort->Link.Down = pList;
                        pList->Link.Up = pPort;
                        /* end */
                        pList = NULL;
                    }
                }
            }
        }
        /* mutex give */
        FuncRetCode = AmbaKAL_MutexGive(&(pVin->Timing.Mutex));
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
    } else {
        /* mutex fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba image sensor sync timing check
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_TimingCheck(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i, j;

    SENSOR_SYNC_MEM_INFO_s MemInfo;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;
    AMBA_IMG_CHANNEL_TIMING_s *pTiming;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_CHANNEL_TIMING_PORT_s *pPort;

    UINT32 MinMofTimetick = 0xFFFFU;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* sync vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(pVin->Timing.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* list clear */
            pVin->Timing.pSofList = NULL;
            pVin->Timing.pEofList = NULL;
            /* timing port (chan) hook */
            if ((pFrwImageChannel != NULL) &&
                (pFrwImageChannel[VinId] != NULL)) {
                for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                    /* image channel get */
                    MemInfo.Ctx.pImageChannel = &(pFrwImageChannel[VinId][i]);
                    /* info (chan) get */
                    FuncRetCode = AmbaImgSensorHAL_InfoGet(MemInfo.Ctx.pImageChannel->pCtx->Id, &pInfo);
                    if (FuncRetCode == OK_UL) {
                        /* timing check */
                        if (MemInfo.Ctx.pImageChannel->pTiming != NULL) {
                            /* timing get */
                            pTiming = MemInfo.Ctx.pImageChannel->pTiming;
                            /* sof timing */
                            for (j = 0U; j < (UINT32) CHANNEL_TIMING_ID_TOTAL; j++) {
                                /* time port get */
                                pPort = &(pTiming->Port[j]);
                                /* info put */
                                switch (j) {
                                    case (UINT32) CHANNEL_TIMING_ID_POST_SOF:
                                        /* post sof */
                                        pPort->pName = "Post_Sof";
                                        pPort->Op.Data = pInfo->Op.Timing.PostSof.Data;
                                        pPort->Op.Bits.Id = (UINT8) CHANNEL_TIMING_ID_POST_SOF;
                                        pPort->Data = (UINT64) MemInfo.Ctx.Data;
                                        pPort->SkipFrame = 0U;
                                        pPort->pFunc = SensorSync_PostSof;
                                        pPort->Link.Up = NULL;
                                        pPort->Link.Down = NULL;
                                        break;
                                    case (UINT32) CHANNEL_TIMING_ID_ADV_DSP:
                                        /* adv dsp */
                                        pPort->pName = "Adv_Dsp";
                                        pPort->Op.Data = pInfo->Op.Timing.AdvDsp.Data;
                                        pPort->Op.Bits.Id = (UINT8) CHANNEL_TIMING_ID_ADV_DSP;
                                        pPort->Data = (UINT64) MemInfo.Ctx.Data;
                                        pPort->SkipFrame = 0U;
                                        pPort->pFunc = SensorSync_AdvDsp;
                                        pPort->Link.Up = NULL;
                                        pPort->Link.Down = NULL;
                                        break;
                                    case (UINT32) CHANNEL_TIMING_ID_ADV_EOF:
                                        /* adv eof */
                                        pPort->pName = "Adv_Eof";
                                        pPort->Op.Data = pInfo->Op.Timing.AdvEof.Data;
                                        pPort->Op.Bits.Id = (UINT8) CHANNEL_TIMING_ID_ADV_EOF;
                                        pPort->Data = (UINT64) MemInfo.Ctx.Data;
                                        pPort->SkipFrame = 0U;
                                        pPort->pFunc = SensorSync_AdvEof;
                                        pPort->Link.Up = NULL;
                                        pPort->Link.Down = NULL;
                                        break;
                                    case (UINT32) CHANNEL_TIMING_ID_AIK_EXEC:
                                    default:
                                        /* aik execute */
                                        pPort->pName = "Aik_Exec";
                                        pPort->Op.Data = pInfo->Op.Timing.AikExecute.Data;
                                        pPort->Op.Bits.Id = (UINT8) CHANNEL_TIMING_ID_AIK_EXEC;
                                        pPort->Data = (UINT64) MemInfo.Ctx.Data;
                                        pPort->SkipFrame = IMAGE_SENSOR_SYNC_AIK_EXEC_SKIP;
                                        pPort->pFunc = SensorSync_AikExecute;
                                        pPort->Link.Up = NULL;
                                        pPort->Link.Down = NULL;
                                        break;
                                    /* dead code: j >= 0, j < CHANNEL_TIMING_ID_TOTAL */
                                    //default:
                                        /* */
                                        //break;
                                }
                                /* video ? */
                                if (pInfo->pAux->Ctx.Bits.OpMode < 0xFU) {
                                    /* time port add */
                                    FuncRetCode = SensorSync_TimingPortAdd(VinId, pPort, SYNC_SOF);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                } else {
                                    /* still */
                                    if (j != (UINT32) CHANNEL_TIMING_ID_AIK_EXEC) {
                                        /* time port add */
                                        FuncRetCode = SensorSync_TimingPortAdd(VinId, pPort, SYNC_SOF);
                                        if (FuncRetCode != OK_UL) {
                                            RetCode = NG_UL;
                                        }
                                    }
                                }
                            }
                        }
                        /* mof */
                        if (MinMofTimetick > (UINT32) pInfo->Op.Timing.Mof.Bits.Timetick) {
                            MinMofTimetick = (UINT32) pInfo->Op.Timing.Mof.Bits.Timetick;
                        }
                    }
                }
                /* mof port get */
                pPort = &(AmbaImgChannel_MofTimingPort[VinId]);
                /* mof */
                pPort->pName = "Mof";
                pPort->Op.Data = 0U;
                pPort->Op.Bits.Enable = 1U;
                pPort->Op.Bits.Timetick = (UINT16) (MinMofTimetick & 0xFFFFU);
                pPort->Op.Bits.Id = (UINT8) CHANNEL_TIMING_ID_MOF;
                pPort->Data = VinId;
                pPort->SkipFrame = 0U;
                pPort->pFunc = SensorSync_Mof;
                pPort->Link.Up = NULL;
                pPort->Link.Down = NULL;
                /* time port add */
                FuncRetCode = SensorSync_TimingPortAdd(VinId, pPort, SYNC_SOF);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
            }
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(pVin->Timing.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync reset
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Reset(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;
    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    ImageChanId.Ctx.Data = 0ULL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);

        /* reset */
        AmbaImgSensorHAL_TimingMarkPut(VinId, "Sync_Rsts");
        pVin->Op.Bits.Reset = 1U;
        for (i = 0U; i < 2U; i++) {
            /* eof take */
            FuncRetCode = AmbaKAL_MutexTake(&(pVin->Mutex.Eof), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* eof give */
                FuncRetCode = AmbaKAL_MutexGive(&(pVin->Mutex.Eof));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
            /* sof take */
            FuncRetCode = AmbaKAL_MutexTake(&(pVin->Mutex.Sof), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* sof give */
                FuncRetCode = AmbaKAL_MutexGive(&(pVin->Mutex.Sof));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
            if (i < 1U) {
                FuncRetCode = AmbaKAL_TaskSleep(1U);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
        pVin->Op.Bits.Reset = 0U;
        AmbaImgSensorHAL_TimingMarkPut(VinId, "Sync_Rste");

        /* op reset */
        pVin->Op.Bits.AikExec = 1U;

        /* counter clr */
        pVin->Counter.Sof = 0U;
        pVin->Counter.Eof = 0U;
        pVin->Counter.Raw = 0U;
        pVin->Counter.RawInMof = 0U;
        pVin->Counter.RawInSof = 0U;

        /* timetick clr */
        pVin->Timetick.Sof = 0U;
        pVin->Timetick.LastSof = 0U;
        pVin->Timetick.Eof = 0U;
        pVin->Timetick.LastEof = 0U;

        /* vin event clr */
        FuncRetCode = AmbaImgEvent_VinClr(VinId, 0xFFFFFFFFFFFFFFFFULL);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }

        /* timing cb reset */
        pVin->Timing.pFunc = NULL;

        /* timing list reset */
        pVin->Timing.pSofList = NULL;
        pVin->Timing.pEofList = NULL;

        /* image channel id */
        ImageChanId.Ctx.Bits.VinId = (UINT8) VinId;

        for (i = 0U; AmbaImgSensorSync_Chan[VinId][i].Magic == 0xCafeU; i++) {
            /* ctx reset */
            if (AmbaImgSensorSync_Chan[VinId][i].pCtx != NULL) {
                /* ctx get */
                pCtx = AmbaImgSensorSync_Chan[VinId][i].pCtx;
                /* sem clear */
                while (AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Rdy), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                while (AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.SsiSvr), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                while (AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.Shr), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                while (AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.Agc), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                while (AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.Dgc), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                while (AmbaKAL_SemaphoreTake(&(pCtx->Snap.Req), AMBA_KAL_NO_WAIT) == KAL_ERR_NONE) {};
                /* sem reset */
                FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Rdy));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* sem give fail */
                    RetCode = NG_UL;
                }
                /* op reset */
                pCtx->Snap.Op.Data = 0U;
                /* list reset */
                pCtx->Snap.pList = NULL;

                /* image channel id */
                ImageChanId.Ctx.Bits.ChainId = (UINT8) i;
                /* event clr */
                FuncRetCode = AmbaImgEvent_ChanClr(ImageChanId, 0xFFFFFFFFFFFFFFFFULL);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
            }
        }

        if (RetCode == OK_UL) {
            FuncRetCode = AmbaImgSensorSync_TimingCheck(VinId);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync stop
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Stop(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* diable */
        pVin->Op.Bits.AikExec = 0U;
    } else {
        /* vin exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync raw capture sequence num put
 *  @param[in] VinId vin id
 *  @param[in] RawCapSeq raw capture sequence number
 *  @return error code
 */
UINT32 AmbaImgSensorSync_RawCapSeqPut(UINT32 VinId, UINT32 RawCapSeq)
{
    UINT32 RetCode = OK_UL;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        AmbaImgSensorSync_Vin[VinId].Counter.Raw = RawCapSeq;
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync user buffer config
 *  @param[in] ImageChanId image channel id
 *  @param[in] Size buffer size
 *  @param[in] Count buffer count
 *  @param[in] pMem pointer to the buffer memory
 *  @return error code
 */
UINT32 AmbaImgSensorSync_UserBuffer(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pMem)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pCtx->User.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* buffer register */
                FuncRetCode = AmbaImgSensorHAL_UserBufferCfg(ImageChanId, Size, Count, pMem);
                if (FuncRetCode != OK_UL) {
                    /* register fail */
                    RetCode = NG_UL;
                }
                /* mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(pCtx->User.Mutex));
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
        /* vin null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync user buffer get
 *  @param[in] ImageChanId image channel id
 *  @return pointer to the buffer memory
 */
void *AmbaImgSensorSync_UserBufferGet(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    void *pMem = NULL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pCtx->User.Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* buffer get */
                pMem = AmbaImgSensorHAL_UserBufferGet(ImageChanId);
                /* mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(pCtx->User.Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        }
    }

    return pMem;
}

/**
 *  sync sof
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_Sof(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 SnapListPut;

    UINT32 VinId;
    UINT32 ChainId;

    UINT64 ActualFlag;

    AMBA_IMG_SENSOR_HAL_SHR_CTRL_s Shr2;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s *pPort;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;

            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* svr chg in sof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_SVR_CHG, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_SVR_CHG) > 0ULL) {
                    AMBA_IMG_SENSOR_HAL_DATA_REG_s ActData;
                    const AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s *pCounter;
                    pCounter = &(AmbaImgSensorSync_Vin[VinId].Counter);
                    FuncRetCode = AmbaImgSensorHAL_RingGet(ImageChanId, &ActData, pCounter->RawInSof + 3U);
                    if (FuncRetCode == OK_UL) {
                        /* svr msg (smc on) put */
                        FuncRetCode = AmbaImgMessage_Put(ImageChanId, (UINT64) AMBA_IMG_MESSAGE_ID_SVR_CHG, ActData.User);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                }
                /* svr upd in sof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_SVR_UPD, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_SVR_UPD) > 0ULL) {
                    /* svr upd */
                    if (pInfo->Op.User.ShrDelay > 2U) {
                        /* svr chg event put */
                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_SVR_CHG);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    } else {
                        AMBA_IMG_SENSOR_HAL_DATA_REG_s ActData;
                        FuncRetCode = AmbaImgSensorHAL_ActDataGet(ImageChanId, &ActData);
                        if (FuncRetCode == OK_UL) {
                            /* svr msg (smc on) put */
                            FuncRetCode = AmbaImgMessage_Put(ImageChanId, (UINT64) AMBA_IMG_MESSAGE_ID_SVR_CHG, ActData.User);
                            if (FuncRetCode != OK_UL) {
                                /* */
                            }
                        }
                    }
                }
            }
            /* svr upd in sof */

            /* snap req check in sof */
            FuncRetCode = AmbaKAL_SemaphoreTake(&(pCtx->Snap.Req), AMBA_KAL_NO_WAIT);
            if (FuncRetCode == KAL_ERR_NONE) {
                pCtx->Snap.Op.Bits.Active = 1U;
            }

            /* snap in sof */
            if (pCtx->Snap.Op.Bits.Active == 1U) {
                /* flag clr */
                SnapListPut = 0U;
                /* mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Snap.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* list get */
                    pPort = pCtx->Snap.pList;
                    if (pPort != NULL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Snap_Seq");
                        while (pPort != NULL) {
                            /* count check */
                            if (pPort->Count == 1U) {
                                /* timing mark */
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Snap_FPut");

                                /* shr ctrl get */
                                Shr2.Ctx.Reg = pPort->Info.Ctrl.Shr;

                                /* fifo put */
                                if (Shr2.Ctx.Bits.Hdr > 0U) {
                                    /* hdr */
                                    FuncRetCode = AmbaImgSensorSync_HdrPreloadSet(ImageChanId, &(pPort->Info.HdrData));
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                    FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &(pPort->Info.Ctrl), &(pPort->Info.Data), SYNC_FIFO | SYNC_HDR);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                } else {
                                    /* non-hdr */
                                    FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &(pPort->Info.Ctrl), &(pPort->Info.Data), SYNC_FIFO);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                }

                                /* flag set */
                                SnapListPut = 1U;

                                /* port remove */
                                if (pPort->Link.Up == NULL) {
                                    /* root */
                                    pCtx->Snap.pList = pPort->Link.Down;
                                    if (pPort->Link.Down != NULL) {
                                        pPort->Link.Down->Link.Up = NULL;
                                    }
                                } else {
                                    /* linker */
                                    pPort->Link.Up->Link.Down = pPort->Link.Down;
                                    if (pPort->Link.Down != NULL) {
                                        pPort->Link.Down->Link.Up = pPort->Link.Up;
                                    }
                                }
                            }

                            /* count down */
                            pPort->Count--;

                            /* port next */
                            pPort = pPort->Link.Down;
                        }
                    }

                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Snap.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* mutex fail */
                    RetCode = NG_UL;
                }

                if (SnapListPut == 0U) {
                    /* not specified in list */
                    if (pCtx->Snap.Op.Bits.Count > 1U) {
                        /* progress */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Snap_IPut");
                        /* shr ctrl get */
                        Shr2.Ctx.Reg = pCtx->Snap.In.Ctrl.Shr;
                        /* hdr check */
                        if (Shr2.Ctx.Bits.Hdr > 0U) {
                            FuncRetCode = AmbaImgSensorSync_HdrPreloadSet(ImageChanId, &(pCtx->Snap.In.HdrData));
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &(pCtx->Snap.In.Ctrl), &(pCtx->Snap.In.Data), SYNC_FIFO | SYNC_HDR);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        } else {
                            FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &(pCtx->Snap.In.Ctrl), &(pCtx->Snap.In.Data), SYNC_FIFO);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                        /* count down */
                        pCtx->Snap.Op.Bits.Count--;
                    } else {
                        /* last */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Snap_LPut");
                        /* shr ctrl get */
                        Shr2.Ctx.Reg = pCtx->Snap.Last.Ctrl.Shr;
                        /* hdr check */
                        if (Shr2.Ctx.Bits.Hdr > 0U) {
                            FuncRetCode = AmbaImgSensorSync_HdrPreloadSet(ImageChanId, &(pCtx->Snap.Last.HdrData));
                            if (FuncRetCode == OK_UL) {
                                RetCode = NG_UL;
                            }
                            FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &(pCtx->Snap.Last.Ctrl), &(pCtx->Snap.Last.Data), SYNC_FIFO | SYNC_HDR);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        } else {
                            FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &(pCtx->Snap.Last.Ctrl), &(pCtx->Snap.Last.Data), SYNC_FIFO);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                        /* clr */
                        pCtx->Snap.Op.Data = 0U;
                    }
                }

                if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                    char str[11];
                    str[0] = ' ';str[1] = ' ';
                    var_utoa(pCtx->Snap.Op.Bits.Count, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
                    AmbaImgSensorHAL_TimingMarkPut(VinId, str);
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* vin null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sync post sof
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_PostSof(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT64 ActualFlag;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;

    AMBA_IMG_SENSOR_HAL_DGC_CTRL_s Dgc2;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_s DDgc2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* fifo mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Fifo.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* dsp dgc in post sof */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                        /* ddgc ctrl get */
                        FuncRetCode = AmbaImgSensorHAL_DDgcFifoTiming(ImageChanId, &(DDgc2.Ctx.Reg));
                        if (FuncRetCode == OK_UL) {
                            /* ddgc timing check */
                            if (DDgc2.Ctx.Bits.Put == (UINT8) DGC_PUT_SOF) {
                                /* ddgc fifo check */
                                FuncRetCode = AmbaImgSensorHAL_DDgcFifoCheck(ImageChanId);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc_F");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc_F!");
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                        /* dgc (dsp) ctrl get */
                        FuncRetCode = AmbaImgSensorHAL_DgcFifoTiming(ImageChanId, &(Dgc2.Ctx.Reg));
                        if (FuncRetCode == OK_UL) {
                            /* dgc (dsp) timing check */
                            if (Dgc2.Ctx.Bits.Put == (UINT8) DGC_PUT_SOF) {
                                /* dgc (dsp) fifo check */
                                FuncRetCode = AmbaImgSensorHAL_DgcFifoCheck(ImageChanId);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F!");
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* nothing to do */
                    }
                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Fifo.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }

            /* ssi in post sof */
            ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_SSI_SOF, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
            if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_SSI_SOF) > 0ULL) {
                AMBA_IMG_SENSOR_HAL_SS_STATUS_s *pSsStatus;
                /* ss status get */
                FuncRetCode = AmbaImgSensorHAL_SsStatusGet(ImageChanId, &pSsStatus);
                if (FuncRetCode == OK_UL) {
                    /* slow shutter change */
                    FuncRetCode = AmbaImgSensorHAL_SlowShutter(ImageChanId, pSsStatus->LastSsIndex);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_S");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_S!");
                        RetCode = NG_UL;
                    }
                } else {
                    /* act reg fail */
                    RetCode = NG_UL;
                }
            }

            /* msc in post sof */
            ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_MSC_SOF, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
            if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_MSC_SOF) > 0ULL) {
                /* info get */
                FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
                if (FuncRetCode == OK_UL) {
                    /* msc */
                    FuncRetCode = AmbaImgSensorHAL_MscWrite(ImageChanId, pInfo->Op.Status.LastSvr);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_C");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_C!");
                        RetCode = NG_UL;
                    }
                }
            }

            /* aik in post sof */
            ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AIK_SOF, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
            if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_AIK_SOF) > 0ULL) {
                AMBA_IMG_SENSOR_SYNC_AIK_CB_s AikCb2;
                AMBA_IMG_SENSOR_HAL_DATA_REG_s ActData;
                /* cb msg */
                AikCb2.Ctx.Msg = 0ULL;
                /* act reg get */
                FuncRetCode = AmbaImgSensorHAL_ActDataGet(ImageChanId, &ActData);
                if (FuncRetCode == OK_UL) {
                    /* aik write */
                    FuncRetCode = AmbaImgSensorHAL_AikWrite(ImageChanId, AikCb2.Ctx.Msg, ActData.User);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_S");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_S!");
                        RetCode = NG_UL;
                    }
                    /* efov write */
                    if (pFrwImageChannel[VinId][ChainId].pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                        AikCb2.Ctx.Bits2.Ae = 1U;
                        FuncRetCode = AmbaImgSensorHAL_EFovWrite(ImageChanId, AikCb2.Ctx.Msg, ActData.User);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_S");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_S!");
                        }
                    }
                } else {
                    /* act reg fail */
                    RetCode = NG_UL;
                }
            }

            /* post aik sor in post sof */
            ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_PAIK_SOR, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
            if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_PAIK_SOR) > 0ULL) {
                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_PAIK_EOR);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Eor");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Eor!");
                }
            }

            /* post aik adv in post sof */
            ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_PAIK_ADV, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
            if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_PAIK_ADV) > 0ULL) {
                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_PAIK_SOR);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Sor");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Sor!");
                }
            }
        } else {
            /* ctx null */
            RetCode = NG_UL;
        }
    } else {
        /* vin null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sync mof
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_Mof(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT64 ActualFlag;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* agc ack check in mof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK_MOF, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_AGC_ACK_MOF) > 0ULL) {
                    if (pInfo->Op.User.StatDelay == 0U) {
                        /* ack */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Ack.Agc));
                        if (FuncRetCode == KAL_ERR_NONE) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack!");
                        }
                    } else {
                        /* statistics delay */
                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK_AAA);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_A");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_A!");
                        }
                    }
                }
                /* dgc ack check in mof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK_MOF, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_DGC_ACK_MOF) > 0ULL) {
                    if (pInfo->Op.User.StatDelay == 0U) {
                        /* ack */
                        FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Ack.Dgc));
                        if (FuncRetCode == KAL_ERR_NONE) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack!");
                        }
                    } else {
                        /* statistics delay */
                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK_AAA);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_A");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_A!");
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
 *  sync adv dsp
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_AdvDsp(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_DGC_CTRL_s Dgc2;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_s DDgc2;
    AMBA_IMG_SENSOR_HAL_SHR_CTRL_s Shr2;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* fifo mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Fifo.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* dsp dgc in adv dsp */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                        /* ddgc ctrl get */
                        FuncRetCode = AmbaImgSensorHAL_DDgcFifoTiming(ImageChanId, &(DDgc2.Ctx.Reg));
                        if (FuncRetCode == OK_UL) {
                            /* ddgc timing check */
                            if (DDgc2.Ctx.Bits.Put == (UINT8) DGC_PUT_ADV) {
                                /* ddgc fifo check */
                                FuncRetCode = AmbaImgSensorHAL_DDgcFifoCheck(ImageChanId);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc_F");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "DDgc_F!");
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                        /* dgc (dsp) ctrl get */
                        FuncRetCode = AmbaImgSensorHAL_DgcFifoTiming(ImageChanId, &(Dgc2.Ctx.Reg));
                        if (FuncRetCode == OK_UL) {
                            /* dgc (dsp) timing check */
                            if (Dgc2.Ctx.Bits.Put == (UINT8) DGC_PUT_ADV) {
                                /* dgc (dsp) fifo check */
                                FuncRetCode = AmbaImgSensorHAL_DgcFifoCheck(ImageChanId);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F!");
                                    RetCode = NG_UL;
                                }
                            }
                        }
                    } else {
                        /* nothing to do */
                    }

                    /* ssi in adv dsp, shr ctrl get */
                    FuncRetCode = AmbaImgSensorHAL_ShrFifoTiming(ImageChanId, &(Shr2.Ctx.Reg));
                    if (FuncRetCode == OK_UL) {
                        /* ssi timing check */
                        if (Shr2.Ctx.Bits.Ssi == (UINT8) SHR_SSI_ADV) {
                            /* ssi fifo check  */
                            FuncRetCode = AmbaImgSensorHAL_SsiFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Ssi_F!");
                                RetCode = NG_UL;
                            }
                        }
                    }

                    /* fifo mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Fifo.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            } else {
                /* info fail */
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
 *  sync adv aik
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_AdvAik(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_SHR_CTRL_s Shr2;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* fifo mutex get */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Fifo.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* shr ctrl get */
                    FuncRetCode = AmbaImgSensorHAL_ShrFifoTiming(ImageChanId, &(Shr2.Ctx.Reg));
                    if (FuncRetCode == OK_UL) {
                        /* aik timing check */
                        if (Shr2.Ctx.Bits.Aik >= (UINT8) SHR_AIK_ADV) {
                            /* aik fifo check */
                            FuncRetCode = AmbaImgSensorHAL_AikFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_F!");
                                RetCode = NG_UL;
                            }
                        }
                    }
                    /* fifo mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Fifo.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }
        }
    } else {
        /* chan null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sync iso disable callback
 *  @param[in] ImageChanId image channel id
 *  @param[in] ZoneId zone id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_IsoDisCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 ZoneId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 ZoneIsoDisId;

    UINT32 EffectRaw;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s *pCounter;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SYSTEM_AIK_CTX_s ActAikCtx;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    /* zone iso disable id get */
    ZoneIsoDisId = pFrwImageChannel[VinId][ChainId].pCtx->Aik.Ctx.Bits.IsoDisId & ZoneId;
    /* zone iso disable check */
    if ((ZoneIsoDisId > 0U) &&
        (pFrwImageChannel[VinId][ChainId].IsoCb != NULL)) {
        /* info get */
        FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
        if (FuncRetCode == OK_UL) {
            /* image channel id get */
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            /* vin counter get */
            pCounter = &(AmbaImgSensorSync_Vin[VinId].Counter);
            /* effect raw get */
            if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                EffectRaw = pCounter->RawInSof + 4U;
            } else {
                EffectRaw = pCounter->RawInSof + 3U;
            }
            /* zone iso disable found */
            for (i = 0U; (i < AMBA_IMGFRW_NUM_FOV_CHANNEL) && ((ZoneIsoDisId >> i) > 0U); i++) {
                if ((ZoneIsoDisId & (((UINT32) 1U) << i)) > 0U) {
                    /* zone id assign */
                    ImageChanId2.Ctx.Bits.ZoneId = ((UINT32) 1U) << i;
                    /* active aik ctx get */
                    FuncRetCode = AmbaImgSystem_ActAikCtxGet(ImageChanId2, &(ActAikCtx));
                    if (FuncRetCode == OK_UL) {
                        AMBA_IMG_CHANNEL_ISO_INFO_s IsoInfo;
                        /* raw cap seq put */
                        IsoInfo.RawCapSeq = EffectRaw;
                        /* post aik cnt put */
                        IsoInfo.AikCnt = pCounter->Sof;
                        /* vin skip frame put */
                        IsoInfo.VinSkipFrame = (((UINT32) pInfo->pAux->Ctx.Bits.VinSkipFrameEn) << 31U) | ((UINT32) pInfo->pAux->Ctx.Bits.VinSkipFrame);;
                        /* zone id put */
                        IsoInfo.ZoneId = i;
                        /* context id put */
                        IsoInfo.ContextId = AmbaImgSystem_ContextIdGet(i);
                        /* iso ik id put */
                        IsoInfo.IkId = ActAikCtx.Id;
                        /* iso cfg put */
                        IsoInfo.pIsoCfg = ActAikCtx.pIsoCfg;
                        /* user data put (TBD) */
                        IsoInfo.UserData = 0ULL;
                        /* iso cb */
                        FuncRetCode = pFrwImageChannel[VinId][ChainId].IsoCb(&IsoInfo);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Iso_Cb");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Iso_Cb!");
                        }
#if 1
                        /* debug message */
                        if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                            SENSOR_SYNC_MEM_INFO_s MemInfo;
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
        } else {
            /* info fail */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  sync aik execute
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_AikExecute(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;
    UINT32 ZoneId;

    UINT32 EffectRaw;

    const AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s *pCounter;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    UINT32 PostZoneId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;
    ZoneId = ImageChanId.Ctx.Bits.ZoneId;

    /* info get */
    FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
    if (FuncRetCode == OK_UL) {
        /* post zone id get */
        PostZoneId = pFrwImageChannel[VinId][ChainId].pCtx->PAik.Ctx.Bits.ZoneId;
        /* post zone check */
        if (PostZoneId == 0U) {
            /* efov adv aik */
            if (pFrwImageChannel[VinId][ChainId].pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                AMBA_IMG_SENSOR_SYNC_AIK_CB_s AikCb2;
                AikCb2.Ctx.Msg = 0ULL;
                AikCb2.Ctx.Bits2.AikExec = 1U;
                FuncRetCode = AmbaImgSensorHAL_EFovWrite(ImageChanId, AikCb2.Ctx.Msg, 0ULL);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Aik");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "EFov_Aik!");
                }
            }
            /* adv aik */
            FuncRetCode = AmbaImgSystem_AikExecute(ImageChanId, AIK_EXECUTE_ATTACH);
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik!");
                RetCode = NG_UL;
            }
            /* iso dis cb */
            FuncRetCode = Sync_IsoDisCb(ImageChanId, ZoneId);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        } else {
            /* image channel id get */
            ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
            /* zone id get, excluding post zone id */
            ZoneId = ZoneId & (~PostZoneId);

            /* adv aik */
            if (ZoneId > 0U) {
                /* zone id get */
                ImageChanId2.Ctx.Bits.ZoneId = ZoneId;
                /* aik execute & attach */
                FuncRetCode = AmbaImgSystem_AikExecute(ImageChanId2, AIK_EXECUTE_ATTACH);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik!");
                    RetCode = NG_UL;
                }
                /* iso dis cb */
                FuncRetCode = Sync_IsoDisCb(ImageChanId, ZoneId);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
            }

            /* post zone id get */
            ImageChanId2.Ctx.Bits.ZoneId = PostZoneId;
            /* aik execute (no warp) & attach */
            FuncRetCode = AmbaImgSystem_AikExecute(ImageChanId2, AIK_EXECUTE_NO_WARP_ATTACH);
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik!");
                RetCode = NG_UL;
            }

            /* vin counter get */
            pCounter = &(AmbaImgSensorSync_Vin[VinId].Counter);
            /* effect raw get */
            if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                EffectRaw = pCounter->RawInSof + 4U;
            } else {
                EffectRaw = pCounter->RawInSof + 3U;
            }
            /* post aik info put */
            FuncRetCode = AmbaImgSystem_PostAikPut(ImageChanId2, EffectRaw, pCounter->Sof);
            if (FuncRetCode == OK_UL) {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Put");
            } else {
                AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Put!");
            }

            /* post aik timing event */
            if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_PAIK_ADV);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Adv");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Adv!");
                }
            } else {
                FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_PAIK_SOR);
                if (FuncRetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Sor");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Sor!");
                }
            }
        }
    } else {
        AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik!");
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sync adv eof
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_AdvEof(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* fifo mutex get */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Fifo.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* svr in adv eof */
                    if (pInfo->Op.User.SvrDelay > 2U) {
                        FuncRetCode = AmbaImgSensorHAL_SvrFifoCheck(ImageChanId);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_F");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Svr_F!");
                            RetCode = NG_UL;
                        }
                    }

                    /* shr in adv eof */
                    FuncRetCode = AmbaImgSensorHAL_ShrFifoCheck(ImageChanId);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_F");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_F!");
                        RetCode = NG_UL;
                    }

                    /* wgc in adv eof */
                    if (pInfo->Op.User.WgcDelay > 0U) {
                        /* wgc is frame sync reg */
                        FuncRetCode = AmbaImgSensorHAL_WgcFifoCheck(ImageChanId);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_F");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_F!");
                            RetCode = NG_UL;
                        }
                    }

                    /* dgc in adv eof */
                    if ((pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_SENSOR) ||
                        (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH)) {
                        /* dgc (sensor) */
                        if (pInfo->Op.User.DgcDelay > 0U) {
                            /* dgc (sensor) is frame sync reg */
                            FuncRetCode = AmbaImgSensorHAL_DgcFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F!");
                                RetCode = NG_UL;
                            }
                        }
                    }

                    /* agc in adv eof */
                    if (pInfo->Op.User.AgcDelay > 0U) {
                        /* agc is frame sync reg */
                        FuncRetCode = AmbaImgSensorHAL_AgcFifoCheck(ImageChanId);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_F");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_F!");
                            RetCode = NG_UL;
                        }
                    }

                    /* mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Fifo.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* fifo mutex fail */
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
 *  sync sof end
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_SofEnd(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    (void) ImageChanId;
    return RetCode;
}

/**
 *  sync eof
 *  @param[in] ImageChanId image channel id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 Sync_Eof(AMBA_IMG_CHANNEL_ID_s ImageChanId)
{
    UINT32 RetCode = OK_UL;
    UINT64 ActualFlag;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_HAL_DATA_REG_s Data;

    const AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s *pCounter;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo;
    AMBA_IMG_CHANNEL_PAIK_CB_f PostAikCb;
    SENSOR_SYNC_MEM_INFO_s MemInfo;

    UINT32 PostZoneId;
    AMBA_IMG_CHANNEL_ID_s ImageChanId2;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* fifo mutex get */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Fifo.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    if ((pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_SENSOR) ||
                        (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH)) {
                        /* wgc in eof */
                        if (pInfo->Op.User.WgcDelay == 0U) {
                            /* wgc is frame sync reg */
                            FuncRetCode = AmbaImgSensorHAL_WgcFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_F!");
                                RetCode = NG_UL;
                            }
                        }

                        /* dgc (sensor) in eof */
                        if (pInfo->Op.User.DgcDelay == 0U) {
                            /* dgc (sensor) fifo check */
                            FuncRetCode = AmbaImgSensorHAL_DgcFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_F!");
                                RetCode = NG_UL;
                            }
                        }

                        /* agc in eof */
                        if (pInfo->Op.User.AgcDelay == 0U) {
                            /* agc fifo check */
                            FuncRetCode = AmbaImgSensorHAL_AgcFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_F!");
                                RetCode = NG_UL;
                            }
                        }
                    } else {
                        /* wgc in eof */
                        if (pInfo->Op.User.WgcDelay == 0U) {
                            /* wgc is frame sync reg */
                            FuncRetCode = AmbaImgSensorHAL_WgcFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Wgc_F!");
                                RetCode = NG_UL;
                            }
                        }

                        /* agc in eof */
                        if (pInfo->Op.User.AgcDelay == 0U) {
                            /* agc fifo check */
                            FuncRetCode = AmbaImgSensorHAL_AgcFifoCheck(ImageChanId);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_F");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_F!");
                                RetCode = NG_UL;
                            }
                        }
                    }

                    /* user in eof */
                    FuncRetCode = AmbaImgSensorHAL_UserFifoCheck(ImageChanId);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "User_F");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "User_F!");
                        RetCode = NG_UL;
                    }

                    /* fifo mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Fifo.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* fifo mutex fail */
                    RetCode = NG_UL;
                }

                /* ssi/svr ack in eof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_SSI_SVR_ACK, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_SSI_SVR_ACK) > 0ULL) {
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Ack.SsiSvr));
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "SsSvr_Ack");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "SsSvr_Ack!");
                        RetCode = NG_UL;
                    }
                }

                /* shr ack in eof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_SHR_ACK, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_SHR_ACK) > 0ULL) {
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Ack.Shr));
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Shr_Ack!");
                        RetCode = NG_UL;
                    }
                }

                /* agc ack check in eof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK_AAA, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_AGC_ACK_AAA) > 0ULL) {
                    /* ack (r2y background) in eof */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Ack.Agc));
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack!");
                    }
                }
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK_EOF, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_AGC_ACK_EOF) > 0ULL) {
                    /* ack to mof after one frame delay */
                    FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK_MOF);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_M");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_M!");
                        RetCode = NG_UL;
                    }
                }
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_AGC_ACK) > 0ULL) {
                    if (pInfo->Op.User.AgcDelay < 2U) {
                        /* ack to mof */
                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK_MOF);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_M");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_M!");
                            RetCode = NG_UL;
                        }
                    } else {
                        /* ack to next eof (one frame delay) */
                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_AGC_ACK_EOF);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_D");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Agc_Ack_D!");
                            RetCode = NG_UL;
                        }
                    }
                }

                /* dgc (sensor) ack check in eof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK_AAA, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_DGC_ACK_AAA) > 0ULL) {
                    /* ack (r2y background) */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Ack.Dgc));
                    if (FuncRetCode == KAL_ERR_NONE) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack!");
                    }
                }
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK_EOF, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_DGC_ACK_EOF) > 0ULL) {
                    /* ack to mof */
                    FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK_MOF);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_M");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_M!");
                        RetCode = NG_UL;
                    }
                }
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_DGC_ACK) > 0ULL) {
                    if (pInfo->Op.User.DgcDelay < 2U) {
                        /* ack to mof */
                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK_MOF);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_M");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_M!");
                            RetCode = NG_UL;
                        }
                    } else {
                        /* ack to next eof, one frame delay */
                        FuncRetCode = AmbaImgEvent_ChanPut(ImageChanId, IMAGE_EVENT_CHAN_FLAG_DGC_ACK_EOF);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_D");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Dgc_Ack_D!");
                            RetCode = NG_UL;
                        }
                    }
                }

                /* post aik eor in eof */
                ActualFlag = AmbaImgEvent_ChanGet(ImageChanId, IMAGE_EVENT_CHAN_FLAG_PAIK_EOR, IMG_EVENT_AND_CLR, AMBA_KAL_NO_WAIT);
                if ((ActualFlag & IMAGE_EVENT_CHAN_FLAG_PAIK_EOR) > 0ULL) {
                    /* image channel id get */
                    ImageChanId2.Ctx.Data = ImageChanId.Ctx.Data;
                    /* post zone id get */
                    PostZoneId = pFrwImageChannel[VinId][ChainId].pCtx->PAik.Ctx.Bits.ZoneId;
                    /* post aik get */
                    for (i = 0U; (i < AMBA_IMGFRW_NUM_FOV_CHANNEL) && ((PostZoneId >> i) > 0U); i++) {
                        if ((PostZoneId & (((UINT32) 1U) << i)) > 0U) {
                            /* post zone id get (single zone) */
                            ImageChanId2.Ctx.Bits.ZoneId = (((UINT32) 1U) << i);
                            /* post aik info get (single zone) */
                            FuncRetCode = AmbaImgSystem_PostAikGet(ImageChanId2, &pPostAikInfo);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Get");
                                /* eor timetick */
                                pPostAikInfo->EorTimetick = AmbaImgSensorSync_Vin[VinId].Timetick.Eof;
                                /* user */
                                FuncRetCode = AmbaImgSensorHAL_RingGet(ImageChanId2, &Data, pPostAikInfo->RawCapSeq);
                                if (FuncRetCode == OK_UL) {
                                    pPostAikInfo->User = Data.User;
                                } else {
                                    pPostAikInfo->User = 0ULL;
                                }
                                /* vin skip frame put */
                                pPostAikInfo->VinSkipFrame = (((UINT32) pInfo->pAux->Ctx.Bits.VinSkipFrameEn) << 31U) | ((UINT32) pInfo->pAux->Ctx.Bits.VinSkipFrame);
                                /* magic */
                                pPostAikInfo->Magic = 0xCafeU;
                                /* post aik cb get */
                                PostAikCb = pFrwImageChannel[VinId][ChainId].PostAikCb;
                                if (PostAikCb != NULL) {
                                    MemInfo.Ctx.pPostAikInfo = pPostAikInfo;
                                    /* post aik cb (single zone) */
                                    FuncRetCode = PostAikCb((UINT64) MemInfo.Ctx.Data);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Cb");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Cb!");
                                    }
                                }
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Get!");
                                RetCode = NG_UL;
                            }
                        }
                    }
                    /* post zone id get */
                    ImageChanId2.Ctx.Bits.ZoneId = PostZoneId;
                    /* post aik go */
                    FuncRetCode = AmbaImgSystem_PostAikGo(ImageChanId2);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Go");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "PAik_Go!");
                    }
                }

                /* act reg put in eof */
                FuncRetCode = AmbaImgSensorHAL_ActDataGet(ImageChanId, &Data);
                if (FuncRetCode == OK_UL) {
                    pCounter = &(AmbaImgSensorSync_Vin[VinId].Counter);
                    FuncRetCode = AmbaImgSensorHAL_RingPut(ImageChanId, &Data, pCounter->RawInMof + 3U + pInfo->Op.User.AuxDelay);
                    if (FuncRetCode != OK_UL) {
                        /* ring fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* act reg fail */
                    RetCode = NG_UL;
                }
            } else {
                /* info fail */
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
 *  sensor sync sof callback
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_SofCb(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    SENSOR_SYNC_MEM_INFO_s MemInfo;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if ((pFrwImageChannel != NULL) &&
            (pFrwImageChannel[VinId] != NULL)) {
            for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                    /* image channel get */
                    MemInfo.Ctx.pImageChannel = &(pFrwImageChannel[VinId][i]);
                    /* sof callback */
                    if (MemInfo.Ctx.pImageChannel->SofCb != NULL) {
                        FuncRetCode = MemInfo.Ctx.pImageChannel->SofCb(MemInfo.Ctx.pImageChannel->pCtx->Id);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sof_Cb");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sof_Cb!");
                            RetCode = NG_UL;
                        }
                    }
                }
            }
        } else {
            /* chan null */
            RetCode = NG_UL;
        }
    } else {
        /* vin exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync sof
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_Sof(UINT32 VinId)
{
    UINT32 RetCode = NG_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_OP_TIMING_s *pTiming;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    /* raw cap seq in sof */
    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin ctx get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* raw sync in sof */
        pVin->Counter.RawInSof = pVin->Counter.RawInMof;
        /* raw cap seq put for hal */
        FuncRetCode = AmbaImgSensorHAL_RawCapSeqPut(VinId, pVin->Counter.RawInSof);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
        /* raw cap seq put for system */
        FuncRetCode = AmbaImgSystem_RawCapSeqPut(VinId, pVin->Counter.RawInSof);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    }

    /* sof */
    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if ((pFrwImageChannel != NULL) &&
            (pFrwImageChannel[VinId] != NULL)) {
            for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                    /* ctx get */
                    pCtx = pFrwImageChannel[VinId][i].pCtx;

                    /* sof */
                    FuncRetCode = Sync_Sof(pCtx->Id);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }

                    /* info (chan) get */
                    FuncRetCode = AmbaImgSensorHAL_InfoGet(pCtx->Id, &pInfo);
                    if (FuncRetCode == OK_UL) {
                        /* timing get */
                        pTiming = &(pInfo->Op.Timing);
                        /* pst sof (disable) */
                        if (pTiming->PostSof.Bits.Enable == 0U) {
                            /* post sof in sof */
                            FuncRetCode = Sync_PostSof(pCtx->Id);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
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
            }
        } else {
            /* frw chan null */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    /* sof wait */
    FuncRetCode = SensorSync_Wait(VinId, (UINT32) SYNC_WAIT_SOF);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync post sof
 *  @param[in] Data pointer to image channel context
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_PostSof(UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    SENSOR_SYNC_MEM_INFO_s MemInfo;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_OP_TIMING_s *pTiming;

    /* ctx get */
    MemInfo.Ctx.Data = (AMBA_IMG_MEM_ADDR) Data;
    pCtx = MemInfo.Ctx.pImageChannel->pCtx;

    /* info (chan) get */
    FuncRetCode = AmbaImgSensorHAL_InfoGet(pCtx->Id, &pInfo);
    if (FuncRetCode == OK_UL) {
        /* timing get */
        pTiming = &(pInfo->Op.Timing);
        /* post sof enable? */
        if (pTiming->PostSof.Bits.Enable > 0U) {
            /* post sof */
            FuncRetCode = Sync_PostSof(pCtx->Id);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        }
    } else {
        /* info null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync mof
 *  @param[in] Data vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_Mof(UINT64 Data)
{
    UINT32 RetCode = NG_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId = (UINT32) (Data & 0xFFFFFFFFULL);

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;
    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s *pCounter;
    const AMBA_IMG_CHANNEL_INTER_s *pInter;

    UINT32 EffectRaw = 0U;
    UINT32 FrameId = 0U;

    /* raw cap seq in mof */
    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin ctx get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* raw sync in mof */
        pVin->Counter.RawInMof = pVin->Counter.Raw;
    }

    /* mof */
    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if ((pFrwImageChannel != NULL) &&
            (pFrwImageChannel[VinId] != NULL)) {
            for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                    /* vin ctx get */
                    pVin = &(AmbaImgSensorSync_Vin[VinId]);
                    /* ctx get */
                    pCtx = pFrwImageChannel[VinId][i].pCtx;
                    /* mof */
                    FuncRetCode = Sync_Mof(pCtx->Id);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* counter get */
                    pCounter = &(AmbaImgSensorSync_Vin[pCtx->Id.Ctx.Bits.VinId].Counter);
                    /* inter get */
                    pInter = &(pCtx->Inter);
                    /* inter? */
                    if (pInter->Ctx.Bits.Num > 1U) {
                        if (pCounter->Sof > pInter->Ctx.Bits.Skip) {
                            /* info (chan) get */
                            FuncRetCode = AmbaImgSensorHAL_InfoGet(pCtx->Id, &pInfo);
                            if (FuncRetCode == OK_UL) {
                                /* effect raw */
                                if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                                    /* same frame with shr */
                                    EffectRaw = pCounter->RawInSof + 4U;
                                } else {
                                    /* non-adv aik */
                                    if (pInfo->Op.Ctrl.PreAikCmd == 0U) {
                                        /* following frame to shr */
                                        EffectRaw = pCounter->RawInSof + 3U + pInfo->Op.User.AuxDelay;
                                    } else {
                                        /* not support */
                                        ChkCode = NG_UL;
                                    }
                                }
                                if (ChkCode == OK_UL) {
                                    /* frame id get */
                                    FrameId = EffectRaw % ((UINT32) pCtx->Inter.Ctx.Bits.Num);
                                    /* belong? */
                                    if ((((UINT32) pCtx->Inter.Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                                        /* not belong */
                                        ChkCode = NG_UL;
                                    }
                                }
                            } else {
                                /* info fail */
                                ChkCode = NG_UL;
                            }
                        } else {
                            ChkCode = NG_UL;
                        }
                    }
                    /* efov mof */
                    if ((ChkCode == OK_UL) &&
                        (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER)) {
                        AMBA_IMG_SENSOR_SYNC_AIK_CB_s AikCb2;
                        AikCb2.Ctx.Msg = 0ULL;
                        AikCb2.Ctx.Bits2.Raw = 1U;
                        AikCb2.Ctx.Bits2.Var = pVin->Counter.RawInMof;
                        FuncRetCode = AmbaImgSensorHAL_EFovWrite(pCtx->Id, AikCb2.Ctx.Msg, 0ULL);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                } else {
                    /* ctx null */
                    RetCode = NG_UL;
                }
            }
        } else {
            /* frw chan null */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    /* mof wait */
    FuncRetCode = SensorSync_Wait(VinId, SYNC_MOF);
    if (FuncRetCode != OK_UL) {
        /* wait fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync adv dsp
 *  @param[in] Data pointer to the image channel context
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_AdvDsp(UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    SENSOR_SYNC_MEM_INFO_s MemInfo;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    /* ctx get */
    MemInfo.Ctx.Data = (AMBA_IMG_MEM_ADDR) Data;
    pCtx = MemInfo.Ctx.pImageChannel->pCtx;

    /* efov slave? */
    if (pCtx->EFov.Ctx.Bits.Mode != (UINT8) IMG_CHAN_EFOV_SLAVE) {
        /* adv dsp */
        FuncRetCode = Sync_AdvDsp(pCtx->Id);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }

        /* adv aik */
        FuncRetCode = Sync_AdvAik(pCtx->Id);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  sensor sync adv eof
 *  @param[in] Data pointer to the image channel context
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_AdvEof(UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    SENSOR_SYNC_MEM_INFO_s MemInfo;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    /* ctx get */
    MemInfo.Ctx.Data = (AMBA_IMG_MEM_ADDR) Data;
    pCtx = MemInfo.Ctx.pImageChannel->pCtx;

    /* efov slave? */
    if (pCtx->EFov.Ctx.Bits.Mode != (UINT8) IMG_CHAN_EFOV_SLAVE) {
        /* adv eof */
        FuncRetCode = Sync_AdvEof(pCtx->Id);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  sensor sync aik execute
 *  @param[in] Data pointer to the image channel context
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_AikExecute(UINT64 Data)
{
    UINT32 RetCode = OK_UL;
    UINT32 ChkCode = OK_UL;
    UINT32 FuncRetCode;

    SENSOR_SYNC_MEM_INFO_s MemInfo;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s *pCounter;
    const AMBA_IMG_CHANNEL_INTER_s *pInter;

    UINT32 EffectRaw = 0U;
    UINT32 FrameId = 0U;

    /* ctx get */
    MemInfo.Ctx.Data = (AMBA_IMG_MEM_ADDR) Data;
    pCtx = MemInfo.Ctx.pImageChannel->pCtx;
    /* counter get */
    pCounter = &(AmbaImgSensorSync_Vin[pCtx->Id.Ctx.Bits.VinId].Counter);
    /* inter get */
    pInter = &(pCtx->Inter);

    /* inter? */
    if (pInter->Ctx.Bits.Num > 1U) {
        if (pCounter->Sof > pInter->Ctx.Bits.Skip) {
            /* info (chan) get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(pCtx->Id, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* effect raw */
                if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                    /* same frame with shr */
                    EffectRaw = pCounter->RawInSof + 4U;
                } else {
                    /* non-adv aik */
                    if (pInfo->Op.Ctrl.PreAikCmd == 0U) {
                        /* following frame to shr */
                        EffectRaw = pCounter->RawInSof + 3U + pInfo->Op.User.AuxDelay;
                    } else {
                        /* not support */
                        ChkCode = NG_UL;
                    }
                }
                if (ChkCode == OK_UL) {
                    /* frame id get */
                    FrameId = EffectRaw % ((UINT32) pCtx->Inter.Ctx.Bits.Num);
                    /* belong? */
                    if ((((UINT32) pCtx->Inter.Ctx.Bits.Id) & (((UINT32) 1U) << (FrameId & 0x1FU))) == 0U) {
                        /* not belong */
                        ChkCode = NG_UL;
                    }
                }
            } else {
                /* info fail */
                ChkCode = NG_UL;
            }
        } else {
            ChkCode = NG_UL;
        }
    }

    /* aik */
    if ((ChkCode == OK_UL) &&
        (pCtx->EFov.Ctx.Bits.Mode != (UINT8) IMG_CHAN_EFOV_SLAVE)) {
        /* aik slot cb */
        if (pCtx->pAikSync != NULL) {
            for (UINT32 i = 0U; i < (UINT32) IMG_CHAN_AIK_SLOT_TOTAL; i++) {
                if (pCtx->pAikSync->Slot[i].pFunc != NULL) {
                    FuncRetCode = pCtx->pAikSync->Slot[i].pFunc(pCtx->Id);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                }
            }
        }
        /* aik cb */
        if (MemInfo.Ctx.pImageChannel->AikCb != NULL) {
            FuncRetCode = MemInfo.Ctx.pImageChannel->AikCb(pCtx->Id);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        }
        /* aik execute */
        FuncRetCode = Sync_AikExecute(pCtx->Id);
        if (FuncRetCode != OK_UL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  sensor sync sof end
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_SofEnd(UINT32 VinId)
{
    UINT32 RetCode = NG_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    /* sof */
    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if ((pFrwImageChannel != NULL) &&
            (pFrwImageChannel[VinId] != NULL)) {
            for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                    /* ctx get */
                    pCtx = pFrwImageChannel[VinId][i].pCtx;
                    /* sof end */
                    FuncRetCode = Sync_SofEnd(pCtx->Id);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* efov sof end */
                    if (pCtx->EFov.Ctx.Bits.Mode == (UINT8) IMG_CHAN_EFOV_MASTER) {
                        AMBA_IMG_SENSOR_SYNC_AIK_CB_s AikCb2;
                        AikCb2.Ctx.Msg = 0ULL;
                        AikCb2.Ctx.Bits2.SofEnd = 1U;
                        FuncRetCode = AmbaImgSensorHAL_EFovWrite(pCtx->Id, AikCb2.Ctx.Msg, 0ULL);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  sensor sync eof callback
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_EofCb(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    SENSOR_SYNC_MEM_INFO_s MemInfo;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if ((pFrwImageChannel != NULL) &&
            (pFrwImageChannel[VinId] != NULL)) {
            for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                    /* image channel get */
                    MemInfo.Ctx.pImageChannel = &(pFrwImageChannel[VinId][i]);
                    /* sof callback */
                    if (MemInfo.Ctx.pImageChannel->EofCb != NULL) {
                        FuncRetCode = MemInfo.Ctx.pImageChannel->EofCb(MemInfo.Ctx.pImageChannel->pCtx->Id);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Eof_Cb");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Eof_Cb!");
                            RetCode = NG_UL;
                        }
                    }
                }
            }
        } else {
            /* chan null */
            RetCode = NG_UL;
        }
    } else {
        /* vin exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync eof
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_Eof(UINT32 VinId)
{
    UINT32 RetCode = NG_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;
    const AMBA_IMG_SENSOR_HAL_OP_TIMING_s *pTiming;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        if ((pFrwImageChannel != NULL) &&
            (pFrwImageChannel[VinId] != NULL)) {
            for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                    /* ctx get */
                    pCtx = pFrwImageChannel[VinId][i].pCtx;

                    /* info (chan) get */
                    FuncRetCode = AmbaImgSensorHAL_InfoGet(pCtx->Id, &pInfo);
                    if (FuncRetCode == OK_UL) {
                        /* timing get */
                        pTiming = &(pInfo->Op.Timing);
                        /* adv eof (disable) */
                        if (pTiming->AdvEof.Bits.Enable == 0U) {
                            /* adv eof in eof */
                            FuncRetCode = Sync_AdvEof(pCtx->Id);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                    }

                    /* eof */
                    FuncRetCode = Sync_Eof(pCtx->Id);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                } else {
                    /* ctx null */
                    RetCode = NG_UL;
                }
            }
        } else {
            /* frw chan null */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    /* wait eof */
    FuncRetCode = SensorSync_Wait(VinId, (UINT32) SYNC_WAIT_EOF);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync wait
 *  @param[in] VinId vin id
 *  @param[in] Id timing port id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_Wait(UINT32 VinId, UINT32 Id)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pPort;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    if ((VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) &&
        (Id < (UINT32) SYNC_WAIT_TOTAL)) {
        /* vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* mutex take */
        FuncRetCode = AmbaKAL_MutexTake(&(pVin->Wait[Id].Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            pPort = pVin->Wait[Id].pList;
            while (pPort != NULL) {
                if (pPort->Op.Bits.Count == 1U) {
                    /* port remove */
                    if (pPort->Link.Up == NULL) {
                        /* root */
                        pVin->Wait[Id].pList = pPort->Link.Down;
                        if (pPort->Link.Down != NULL) {
                            pPort->Link.Down->Link.Up = NULL;
                        }
                    } else {
                        /* linker */
                        pPort->Link.Up->Link.Down = pPort->Link.Down;
                        if (pPort->Link.Down != NULL) {
                            pPort->Link.Down->Link.Up = pPort->Link.Up;
                        }
                    }
                    /* magic clr */
                    pPort->Magic = 0U;
                    /* ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pPort->Sem.Ack));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }

                /* count down */
                pPort->Op.Bits.Count--;

                /* port next */
                pPort = pPort->Link.Down;
            }
            /* mutex give */
            FuncRetCode = AmbaKAL_MutexGive(&(pVin->Wait[Id].Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  sensor sync timing update
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SensorSync_TimingUpdate(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 HdrEnable = 0U;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;
    const AMBA_IMG_CHANNEL_CONTEXT_s *pCtx;

    const AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s *pHdrCtx;

    AMBA_IMG_CHANNEL_ID_s ImageChanId;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* cb? */
        if (pVin->Timing.pFunc != NULL) {
            /* chan check */
            if ((pFrwImageChannel != NULL) &&
                (pFrwImageChannel[VinId] != NULL)) {
                for (i = 0U; pFrwImageChannel[VinId][i].Magic == 0xCafeU; i++) {
                    /* ctx (chan) check */
                    if (pFrwImageChannel[VinId][i].pCtx != NULL) {
                        /* ctx get */
                        pCtx = pFrwImageChannel[VinId][i].pCtx;
                        /* image channel id */
                        ImageChanId.Ctx.Data = pCtx->Id.Ctx.Data;
                        /* chain id get */
                        ChainId = (UINT32) ImageChanId.Ctx.Bits.ChainId;
                        /* hdr get */
                        if ((AmbaImgSystem_Chan[VinId] != NULL) &&
                            (AmbaImgSystem_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                            if (AmbaImgSystem_Chan[VinId][ChainId].pInfo != NULL) {
                                HdrEnable = (UINT32) AmbaImgSystem_Chan[VinId][ChainId].pInfo->Pipe.Hdr.Bits.Enable;
                            } else {
                                /* info null */
                                RetCode = NG_UL;
                            }
                        } else {
                            /* vin or chan null */
                            RetCode = NG_UL;
                        }
                        /* dol-hdr? */
                        if ((RetCode == OK_UL) &&
                            (HdrEnable == 1U)) {
                            if ((AmbaImgSensorHAL_Chan[VinId] != NULL) &&
                                (AmbaImgSensorHAL_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                                /* hdr ctx get */
                                pHdrCtx = AmbaImgSensorHAL_Chan[VinId][ChainId].pHdrCtx;
                                if (pHdrCtx != NULL) {
                                    /* timing cb */
                                    FuncRetCode = pVin->Timing.pFunc(ImageChanId, pHdrCtx->Status.Last.Shr);
                                    if (FuncRetCode == OK_UL) {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Time_Cb");
                                    } else {
                                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Time_Cb!");
                                        RetCode = NG_UL;
                                    }
                                }
                            }
                        }
                    }
                }

                if (RetCode == OK_UL) {
                    /* timing config */
                    FuncRetCode = AmbaImgSensorHAL_TimingConfig(VinId);
                    if (FuncRetCode == OK_UL) {
                        /* timing check */
                        FuncRetCode = AmbaImgSensorSync_TimingCheck(VinId);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    } else {
                        /* timing cfg fail */
                        RetCode = NG_UL;
                    }
                }

                if (RetCode == OK_UL) {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Time_Chk");
                } else {
                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Time_Chk!");
                }
            }
        }
    } else {
        /* vin exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync timing callback
 *  @param[in] VinId vin id
 *  @param[in] pFunc pointer to the callback function
 *  @return error code
 */
UINT32 AmbaImgSensorSync_TimingCb(UINT32 VinId, AMBA_IMG_SENSOR_SYNC_TIMING_CB_f pFunc)
{
    UINT32 RetCode = OK_UL;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* callback register */
        pVin->Timing.pFunc = pFunc;
    } else {
        /* vin exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync sof
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Sof(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 SofTimetick;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    const AMBA_IMG_CHANNEL_TIMING_PORT_s *pList;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        FuncRetCode = AmbaKAL_GetSysTickCount(&SofTimetick);
        if (FuncRetCode == KAL_ERR_NONE) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sof_S");
            /* vin get */
            pVin = &(AmbaImgSensorSync_Vin[VinId]);
            if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                char str[11];
                str[0] = ' ';str[1] = ' ';
                var_utoa(pVin->Counter.Sof, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            }
            /* sof timetick */
            pVin->Timetick.LastSof = pVin->Timetick.Sof;
            pVin->Timetick.Sof = SofTimetick;
            /* sof cb */
            (void) SensorSync_SofCb(VinId);
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pVin->Mutex.Sof), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                if (pVin->Op.Bits.Reset == 0U) {
                    /* sof counter */
                    pVin->Counter.Sof++;

                    /* sof timing update */
                    (void) SensorSync_TimingUpdate(VinId);

                    /* sof */
                    (void) SensorSync_Sof(VinId);

                    /* timing mutex take */
                    FuncRetCode = AmbaKAL_MutexTake(&(pVin->Timing.Mutex), AMBA_KAL_WAIT_FOREVER);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* list get */
                        pList = pVin->Timing.pSofList;
                        /*
                         * post_sof/mof
                         * adv_dsp/adv_eof/aik_exec timing
                         */
                        while (pList != NULL) {
                            if (pList->Op.Bits.Enable > 0U) {
                                FuncRetCode = AmbaImgSensorSync_TimeWaitTo(VinId, SofTimetick + pList->Op.Bits.Timetick);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, pList->pName);
                                    if (pVin->Counter.Sof > pList->SkipFrame) {
                                        (void) pList->pFunc(pList->Data);
                                    }
                                }
                            }
                            pList = pList->Link.Down;
                        }
                        /* timing mutex give */
                        FuncRetCode = AmbaKAL_MutexGive(&(pVin->Timing.Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    }

                    /* sof end */
                    (void) SensorSync_SofEnd(VinId);
                }
                /* mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(pVin->Mutex.Sof));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            } else {
                /* mutex fail */
                RetCode = NG_UL;
            }
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sof_E");
        } else {
            /* tick fail */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync eof
 *  @param[in] VinId vin id
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Eof(UINT32 VinId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 EofTimetick;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        FuncRetCode = AmbaKAL_GetSysTickCount(&EofTimetick);
        if (FuncRetCode == KAL_ERR_NONE) {
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Eof_S");
            /* vin get */
            pVin = &(AmbaImgSensorSync_Vin[VinId]);
            if (AmbaImgSensorHAL_TimingMarkChk(VinId) == OK_UL) {
                char str[11];
                str[0] = ' ';str[1] = ' ';
                var_utoa(pVin->Counter.Eof, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
                AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            }
            /* eof timetick */
            pVin->Timetick.LastEof = pVin->Timetick.Eof;
            pVin->Timetick.Eof = EofTimetick;
            /* eof cb */
            (void) SensorSync_EofCb(VinId);
            /* mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pVin->Mutex.Eof), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                if (pVin->Op.Bits.Reset == 0U) {
                    /* eof counter */
                    pVin->Counter.Eof++;

                    /* eof */
                    (void) SensorSync_Eof(VinId);
                }
                /* mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(pVin->Mutex.Eof));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            } else {
                /* mutex fail */
                RetCode = NG_UL;
            }
            AmbaImgSensorHAL_TimingMarkPut(VinId, "Eof_E");
        } else {
            /* tick fail */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync wait
 *  @param[in] ImageChanId image channel id
 *  @param[in] pPort pointer to the wait port
 *  @param[in] Mode sync mode (SYNC_SOF/SYNC_MOF/SYNC_EOF)
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Wait(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pPort, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT8 WaitId;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;
    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;
    AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pList;

    VinId = ImageChanId.Ctx.Bits.VinId;

    CmdMsg.Ctx.Data = Mode;

    if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
        /* vin get */
        pVin = &(AmbaImgSensorSync_Vin[VinId]);
        /* mode check */
        if ((CmdMsg.Ctx.Com.Cmd >= (UINT8) SYNC_CMD_SOF) &&
            (CmdMsg.Ctx.Com.Cmd <= (UINT8) SYNC_CMD_EOF)) {
            /* wait id get */
            WaitId = (UINT8) CmdMsg.Ctx.Com.Cmd - (UINT8) SYNC_CMD_SOF;
            /* list mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pVin->Wait[WaitId].Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* port used? */
                if ((pPort != NULL) && (pPort->Magic != 0xCafeU)) {
                    /* image channel id */
                    pPort->ImageChanId.Ctx.Data = ImageChanId.Ctx.Data;
                    /* count */
                    pPort->Op.Bits.Count = CmdMsg.Ctx.Wait.Count;
                    /* wiat id */
                    pPort->Op.Bits.Id = WaitId;
                    /* link */
                    pPort->Link.Up = NULL;
                    pPort->Link.Down = NULL;
                    /* ack sem create */
                    FuncRetCode = AmbaKAL_SemaphoreCreate(&(pPort->Sem.Ack), NULL, 0U);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* list get */
                        pList = pVin->Wait[pPort->Op.Bits.Id].pList;
                        if (pList == NULL) {
                            /* hook to header */
                            pVin->Wait[pPort->Op.Bits.Id].pList = pPort;
                        } else {
                            /* tailer find */
                            while (pList->Link.Down != NULL) {
                                pList = pList->Link.Down;
                            }
                            /* hook to tailer */
                            pList->Link.Down = pPort;
                            pPort->Link.Up = pList;
                        }

                        /* magic */
                        pPort->Magic = 0xCafeU;

                        /* list mutex give */
                        FuncRetCode = AmbaKAL_MutexGive(&(pVin->Wait[WaitId].Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }

                        /* wait for ack */
                        FuncRetCode = AmbaKAL_SemaphoreTake(&(pPort->Sem.Ack), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* ack sem delete */
                            FuncRetCode = AmbaKAL_SemaphoreDelete(&(pPort->Sem.Ack));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    } else {
                        /* list mutex give */
                        FuncRetCode = AmbaKAL_MutexGive(&(pVin->Wait[WaitId].Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                        /* ack sem fail */
                        RetCode = NG_UL;
                    }
                } else {
                    /* list mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pVin->Wait[WaitId].Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* port null or used */
                    RetCode = NG_UL;
                }
            } else {
                /* list access fail */
                RetCode = NG_UL;
            }
        } else {
            /* wait id exceed */
            RetCode = NG_UL;
        }
    } else {
        /* vin id exceed */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync wait push
 *  @param[in] pPort pointer to the wait port
 *  @return error code
 */
UINT32 AmbaImgSensorSync_WaitPush(AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pPort)
{
    UINT32 RetCode = NG_UL;
    UINT32 FuncRetCode;

    UINT32 VinId = 0xFFU;

    AMBA_IMG_SENSOR_SYNC_VIN_s *pVin;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        /* vin id get */
        VinId = pPort->ImageChanId.Ctx.Bits.VinId;
        /* vin check */
        if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
            /* vin get */
            pVin = &(AmbaImgSensorSync_Vin[VinId]);
            /* list mutex take */
            FuncRetCode = AmbaKAL_MutexTake(&(pVin->Wait[pPort->Op.Bits.Id].Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* port still valid? */
                if (pPort->Magic == 0xCafeU) {
                    /* port remove */
                    if (pPort->Link.Up == NULL) {
                        /* root */
                        pVin->Wait[pPort->Op.Bits.Id].pList = pPort->Link.Down;
                        if (pPort->Link.Down != NULL) {
                            pPort->Link.Down->Link.Up = NULL;
                        }
                    } else {
                        /* linker */
                        pPort->Link.Up->Link.Down = pPort->Link.Down;
                        if (pPort->Link.Down != NULL) {
                            pPort->Link.Down->Link.Up = pPort->Link.Up;
                        }
                    }
                    /* magic clr */
                    pPort->Magic = 0U;
                    /* ack */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pPort->Sem.Ack));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        RetCode = NG_UL;
                    }
                }
                /* list mutex give */
                FuncRetCode = AmbaKAL_MutexGive(&(pVin->Wait[pPort->Op.Bits.Id].Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }
        } else {
            /* vin id exceed */
            RetCode = NG_UL;
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image sensor sync HDR preload set
 *  @param[in] ImageChanId image channel id
 *  @param[in] pHdrData pointer to the HAL HDR data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_HdrPreloadSet(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_HDR_DATA_s *pHdrData)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            if (pHdrData != NULL) {
                /* hdr preload set */
                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                    pCtx->HdrPreload.Agc[i] = pHdrData->Agc[i];
                    pCtx->HdrPreload.Dgc[i] = pHdrData->Dgc[i];
                    pCtx->HdrPreload.Wgc[i] = pHdrData->Wgc[i];
                    pCtx->HdrPreload.Shr[i] = pHdrData->Shr[i];
                    pCtx->HdrPreload.DDgc[i] = pHdrData->DDgc[i];
                }
            } else {
                /* data null */
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
 *  Amba image sensor sync FIFO put
 *  @param[in] ImageChanId image channel id
 *  @param[in] pCtrl pointer to the HAL control reg
 *  @param[in] pData pointer to the HAL data reg
 *  @param[in] Mode sync command and message
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_FifoPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_CTRL_REG_s *pCtrl, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_s Ctrl;
    AMBA_IMG_SENSOR_HAL_DATA_s Data;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pCtrl != NULL) &&
        (pData != NULL) &&
        (AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctrl get */
            Ctrl.Agc.Ctx.Reg = pCtrl->Agc;
            Ctrl.Dgc.Ctx.Reg = pCtrl->Dgc;
            Ctrl.Wgc.Ctx.Reg = pCtrl->Wgc;
            Ctrl.Shr.Ctx.Reg = pCtrl->Shr;
            Ctrl.DDgc.Ctx.Reg = pCtrl->DDgc;
            Ctrl.User.Ctx.Reg = pCtrl->User;
            /* data get */
            Data.Agc.Ctx.Reg = pData->Agc;
            Data.Dgc.Ctx.Reg = pData->Dgc;
            Data.Wgc.Ctx.Reg = pData->Wgc;
            Data.Shr.Ctx.Reg = pData->Shr;
            Data.DDgc.Ctx.Reg = pData->DDgc;
            Data.User.Ctx.Reg = pData->User;

            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* fifo mutex take */
                FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Fifo.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == OK_UL) {
                    /* fifo put */
                    FuncRetCode = AmbaImgSensorHAL_FifoPut(ImageChanId, pCtrl, pData);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fifo_Put");
                        /* hdr fifo put */
                        if (CmdMsg.Ctx.Fifo.Hdr > 0U) {
                            FuncRetCode = AmbaImgSensorHAL_HdrFifoPut(ImageChanId, &(pCtx->HdrPreload));
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_F_Put");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Hdr_F_Put!");
                                RetCode = NG_UL;
                            }
                        }
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Fifo_Put!");
                        RetCode = NG_UL;
                    }
                    /* fifo mutex give */
                    FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Fifo.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }

                if (RetCode == OK_UL) {
                    AMBA_IMG_SENSOR_HAL_DATA_REG_s DataIn;
                    AMBA_IMG_SENSOR_HAL_DATA_REG_s LastDataIn;

                    FuncRetCode = AmbaImgSensorHAL_DataGet(ImageChanId, &LastDataIn);
                    if (FuncRetCode == OK_UL) {
                        /* shr in */
                        if ((Ctrl.Shr.Ctx.Bits.Sig == (UINT8) SHR_SIG_SHR) ||
                            (Ctrl.Shr.Ctx.Bits.Sig == (UINT8) SHR_SIG_BOTH) ||
                            (Ctrl.Shr.Ctx.Bits.Sig == (UINT8) SHR_SIG_SSR) ||
                            (Ctrl.Shr.Ctx.Bits.Sig == (UINT8) SHR_SIG_SSG)) {
                            DataIn.Shr = Data.Shr.Ctx.Bits.Shr;
                        } else {
                            DataIn.Shr = LastDataIn.Shr;
                        }

                        /* dgc in */
                        if (Ctrl.Dgc.Ctx.Bits.Sig == (UINT8) DGC_SIG_DGC) {
                            DataIn.Dgc = Data.Dgc.Ctx.Bits.Dgc;
                        } else {
                            DataIn.Dgc = LastDataIn.Dgc;
                        }

                        /* Wgc in */
                        if (Ctrl.Wgc.Ctx.Bits.Sig == (UINT8) DGC_SIG_DGC) {
                            DataIn.Wgc = Data.Wgc.Ctx.Bits.Wgc;
                        } else {
                            DataIn.Wgc = LastDataIn.Wgc;
                        }

                        /* agc in */
                        if (Ctrl.Agc.Ctx.Bits.Sig == (UINT8) AGC_SIG_AGC) {
                            DataIn.Agc = Data.Agc.Ctx.Bits.Agc;
                        } else {
                            DataIn.Agc = LastDataIn.Agc;
                        }

                        /* ddgc in */
                        if (Ctrl.DDgc.Ctx.Bits.Sig == (UINT8) DGC_SIG_DGC) {
                            DataIn.DDgc = Data.DDgc.Ctx.Bits.Dgc;
                        } else {
                            DataIn.DDgc = LastDataIn.DDgc;
                        }

                        /* user in */
                        if ((Ctrl.User.Ctx.Bits.Sig == (UINT8) USER_SIG_USER) ||
                            (Ctrl.User.Ctx.Bits.Sig == (UINT8) USER_SIG_NONE)) {
                            DataIn.User = Data.User.Ctx.Bits.User;
                        } else {
                            DataIn.User = LastDataIn.User;
                        }

                        FuncRetCode = AmbaImgSensorHAL_DataIn(ImageChanId, &DataIn);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Data_In");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Data_In!");
                            RetCode = NG_UL;
                        }
                    } else {
                        /* reg get fail */
                        RetCode = NG_UL;
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
 *  Amba image sensor sync HDR preload
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] Mode sync command and message
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_HdrPreload(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_DATA_s Data;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pSyncData != NULL) &&
        (AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* data get */
            Data.Agc.Ctx.Bits.Agc = pSyncData->Agc.Ctx.Bits.Agc;
            Data.Dgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.Dgc;
            Data.Wgc.Ctx.Bits.Wgc = pSyncData->Wgc.Ctx.Bits.Wgc;
            Data.Shr.Ctx.Bits.Shr = pSyncData->Shr.Ctx.Bits.Shr;
            Data.DDgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.DDgc;

            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            if (CmdMsg.Ctx.HdrPreload.Count < AMBA_IMG_SENSOR_HAL_HDR_SIZE) {
                UINT32 Index = (UINT32) CmdMsg.Ctx.HdrPreload.Count;
                /* hdr preload */
                pCtx->HdrPreload.Agc[Index] = Data.Agc.Ctx.Bits.Agc;
                pCtx->HdrPreload.Dgc[Index] = Data.Dgc.Ctx.Bits.Dgc;
                pCtx->HdrPreload.Wgc[Index] = Data.Wgc.Ctx.Bits.Wgc;
                pCtx->HdrPreload.Shr[Index] = Data.Shr.Ctx.Bits.Shr;
                pCtx->HdrPreload.DDgc[Index] = Data.DDgc.Ctx.Bits.Dgc;
            } else {
                /* count exceed */
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
 *  Amba image sensor sync snap
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] Mode sync command and message
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_Snap(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_s Ctrl;
    AMBA_IMG_SENSOR_HAL_DATA_s Data;

    UINT32 Svr;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pSyncData != NULL) &&
        (AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctrl reset */
            Ctrl.Agc.Ctx.Reg = 0ULL;
            Ctrl.Dgc.Ctx.Reg = 0ULL;
            Ctrl.Wgc.Ctx.Reg = 0ULL;
            Ctrl.Shr.Ctx.Reg = 0ULL;
            Ctrl.DDgc.Ctx.Reg = 0ULL;
            Ctrl.User.Ctx.Reg = 0ULL;
            /* data get */
            Data.Agc.Ctx.Bits.Agc = pSyncData->Agc.Ctx.Bits.Agc;
            Data.Dgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.Dgc;
            Data.Wgc.Ctx.Bits.Wgc = pSyncData->Wgc.Ctx.Bits.Wgc;
            Data.Shr.Ctx.Bits.Shr = pSyncData->Shr.Ctx.Bits.Shr;
            Data.DDgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.DDgc;
            Data.User.Ctx.Bits.User = pSyncData->User.Ctx.Bits.User;

            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* svr check */
                if (CmdMsg.Ctx.Snap.Hdr == 0U) {
                    FuncRetCode = AmbaImgSensorHAL_SvrCheck(ImageChanId, Data.Shr.Ctx.Bits.Shr, &Svr);
                } else {
                    FuncRetCode = AmbaImgSensorHAL_HdrSvrCheck(ImageChanId, pCtx->HdrPreload.Shr, &Svr);
                }
                if (FuncRetCode == OK_UL) {
                    /* ssi (none) */
                    Ctrl.Shr.Ctx.Bits.Ssi = (UINT8) SHR_SSI_NONE;
                    /* fll (not support one frame advance in snap,
                       needed enable auto fll of sensor if svr is one frame advance */
                    Ctrl.Shr.Ctx.Bits.Fll = (UINT8) SHR_SVR_NON_ADV;
                    /* hdr */
                    if (CmdMsg.Ctx.Snap.Hdr > 0U) {
                        /* agc */
                        Ctrl.Agc.Ctx.Bits.Hdr = (UINT8) AGC_HDR_ON;
                        if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                            /* dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                        } else {
                            /* dgc (sensor) */
                            Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        }
                        /* wgc (sensor) */
                        Ctrl.Wgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        /* shr */
                        Ctrl.Shr.Ctx.Bits.Hdr = (UINT8) SHR_HDR_ON;
                        /* dsp dgc */
                        Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                    }

                    /* rgb (gain) */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                        /* ddgc (dsp), DGC_IMG_STILL, no write2dsp */
                        if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                            /* adv dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                            /* no shr precondition in dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                            /* dsp dgc precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                        } else {
                            /* non-adv dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                            /* shr precondition in dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                            /* no dsp dgc precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        }
                    } else if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                        /* dgc (dsp) */
                        if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                            /* adv dgc (dsp), DGC_IMG_STILL, no write2dsp */
                            Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                            /* no shr precondition in dgc (dsp)*/
                            Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                            /* dgc (dsp) precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                        } else {
                            /* non-adv dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                            /* shr precondition in shr */
                            Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                            /* no dgc (dsp) precondition in shr*/
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        }
                    } else {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                    }

                    /* aik */
                    if (CmdMsg.Ctx.Snap.Aik > 0U) {
                        if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                            /* adv aik */
                            Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV;
                        } else {
                            if (CmdMsg.Ctx.Snap.PreAik > 0U) {
                                /* pre aik & aik */
                                Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV_SOF;
                            } else {
                                /* aik */
                                Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_SOF;
                            }
                        }
                    }

                    /* msc */
                    if (pInfo->Op.User.ShrDelay <= 2U) {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD;
                    } else {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD_SOF;
                    }

                    /* sls */
                    if (CmdMsg.Ctx.Snap.Sls > 0U) {
                        /* adv sls */
                        Ctrl.Shr.Ctx.Bits.Sls = (UINT8) SHR_SLS_ADV;
                    }

                    /* wgc (sensor) */
                    Ctrl.Agc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Agc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;

                    /* dgc  */
                    Ctrl.Dgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Dgc.Ctx.Bits.Img = (UINT8) DGC_IMG_STILL;

                    /* agc */
                    Ctrl.Agc.Ctx.Bits.Sig = (UINT8) AGC_SIG_AGC;
                    Ctrl.Agc.Ctx.Bits.Shr = (UINT8) AGC_SHR_ADV;

                    /* shr */
                    if (pInfo->Cfg.ShutterMode == (UINT32) IMAGE_SHR_SSR) {
                        Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_SSR;
                    } else {
                        Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_SSG;
                    }

                    /* svr */
                    Ctrl.Shr.Ctx.Bits.Svr = (UINT16) (Svr & 0xFFFFU);

                    /* ddgc */
                    Ctrl.DDgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.DDgc.Ctx.Bits.Img = (UINT8) DGC_IMG_STILL;

                    /* user */
                    if (CmdMsg.Ctx.Snap.User > 0U) {
                        Ctrl.User.Ctx.Bits.Sig = (UINT8) USER_SIG_USER;
                    }
                    Ctrl.User.Ctx.Bits.Shr = (UINT8) USER_SHR_ADV;

                    if (CmdMsg.Ctx.Snap.Count == 0U) {
                        /* first */
                        pInfo->Op.Status.LastSvr = Svr;
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Snap_In");
                        if (CmdMsg.Ctx.Snap.Hdr > 0U) {
                            for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                /* snap in (hdr) */
                                pCtx->Snap.In.HdrData.Agc[i] = pCtx->HdrPreload.Agc[i];
                                pCtx->Snap.In.HdrData.Dgc[i] = pCtx->HdrPreload.Dgc[i];
                                pCtx->Snap.In.HdrData.Wgc[i] = pCtx->HdrPreload.Wgc[i];
                                pCtx->Snap.In.HdrData.Shr[i] = pCtx->HdrPreload.Shr[i];
                                pCtx->Snap.In.HdrData.DDgc[i] = pCtx->HdrPreload.DDgc[i];
                            }
                        }
                        /* snap in (ctrl) */
                        pCtx->Snap.In.Ctrl.Agc = Ctrl.Agc.Ctx.Reg;
                        pCtx->Snap.In.Ctrl.Dgc = Ctrl.Dgc.Ctx.Reg;
                        pCtx->Snap.In.Ctrl.Wgc = Ctrl.Wgc.Ctx.Reg;
                        pCtx->Snap.In.Ctrl.Shr = Ctrl.Shr.Ctx.Reg;
                        pCtx->Snap.In.Ctrl.DDgc = Ctrl.DDgc.Ctx.Reg;
                        pCtx->Snap.In.Ctrl.User = Ctrl.User.Ctx.Reg;
                        /* snap in (data) */
                        pCtx->Snap.In.Data.Agc = Data.Agc.Ctx.Reg;
                        pCtx->Snap.In.Data.Dgc = Data.Dgc.Ctx.Reg;
                        pCtx->Snap.In.Data.Wgc = Data.Wgc.Ctx.Reg;
                        pCtx->Snap.In.Data.Shr = Data.Shr.Ctx.Reg;
                        pCtx->Snap.In.Data.DDgc = Data.DDgc.Ctx.Reg;
                        pCtx->Snap.In.Data.User = Data.User.Ctx.Reg;

                        if (pInfo->Cfg.ShutterMode == (UINT32) IMAGE_SHR_SSR) {
                            /* hdr load */
                            if (Ctrl.Shr.Ctx.Bits.Hdr > 0U) {
                                FuncRetCode = AmbaImgSensorHAL_HdrShrLoad(ImageChanId, pCtx->Snap.In.HdrData.Shr);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            }
                            if (Ctrl.Wgc.Ctx.Bits.Hdr > 0U) {
                                FuncRetCode = AmbaImgSensorHAL_HdrWgcLoad(ImageChanId, pCtx->Snap.In.HdrData.Wgc);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            }
                            if (Ctrl.Dgc.Ctx.Bits.Hdr > 0U) {
                                FuncRetCode = AmbaImgSensorHAL_HdrDgcLoad(ImageChanId, pCtx->Snap.In.HdrData.Dgc);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            }
                            if (Ctrl.Agc.Ctx.Bits.Hdr > 0U) {
                                FuncRetCode = AmbaImgSensorHAL_HdrAgcLoad(ImageChanId, pCtx->Snap.In.HdrData.Agc);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            }

                            /* user write (first) */
                            FuncRetCode = AmbaImgSensorHAL_UserWrite(ImageChanId, Ctrl.User.Ctx.Reg, Data.User.Ctx.Reg);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* shr write */
                            FuncRetCode = AmbaImgSensorHAL_ShrWrite(ImageChanId, Ctrl.Shr.Ctx.Reg, Data.Shr.Ctx.Reg);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* wgc write */
                            FuncRetCode = AmbaImgSensorHAL_WgcWrite(ImageChanId, Ctrl.Wgc.Ctx.Reg, Data.Wgc.Ctx.Reg);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* dgc write */
                            FuncRetCode = AmbaImgSensorHAL_DgcWrite(ImageChanId, Ctrl.Dgc.Ctx.Reg, Data.Dgc.Ctx.Reg);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            /* agc write */
                            FuncRetCode = AmbaImgSensorHAL_AgcWrite(ImageChanId, Ctrl.Agc.Ctx.Reg, Data.Agc.Ctx.Reg);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }

                            if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                                /* hdr load */
                                if (Ctrl.DDgc.Ctx.Bits.Hdr > 0U) {
                                    FuncRetCode = AmbaImgSensorHAL_HdrDDgcLoad(ImageChanId, pCtx->Snap.In.HdrData.DDgc);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                }
                                /* ddgc write */
                                FuncRetCode = AmbaImgSensorHAL_DDgcWrite(ImageChanId, Ctrl.DDgc.Ctx.Reg, Data.DDgc.Ctx.Reg);
                                if (FuncRetCode != OK_UL) {
                                    RetCode = NG_UL;
                                }
                            }

                            /* aik cb */
                            if (CmdMsg.Ctx.Snap.Aik > 0U) {
                                AMBA_IMG_SENSOR_SYNC_AIK_CB_s AikCb2;
                                AikCb2.Ctx.Msg = 0ULL;
                                if (pInfo->Op.Ctrl.AdvAikCmd == 0U) {
                                    /* pre aik */
                                    if (CmdMsg.Ctx.Snap.PreAik > 0U) {
                                        AikCb2.Ctx.Bits.PreAik = 1U;
                                        FuncRetCode = AmbaImgSensorHAL_AikWrite(ImageChanId, AikCb2.Ctx.Msg, Data.User.Ctx.Bits.User);
                                        if (FuncRetCode == OK_UL) {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_Aik_C");
                                        } else {
                                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_Aik_C!");
                                            RetCode = NG_UL;
                                        }
                                        AikCb2.Ctx.Bits.PreAik = 0U;
                                    }
                                }
                                /* aik */
                                FuncRetCode = AmbaImgSensorHAL_AikWrite(ImageChanId, AikCb2.Ctx.Msg, Data.User.Ctx.Bits.User);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_C");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_C!");
                                     RetCode = NG_UL;
                                }
                            }

                            /* msc */
                            FuncRetCode = AmbaImgSensorHAL_MscWrite(ImageChanId, pInfo->Op.Status.LastSvr);
                            if (FuncRetCode == OK_UL) {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_C");
                            } else {
                                AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_C!");
                                RetCode = NG_UL;
                            }

                            /* sls cb */
                            if (CmdMsg.Ctx.Snap.Sls > 0U) {
                                FuncRetCode = AmbaImgSensorHAL_SlsWrite(ImageChanId, Data.User.Ctx.Reg);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Sls_C");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Sls_C!");
                                    RetCode = NG_UL;
                                }
                            }
                        } else {
                            /*
                             * ssg:
                             * nothing to do in here, will be processed and preset in first ssr frame.
                             */
                        }
                    } else {
                        /* last */
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Snap_Last");
                        if (CmdMsg.Ctx.Snap.Hdr > 0U) {
                            for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                /* snap Last (hdr) */
                                pCtx->Snap.Last.HdrData.Agc[i] = pCtx->HdrPreload.Agc[i];
                                pCtx->Snap.Last.HdrData.Dgc[i] = pCtx->HdrPreload.Dgc[i];
                                pCtx->Snap.Last.HdrData.Wgc[i] = pCtx->HdrPreload.Wgc[i];
                                pCtx->Snap.Last.HdrData.Shr[i] = pCtx->HdrPreload.Shr[i];
                                pCtx->Snap.Last.HdrData.DDgc[i] = pCtx->HdrPreload.DDgc[i];
                            }
                        }
                        /* snap last (ctrl) */
                        pCtx->Snap.Last.Ctrl.Agc = Ctrl.Agc.Ctx.Reg;
                        pCtx->Snap.Last.Ctrl.Dgc = Ctrl.Dgc.Ctx.Reg;
                        pCtx->Snap.Last.Ctrl.Wgc = Ctrl.Wgc.Ctx.Reg;
                        pCtx->Snap.Last.Ctrl.Shr = Ctrl.Shr.Ctx.Reg;
                        pCtx->Snap.Last.Ctrl.DDgc = Ctrl.DDgc.Ctx.Reg;
                        pCtx->Snap.Last.Ctrl.User = Ctrl.User.Ctx.Reg;
                        /* snap last (data) */
                        pCtx->Snap.Last.Data.Agc = Data.Agc.Ctx.Reg;
                        pCtx->Snap.Last.Data.Dgc = Data.Dgc.Ctx.Reg;
                        pCtx->Snap.Last.Data.Wgc = Data.Wgc.Ctx.Reg;
                        pCtx->Snap.Last.Data.Shr = Data.Shr.Ctx.Reg;
                        pCtx->Snap.Last.Data.DDgc = Data.DDgc.Ctx.Reg;
                        pCtx->Snap.Last.Data.User = Data.User.Ctx.Reg;

                        pCtx->Snap.Op.Bits.Count = CmdMsg.Ctx.Snap.Count;
                        if (pInfo->Cfg.ShutterMode != (UINT8) IMAGE_SHR_SSR) {
                            pCtx->Snap.Op.Bits.Count++;
                        }

                        FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Snap.Req));
                        if (FuncRetCode != OK_UL) {
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
 *  Amba image sensor sync direct
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] Mode sync command and message
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_Direct(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_s Ctrl;
    AMBA_IMG_SENSOR_HAL_DATA_s Data;

    UINT32 SsIndex;
    UINT32 SsStatus;

    AMBA_IMG_SENSOR_HAL_DATA_REG_s DataIn;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pSyncData != NULL) &&
        (AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctrl reset */
            Ctrl.Agc.Ctx.Reg = 0ULL;
            Ctrl.Dgc.Ctx.Reg = 0ULL;
            Ctrl.Wgc.Ctx.Reg = 0ULL;
            Ctrl.Shr.Ctx.Reg = 0ULL;
            Ctrl.DDgc.Ctx.Reg = 0ULL;
            Ctrl.User.Ctx.Reg = 0ULL;
            /* data get */
            Data.Agc.Ctx.Bits.Agc = pSyncData->Agc.Ctx.Bits.Agc;
            Data.Dgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.Dgc;
            Data.Wgc.Ctx.Bits.Wgc = pSyncData->Wgc.Ctx.Bits.Wgc;
            Data.Shr.Ctx.Bits.Shr = pSyncData->Shr.Ctx.Bits.Shr;
            Data.DDgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.DDgc;
            Data.User.Ctx.Bits.User = pSyncData->User.Ctx.Bits.User;

            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* shr check */
                if (CmdMsg.Ctx.Video.Hdr == 0U) {
                    FuncRetCode = AmbaImgSensorHAL_ShrCheck(ImageChanId, Data.Shr.Ctx.Bits.Shr, &SsIndex, &SsStatus);
                } else {
                    FuncRetCode = AmbaImgSensorHAL_HdrShrCheck(ImageChanId, pCtx->HdrPreload.Shr, &SsIndex, &SsStatus);
                }
                if (FuncRetCode == OK_UL) {
                    /* ssi (force update in first frame) */
                    Ctrl.Shr.Ctx.Bits.Ssi = (UINT8) SHR_SSI_SOF;
                    /* fll */
                    Ctrl.Shr.Ctx.Bits.Fll = (UINT8) SHR_SVR_NON_ADV;
                    /* hdr */
                    if (CmdMsg.Ctx.Video.Hdr > 0U) {
                        /* agc */
                        Ctrl.Agc.Ctx.Bits.Hdr = (UINT8) AGC_HDR_ON;
                        if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                            /* dgc (dsp) */
                            if (CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) {
                                Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                            } else {
                                Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                            }
                        } else {
                            /* dgc (sensor) */
                            Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        }
                        /* wgc (sensor) */
                        Ctrl.Wgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        /* shr */
                        Ctrl.Shr.Ctx.Bits.Hdr = (UINT8) SHR_HDR_ON;
                        /* dsp dgc */
                        if (CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) {
                            Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        } else {
                            Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                        }
                    }

                    /* rgb (gain) */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                        /* ddgc (dsp) */
                        /* adv dsp dgc (disable) */
                        Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_NONE;
                        /* no precondition in dsp dgc */
                        Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                        /* no precondition in shr */
                        Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        /* ddgc (dsp) id */
                        Ctrl.DDgc.Ctx.Bits.WbId = CmdMsg.Ctx.Video.WbId;
                    } else if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                        /* dgc (dsp) */
                        /* adv dgc (dsp) (disable) */
                        Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_NONE;
                        /* no precondition in dsp dgc */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                        /* no precondition in shr */
                        Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        /* dgc (dsp) id */
                        Ctrl.Dgc.Ctx.Bits.WbId = CmdMsg.Ctx.Video.WbId;
                    } else {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) AGC_SHR_NON_ADV;
                    }

                    /* aik */
                    if (CmdMsg.Ctx.Video.Aik > 0U) {
                        Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_NONE;
                    }
                    /* msc */
                    if (pInfo->Op.User.ShrDelay <= 2U) {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD;
                    } else {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD_SOF;
                    }

                    /* sls */
                    if (CmdMsg.Ctx.Video.Sls > 0U) {
                        /* adv sls */
                        Ctrl.Shr.Ctx.Bits.Sls = (UINT8) SHR_SLS_ADV;
                    }

                    /* wgc (sensor) */
                    Ctrl.Wgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Wgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;

                    /* dgc  */
                    Ctrl.Dgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Dgc.Ctx.Bits.Img = (UINT8) DGC_IMG_VIDEO;

                    /* agc */
                    Ctrl.Agc.Ctx.Bits.Sig = (UINT8) AGC_SIG_AGC;
                    Ctrl.Agc.Ctx.Bits.Shr = (UINT8) AGC_SHR_NON_ADV;

                    /* shr */
                    Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_BOTH;

                    /* svr */
                    Ctrl.Shr.Ctx.Bits.Svr = (UINT16) (SsIndex & 0xFFFFU);

                    /* ddgc */
                    Ctrl.DDgc.Ctx.Bits.Sig  = (UINT8) DGC_SIG_DGC;
                    Ctrl.DDgc.Ctx.Bits.Img  = (UINT8) DGC_IMG_VIDEO;

                    /* user */
                    if (CmdMsg.Ctx.Video.User > 0U) {
                        Ctrl.User.Ctx.Bits.Sig = (UINT8) USER_SIG_USER;
                    }
                    Ctrl.User.Ctx.Bits.Shr = (UINT8) USER_SHR_ADV;

                    /* hdr load*/
                    if (Ctrl.Shr.Ctx.Bits.Hdr > 0U) {
                        FuncRetCode = AmbaImgSensorHAL_HdrShrLoad(ImageChanId, pCtx->HdrPreload.Shr);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    }
                    if (Ctrl.Dgc.Ctx.Bits.Hdr > 0U) {
                        FuncRetCode = AmbaImgSensorHAL_HdrDgcLoad(ImageChanId, pCtx->HdrPreload.Dgc);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    }
                    if (Ctrl.Wgc.Ctx.Bits.Hdr > 0U) {
                        FuncRetCode = AmbaImgSensorHAL_HdrWgcLoad(ImageChanId, pCtx->HdrPreload.Wgc);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    }
                    if (Ctrl.Agc.Ctx.Bits.Hdr > 0U) {
                        FuncRetCode = AmbaImgSensorHAL_HdrAgcLoad(ImageChanId, pCtx->HdrPreload.Agc);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    }

                    /* user write (first) */
                    FuncRetCode = AmbaImgSensorHAL_UserWrite(ImageChanId, Ctrl.User.Ctx.Reg, Data.User.Ctx.Reg);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* svr */
                    if (pInfo->Op.User.SvrDelay > 2U) {
                        FuncRetCode = AmbaImgSensorHAL_SvrWrite(ImageChanId, Ctrl.Shr.Ctx.Reg, Data.Shr.Ctx.Reg);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    }
                    /* shr write */
                    FuncRetCode = AmbaImgSensorHAL_ShrWrite(ImageChanId, Ctrl.Shr.Ctx.Reg, Data.Shr.Ctx.Reg);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* wgc write */
                    FuncRetCode = AmbaImgSensorHAL_WgcWrite(ImageChanId, Ctrl.Wgc.Ctx.Reg, Data.Wgc.Ctx.Reg);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* dgc write */
                    FuncRetCode = AmbaImgSensorHAL_DgcWrite(ImageChanId, Ctrl.Dgc.Ctx.Reg, Data.Dgc.Ctx.Reg);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* agc write */
                    FuncRetCode = AmbaImgSensorHAL_AgcWrite(ImageChanId, Ctrl.Agc.Ctx.Reg, Data.Agc.Ctx.Reg);
                    if (FuncRetCode != OK_UL) {
                        RetCode = NG_UL;
                    }
                    /* ddgc check? */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                        /* hdr load */
                        if (Ctrl.DDgc.Ctx.Bits.Hdr > 0U) {
                            FuncRetCode = AmbaImgSensorHAL_HdrDDgcLoad(ImageChanId, pCtx->HdrPreload.DDgc);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                        /* ddgc write */
                        FuncRetCode = AmbaImgSensorHAL_DDgcWrite(ImageChanId, Ctrl.DDgc.Ctx.Reg, Data.DDgc.Ctx.Reg);
                        if (FuncRetCode != OK_UL) {
                            RetCode = NG_UL;
                        }
                    }

                    /* reg in */
                    DataIn.Shr = Data.Shr.Ctx.Bits.Shr;
                    DataIn.Wgc = Data.Wgc.Ctx.Bits.Wgc;
                    DataIn.Dgc = Data.Dgc.Ctx.Bits.Dgc;
                    DataIn.Agc = Data.Agc.Ctx.Bits.Agc;
                    DataIn.DDgc = Data.DDgc.Ctx.Bits.Dgc;
                    DataIn.User = Data.User.Ctx.Bits.User;

                    FuncRetCode = AmbaImgSensorHAL_DataIn(ImageChanId, &DataIn);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Reg_In");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Reg_In!");
                        RetCode = NG_UL;
                    }

                    FuncRetCode = AmbaImgSensorHAL_ActDataIn(ImageChanId, &DataIn);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Act_In");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Act_In!");
                    }

                    FuncRetCode = AmbaImgSensorHAL_RingPut(ImageChanId, &DataIn, 0U);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put0");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put0!");
                    }

                    FuncRetCode = AmbaImgSensorHAL_RingPut(ImageChanId, &DataIn, 1U);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put1");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put1!");
                    }

                    FuncRetCode = AmbaImgSensorHAL_RingPut(ImageChanId, &DataIn, 2U);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put2");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Ring_Put2!");
                    }

                    /* aik cb */
                    if (CmdMsg.Ctx.Video.Aik > 0U) {
                        AMBA_IMG_SENSOR_SYNC_AIK_CB_s AikCb2;
                        AikCb2.Ctx.Msg = 0ULL;
                        if (pInfo->Op.Ctrl.AdvAikCmd == 0U) {
                            /* pre aik */
                            if (CmdMsg.Ctx.Video.PreAik > 0U) {
                                AikCb2.Ctx.Bits.PreAik = 1U;
                                FuncRetCode = AmbaImgSensorHAL_AikWrite(ImageChanId, AikCb2.Ctx.Msg, Data.User.Ctx.Bits.User);
                                if (FuncRetCode == OK_UL) {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_Aik_C");
                                } else {
                                    AmbaImgSensorHAL_TimingMarkPut(VinId, "Pre_Aik_C!");
                                    RetCode = NG_UL;
                                }
                                AikCb2.Ctx.Bits.PreAik = 0U;
                            }
                        }
                        /* aik */
                        FuncRetCode = AmbaImgSensorHAL_AikWrite(ImageChanId, AikCb2.Ctx.Msg, Data.User.Ctx.Bits.User);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_C");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Aik_C!");
                             RetCode = NG_UL;
                        }
                    }

                    /* msc */
                    FuncRetCode = AmbaImgSensorHAL_MscWrite(ImageChanId, pInfo->Op.Status.LastSvr);
                    if (FuncRetCode == OK_UL) {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_C");
                    } else {
                        AmbaImgSensorHAL_TimingMarkPut(VinId, "Msc_C!");
                        RetCode = NG_UL;
                    }

                    /* sls cb */
                    if (CmdMsg.Ctx.Video.Sls > 0U) {
                        FuncRetCode = AmbaImgSensorHAL_SlsWrite(ImageChanId, Data.User.Ctx.Bits.User);
                        if (FuncRetCode == OK_UL) {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sls_C");
                        } else {
                            AmbaImgSensorHAL_TimingMarkPut(VinId, "Sls_C!");
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
 *  Amba image sensor sync slow shutter change
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] SsIndex slow shutter index
 *  @param[in] Mode sync command and message
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_SsChange(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 SsIndex, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_s Ctrl;
    AMBA_IMG_SENSOR_HAL_DATA_s Data;

    AMBA_IMG_SENSOR_HAL_CTRL_REG_s  CtrlReg;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s  DataReg;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pSyncData != NULL) &&
        (AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctrl reset */
            Ctrl.Agc.Ctx.Reg = 0ULL;
            Ctrl.Dgc.Ctx.Reg = 0ULL;
            Ctrl.Wgc.Ctx.Reg = 0ULL;
            Ctrl.Shr.Ctx.Reg = 0ULL;
            Ctrl.DDgc.Ctx.Reg = 0ULL;
            Ctrl.User.Ctx.Reg = 0ULL;
            /* data get */
            Data.Agc.Ctx.Bits.Agc = pSyncData->Agc.Ctx.Bits.Agc;
            Data.Dgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.Dgc;
            Data.Wgc.Ctx.Bits.Wgc = pSyncData->Wgc.Ctx.Bits.Wgc;
            Data.Shr.Ctx.Bits.Shr = pSyncData->Shr.Ctx.Bits.Shr;
            Data.DDgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.DDgc;
            Data.User.Ctx.Bits.User = pSyncData->User.Ctx.Bits.User;

            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* semaphore take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Rdy), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* ssi, advance or one frame advance */
                    Ctrl.Shr.Ctx.Bits.Ssi = (UINT8) SHR_SSI_ADV;
                    if (pInfo->Op.Ctrl.AdvSsiCmd > 1U) {
                        /* ack needed, if ssi is one frame advance */
                        Ctrl.Shr.Ctx.Bits.Ack = (UINT8) SHR_ACK_SSI_SVR;
                    }
                    /* fll */
                    if (pInfo->Op.User.SvrDelay > 2U) {
                        /* adv svr (one frame advance) */
                        Ctrl.Shr.Ctx.Bits.Fll = (UINT8) SHR_SVR_ADV;
                        /* ack needed, if svr is one frame advance */
                        Ctrl.Shr.Ctx.Bits.Ack = (UINT8) SHR_ACK_SSI_SVR;
                    }
                    /* hdr */
                    if (CmdMsg.Ctx.Video.Hdr > 0U) {
                        /* agc */
                        Ctrl.Agc.Ctx.Bits.Hdr = (UINT8) AGC_HDR_ON;
                        if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                            /* dgc (dsp) */
                            if (CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) {
                                Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                            } else {
                                Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                            }
                        } else {
                            /* dgc (sensor) */
                            Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        }
                        /* wgc (sensor) */
                        Ctrl.Wgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        /* shr */
                        Ctrl.Shr.Ctx.Bits.Hdr = (UINT8) SHR_HDR_ON;
                        /* dsp dgc */
                        if (CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) {
                            Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        } else {
                            Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                        }
                    }

                    /* rgb (gain) */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                        /* ddgc (dsp) */
                        if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                            /* adv dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                            /* no shr precondition in dgc */
                            Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                            /* dsp dgc precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                        } else {
                            /* non-adv dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                            /* shr precondition in dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                            /* no dsp dgc preconditon in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        }
                        /* ddgc (dsp) */
                        if (pInfo->Op.User.SvrDelay > 2U) {
                            Ctrl.DDgc.Ctx.Bits.Svr = (UINT8) DGC_SVR_ADV;
                        }
                        if (pInfo->Op.Ctrl.AdvSsiCmd > 1U) {
                            Ctrl.DDgc.Ctx.Bits.Ssi = (UINT8) DGC_SSI_ADV;
                        }
                        /* ddgc (dsp) id */
                        Ctrl.DDgc.Ctx.Bits.WbId = CmdMsg.Ctx.Video.WbId;
                        /* fe ddgc (dsp)? */
                        if ((CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) &&
                            (CmdMsg.Ctx.Video.Hdr > 0U)) {
                            if ((pInfo->Op.Ctrl.AdvAikCmd > 0U) ||
                                (pInfo->Op.Ctrl.PreAikCmd > 0U)) {
                                /* adv dsp dgc */
                                Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                                /* no shr percondition in dgc */
                                Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                                /* dsp dgc precondition in shr */
                                Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                            }
                        }
                    } else if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                        /* dgc (dsp) */
                        if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                            /* adv dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                            /* no shr percondition in dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                            /* dgc (dsp) precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                        } else {
                            /* non-adv dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                            /* shr precondition in dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                            /* no dgc (dsp) precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        }
                        /* dgc (dsp) */
                        if (pInfo->Op.User.SvrDelay > 2U) {
                            Ctrl.Dgc.Ctx.Bits.Svr = (UINT8) DGC_SVR_ADV;
                        }
                        if (pInfo->Op.Ctrl.AdvSsiCmd > 1U) {
                            Ctrl.Dgc.Ctx.Bits.Ssi = (UINT8) DGC_SSI_ADV;
                        }
                        /* dgc (dsp) id */
                        Ctrl.Dgc.Ctx.Bits.WbId = CmdMsg.Ctx.Video.WbId;
                        /* fe dgc (dsp)? */
                        if ((CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) &&
                            (CmdMsg.Ctx.Video.Hdr > 0U)) {
                            if ((pInfo->Op.Ctrl.AdvAikCmd > 0U) ||
                                (pInfo->Op.Ctrl.PreAikCmd > 0U)) {
                                /* adv dgc (dsp) */
                                Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                                /* no shr percondition in dgc (dsp) */
                                Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                                /* dgc (dsp) precondition in shr */
                                Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                            }
                        }
                    } else {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                    }

                    /* aik */
                    if (CmdMsg.Ctx.Video.Aik > 0U) {
                        if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                            /* adv aik */
                            Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV;
                        } else {
                            if (CmdMsg.Ctx.Video.PreAik > 0U) {
                                /* pre aik & aik */
                                Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV_SOF;
                            } else {
                                /* aik */
                                Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_SOF;
                            }
                        }
                    }

                    /* msc */
                    if (pInfo->Op.User.ShrDelay <= 2U) {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD;
                    } else {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD_SOF;
                    }

                    /* sls */
                    if (CmdMsg.Ctx.Video.Sls > 0U) {
                        /* adv sls */
                        Ctrl.Shr.Ctx.Bits.Sls = (UINT8) SHR_SLS_ADV;
                    }

                    /* wgc (sensor) */
                    Ctrl.Wgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Wgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;

                    /* dgc  */
                    Ctrl.Dgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Dgc.Ctx.Bits.Img = (UINT8) DGC_IMG_VIDEO;

                    /* agc */
                    Ctrl.Agc.Ctx.Bits.Sig = (UINT8) AGC_SIG_AGC;
                    Ctrl.Agc.Ctx.Bits.Shr = (UINT8) AGC_SHR_ADV;

                    /* shr */
                    Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_BOTH;

                    /* svr */
                    Ctrl.Shr.Ctx.Bits.Svr = (UINT16) SsIndex;

                    /* ddgc */
                    Ctrl.DDgc.Ctx.Bits.Sig  = (UINT8) DGC_SIG_DGC;
                    Ctrl.DDgc.Ctx.Bits.Img  = (UINT8) DGC_IMG_VIDEO;

                    /* user */
                    if (CmdMsg.Ctx.Video.User > 0U) {
                        Ctrl.User.Ctx.Bits.Sig = (UINT8) USER_SIG_USER;
                    }
                    Ctrl.User.Ctx.Bits.Shr = (UINT8) USER_SHR_ADV;

                    /* ack mode? */
                    if (CmdMsg.Ctx.Video.Cmd == (UINT8) SYNC_CMD_ACK) {
                        Ctrl.Agc.Ctx.Bits.Ack = (UINT8) AGC_ACK_ON;
                    } else {
                        Ctrl.Agc.Ctx.Bits.Ack = (UINT8) AGC_ACK_NONE;
                    }

                    /* ctrl put */
                    CtrlReg.Agc = Ctrl.Agc.Ctx.Reg;
                    CtrlReg.Dgc = Ctrl.Dgc.Ctx.Reg;
                    CtrlReg.Wgc = Ctrl.Wgc.Ctx.Reg;
                    CtrlReg.Shr = Ctrl.Shr.Ctx.Reg;
                    CtrlReg.DDgc = Ctrl.DDgc.Ctx.Reg;
                    CtrlReg.User = Ctrl.User.Ctx.Reg;

                    /* data put */
                    DataReg.Agc = Data.Agc.Ctx.Reg;
                    DataReg.Dgc = Data.Dgc.Ctx.Reg;
                    DataReg.Wgc = Data.Wgc.Ctx.Reg;
                    DataReg.Shr = Data.Shr.Ctx.Reg;
                    DataReg.DDgc = Data.DDgc.Ctx.Reg;
                    DataReg.User = Data.User.Ctx.Reg;

                    /* fifo put */
                    FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &CtrlReg, &DataReg, Mode);
                    if (FuncRetCode == OK_UL) {
                        if (Ctrl.Shr.Ctx.Bits.Ack == (UINT8) SHR_ACK_SSI_SVR) {
                            /* adv svr wait */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.SsiSvr), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                        if (Ctrl.Agc.Ctx.Bits.Ack == (UINT8) AGC_ACK_ON) {
                            /* agc ack wait */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.Agc), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                    } else {
                        RetCode = NG_UL;
                    }

                    /* semaphore give */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Rdy));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* semaphore fail */
                    RetCode = NG_UL;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image sensor sync slow shutter stay
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] SsIndex slow shutter index
 *  @param[in] Mode sync command and message
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_SsStay(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 SsIndex, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_s Ctrl;
    AMBA_IMG_SENSOR_HAL_DATA_s Data;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    AMBA_IMG_SENSOR_HAL_CTRL_REG_s CtrlReg;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s DataReg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pSyncData != NULL) &&
        (AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctrl reset */
            Ctrl.Agc.Ctx.Reg = 0ULL;
            Ctrl.Dgc.Ctx.Reg = 0ULL;
            Ctrl.Wgc.Ctx.Reg = 0ULL;
            Ctrl.Shr.Ctx.Reg = 0ULL;
            Ctrl.DDgc.Ctx.Reg = 0ULL;
            Ctrl.User.Ctx.Reg = 0ULL;
            /* data get */
            Data.Agc.Ctx.Bits.Agc = pSyncData->Agc.Ctx.Bits.Agc;
            Data.Dgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.Dgc;
            Data.Wgc.Ctx.Bits.Wgc = pSyncData->Wgc.Ctx.Bits.Wgc;
            Data.Shr.Ctx.Bits.Shr = pSyncData->Shr.Ctx.Bits.Shr;
            Data.DDgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.DDgc;
            Data.User.Ctx.Bits.User = pSyncData->User.Ctx.Bits.User;

            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* semaphore take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Rdy), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* ssi (none) */
                    Ctrl.Shr.Ctx.Bits.Ssi = (UINT8) SHR_SSI_NONE;
                    /* fll (none) */
                    Ctrl.Shr.Ctx.Bits.Fll = (UINT8) SHR_SVR_NON_ADV;
                    /* hdr */
                    if (CmdMsg.Ctx.Video.Hdr > 0U) {
                        /* agc */
                        Ctrl.Agc.Ctx.Bits.Hdr = (UINT8) AGC_HDR_ON;
                        if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                            /* dgc (dsp) */
                            if (CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) {
                                Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                            } else {
                                Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                            }
                        } else {
                            /* dgc (sensor) */
                            Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        }
                        /* wgc (sensor) */
                        Ctrl.Wgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        /* shr */
                        Ctrl.Shr.Ctx.Bits.Hdr = (UINT8) SHR_HDR_ON;
                        /* dsp dgc */
                        if (CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) {
                            Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                        } else {
                            Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                        }
                    }

                    /* rgb (gain) */
                    if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                        /* ddgc (dsp) */
                        if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                            /* adv dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                            /* no shr precondition in dgc */
                            Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                            /* dsp dgc precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                        } else {
                            /* non-adv dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                            /* shr precondition in dsp dgc */
                            Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                            /* no dsp dgc preconditon in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        }
                        /* ddgc (dsp) id */
                        Ctrl.DDgc.Ctx.Bits.WbId = CmdMsg.Ctx.Video.WbId;
                        /* fe dgc (dsp)? */
                        if ((CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) &&
                            (CmdMsg.Ctx.Video.Hdr > 0U)) {
                            if ((pInfo->Op.Ctrl.AdvAikCmd > 0U) ||
                                (pInfo->Op.Ctrl.PreAikCmd > 0U)) {
                                /* adv dsp dgc */
                                Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                                /* no shr percondition in dgc */
                                Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                                /* dsp dgc precondition in shr */
                                Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                            }
                        }
                    } else if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                        /* dgc (dsp) */
                        if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                            /* adv dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                            /* no shr percondition in dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                            /* dgc (dsp) precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                        } else {
                            /* non-adv dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                            /* shr precondition in dgc (dsp) */
                            Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                            /* no dgc (dsp) precondition in shr */
                            Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                        }
                        /* dgc (dsp) id */
                        Ctrl.Dgc.Ctx.Bits.WbId = CmdMsg.Ctx.Video.WbId;
                        /* fe dgc (dsp)? */
                        if ((CmdMsg.Ctx.Video.WbId == (UINT8) WB_ID_FE) &&
                            (CmdMsg.Ctx.Video.Hdr > 0U)) {
                            if ((pInfo->Op.Ctrl.AdvAikCmd > 0U) ||
                                (pInfo->Op.Ctrl.PreAikCmd > 0U)) {
                                /* adv dgc (dsp) */
                                Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                                /* no shr percondition in dgc (dsp) */
                                Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                                /* dgc (dsp) precondition in shr */
                                Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                            }
                        }
                    } else {
                        /* dgc (sensor) */
                        Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                    }

                    /* aik */
                    if (CmdMsg.Ctx.Video.Aik > 0U) {
                        if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                            /* adv aik */
                            Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV;
                        } else {
                            if (CmdMsg.Ctx.Video.PreAik > 0U) {
                                /* pre aik & aik */
                                Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV_SOF;
                            } else {
                                /* aik */
                                Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_SOF;
                            }
                        }
                    }

                    /* msc */
                    if (pInfo->Op.User.ShrDelay <= 2U) {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD; /* enabled for time division */
                    } else {
                        Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD_SOF; /* enabled for time division */
                    }

                    /* sls */
                    if (CmdMsg.Ctx.Video.Sls > 0U) {
                        /* adv sls */
                        Ctrl.Shr.Ctx.Bits.Sls = (UINT8) SHR_SLS_ADV;
                    }

                    /* wgc (sensor) */
                    Ctrl.Wgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Wgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;

                    /* dgc  */
                    Ctrl.Dgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                    Ctrl.Dgc.Ctx.Bits.Img = (UINT8) DGC_IMG_VIDEO;

                    /* agc */
                    Ctrl.Agc.Ctx.Bits.Sig = (UINT8) AGC_SIG_AGC;
                    Ctrl.Agc.Ctx.Bits.Shr = (UINT8) AGC_SHR_ADV;

                    /* shr */
                    if (CmdMsg.Ctx.Video.Force > 0U) {
                        Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_BOTH;
                        if (pInfo->Op.User.SvrDelay > 2U) {
                            /* adv svr */
                            Ctrl.Shr.Ctx.Bits.Fll = (UINT8) SHR_SVR_ADV;
                            Ctrl.Shr.Ctx.Bits.Ack = (UINT8) SHR_ACK_SSI_SVR;
                        }
                    } else {
                        Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_SHR;
                    }

                    /* svr */
                    Ctrl.Shr.Ctx.Bits.Svr = (UINT16) SsIndex;

                    /* ddgc */
                    Ctrl.DDgc.Ctx.Bits.Sig  = (UINT8) DGC_SIG_DGC;
                    Ctrl.DDgc.Ctx.Bits.Img  = (UINT8) DGC_IMG_VIDEO;

                    /* user */
                    if (CmdMsg.Ctx.Video.User > 0U) {
                        Ctrl.User.Ctx.Bits.Sig = (UINT8) USER_SIG_USER;
                    }
                    Ctrl.User.Ctx.Bits.Shr = (UINT8) USER_SHR_ADV;

                    /* ack mode? */
                    if (CmdMsg.Ctx.Video.Cmd == (UINT8) SYNC_CMD_ACK) {
                        Ctrl.Agc.Ctx.Bits.Ack = (UINT8) AGC_ACK_ON;
                    } else {
                        Ctrl.Agc.Ctx.Bits.Ack = (UINT8) AGC_ACK_NONE;
                    }

                    /* ctrl put */
                    CtrlReg.Agc = Ctrl.Agc.Ctx.Reg;
                    CtrlReg.Dgc = Ctrl.Dgc.Ctx.Reg;
                    CtrlReg.Wgc = Ctrl.Wgc.Ctx.Reg;
                    CtrlReg.Shr = Ctrl.Shr.Ctx.Reg;
                    CtrlReg.DDgc = Ctrl.DDgc.Ctx.Reg;
                    CtrlReg.User = Ctrl.User.Ctx.Reg;

                    /* data put */
                    DataReg.Agc = Data.Agc.Ctx.Reg;
                    DataReg.Dgc = Data.Dgc.Ctx.Reg;
                    DataReg.Wgc = Data.Wgc.Ctx.Reg;
                    DataReg.Shr = Data.Shr.Ctx.Reg;
                    DataReg.DDgc = Data.DDgc.Ctx.Reg;
                    DataReg.User = Data.User.Ctx.Reg;

                    /* fifo put */
                    FuncRetCode = AmbaImgSensorSync_FifoPut(ImageChanId, &CtrlReg, &DataReg, Mode);
                    if (FuncRetCode == OK_UL) {
                        if (Ctrl.Shr.Ctx.Bits.Ack == (UINT8) SHR_ACK_SSI_SVR) {
                            /* adv svr wait */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.SsiSvr), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                        if (Ctrl.Agc.Ctx.Bits.Ack == (UINT8) AGC_ACK_ON) {
                            /* agc ack wait */
                            FuncRetCode = AmbaKAL_SemaphoreTake(&(pCtx->Fifo.Ack.Agc), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                        }
                    } else {
                        RetCode = NG_UL;
                    }

                    /* semaphore give */
                    FuncRetCode = AmbaKAL_SemaphoreGive(&(pCtx->Fifo.Rdy));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    /* semaphore fail */
                    RetCode = NG_UL;
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba image sensor sync write
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] Mode sync command and message
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaImgSensorSync_Write(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;
    UINT32 ChainId;

    const AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    UINT32 SsIndex;
    UINT32 SsStatus;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pSyncData != NULL) &&
        (AmbaImgSensorSync_Chan[VinId] != NULL) &&
        (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
        if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
            /* ctx get */
            pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
            /* info get */
            FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
            if (FuncRetCode == OK_UL) {
                /* shr check */
                if (CmdMsg.Ctx.Video.Hdr == 0U) {
                    FuncRetCode = AmbaImgSensorHAL_ShrCheck(ImageChanId, (UINT64) pSyncData->Shr.Ctx.Bits.Shr, &SsIndex, &SsStatus);
                } else {
                    FuncRetCode = AmbaImgSensorHAL_HdrShrCheck(ImageChanId, pCtx->HdrPreload.Shr, &SsIndex, &SsStatus);
                }
                if (FuncRetCode == OK_UL) {
                    switch (SsStatus) {
                        case (UINT32) SS_STATE_ENTER:
                        case (UINT32) SS_STATE_MORE:
                            switch (pInfo->Op.User.SsType) {
                                case (UINT32) IMAGE_SS_TYPE_0:
                                    FuncRetCode = AmbaImgSensorSync_SsChange(ImageChanId, pSyncData, SsIndex, Mode);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                    break;
                                case (UINT32) IMAGE_SS_TYPE_1:
                                    //FuncRetCode = AmbaImgSensorSync_SsMoreTwoStage(ImageChanId, pSyncData, SsIndex, Mode);
                                    //if (FuncRetCode != OK_UL) {
                                        //RetCode = NG_UL;
                                    //}
                                    break;
                                default:
                                    RetCode = NG_UL;
                                    break;
                            }
                            break;
                        case (UINT32) SS_STATE_LEAVE:
                        case (UINT32) SS_STATE_LESS:
                            switch (pInfo->Op.User.SsType) {
                                case (UINT32) IMAGE_SS_TYPE_0:
                                    FuncRetCode = AmbaImgSensorSync_SsChange(ImageChanId, pSyncData, SsIndex, Mode);
                                    if (FuncRetCode != OK_UL) {
                                        RetCode = NG_UL;
                                    }
                                    break;
                                case (UINT32) IMAGE_SS_TYPE_1:
                                    //FuncRetCode = AmbaImgSensorSync_SsLessTwoStage(ImageChanId, pSyncData, SsIndex, Mode);
                                    //if (FuncRetCode != OK_UL) {
                                        //RetCode = NG_UL;
                                    //}
                                    break;
                                default:
                                    RetCode = NG_UL;
                                    break;
                            }
                            break;
                        case (UINT32) SS_STATE_NONE:
                        case (UINT32) SS_STATE_STILL:
                            FuncRetCode = AmbaImgSensorSync_SsStay(ImageChanId, pSyncData, SsIndex, Mode);
                            if (FuncRetCode != OK_UL) {
                                RetCode = NG_UL;
                            }
                            break;
                        default:
                            RetCode = NG_UL;
                            break;
                    }
                }
            } else {
                /* info fail */
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
 *  Amba image sensor sync request
 *  @param[in] ImageChanId image channel id
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] Mode sync command and message
 *  @return error code
 */
UINT32 AmbaImgSensorSync_Request(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 VinId;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;

    VinId = ImageChanId.Ctx.Bits.VinId;

    CmdMsg.Ctx.Data = Mode;

    if (pSyncData != NULL) {
        FuncRetCode = AmbaImgSensorHAL_TimingMarkChk(VinId);
        if (FuncRetCode == OK_UL) {
            char str[11];
            /* Shr */
            str[0] = 's';str[1] = ' ';
            var_utoa(pSyncData->Shr.Ctx.Bits.Shr, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            /* agc */
            str[0] = 'a';
            var_utoa(pSyncData->Agc.Ctx.Bits.Agc, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            /* dgc */
            str[0] = 'd';
            var_utoa(pSyncData->Dgc.Ctx.Bits.Dgc, &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            /* wgc */
            str[0] = 'w';
            var_utoa((UINT32) (pSyncData->Wgc.Ctx.Bits.Wgc & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            /* ddgc */
            str[0] = 'd';
            var_utoa(pSyncData->Dgc.Ctx.Bits.DDgc, &(str[2]), 10U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
            /* user */
            str[0] = 'u';
            var_utoa((UINT32) (pSyncData->User.Ctx.Bits.User & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) VAR_LEADING_SPACE);
            AmbaImgSensorHAL_TimingMarkPut(VinId, str);
        }

        switch ((UINT8) CmdMsg.Ctx.Com.Cmd) {
            case (UINT8) SYNC_CMD_DIRECT:
                /* direct */
                FuncRetCode = AmbaImgSensorSync_Direct(ImageChanId, pSyncData, Mode);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
                break;
            case (UINT8) SYNC_CMD_ACK:
            case (UINT8) SYNC_CMD_FIFO:
                /* fifo & ack */
                FuncRetCode = AmbaImgSensorSync_Write(ImageChanId, pSyncData, Mode);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
                break;
            case (UINT8) SYNC_CMD_SNAP:
                /* snap */
                FuncRetCode = AmbaImgSensorSync_Snap(ImageChanId, pSyncData, Mode);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
                break;
            case (UINT8) SYNC_CMD_HDR_PRELOAD:
                /* hdr preload */
                FuncRetCode = AmbaImgSensorSync_HdrPreload(ImageChanId, pSyncData, Mode);
                if (FuncRetCode != OK_UL) {
                    RetCode = NG_UL;
                }
                break;
             default:
                /* unknow command */
                RetCode = NG_UL;
                break;
        }
    }

    return RetCode;
}

/**
 *  Amba image sensor sync snap sequence
 *  @param[in] ImageChanId image channel id
 *  @param[in] pPort pointer to the snap port
 *  @param[in] pSyncData pointer to the sync data
 *  @param[in] Mode sync command and message
 *  @return error code
 */
UINT32 AmbaImgSensorSync_SnapSequence(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s *pPort, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 VinId;
    UINT32 ChainId;

    AMBA_IMG_SENSOR_SYNC_CONTEXT_s *pCtx;
    AMBA_IMG_SENSOR_HAL_INFO_s *pInfo;

    AMBA_IMG_SENSOR_HAL_CTRL_s Ctrl;
    AMBA_IMG_SENSOR_HAL_DATA_s Data;

    UINT32 Svr;

    AMBA_IMG_SENSOR_SYNC_CMD_MSG_s CmdMsg;
    AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s *pList;

    VinId = ImageChanId.Ctx.Bits.VinId;
    ChainId = ImageChanId.Ctx.Bits.ChainId;

    CmdMsg.Ctx.Data = Mode;

    if ((pPort != NULL) &&
        (pSyncData != NULL)) {
        if (CmdMsg.Ctx.Snap.Count > 0U) {
            /* ctrl reset */
            Ctrl.Agc.Ctx.Reg = 0ULL;
            Ctrl.Dgc.Ctx.Reg = 0ULL;
            Ctrl.Wgc.Ctx.Reg = 0ULL;
            Ctrl.Shr.Ctx.Reg = 0ULL;
            Ctrl.DDgc.Ctx.Reg = 0ULL;
            Ctrl.User.Ctx.Reg = 0ULL;
            /* data get */
            Data.Agc.Ctx.Bits.Agc = pSyncData->Agc.Ctx.Bits.Agc;
            Data.Dgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.Dgc;
            Data.Wgc.Ctx.Bits.Wgc = pSyncData->Wgc.Ctx.Bits.Wgc;
            Data.Shr.Ctx.Bits.Shr = pSyncData->Shr.Ctx.Bits.Shr;
            Data.DDgc.Ctx.Bits.Dgc = pSyncData->Dgc.Ctx.Bits.DDgc;
            Data.User.Ctx.Bits.User = pSyncData->User.Ctx.Bits.User;

            if ((AmbaImgSensorSync_Chan[VinId] != NULL) &&
                (AmbaImgSensorSync_Chan[VinId][ChainId].Magic == 0xCafeU)) {
                if (AmbaImgSensorSync_Chan[VinId][ChainId].pCtx != NULL) {
                    /* ctx get */
                    pCtx = AmbaImgSensorSync_Chan[VinId][ChainId].pCtx;
                    /* info get */
                    FuncRetCode = AmbaImgSensorHAL_InfoGet(ImageChanId, &pInfo);
                    if (FuncRetCode == OK_UL) {
                        /* svr check */
                        if (CmdMsg.Ctx.Snap.Hdr == 0U) {
                            FuncRetCode = AmbaImgSensorHAL_SvrCheck(ImageChanId, Data.Shr.Ctx.Bits.Shr, &Svr);
                        } else {
                            FuncRetCode = AmbaImgSensorHAL_HdrSvrCheck(ImageChanId, pCtx->HdrPreload.Shr, &Svr);
                        }
                        if (FuncRetCode == OK_UL) {
                            /* ssi (none) */
                            Ctrl.Shr.Ctx.Bits.Ssi = (UINT8) SHR_SSI_NONE;
                            /* fll (not support one frame advance in snap,
                               needed enable auto fll of sensor if svr is one frame advance) */
                            Ctrl.Shr.Ctx.Bits.Fll = (UINT8) SHR_SVR_NON_ADV;
                            /* hdr */
                            if (CmdMsg.Ctx.Snap.Hdr > 0U) {
                                /* agc */
                                Ctrl.Agc.Ctx.Bits.Hdr = (UINT8) AGC_HDR_ON;
                                if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                                    /* dgc (dsp) */
                                    Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                                } else {
                                    /* dgc (sensor) */
                                    Ctrl.Dgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                                }
                                /* wgc (sensor) */
                                Ctrl.Wgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_ON;
                                /* shr */
                                Ctrl.Shr.Ctx.Bits.Hdr = (UINT8) SHR_HDR_ON;
                                /* dsp dgc */
                                Ctrl.DDgc.Ctx.Bits.Hdr = (UINT8) DGC_HDR_OFF;
                            }

                            /* rgb (gain) */
                            if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_BOTH) {
                                /* dgc (sensor) */
                                Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                                /* ddgc (dsp), DGC_IMG_STILL, no write2dsp */
                                if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                                    /* adv dsp dgc */
                                    Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                                    /* no shr precondition in dgc */
                                    Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                                    /* dsp dgc precondition in shr */
                                    Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                                } else {
                                    /* non-adv dsp dgc */
                                    Ctrl.DDgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                                    /* shr precondition in dsp dgc */
                                    Ctrl.DDgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                                    /* no dsp dgc preconditon in shr */
                                    Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                                }
                            } else if (pInfo->Op.User.DgcType == (UINT32) IMAGE_DGC_DSP) {
                                /* dgc (dsp), DGC_IMG_STILL, no write2dsp */
                                if (pInfo->Op.Ctrl.AdvRgbCmd > 0U) {
                                    /* adv dgc (dsp) */
                                    Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_ADV;
                                    /* no shr percondition in dgc (dsp) */
                                    Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_NON_ADV;
                                    /* dgc (dsp) precondition in shr */
                                    Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_ADV;
                                } else {
                                    /* non-adv dgc (dsp) */
                                    Ctrl.Dgc.Ctx.Bits.Put = (UINT8) DGC_PUT_SOF;
                                    /* shr precondition in dgc (dsp) */
                                    Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                                    /* no dgc (dsp) precondition in shr */
                                    Ctrl.Shr.Ctx.Bits.Rgb = (UINT8) SHR_RGB_NON_ADV;
                                }
                            } else {
                                /* dgc (sensor) */
                                Ctrl.Dgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;
                            }

                            /* aik */
                            if (CmdMsg.Ctx.Snap.Aik > 0U) {
                                if (pInfo->Op.Ctrl.AdvAikCmd > 0U) {
                                    /* adv aik (disable) */
                                    Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV;
                                } else {
                                    if (CmdMsg.Ctx.Snap.PreAik > 0U) {
                                        /* pre aik & aik (disable) */
                                        Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_ADV_SOF;
                                    } else {
                                        /* aik (disable) */
                                        Ctrl.Shr.Ctx.Bits.Aik = (UINT8) SHR_AIK_SOF;
                                    }
                                }
                            }

                            /* msc */
                            if (pInfo->Op.User.ShrDelay <= 2U) {
                                Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD;
                            } else {
                                Ctrl.Shr.Ctx.Bits.Msc = (UINT8) SHR_MSC_UPD_SOF;
                            }

                            /* sls */
                            if (CmdMsg.Ctx.Snap.Sls > 0U) {
                                /* adv sls */
                                Ctrl.Shr.Ctx.Bits.Sls = (UINT8) SHR_SLS_ADV;
                            }

                            /* wgc (sensor) */
                            Ctrl.Wgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                            Ctrl.Wgc.Ctx.Bits.Shr = (UINT8) DGC_SHR_ADV;

                            /* dgc  */
                            Ctrl.Dgc.Ctx.Bits.Sig = (UINT8) DGC_SIG_DGC;
                            Ctrl.Dgc.Ctx.Bits.Img = (UINT8) DGC_IMG_STILL;

                            /* agc */
                            Ctrl.Agc.Ctx.Bits.Sig = (UINT8) AGC_SIG_AGC;
                            Ctrl.Agc.Ctx.Bits.Shr = (UINT8) AGC_SHR_ADV;

                            /* shr */
                            if (pInfo->Cfg.ShutterMode == (UINT32) IMAGE_SHR_SSR) {
                                Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_SSR;
                            } else {
                                Ctrl.Shr.Ctx.Bits.Sig = (UINT8) SHR_SIG_SSG;
                            }

                            /* svr */
                            Ctrl.Shr.Ctx.Bits.Svr = (UINT16) (Svr & 0xFFFFU);

                            /* ddgc */
                            Ctrl.DDgc.Ctx.Bits.Sig  = (UINT8) DGC_SIG_DGC;
                            Ctrl.DDgc.Ctx.Bits.Img  = (UINT8) DGC_IMG_STILL;

                            /* user */
                            if (CmdMsg.Ctx.Snap.User > 0U) {
                                Ctrl.User.Ctx.Bits.Sig = (UINT8) USER_SIG_USER;
                            }
                            Ctrl.User.Ctx.Bits.Shr = (UINT8) USER_SHR_ADV;

                            if (CmdMsg.Ctx.Snap.Hdr > 0U) {
                                for (i = 0U; i < AMBA_IMG_SENSOR_HAL_HDR_SIZE; i++) {
                                    /* port in (hdr) */
                                    pPort->Info.HdrData.Agc[i] = pCtx->HdrPreload.Agc[i];
                                    pPort->Info.HdrData.Dgc[i] = pCtx->HdrPreload.Dgc[i];
                                    pPort->Info.HdrData.Wgc[i] = pCtx->HdrPreload.Wgc[i];
                                    pPort->Info.HdrData.Shr[i] = pCtx->HdrPreload.Shr[i];
                                    pPort->Info.HdrData.DDgc[i] = pCtx->HdrPreload.DDgc[i];
                                }
                            }

                            /* port in (ctrl) */
                            pPort->Info.Ctrl.Agc = Ctrl.Agc.Ctx.Reg;
                            pPort->Info.Ctrl.Dgc = Ctrl.Dgc.Ctx.Reg;
                            pPort->Info.Ctrl.Wgc = Ctrl.Wgc.Ctx.Reg;
                            pPort->Info.Ctrl.Shr = Ctrl.Shr.Ctx.Reg;
                            pPort->Info.Ctrl.DDgc = Ctrl.DDgc.Ctx.Reg;
                            pPort->Info.Ctrl.User = Ctrl.User.Ctx.Reg;
                            /* port in (data) */
                            pPort->Info.Data.Agc = Data.Agc.Ctx.Reg;
                            pPort->Info.Data.Dgc = Data.Dgc.Ctx.Reg;
                            pPort->Info.Data.Wgc = Data.Wgc.Ctx.Reg;
                            pPort->Info.Data.Shr = Data.Shr.Ctx.Reg;
                            pPort->Info.Data.DDgc = Data.DDgc.Ctx.Reg;
                            pPort->Info.Data.User = Data.User.Ctx.Reg;

                            if (pInfo->Cfg.ShutterMode == (UINT32) IMAGE_SHR_SSR) {
                                pPort->Count = (UINT32) CmdMsg.Ctx.Snap.Count;
                            } else {
                                pPort->Count = (UINT32) CmdMsg.Ctx.Snap.Count + 1U;
                            }

                            pPort->Link.Up = NULL;
                            pPort->Link.Down = NULL;

                            /* mutex take */
                            FuncRetCode = AmbaKAL_MutexTake(&(pCtx->Snap.Mutex), AMBA_KAL_WAIT_FOREVER);
                            if (FuncRetCode == OK_UL) {
                                /* hook to list */
                                pList = pCtx->Snap.pList;
                                if (pList == NULL) {
                                    /* root */
                                    pCtx->Snap.pList = pPort;
                                } else {
                                    /* tailer */
                                    while (pList->Link.Down != NULL) {
                                        pList = pList->Link.Down;
                                    }
                                    pList->Link.Down = pPort;
                                    pPort->Link.Up = pList;
                                }
                                /* mutext give */
                                FuncRetCode = AmbaKAL_MutexGive(&(pCtx->Snap.Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                        } else {
                            /* svr fail */
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
        } else {
            /* first frame */
            FuncRetCode = AmbaImgSensorSync_Snap(ImageChanId, pSyncData, Mode);
            if (FuncRetCode != OK_UL) {
                RetCode = NG_UL;
            }
        }
    } else {
        /* port or sync data null */
        RetCode = NG_UL;
    }

    return RetCode;
}
