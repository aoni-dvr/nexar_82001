/**
 *  @file SvcImgTask.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc img task
 *
 */

#include "AmbaTypes.h"
#include "AmbaShell.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaImg_Proc.h"
#include "AmbaIQParamHandlerSample.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcImg.h"
#include "SvcVinSrc.h"
#include "SvcResCfg.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "SvcLiveview.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcTiming.h"

#include "SvcImgTask.h"

#define SVC_LOG_IMG_TASK "IMG_TASK"

typedef union /*_SVC_IMG_TASK_MEM_INFO_u_*/ {
    SVC_IMG_MEM_ADDR     Data;
    void                 *pVoid;
    SVC_APP_STAT_IQ_s    *pIqStatus;
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
    const void           *pCvoid;
    SVC_IMG_ISO_INFO_s   *pIsoInfo;
#endif
} SVC_IMG_TASK_MEM_INFO_u;

typedef struct /*_SVC_IMG_TASK_MEM_INFO_s_*/ {
    SVC_IMG_TASK_MEM_INFO_u    Ctx;
} SVC_IMG_TASK_MEM_INFO_s;

#define MAX_ALGO_NUM     CONFIG_ICAM_IMGPROC_MAX_ALGO_NUM
#define MAX_EXPOSURE_NUM 4U

#define IMG_TASK_IQ_ALL_PRELOAD_DONE (0x1U)
#define IMG_TASK_IQ_VID_PRELOAD_DONE (0x2U)
#define IMG_TASK_IQ_IMG_CONFIG_DONE  (0x4U)

AMBA_KAL_EVENT_FLAG_t SvcImgEventId;

static void SvcImgTask_IqLoadCB(UINT32 StatIdx, void *pInfo)
{
    UINT32 FuncRetVal;
    SVC_IMG_TASK_MEM_INFO_s MemInfo;

    switch (StatIdx) {
        case SVC_APP_STAT_IQ:
            MemInfo.Ctx.pVoid = pInfo;
            if (MemInfo.Ctx.pIqStatus->Status == SVC_APP_STAT_IQ_VID_READY) {
                FuncRetVal = AmbaKAL_EventFlagSet(&SvcImgEventId, IMG_TASK_IQ_VID_PRELOAD_DONE);
                if (FuncRetVal != KAL_ERR_NONE) {
                    /* */
                }
            } else if (MemInfo.Ctx.pIqStatus->Status == SVC_APP_STAT_IQ_ALL_READY) {
                FuncRetVal = AmbaKAL_EventFlagSet(&SvcImgEventId, IMG_TASK_IQ_ALL_PRELOAD_DONE);
                if (FuncRetVal != KAL_ERR_NONE) {
                    /* */
                }
            } else if (MemInfo.Ctx.pIqStatus->Status == SVC_APP_STAT_IQ_IMGCFG_READY) {
                FuncRetVal = AmbaKAL_EventFlagSet(&SvcImgEventId, IMG_TASK_IQ_IMG_CONFIG_DONE);
                if (FuncRetVal != KAL_ERR_NONE) {
                    /* */
                }
            } else {
                // do nothing.
            }
        break;
        default:
            SvcLog_NG(SVC_LOG_IMG_TASK, "Svc Sys State: Unkown StatIdx (%d)", StatIdx, 0U);
        break;
    }
}

static void SvcImgTask_WaitIqVidPreloadDone(void)
{
    UINT32 FuncRetVal;
    UINT32 ActualFlag = 0U;

    SvcLog_DBG(SVC_LOG_IMG_TASK, "IQ Vid Preload Check", 0U, 0U);
    FuncRetVal = AmbaKAL_EventFlagGet(&SvcImgEventId, IMG_TASK_IQ_VID_PRELOAD_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetVal != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "Err when waiting IQ Vid Preloading", 0U, 0U);
    }
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_IQ_LOAD_CHECK, "IQ table CHECK");
    #endif
}
#ifdef CONFIG_BUILD_IMGFRW_EFOV
/**
 *  svc image task external fov tx callback
 *  @param[in] TxData tx data
 *  @param[in] Size tx data size
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 SvcImgTask_EFovTxCb(UINT64 TxData, UINT32 Size)
{
    UINT32 RetCode = SVC_OK;
#ifdef CONFIG_BUILD_IMGFRW_EFOV_LPBK
    SvcImg_EFovRxPut(TxData, Size);
#else
    (void) TxData;
    (void) Size;
#endif
    return RetCode;
}
#endif
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
static UINT32 SvcImgTask_IsoDisCb(const void *pData)
{
    /*
     * note: this callback is at almostly highest priority task
     */
    UINT32 RetCode = SVC_OK;
#if 0
    (void) pData;
    /*
     * put to buffer slot and send message with address to queue in here
     */
#else
    /* example for test only, don't enable */
    UINT32 ContextId;

    SVC_IMG_TASK_MEM_INFO_s MemInfo;
    SVC_IMG_TASK_MEM_INFO_s MemInfo2;
#if 0
    static UINT32 IsoDisCbCnt[SVC_NUM_FOV_CHANNEL] = {0};
#endif
    /* iso info get  */
    MemInfo.Ctx.pCvoid = pData;
    /* context id get */
    ContextId = MemInfo.Ctx.pIsoInfo->ContextId;
    /* context id check */
    if (ContextId < SVC_NUM_FOV_CHANNEL) {
#if 1
        /* example for iso cfg update (test only), don't enable (timing may not be correctly for post zone) */
        UINT32 FuncRetCode;
        UINT64 RawSeqNum;
        AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s IsoCfgCtrl;

        IsoCfgCtrl.ViewZoneId = (UINT16) (MemInfo.Ctx.pIsoInfo->ZoneId & 0xFFU);
        IsoCfgCtrl.CtxIndex = MemInfo.Ctx.pIsoInfo->ContextId;
        IsoCfgCtrl.CfgIndex = MemInfo.Ctx.pIsoInfo->IkId;
        IsoCfgCtrl.HdrCfgIndex = MemInfo.Ctx.pIsoInfo->IkId;
        MemInfo2.Ctx.pVoid = MemInfo.Ctx.pIsoInfo->pIsoCfg;
        IsoCfgCtrl.CfgAddress = MemInfo2.Ctx.Data;
        FuncRetCode = AmbaDSP_LiveviewUpdateIsoCfg(1U, &IsoCfgCtrl, &RawSeqNum);
        if (FuncRetCode != OK) {
            RetCode = SVC_NG;
        }
#endif
#if 0
        /* example for iso info usage, don't enable because of almostly highest priority task */
        if (IsoDisCbCnt[ContextId] < 1U) {
            SvcLog_DBG(SVC_LOG_IMG_TASK, "\n\n", 0U, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "iso dis cb (%d)", IsoDisCbCnt[ContextId], 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "raw cap seq (%d)", MemInfo.Ctx.pIsoInfo->RawCapSeq, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "aik cnt (%d)", MemInfo.Ctx.pIsoInfo->AikCnt, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "vin skip frame en (%d)", MemInfo.Ctx.pIsoInfo->VinSkipFrame >> 31U, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "vin skip frame (%d)", MemInfo.Ctx.pIsoInfo->VinSkipFrame & 0xFFU, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "post zone flag (%d)", MemInfo.Ctx.pIsoInfo->ZoneId >> 31U, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "zone id (%d)", MemInfo.Ctx.pIsoInfo->ZoneId & 0xFFU, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "ik context id (%d)", MemInfo.Ctx.pIsoInfo->ContextId, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "ik id (%x)", MemInfo.Ctx.pIsoInfo->IkId, 0U);
            MemInfo2.Ctx.pVoid = MemInfo.Ctx.pIsoInfo->pIsoCfg;
            SvcLog_DBG(SVC_LOG_IMG_TASK, "ik iso cfg (%x)", (UINT32) MemInfo2.Ctx.Data, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "user data (%x)", (UINT32) MemInfo.Ctx.pIsoInfo->UserData, 0U);
            SvcLog_DBG(SVC_LOG_IMG_TASK, "\n\n", 0U, 0U);
            IsoDisCbCnt[ContextId]++;
        }
#endif
    }
#endif
    return RetCode;
}
#endif
/**
 *  svc image task iq memory size query
 *  @param[out] pSize pointer to the size data
 *  @return error code
 */
UINT32 SvcImgTask_IqMemSizeQuery(UINT32 *pSize)
{
   UINT32 RetVal = SVC_OK;

   *pSize = SvcImg_IqMemSizeQuery(MAX_ALGO_NUM);
   SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Max Algo Num = %d", MAX_ALGO_NUM, 0U);
   SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ IQ Memory Size Query = %d", *pSize, 0U);

   return RetVal;
}

/**
 *  svc image task algo memory size query
 *  @param[out] pSize pointer to the size data
 *  @return error code
 */
UINT32 SvcImgTask_AlgoMemSizeQuery(UINT32 *pSize)
{
   UINT32 RetVal = SVC_OK;

   *pSize = SvcImg_AlgoMemSizeQuery(MAX_ALGO_NUM, MAX_EXPOSURE_NUM);
   SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Max Algo Num = %d", MAX_ALGO_NUM, 0U);
   SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Max Exposure Num = %d", MAX_EXPOSURE_NUM, 0U);
   SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ AAA Memory Size Query = %d", *pSize, 0U);

   return RetVal;
}

/**
 *  svc image task memory size query
 *  @param[out] pSize pointer to the size data
 *  @return error code
 */
UINT32 SvcImgTask_MemSizeQuery(UINT32 *pSize)
{
   UINT32 RetVal = SVC_OK;

   *pSize = SvcImg_MemSizeQuery();
   SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Memory Size Query = %d", *pSize, 0U);

   return RetVal;
}

/**
 *  svc image task init
 *  @return error code
 */
UINT32 SvcImgTask_Init(void)
{
    UINT32 RetVal;

    ULONG  MemBase;
    UINT32 MemSize;

    SVC_IMG_TASK_MEM_INFO_s MemInfo;

    UINT32 CtrlID = 0U;
    static char ImgTaskIqEventName[] = "ImgTaskIqEvent";

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Init Begin", 0U, 0U);

    /* iCam: memory request */
    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_IMG_MAIN_MEM, &MemBase, &MemSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "image memory buffer request error", 0U, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_IMG_TASK, "image memory base = 0x%08lx, Memory size = %d", (UINT32) MemBase, MemSize);
        /* iCam: memory int */
        MemInfo.Ctx.Data = (SVC_IMG_MEM_ADDR) MemBase;
        SvcImg_MemInit(MemInfo.Ctx.pVoid, MemSize);
    }

    /* iCam: IQ table buffer for MAX_ALGO_NUM */
    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_IMG_IQ_TBL, &MemBase, &MemSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "iq buffer request error", 0U, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_IMG_TASK, "iq memory base = 0x%08lx, Memory size = %d", (UINT32) MemBase, MemSize);

        /* iCam: IQ memory int for MAX_ALGO_NUM */
        MemInfo.Ctx.Data = (SVC_IMG_MEM_ADDR) MemBase;
        SvcImg_IqMemInit(MAX_ALGO_NUM, MemInfo.Ctx.pVoid, MemSize);
    }

    /* iCam: AAA memory request for MAX_ALGO_NUM */
    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_IMG_AAA_MEM, &MemBase, &MemSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "algo memory buffer request error", 0U, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_IMG_TASK, "algo memory base = 0x%08lx, Memory size = %d", (UINT32) MemBase, MemSize);

        /* iCam: AAA memory int for MAX_ALGO_NUM */
        MemInfo.Ctx.Data = (SVC_IMG_MEM_ADDR) MemBase;
        SvcImg_AlgoMemInit(MemInfo.Ctx.pVoid, MemSize);

        /* iCam: max algo num = MAX_ALGO_NUM, max exposure num = MAX_EXPOSURE_NUM */
        SvcImg_Init(MAX_ALGO_NUM, MAX_EXPOSURE_NUM);

        /* install img command */
        SvcImg_CmdInstall();

        RetVal = AmbaKAL_EventFlagCreate(&SvcImgEventId, ImgTaskIqEventName);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_IMG_TASK, "Create event flag failed %d", RetVal, 0U);
        }

        RetVal = SvcSysStat_Register(SVC_APP_STAT_IQ, SvcImgTask_IqLoadCB, &CtrlID);
    }

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Init End", 0U, 0U);

    return RetVal;
}

/**
 *  svc image task config
 *  @return error code
 */
UINT32 SvcImgTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 FuncRetVal;
    UINT32 i, j, k;

    UINT32 SensorNum;
    UINT32 SensorIndex[SVC_NUM_VIN_SENSOR] = {0};

    UINT32 FovNum;
    UINT32 FovIndex[SVC_NUM_FOV_CHANNEL] = {0};

    UINT32 VinSrc = 0U;

    UINT32 VinSensorId;
    UINT32 SensorAAAGroupId;

    SVC_IMG_CFG_s ImgCfg = {0};
    const SVC_RES_CFG_s *pResCfg;
    SVC_APP_STAT_IQ_s IqLoaderStatus = { 0 };

    UINT32 FrameCnt;
    UINT32 BitsCnt;
    UINT32 FrameSelectBits;

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Config Begin", 0U, 0U);
    /* res cfg get */
    pResCfg = SvcResCfg_Get();

    /* vin select (bits) reset */
    ImgCfg.VinSelectBits = 0U;

    /* cfg info get */
    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
        /* sensor select (bits) reset */
        ImgCfg.SensorSelectBits[i] = 0U;
        /* sensor idx in vin */
        FuncRetVal = SvcResCfg_GetSensorIdxsInVinID(i, SensorIndex, &SensorNum);
        if (FuncRetVal == SVC_OK) {
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(i, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* is valid? */
                if (SensorNum > 0U) {
                    /* vin select (bits) */
                    ImgCfg.VinSelectBits |= (((UINT32) 1U) << i);
                    /* time division check */
                    if (pResCfg->VinCfg[i].SubChanCfg[0].TDNum > 1U) {
                        /* time division num get */
                        ImgCfg.TimeDivision[i].Num = pResCfg->VinCfg[i].SubChanCfg[0].TDNum;
                        /* time division frame cnt/bits calculate */
                        FrameCnt = 0U;
                        BitsCnt = 0U;
                        for (j = 0U; j < ImgCfg.TimeDivision[i].Num; j++) {
                            /* frame cnt accumulate */
                            FrameCnt += pResCfg->VinCfg[i].SubChanTDFrmNum[0][j];
                            /* frame select bits reset */
                            FrameSelectBits = 0U;
                            for (k = 0U; k < pResCfg->VinCfg[i].SubChanTDFrmNum[0][j]; k++) {
                                /* frame select bits accumulate */
                                FrameSelectBits |= (((UINT32) 1U) << (BitsCnt & 0x1FU));
                                BitsCnt++;
                            }
                            /* frame select bits get */
                            ImgCfg.TimeDivision[i].FrameSelectBits[j] = FrameSelectBits;
                        }
                        /* frame group cnt get */
                        ImgCfg.TimeDivision[i].FrameGroupCnt = FrameCnt;
                    } else {
                        /* normal */
                        ImgCfg.TimeDivision[i].Num = 1U;
                        ImgCfg.TimeDivision[i].FrameGroupCnt = 1U;
                        ImgCfg.TimeDivision[i].FrameSelectBits[0] = 0U;
                    }
                    /* sensor in vin */
                    for (j = 0U; j < SensorNum; j++) {
                        /* sensor select (bits) in vin */
                        ImgCfg.SensorSelectBits[i] |= (((UINT32) 1U) << (SensorIndex[j] & 0x1FU));
                        /* sensor id get */
                        FuncRetVal = SvcResCfg_GetSensorIDOfSensorIdx(i, SensorIndex[j], &VinSensorId);
                        if (FuncRetVal == SVC_OK) {
                            ImgCfg.VinSensorId[i][SensorIndex[j]] = VinSensorId;
                        }
                        /* fov selet (bits) reset */
                        ImgCfg.FovSelectBitsInSensor[i][SensorIndex[j]] = 0U;
                        /* fov idx in sensor */
                        FuncRetVal = SvcResCfg_GetFovIdxsInSensorIdx(i, SensorIndex[j], FovIndex, &FovNum);
                        if (FuncRetVal == SVC_OK) {
                            for (k = 0U; k < FovNum; k++) {
                                /* fov select (bits) in sensor */
                                ImgCfg.FovSelectBitsInSensor[i][SensorIndex[j]] |= (((UINT32) 1U) << (FovIndex[k] & 0x1FU));
                                /* fov group id */
                                ImgCfg.FovAdj[FovIndex[k]].GroupId = pResCfg->FovCfg[FovIndex[k]].FovGroup;
                                /* fov iq table */
                                ImgCfg.FovAdj[FovIndex[k]].IqTable = pResCfg->FovCfg[FovIndex[k]].IQTable;
                                /* linear ce */
                                ImgCfg.FovCfg[FovIndex[k]].LinearCE = pResCfg->FovCfg[FovIndex[k]].PipeCfg.LinearCE;
                            }
                        }
                        /* sensor aaa group id get */
                        FuncRetVal = SvcResCfg_GetSensorGroupOfSensorIdx(i, SensorIndex[j], &SensorAAAGroupId);
                        if (FuncRetVal == SVC_OK) {
                            ImgCfg.SensorAaaGroup[i][SensorIndex[j]] = SensorAAAGroupId;
                        } else {
                            ImgCfg.SensorAaaGroup[i][SensorIndex[j]] = 0U;
                        }
                        /* aaa iq table */
                        ImgCfg.AaaIqTable[i][SensorIndex[j]] = pResCfg->SensorCfg[i][SensorIndex[j]].IQTable;
                        /* time division iq table */
                        if (ImgCfg.TimeDivision[i].Num > 1U) {
                            /* time division */
                            for (k = 0U; k < ImgCfg.TimeDivision[i].Num; k++) {
                                ImgCfg.TimeDivision[i].AaaIqTable[SensorIndex[j]][k] = pResCfg->SensorCfg[i][SensorIndex[j]].TimeDivisionIQTable[k];
                            }
                        }
                        /* sensor ext-fov id get */
                        ImgCfg.SensorEFov[i][SensorIndex[j]] = pResCfg->SensorCfg[i][SensorIndex[j]].SensorEFov;
#ifdef CONFIG_BUILD_IMGFRW_SMC
                        ImgCfg.SmcI2sId[i][j] = pResCfg->SensorCfg[i][SensorIndex[j]].SmcI2sId;
#endif
                    }
                } else {
                    /* sensor empty */
                    ImgCfg.TimeDivision[i].Num = 0U;
                    for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                        ImgCfg.VinSensorId[i][j] = 0U;
                        ImgCfg.FovSelectBitsInSensor[i][j] = 0U;
                        ImgCfg.SensorAaaGroup[i][j] = 0U;
                        ImgCfg.AaaIqTable[i][j] = 0U;
                        ImgCfg.SensorEFov[i][j] = 0ULL;
#ifdef CONFIG_BUILD_IMGFRW_SMC
                        ImgCfg.SmcI2sId[i][j] = 0U;
#endif
                    }
                }
            } else {
                /* vin src fail */
                ImgCfg.TimeDivision[i].Num = 0U;
                for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                    ImgCfg.VinSensorId[i][j] = 0U;
                    ImgCfg.FovSelectBitsInSensor[i][j] = 0U;
                    ImgCfg.SensorAaaGroup[i][j] = 0U;
                    ImgCfg.AaaIqTable[i][j] = 0U;
                    ImgCfg.SensorEFov[i][j] = 0ULL;
#ifdef CONFIG_BUILD_IMGFRW_SMC
                    ImgCfg.SmcI2sId[i][j] = 0U;
#endif
                }
            }
        } else {
            /* sensor fail */
            ImgCfg.TimeDivision[i].Num = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                ImgCfg.VinSensorId[i][j] = 0U;
                ImgCfg.FovSelectBitsInSensor[i][j] = 0U;
                ImgCfg.SensorAaaGroup[i][j] = 0U;
                ImgCfg.AaaIqTable[i][j] = 0U;
                ImgCfg.SensorEFov[i][j] = 0ULL;
#ifdef CONFIG_BUILD_IMGFRW_SMC
                ImgCfg.SmcI2sId[i][j] = 0U;
#endif
            }
        }
    }

    SvcImgTask_WaitIqVidPreloadDone();
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    SvcImg_EFovTxHook(SvcImgTask_EFovTxCb);
#endif
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
    SvcImg_IsoDisCbHook(SvcImgTask_IsoDisCb);
#endif
    /* img config */
    SvcImg_ConfigEx(&ImgCfg, (UINT32) SVC_ALGO_IQ_LOAD_VIDEO);

    IqLoaderStatus.Status = SVC_APP_STAT_IQ_IMGCFG_READY;
    FuncRetVal = SvcSysStat_Issue(SVC_APP_STAT_IQ, &IqLoaderStatus);
    if (SVC_OK != FuncRetVal) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "## fail to set SVC_APP_STAT_IQ_IMGCFG_READY flag", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Config End", 0U, 0U);

    return RetVal;
}

/**
 *  svc image task config null
 *  @return error code
 */
UINT32 SvcImgTask_ConfigNull(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_IQ_s IqLoaderStatus = { 0 };

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ ConfigNull Begin", 0U, 0U);
    SvcImgTask_WaitIqVidPreloadDone();

    IqLoaderStatus.Status = SVC_APP_STAT_IQ_IMGCFG_READY;
    RetVal = SvcSysStat_Issue(SVC_APP_STAT_IQ, &IqLoaderStatus);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "## fail to set SVC_APP_STAT_IQ_IMGCFG_READY flag", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ ConfigNull End", 0U, 0U);

    return RetVal;
}

/**
 *  svc image task stop
 *  @return error code
 */
UINT32 SvcImgTask_Stop(void)
{
    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Stop Begin", 0U, 0U);

    SvcImg_Stop();

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Stop Done", 0U, 0U);

    return SVC_OK;
}

/**
 *  svc image task lock
 *  @return error code
 */
UINT32 SvcImgTask_Lock(void)
{
    UINT32 FuncRetVal;
    UINT32 i;

    UINT32 VinId;
    UINT32 VinSrc = 0U;

    SVC_LIV_INFO_s LivInfo;

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Lock Begin", 0U, 0U);

    /* liv info get */
    SvcLiveview_InfoGet(&LivInfo);
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && (VinSrc == SVC_VIN_SRC_SENSOR)) {
            /* vin img lock */
            SvcImg_Lock(VinId);
        }
    }

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Lock Done", 0U, 0U);

    return SVC_OK;
}

/**
 *  svc image task still stop
 *  @return error code
 */
UINT32 SvcImgTask_StillStop(void)
{
    UINT32 FuncRetVal = SVC_OK;
    UINT32 i;

    UINT32 VinId;
    UINT32 VinSrc = 0U;

    SVC_LIV_INFO_s LivInfo;

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Still Stop Begin", 0U, 0U);

    /* liv info get */
    SvcLiveview_InfoGet(&LivInfo);
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && (VinSrc == SVC_VIN_SRC_SENSOR)) {
            /* vin img stop */
            SvcImg_StillStop(VinId);
        }
    }

    SvcLog_DBG(SVC_LOG_IMG_TASK, "@@ Still Stop Done", 0U, 0U);

    return FuncRetVal;
}

/**
 *  svc image task wait iq preload done
 *  @return error code
 */
UINT32 SvcImgTask_WaitIqPreloadDone(void)
{
    UINT32 RetVal;
    UINT32 ActualFlag = 0U;

    SvcLog_DBG(SVC_LOG_IMG_TASK, "IQ Preload Check", 0U, 0U);
    RetVal = AmbaKAL_EventFlagGet(&SvcImgEventId, IMG_TASK_IQ_ALL_PRELOAD_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, 1000U);
    if (RetVal != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "Err when waiting IQ Preloading", 0U, 0U);
    }

    return RetVal;
}

UINT32 SvcImgTask_LoadIqPartial0(void)
{
    UINT32 Rval;
    SVC_APP_STAT_IQ_s IqLoaderStatus = { 0 };

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_IQ_LOAD_START, "IQ table load START");
    #endif

    SvcImg_AlgoIqPreloadEx((UINT32) SVC_ALGO_IQ_LOAD_VIDEO);

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_IQ_LOAD_DONE_PART, "IQ table load partial DONE");
    #endif

    IqLoaderStatus.Status = SVC_APP_STAT_IQ_VID_READY;
    Rval = SvcSysStat_Issue(SVC_APP_STAT_IQ, &IqLoaderStatus);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "## fail to set SVC_APP_STAT_IQ_VID_READY flag", 0U, 0U);
    }

    return Rval;
}

UINT32 SvcImgTask_LoadIqPartial1(void)
{
    UINT32 Rval = SVC_OK;
    UINT32 ActualFlag = 0U;
    SVC_APP_STAT_IQ_s IqLoaderStatus = { 0 };

    Rval = AmbaKAL_EventFlagGet(&SvcImgEventId, IMG_TASK_IQ_IMG_CONFIG_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, AMBA_KAL_WAIT_FOREVER);
    if (Rval != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "Err when waiting Img Config", 0U, 0U);
    }

#if !defined (CONFIG_SOC_H22)
    SvcImg_AlgoIqPreloadEx((UINT32) SVC_ALGO_IQ_LOAD_STILL);
    SvcImg_AlgoIqInitEx((UINT32) SVC_ALGO_IQ_LOAD_STILL);
#endif

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_IQ_LOAD_DONE, "IQ table load DONE");
    SvcTime_PrintBootTime();
    #endif

    IqLoaderStatus.Status = SVC_APP_STAT_IQ_ALL_READY;
    Rval = SvcSysStat_Issue(SVC_APP_STAT_IQ, &IqLoaderStatus);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_IMG_TASK, "## fail to set SVC_APP_STAT_IQ_ALL_READY flag", 0U, 0U);
    }

    return Rval;
}
