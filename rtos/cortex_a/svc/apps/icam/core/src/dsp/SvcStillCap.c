/**
*  @file SvcStillCap.c
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
*  @details svc boot device
*
*/

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaSD.h"
#include "AmbaSensor.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaFS.h"

#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_VOUT_Def.h"

#include "SvcStillMux.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcResCfg.h"
#include "SvcDSP.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillProc.h"
#include "SvcTask.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#if defined(CONFIG_BUILD_COMSVC_IMGFRW)
#include "SvcVinSrc.h"
#include "SvcImg.h"
#endif
static inline UINT32 SCAP_ALIGN(UINT32 AlignVal, UINT32 Size)
{
    UINT32 RetVal = 0U;

    if ((AlignVal >= 2U) && (Size > 0U)) {
        RetVal = ( Size + ( AlignVal - 1U ) ) & ( ~ ( AlignVal - 1U ) );
    }
    return RetVal;
}

#define NUM_CAP_CFG (SVC_STL_MUX_NUM_FILES)

#define STL_CAP_EVENT_CAP_START     (1U)
#define STL_CAP_EVENT_CAP_QV_START  (2U)
#define STL_CAP_EVENT_CAP_QV_STOP   (3U)

#define QV_TIMEOUT_VOUT_0   (1U)
#define QV_TIMEOUT_VOUT_1   (2U)
#define QV_TIMEOUT_VOUT_All (QV_TIMEOUT_VOUT_0 | QV_TIMEOUT_VOUT_1)

#define SVC_STL_CAP_SENT (1)

#define SVC_QV_INTERVAL    (1000U)   // 1 sec
#define QV_TASK_STACK_SIZE (0x1000U)

typedef struct {
    AMBA_DSP_BUF_s BufInfo; /* set effective area size */
    UINT32 BufSize;
    UINT16 Index;           /* Bit selection of YuvStrmId when SyncEncStart = 0. EncStrmId when SyncEncStart = 1. */
    UINT16 StreamID;           /* YuvStrmId */
    UINT8  PicType;
    UINT8  SyncEncStart;
    UINT8  CapOnly;
    UINT8  CapFromEncStrm;  /* capture from encode stream */
    UINT8  CapType;
    UINT8  RawCmpr;
    UINT8  RestartLiveview;
    UINT8  Status;
    UINT32 CapNum;
    UINT32 AebNum;
} CAP_INFO_s;

typedef struct {
    UINT8                  Event;
    UINT8                  QvVoutId; /* quick view vout id */
    UINT8                  Rsvd[2];
    SVC_CAP_CFG_s          Cfg;
} SVC_STL_CAP_EVENT_s;

typedef struct {
    ULONG                  AddrY;  /* default image Y address */
    ULONG                  AddrUV; /* default image UV address */
    UINT16                 Width;  /* default image width */
    UINT16                 Height; /* default image height */
    UINT16                 Pitch;  /* default image pitch */
    UINT8                  Enable; /* 1 - quick view on. 0 - quick view off */
    AMBA_KAL_TIMER_t       Timer;  /* timer for quick view */
} SVC_QVIEW_INFO_s;

typedef struct {
    SVC_STL_CAP_EVENT_s     CapQueBuf[NUM_CAP_CFG];
    AMBA_KAL_MSG_QUEUE_t    CapQue;
    CAP_INFO_s              CapInfo[SVC_MAX_PIV_STREAMS][NUM_CAP_CFG];
    UINT8                   CapCfgWp[SVC_MAX_PIV_STREAMS];
    UINT8                   CapCfgRp[SVC_MAX_PIV_STREAMS];
    UINT8                   CapInfoId[SVC_MAX_PIV_STREAMS];
    UINT32                  StartTime;
    SVC_TASK_CTRL_s         TaskCtrl;
    SVC_STILL_CAP_VININFO_s Vin[AMBA_DSP_MAX_VIN_NUM][NUM_CAP_CFG];
    UINT8                   VinInfoWp[AMBA_DSP_MAX_VIN_NUM];
    UINT8                   VinInfoRp[AMBA_DSP_MAX_VIN_NUM];
    UINT16                  CapCnt[NUM_CAP_CFG][SVC_CAP_MAX_SENSOR_PER_VIN];
    UINT8                   SetOutputSize[NUM_CAP_CFG];
    UINT16                  OutputWidth[NUM_CAP_CFG];
    UINT16                  OutputHeight[NUM_CAP_CFG];
    UINT32                  CapVinMask;
    UINT32                  Status;                         /* capture status */
    UINT16                  NumCapInstance;                 /* number of capture instance */
    char                    StorageDrive;                   /* storage drive */
    UINT32                  MinStorageSize;                 /* minimum required storage size */
    SVC_STILL_QVIEW_CFG_s   Qview;                          /* quick view setting for r2y flow */
    SVC_PIV_QVIEW_CFG_s     PivQview;                       /* quick view configuration */
    SVC_QVIEW_INFO_s        QvInfo[AMBA_DSP_MAX_VOUT_NUM];  /* quick view info */
    SVC_TASK_CTRL_s         QvTask;                         /* quick view task */
    AMBA_KAL_EVENT_FLAG_t   QvFlag;                         /* quick view event flag */
    AMBA_KAL_EVENT_FLAG_t   Flag;                           /* capture flag */
    AMBA_KAL_MUTEX_t        Mutex;                          /* mutex for capture status */
    UINT8                   QvTaskInit;                     /* quick view task initialized */
} SVC_STL_CAP_CTRL_s;

static UINT8 StlCap_DebugOn = 0;
static UINT8 StlCap_Created = 0;
static UINT8 StlCap_CleanRawDump = 0;
static SVC_STL_CAP_CTRL_s StlCap GNU_SECTION_NOZEROINIT;

#ifdef CONFIG_BSP_CV2DK
static UINT32 AllSensorID_Disable[AMBA_DSP_MAX_VIN_NUM];
#endif

/**
* get capture status
* @param [out] pStatus pointer to capture stataus
*/
void SvcStillCap_GetStatus(UINT32 *pStatus)
{
    UINT32 Rval;
    Rval = AmbaKAL_MutexTake(&StlCap.Mutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        *pStatus = StlCap.Status;
        Rval = AmbaKAL_MutexGive(&StlCap.Mutex);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaKAL_MutexGive return 0x%x", __LINE__, Rval);
        }
    }
}

/**
* set capture status
* @param [in] Status capture stataus to be set
*/
void SvcStillCap_SetStatus(UINT32 Status)
{
    UINT32 Rval;
    Rval = AmbaKAL_MutexTake(&StlCap.Mutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        StlCap.Status |= Status;
        //if (StlCap_DebugOn == 1U) {
            SvcLog_DBG(SVC_LOG_STL_CAP, "status | 0x%x = 0x%x", Status, StlCap.Status);
        //}
        Rval = AmbaKAL_MutexGive(&StlCap.Mutex);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaKAL_MutexGive return 0x%x", __LINE__, Rval);
        }
    }
}

/**
* clear capture status
* @param [in] Status capture stataus to be cleared
*/
void SvcStillCap_ClearStatus(UINT32 Status)
{
    UINT32 Rval;
    Rval = AmbaKAL_MutexTake(&StlCap.Mutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        StlCap.Status &= (~Status);
        //if (StlCap_DebugOn == 1U) {
            SvcLog_DBG(SVC_LOG_STL_CAP, "status & ~0x%x = 0x%x", Status, StlCap.Status);
        //}
        Rval = AmbaKAL_MutexGive(&StlCap.Mutex);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaKAL_MutexGive return 0x%x", __LINE__, Rval);
        }
    }
}

UINT32 SvcStillCap_GetCleanRawDump(void)
{
    return (UINT32)StlCap_CleanRawDump;
}

static void ProcessEventDataRaw(const SVC_CAP_MSG_s *pMsg)
{
    SVC_STL_PROC_INFO_s Info;
    UINT8 VinID = 0, SensorNum = 0;
    UINT32 Rval;
    UINT16 SensorIdx, AebIdx;

    Rval = AmbaWrap_memset(&Info, 0x0, sizeof(Info));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].StreamID < AMBA_DSP_MAX_VIN_NUM) {
            VinID = (UINT8)StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].StreamID;
        } else {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] StreamID %u", __LINE__, StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].StreamID);
        }
        Info.PicType = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].PicType;

        if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].AebNum > 1U) {
            AebIdx = StlCap.CapCnt[pMsg->CapInstance][0];
        } else {
            AebIdx = 0;
        }

        if (Info.PicType == SVC_STL_TYPE_RAW) {
            SVC_STL_MUX_INPUT_s MuxInput;
            Rval = AmbaWrap_memset(&MuxInput, 0, sizeof(MuxInput));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                if (pMsg->CeAvail == 1U) {
                    MuxInput.PicType = SVC_STL_TYPE_RAW;
                } else {
                    MuxInput.PicType = SVC_STL_TYPE_RAWONLY;
                }
                Info.PicType = MuxInput.PicType;
                Rval = SvcStillMux_CreatePicFile(Info.PicType,
                                                 StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapType,
                                                 (UINT16)VinID, 0, StlCap.CapCnt[pMsg->CapInstance][0], AebIdx);
                if (Rval == OK) {
                    MuxInput.PicRdy.StartAddr = pMsg->BaseAddrY;
                    SvcLog_OK(SVC_LOG_STL_CAP, "Pitch %u Height %u", pMsg->Pitch, pMsg->Height);
                    MuxInput.PicRdy.PicSize = (UINT32)pMsg->Pitch * (UINT32)pMsg->Height;
                    SvcLog_OK(SVC_LOG_STL_CAP, "RawSize %u", MuxInput.PicRdy.PicSize, 0);
                    Rval = SvcStillMux_InputPic(&MuxInput);
                }
                if ((Rval == OK) && (pMsg->CeAvail == 1U)) {
                    Rval = SvcStillMux_CreatePicFile(SVC_STL_TYPE_CE,
                                                     StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapType,
                                                     (UINT16)VinID, 0, StlCap.CapCnt[pMsg->CapInstance][0], AebIdx);
                    if (Rval == OK) {
                        MuxInput.PicType = SVC_STL_TYPE_CE;
                        MuxInput.PicRdy.StartAddr = pMsg->BaseAddrUV;
                        MuxInput.PicRdy.PicSize = MuxInput.PicRdy.PicSize >> 2U;
                        Rval = SvcStillMux_InputPic(&MuxInput);
                        if (Rval != OK) {
                            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] SvcStillMux_InputPic return 0x%x", __LINE__, Rval);
                        }
                    }
                }
            }
        }

        if (Info.PicType == SVC_STL_TYPE_CLEAN_RAW) {
            Rval = SvcStillMux_CreatePicFile(Info.PicType,
                                             StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapType,
                                             (UINT16)VinID, 0, StlCap.CapCnt[pMsg->CapInstance][0], AebIdx);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] SvcStillMux_CreatePicFile return 0x%x", __LINE__, Rval);
            }
        }

        for (SensorIdx = 0; SensorIdx < SVC_CAP_MAX_SENSOR_PER_VIN; SensorIdx ++) {
            if (((UINT32)StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanMask & ((UINT32)1 << SensorIdx)) != 0U) {
                UINT16 RawWidth, Pitch;
                Info.StreamId = VinID;
                Info.SensorNum = SensorNum;
                Rval = AmbaWrap_memcpy(&Info.InputInfo, pMsg, sizeof(SVC_CAP_MSG_s));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
                } else {
                    UINT32 Temp;
                    Info.InputInfo.Width = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].Width;
                    Info.InputInfo.Height = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].Height;
                    Info.InputInfo.OffsetX  = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].OffsetX ;
                    Info.InputInfo.OffsetY  = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].OffsetY ;
                    if (StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].R2yScale == 1U) {
                        Info.OutMainW  = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].OutWidth ;
                        Info.OutMainH  = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].OutHeight ;
                    } else {
                        Info.OutMainW = Info.InputInfo.Width;
                        Info.OutMainH = Info.InputInfo.Height;
                    }
                    /* raw */
                    Info.InputInfo.BaseAddrY = (Info.InputInfo.BaseAddrY + ((ULONG)StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].OffsetX << 1U)) +
                                               ((ULONG)StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanWin[SensorIdx].OffsetY * (ULONG)Info.InputInfo.Pitch);
                    /* ce */
                    if ((SensorNum < SVC_CAP_MAX_SENSOR_PER_VIN) && ((Info.InputInfo.Height < SVC_STL_MAX_H) && (Info.InputInfo.Pitch < SVC_STL_MAX_W))) {
                        Info.InputInfo.BaseAddrUV = Info.InputInfo.BaseAddrUV + ((ULONG)SensorNum * (ULONG)Info.InputInfo.Height * ((ULONG)Info.InputInfo.Pitch >> 2U));
                    }
                    Rval = AmbaDSP_GetCmprRawBufInfo(StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].EffectW, StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].Cmpr, &RawWidth, &Pitch);
                    Info.InputInfo.Pitch = Pitch;
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[%u] AmbaDSP_GetCmprRawBufInfo failed, return 0x%x", __LINE__, Rval);
                    } else {
                        SvcLog_DBG(SVC_LOG_STL_CAP, "SensorIdx %u, SensorNum %u", SensorIdx, SensorNum);
                        SvcStillLog("[ProcessEventDataRaw] raw addr %p ce addr %p pitch %u main w %u h %u", Info.InputInfo.BaseAddrY, Info.InputInfo.BaseAddrUV, Info.InputInfo.Pitch, Info.OutMainW, Info.OutMainH);
                    }
                    if (SensorNum < SVC_CAP_MAX_SENSOR_PER_VIN) {
                        SensorNum ++;
                    }
                    if (Info.OutMainW <= SVC_STL_SCRN_W) {
                        Info.OutScrnW = 0;
                        Info.OutScrnH = 0;
                    } else {
                        Info.OutScrnW = SVC_STL_SCRN_W;
                        Temp = SCAP_ALIGN(2, (SVC_STL_SCRN_W * (UINT32)Info.OutMainH / (UINT32)Info.OutMainW));
                        if (Temp <= SVC_STL_MAX_H) {
                            Info.OutScrnH = (UINT16)Temp;
                        } else {
                            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] Temp %d > SVC_STL_MAX_H", __LINE__, Temp);
                        }
                    }
                    Info.OutThmbW = SVC_STL_THMB_W;
                    Temp = SCAP_ALIGN(2, (SVC_STL_THMB_W * (UINT32)Info.OutMainH / (UINT32)Info.OutMainW));
                    if (Temp <= SVC_STL_MAX_H) {
                        Info.OutThmbH = (UINT16)Temp;
                    } else {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] Temp %d > SVC_STL_MAX_H", __LINE__, Temp);
                    }
                    SvcLog_OK(SVC_LOG_STL_CAP, "scrn w %u h %u", Info.OutScrnW, Info.OutScrnH);
                    SvcLog_OK(SVC_LOG_STL_CAP, "thmb w %u h %u", Info.OutThmbW, Info.OutThmbH);

                    if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].AebNum > 1U) {
                        AebIdx = StlCap.CapCnt[pMsg->CapInstance][SensorIdx];
                    } else {
                        AebIdx = 0;
                    }

                    if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U) {
                        if ((Info.PicType != SVC_STL_TYPE_THM) && (Info.PicType != SVC_STL_TYPE_NULL_WR)) { /* RAW saving jobs is already assigned above. */
                            Info.PicType = SVC_STL_TYPE_JPEG;
                        }
                        Info.CapInstance = pMsg->CapInstance;
                        Info.InputInfo.Index = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].Index;
                        Info.RestartLiveview = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].RestartLiveview;

                        Rval = SvcStillMux_CreatePicFile(Info.PicType,
                                                         StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapType,
                                                         (UINT16)VinID, SensorIdx, StlCap.CapCnt[pMsg->CapInstance][SensorIdx], AebIdx);

                        if (Rval == OK) {
                            StlCap.CapCnt[pMsg->CapInstance][SensorIdx] ++;
                            Info.InputInfo.IsHiso = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].IsHiso[SensorIdx];
                            Info.InputInfo.SensorIdx = (UINT8)SensorIdx;
                            if (SVC_OK != AmbaWrap_memcpy(&Info.Qview, &StlCap.Qview, sizeof(SVC_STILL_QVIEW_CFG_s))) {
                                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memcpy qview failed", 0U, 0U);
                            } else {
                                SvcLog_DBG(SVC_LOG_STL_CAP, "Qview Enable %u NumQview %u", Info.Qview.Enable, Info.Qview.NumQview);
                            }
                            Rval = SvcStillProc_InputData(&Info);
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] SvcStillProc_InputData return 0x%x", __LINE__, Rval);
                            }
                        }
                    }
                }
            }
        }
        if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].Status == StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapNum) {
            if (StlCap.VinInfoRp[VinID] == ((UINT8)NUM_CAP_CFG - 1U)) {
                StlCap.VinInfoRp[VinID] = 0;
            } else {
                StlCap.VinInfoRp[VinID] ++;
            }
        }
    }
}

/**
* set output main size, currently for PIV only
* @param [in] SetOutputSize 0 - output main size is the same as input. 1 - set different output main size
* @param [in] StreamID      stream id
* @param [in] Width         output width
* @param [in] Height        output height
* @return 0-OK, 1-NG
*/
void SvcStillCap_SetOutputSize(UINT8 SetOutputSize, UINT16 StreamID, UINT16 Width, UINT16 Height)
{
    StlCap.SetOutputSize[StreamID] = SetOutputSize;
    StlCap.OutputWidth[StreamID] = Width;
    StlCap.OutputHeight[StreamID] = Height;
    SvcLog_OK(SVC_LOG_STL_CAP, "SetOutputSize %u StreamID %u", SetOutputSize, StreamID);
    SvcLog_OK(SVC_LOG_STL_CAP, "Width %u Height %u", Width, Height);
}

static void ProcessEventDataYuv(const SVC_CAP_MSG_s *pMsg)
{
    UINT32 Rval;
    UINT16 StreamID;

    SVC_STL_PROC_INFO_s Info;
    Rval = AmbaWrap_memset(&Info, 0x0, sizeof(Info));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        StreamID = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].StreamID;
        if (StreamID < NUM_CAP_CFG) {
            Info.StreamId = (UINT8)StreamID;
            if (StlCap.SetOutputSize[StreamID] == 1U) {
                Info.OutMainW = StlCap.OutputWidth[StreamID];
                Info.OutMainH = StlCap.OutputHeight[StreamID];
            } else {
                Info.OutMainW = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].BufInfo.Window.Width;
                Info.OutMainH = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].BufInfo.Window.Height;
            }
        } else {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] StreamID %u >= NUM_CAP_CFG", __LINE__, StreamID);
        }

        Rval = AmbaWrap_memcpy(&Info.InputInfo, pMsg, sizeof(SVC_CAP_MSG_s));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
        } else {
            UINT32 Temp;
            if (Info.OutMainW <= SVC_STL_SCRN_W) {
                Info.OutScrnW = 0;
                Info.OutScrnH = 0;
            } else {
                Info.OutScrnW = SVC_STL_SCRN_W;
                Temp = SCAP_ALIGN(2, (SVC_STL_SCRN_W * (UINT32)Info.OutMainH / (UINT32)Info.OutMainW));
                if (Temp <= SVC_STL_MAX_H) {
                    Info.OutScrnH = (UINT16)Temp;
                } else {
                    SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] Temp %u > SVC_STL_MAX_H", __LINE__, Temp);
                }
            }
            Info.OutThmbW = SVC_STL_THMB_W;
            Temp = SCAP_ALIGN(2, (SVC_STL_THMB_W * (UINT32)Info.OutMainH / (UINT32)Info.OutMainW));
            if (Temp <= SVC_STL_MAX_H) {
                Info.OutThmbH = (UINT16)Temp;
            } else {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] Temp %u > SVC_STL_MAX_H", __LINE__, Temp);
            }
            SvcLog_OK(SVC_LOG_STL_CAP, "CapInstance %u", pMsg->CapInstance, 0);
            SvcLog_OK(SVC_LOG_STL_CAP, "main w %u h %u", Info.OutMainW, Info.OutMainH);
            SvcLog_OK(SVC_LOG_STL_CAP, "scrn w %u h %u", Info.OutScrnW, Info.OutScrnH);
            SvcLog_OK(SVC_LOG_STL_CAP, "thmb w %u h %u", Info.OutThmbW, Info.OutThmbH);

            Info.PicType = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].PicType;
            if ((Info.PicType != SVC_STL_TYPE_THM) && (Info.PicType != SVC_STL_TYPE_NULL_WR)) { /* YUV saving jobs is already assigned above. */
                 Info.PicType = SVC_STL_TYPE_JPEG;
            }
            Info.CapInstance = pMsg->CapInstance;
            Info.InputInfo.Index = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].Index;
            Info.RestartLiveview = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].RestartLiveview;
            Rval = SvcStillProc_InputData(&Info);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] SvcStillProc_InputData return 0x%x", __LINE__, Rval);
            }
        }
    }
}

static void CheckQview(const SVC_CAP_MSG_s *pMsg, UINT8 *pDoProcess)
{
    if (StlCap.PivQview.Enable == 1U) {
        UINT8 i;
        UINT32 Rval;
        UINT16 StreamID = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].StreamID;
        for (i = 0; i < StlCap.PivQview.NumStrm; i ++) {
            if (StreamID == StlCap.PivQview.StrmId[i]) {
                SVC_STL_CAP_EVENT_s CapEvent;
                StlCap.QvInfo[StlCap.PivQview.VoutId[i]].Enable = 1;
                StlCap.QvInfo[StlCap.PivQview.VoutId[i]].AddrY = pMsg->BaseAddrY;
                StlCap.QvInfo[StlCap.PivQview.VoutId[i]].AddrUV = pMsg->BaseAddrUV;
                StlCap.QvInfo[StlCap.PivQview.VoutId[i]].Width = pMsg->Width;
                StlCap.QvInfo[StlCap.PivQview.VoutId[i]].Height = pMsg->Height;
                StlCap.QvInfo[StlCap.PivQview.VoutId[i]].Pitch = pMsg->Pitch;
                CapEvent.Event = STL_CAP_EVENT_CAP_QV_START;
                CapEvent.QvVoutId = StlCap.PivQview.VoutId[i];
                SvcLog_OK(SVC_LOG_STL_CAP, "[QV] VoutId %u QvOnly %u", CapEvent.QvVoutId, StlCap.PivQview.QvOnly[i]);
                SvcStillCap_SetStatus(SVC_STL_QV_BUSY);
                Rval = AmbaKAL_MsgQueueSend(&StlCap.CapQue, &CapEvent, 1000);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "ln %u AmbaKAL_MsgQueueSend failed! return %u", __LINE__, Rval);
                } else {
                    if (StlCap.PivQview.QvOnly[i] == 1U) {
                        *pDoProcess = 0;
                    }
                }
                break;
            }
        }
    }
}

static void UpdateCapInfo(const SVC_CAP_MSG_s *pMsg)
{
    if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].Status == StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapNum) {
        UINT16 i;
        StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].Status = 0;
        for (i = 0; i < StlCap.NumCapInstance; i ++) {
            if (StlCap.CapInfo[i][StlCap.CapCfgRp[i]].Status != 0U)  {
                break;
            } else {
                if (i == (StlCap.NumCapInstance - 1U)) {
                    SvcStillCap_ClearStatus(SVC_STL_CAP_BUSY);
                }
            }
        }
        if (StlCap.CapCfgRp[pMsg->CapInstance] == ((UINT8)NUM_CAP_CFG - 1U)) {
            StlCap.CapCfgRp[pMsg->CapInstance] = 0;
        } else {
            StlCap.CapCfgRp[pMsg->CapInstance] ++;
        }
    } else {
        StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].Status ++;
    }
}

static void ProcessEventData(const SVC_CAP_MSG_s *pMsg)
{
    UINT32 Rval = OK;
    UINT8 DoProcess = 1;

    if (pMsg->CapType == SVC_FRAME_TYPE_YUV) {
#if defined(CONFIG_SVC_ENABLE_VOUT_DEF_IMG)
        UINT32 DispNum = 0;
#else
        const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
        UINT32 DispNum = pResCfg->DispNum;
#endif
        UINT16 StreamID = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].StreamID;
        UINT8  PicType = StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].PicType;

        CheckQview(pMsg, &DoProcess);

        if (DoProcess == 1U) {
            if (DispNum > StreamID) {
#ifdef CONFIG_SOC_CV2FS
                UINT8 PicType1 = SVC_STL_TYPE_YUV, PicType2 = (PicType == SVC_STL_TYPE_THM)? SVC_STL_TYPE_THMYUV: SVC_STL_TYPE_THMYUV_PIV;
                if (PicType == SVC_STL_TYPE_YUV) {
                    Rval = SvcStillMux_CreatePicFile(PicType1, SVC_FRAME_TYPE_YUV, StreamID, 1, 0, 0); /* main YUV */
                } else {
                    Rval = OK;
                }
                if ((Rval == OK) && (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U)) {
                    Rval = SvcStillMux_CreatePicFile(PicType2, SVC_FRAME_TYPE_YUV, StreamID, 1, 0, 0);
                }
#else
                if ((PicType == SVC_STL_TYPE_YUV) || (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U)) {
                    Rval = SvcStillMux_CreatePicFile(PicType, SVC_FRAME_TYPE_YUV, StreamID, 1, 0, 0);
                    if ((Rval == OK) && (PicType == SVC_STL_TYPE_YUV) && (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U)) {
                        Rval = SvcStillMux_CreatePicFile(SVC_STL_TYPE_JPEG, SVC_FRAME_TYPE_YUV, StreamID, 1, 0, 0);
                    }
                } else {
                    Rval = OK;
                }
#endif
            } else {
#ifdef CONFIG_SOC_CV2FS
                UINT8 PicType1 = SVC_STL_TYPE_YUV, PicType2 = (PicType == SVC_STL_TYPE_THM)? SVC_STL_TYPE_THMYUV: SVC_STL_TYPE_THMYUV_PIV;
                if (PicType == SVC_STL_TYPE_YUV) {
                    Rval = SvcStillMux_CreatePicFile(PicType1, SVC_FRAME_TYPE_YUV, StreamID, 0, 0, 0); /* main YUV */
                } else {
                    Rval = OK;
                }
                if ((Rval == OK) && (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U)) {
                    Rval = SvcStillMux_CreatePicFile(PicType2, SVC_FRAME_TYPE_YUV, StreamID, 0, 0, 0);
                }
#else
                if ((PicType == SVC_STL_TYPE_YUV) || (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U)) {
                    Rval = SvcStillMux_CreatePicFile(PicType, SVC_FRAME_TYPE_YUV, StreamID, 0, 0, 0);
                    if ((Rval == OK) && (PicType == SVC_STL_TYPE_YUV) && (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U)) {
                        Rval = SvcStillMux_CreatePicFile(SVC_STL_TYPE_JPEG, SVC_FRAME_TYPE_YUV, StreamID, 0, 0, 0);
                    }
                } else {
                    Rval = OK;
                }
#endif
            }
        }
    } else {
        Rval = OK;
    }
    if (DoProcess == 1U) {
        if (Rval == OK) {
            if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].PicType == SVC_STL_TYPE_YUV) {
                SVC_STL_MUX_INPUT_s MuxInput;
                Rval = AmbaWrap_memset(&MuxInput, 0, sizeof(MuxInput));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                } else {
                    MuxInput.PicType = SVC_STL_TYPE_LUMA;
                    MuxInput.PicRdy.StartAddr = pMsg->BaseAddrY;
                    MuxInput.PicRdy.PicSize = (UINT32)pMsg->Pitch * (UINT32)pMsg->Height;
                    Rval = SvcStillMux_InputPic(&MuxInput);
                    if (Rval == OK) {
                        MuxInput.PicType = SVC_STL_TYPE_CHROMA;
                        MuxInput.PicRdy.StartAddr = pMsg->BaseAddrUV;
#ifndef CONFIG_SOC_H22
                        MuxInput.PicRdy.PicSize = MuxInput.PicRdy.PicSize >> 1U;
#endif
                        Rval = SvcStillMux_InputPic(&MuxInput);
                    }
                }
            }
        }
        if (Rval == OK) {
            if (pMsg->CapType == SVC_FRAME_TYPE_RAW) {
                SvcLog_OK(SVC_LOG_STL_CAP, "CapInstance %u", pMsg->CapInstance, 0);
                ProcessEventDataRaw(pMsg);
            } else {
                if (StlCap.CapInfo[pMsg->CapInstance][StlCap.CapCfgRp[pMsg->CapInstance]].CapOnly == 0U) {
                    SvcLog_OK(SVC_LOG_STL_CAP, "CapInstance %u", pMsg->CapInstance, 0);
                    ProcessEventDataYuv(pMsg);
                }
            }
        }
    }
    UpdateCapInfo(pMsg);
}

static UINT32 CapDataReady(const void *pEventInfo)
{
    UINT32 Rval = 0U, BufSize, DoneTime, CurInfoID;
    UINT16 Pitch;
    const AMBA_DSP_STL_RAW_DATA_INFO_s *pInfo;
    SVC_CAP_MSG_s Msg;

    AmbaMisra_TypeCast(&pInfo, &pEventInfo);
    CurInfoID = StlCap.CapInfoId[pInfo->CapInstance];

    if (StlCap_DebugOn == 1U) {
        SvcLog_DBG(SVC_LOG_STL_CAP, "[CapDataReady] CapInstance %u CapInfoId %u", (UINT32)pInfo->CapInstance, CurInfoID);
        if (CurInfoID < SVC_MAX_PIV_STREAMS) {
            SvcLog_DBG(SVC_LOG_STL_CAP, "[CapDataReady] status %u", StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].Status, 0);
        }
    }
    if (CurInfoID < SVC_MAX_PIV_STREAMS) {
        if (StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].Status > 0U) {
            SvcLog_OK(SVC_LOG_STL_CAP, "[CapDataReady] AuxBufFormat %u", pInfo->AuxBufFormat, 0);

            Rval = AmbaKAL_GetSysTickCount(&DoneTime);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
            }
            Rval = AmbaWrap_memset(&Msg, 0, sizeof(Msg));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            } else {
                {
                    UINT32 Duration;
                    if (DoneTime >= StlCap.StartTime) {
                        Duration = DoneTime - StlCap.StartTime;
                    } else {
                        Duration = (0xFFFFFFFFU - StlCap.StartTime) + DoneTime;
                    }
                    SvcLog_OK(SVC_LOG_STL_CAP, "[CapDataReady] CurInfoID %u time %u ms", CurInfoID, Duration);
                }
                //SvcLog_OK(SVC_LOG_STL_CAP, "RawStat %u %u", StlProc.RawStat.MainStat, StlProc.RawStat.ScrnStat);
                //SvcLog_OK(SVC_LOG_STL_CAP, "        %u", StlProc.RawStat.ThmbStat, 0);
                SvcLog_OK(SVC_LOG_STL_CAP, "w %u h %u", pInfo->Buf.Window.Width, pInfo->Buf.Window.Height);
                SvcStillLog("[CapDataReady] addr %p, pitch %u pts %p, seq %p", pInfo->Buf.BaseAddr, pInfo->Buf.Pitch, (ULONG)pInfo->CapPts, (ULONG)pInfo->CapSequence, 0);

                Msg.CapInstance = (UINT16)CurInfoID;
                Msg.Pitch = pInfo->Buf.Pitch;
                //Msg.Pitch = StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].BufInfo.Pitch;
                //SvcLog_OK(SVC_LOG_STL_CAP, "=> pitch %u", Msg.Pitch, 0);
                //Msg.Width = pInfo->Buf.Window.Width;
                Msg.Width = StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].BufInfo.Window.Width;
                Msg.Height = StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].BufInfo.Window.Height;
                //SvcLog_OK(SVC_LOG_STL_CAP, "CapCfgRp %u, img h %u", StlCap.CapCfgRp[CurInfoID], Msg.Height);
                Msg.BaseAddrY = pInfo->Buf.BaseAddr;

                if (pInfo->AuxBufFormat == RAW_FORMAT_MEM_RAW) {
                    Msg.BaseAddrUV = pInfo->AuxBuf.BaseAddr;
                    SvcStillLog("[CapDataReady] aux addr %p, pitch %u", pInfo->AuxBuf.BaseAddr, pInfo->AuxBuf.Pitch, 0, 0, 0);
                    Msg.CapType = SVC_FRAME_TYPE_RAW;
                    Msg.RawCmpr = StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].RawCmpr;
                    if (pInfo->AuxBuf.BaseAddr != 0U) {
                        Msg.CeAvail = 1;
                    }
                    Msg.Height = pInfo->Buf.Window.Height;
                    ProcessEventData(&Msg);
                }
                if (pInfo->AuxBufFormat == RAW_FORMAT_MEM_YUV420) {
                    Msg.CapType = SVC_FRAME_TYPE_YUV;
                    Rval = AmbaDSP_CalcStillYuvExtBufSize(StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].StreamID, DSP_DATACAP_BUFTYPE_YUV, &Pitch, &BufSize);
                    if (Rval == OK) {
                        if (Pitch != Msg.Pitch) {
                            SvcLog_NG(SVC_LOG_STL_CAP, "AMBA_DSP_STL_RAW_DATA_INFO_s.Buf.Pitch %u != expected %u", Msg.Pitch, Pitch);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_CalcStillYuvExtBufSize failed! return 0x%x", Rval, 0U);
                    }
                    Msg.BaseAddrUV = Msg.BaseAddrY + (((ULONG)BufSize << 1UL) / 3UL);
                    if (StlCap_DebugOn == 1U) {
                        SvcStillLog("[CapDataReady] y addr %p, uv %p", Msg.BaseAddrY, Msg.BaseAddrUV, 0, 0, 0);
                    }
                    Msg.DataFmt = AMBA_DSP_YUV420;
                    ProcessEventData(&Msg);
                }
                if (pInfo->AuxBufFormat == RAW_FORMAT_MEM_YUV422) {
                    Msg.CapType = SVC_FRAME_TYPE_YUV;
    #ifdef CONFIG_SOC_H22
                    Pitch = StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].BufInfo.Pitch;
                    BufSize = ((UINT32)Msg.Pitch * SCAP_ALIGN(16, (UINT32)Msg.Height)) << 1U;
    #else
                    Rval = AmbaDSP_CalcStillYuvExtBufSize(StlCap.CapInfo[CurInfoID][StlCap.CapCfgRp[CurInfoID]].StreamID, DSP_DATACAP_BUFTYPE_YUV, &Pitch, &BufSize);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_CalcStillYuvExtBufSize failed! return 0x%x", Rval, 0U);

                    }
    #endif
                    if (Pitch != Msg.Pitch) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "AMBA_DSP_STL_RAW_DATA_INFO_s.Buf.Pitch %u != expected %u", Msg.Pitch, Pitch);
                    }
                    Msg.BaseAddrUV = Msg.BaseAddrY + ((ULONG)BufSize >> 1UL);
                    if (StlCap_DebugOn == 1U) {
                        SvcStillLog("[CapDataReady] BaseAddrUV %p", Msg.BaseAddrUV, 0, 0, 0, 0);
                    }
                    Msg.DataFmt = AMBA_DSP_YUV422;
                    ProcessEventData(&Msg);
                }
            }
        } else {
            Rval = OK;
        }
    }
    return Rval;
}

/**
* stop still image process
*/
UINT32 SvcStillCap_ImgStop(void)
{
    UINT32 Rval = OK;
    UINT16 VinID;

#ifdef CONFIG_BUILD_IMGFRW_AAA
    for (VinID = 0; VinID < AMBA_DSP_MAX_VIN_NUM; VinID++) {
        if ((((UINT32)1 << VinID) & StlCap.CapVinMask) != 0U) {
            SvcImg_StillStop(VinID);
        }
    }
#endif

#ifdef CONFIG_BSP_CV2DK
    {
       AMBA_SENSOR_CHANNEL_s  SsChan;

       Rval = AmbaWrap_memset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
       if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
       } else {
           for (VinID = 0; VinID < AMBA_DSP_MAX_VIN_NUM; VinID++) {
                if (AllSensorID_Disable[VinID] != 0xFFFFFFFFU) {
                    SsChan.VinID    = VinID;
                    SsChan.SensorID = AllSensorID_Disable[VinID];
                    Rval = AmbaSensor_Disable(&SsChan);
                    if (Rval != 0U) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaSensor_Disable() return 0x%x", __LINE__, Rval);
                        SvcLog_NG(SVC_LOG_STL_CAP, "    VinID(%d), SensorID(0x%X)", SsChan.VinID, SsChan.SensorID);
                    }

                }
           }
       }
    }
#endif

    AmbaMisra_TouchUnused(&VinID);
    AmbaMisra_TouchUnused(&Rval);

    return Rval;
}

static UINT32 RawCapVinCheck(const SVC_CAP_CFG_s *pCfg, const UINT16 VinID, const UINT16 NumCapInstance, UINT16 *pNumSensorPerCap, AMBA_SENSOR_STATUS_INFO_s *pSensorStatus)
{
    UINT32 Rval, SensorIdx;
    AMBA_SENSOR_CHANNEL_s  SsChan;

    Rval = AmbaWrap_memset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    for (SensorIdx = 0; SensorIdx < SVC_CAP_MAX_SENSOR_PER_VIN; SensorIdx ++) {
        if (((UINT32)StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanMask & ((UINT32)1 << SensorIdx)) != 0U) {
            UINT32 SensorID = 0;
            UINT32 IsHiso = 0;
            SvcLog_DBG(SVC_LOG_STL_CAP, "VinID (%d) SensorIdx (0x%8x)", VinID, SensorIdx);
            Rval = SvcResCfg_GetSensorIDOfSensorIdx(VinID, SensorIdx, &SensorID);
            SvcLog_DBG(SVC_LOG_STL_CAP, "SensorID (0x%x) NumSensorPerCap %u", SensorID, *pNumSensorPerCap);
            if (Rval == OK) {
                SsChan.VinID    = VinID;
                SsChan.SensorID = SensorID;
                Rval = AmbaSensor_GetStatus(&SsChan, pSensorStatus);
            }

#ifdef CONFIG_BUILD_IMGFRW_AAA
#ifndef CONFIG_SOC_H22
            SvcImg_StillAaa(VinID, (UINT32)*pNumSensorPerCap, pCfg->CapNum, &IsHiso);
#endif
#endif
            if (IsHiso <= 255U) {
                StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].IsHiso[SensorIdx] = (UINT8)IsHiso;
            } else {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] IsHiso %u", __LINE__, IsHiso);
            }
            SvcLog_OK(SVC_LOG_STL_CAP, "IsHiso %u", IsHiso, 0);

            if (pCfg->CapOnly == 0U) {
                if (Rval == OK) {
                    SvcStillLog("[RawCapVinCheck] CapInstance %u VinID %u BaseAddr %p CapNum %u", NumCapInstance, VinID, pCfg->BufferAddr, pCfg->CapNum, 0);
                    if (pCfg->AebNum <= SVC_STL_MAX_AEB_NUM) {
                        Rval = SvcStillMux_SetExifByVin((UINT32)VinID, SensorIdx, (UINT32)*pNumSensorPerCap, (UINT8)pCfg->AebNum);
                    } else {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AebNum %u > SVC_STL_MAX_AEB_NUM", __LINE__, pCfg->AebNum);
                    }
                }
            }
            if (*pNumSensorPerCap < SVC_CAP_MAX_SENSOR_PER_VIN) {
                *pNumSensorPerCap = *pNumSensorPerCap + 1U;
            } else {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] *pNumSensorPerCap %u >= SVC_CAP_MAX_SENSOR_PER_VIN", __LINE__, *pNumSensorPerCap);
            }
        }
    }
    return Rval;
}

static UINT32 RawCapStart(const SVC_CAP_CFG_s *pCfg)
{
    UINT16 CapInstance[AMBA_DSP_MAX_VIN_NUM];
    AMBA_DSP_DATACAP_CFG_s StlCapCfg[AMBA_DSP_MAX_VIN_NUM];
    AMBA_DSP_DATACAP_CTRL_s StlCapCtrl[AMBA_DSP_MAX_VIN_NUM];
    UINT32 Rval, SensorIdx;
    UINT16 VinID, NumCapInstance = 0, NumSensorPerCap = 0, NumCap = 0;
    UINT32 IsCapCe = 0U;

    Rval = AmbaWrap_memset(StlCapCfg, 0x0, sizeof(StlCapCfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(CapInstance, 0x0, sizeof(CapInstance));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(StlCapCtrl, 0x0, sizeof(StlCapCtrl));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    StlCap.CapVinMask = 0;

    AmbaSvcWrap_MisraMemset(StlCap.CapInfoId, 255, sizeof(StlCap.CapInfoId));

    for (VinID = 0; VinID < AMBA_DSP_MAX_VIN_NUM; VinID++) {
        if ((((UINT32)1 << VinID) & pCfg->StrmMsk) != 0U) {
            UINT16 RawWidth;
            AMBA_SENSOR_STATUS_INFO_s SensorStatus;
            Rval = AmbaWrap_memset(&SensorStatus, 0, sizeof(AMBA_SENSOR_STATUS_INFO_s));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
            }
            NumSensorPerCap = 0;

#ifdef CONFIG_BUILD_IMGFRW_AAA
            SvcImg_StillStart(VinID);
#endif

            Rval = RawCapVinCheck(pCfg, VinID, NumCapInstance, &NumSensorPerCap, &SensorStatus);

            if (Rval == OK) {
                UINT16 Pitch;
                StlCapCfg[NumCapInstance].CapDataType = DSP_DATACAP_TYPE_RAW;
                StlCapCfg[NumCapInstance].Index = VinID;
                StlCapCfg[NumCapInstance].AllocType = ALLOC_EXTERNAL_CYCLIC;
                StlCapCfg[NumCapInstance].BufNum = (UINT16)pCfg->CapNum;
                StlCapCfg[NumCapInstance].OverFlowCtrl = 0;            /* 0 : Wait for Append, 1: Rounding */
#ifndef CONFIG_SOC_H22
                StlCapCfg[NumCapInstance].CmprRate = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].Cmpr;
#endif
                StlCapCfg[NumCapInstance].DataBuf.Window.Width  = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].CapW;
                StlCapCfg[NumCapInstance].DataBuf.Window.Height = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].CapH;
                Rval = AmbaDSP_GetCmprRawBufInfo(StlCapCfg[NumCapInstance].DataBuf.Window.Width, StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].Cmpr, &RawWidth, &Pitch);
                StlCapCfg[NumCapInstance].DataBuf.Pitch = Pitch;
            }
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[%u] AmbaDSP_GetCmprRawBufInfo failed, return 0x%x", __LINE__, Rval);
            } else {
                SvcLog_DBG(SVC_LOG_STL_CAP, "[AmbaDSP_DataCapCfg] Pitch %u", StlCapCfg[NumCapInstance].DataBuf.Pitch, 0);
                if (NumCapInstance == 0U) {
                    StlCapCfg[NumCapInstance].DataBuf.BaseAddr      = pCfg->BufferAddr;
                } else {
                    StlCapCfg[NumCapInstance].DataBuf.BaseAddr      = StlCapCfg[NumCapInstance - 1U].DataBuf.BaseAddr + (ULONG)StlCap.CapInfo[NumCapInstance - 1U][StlCap.CapCfgWp[NumCapInstance]].BufSize;
                }
                if (SensorStatus.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
                    IsCapCe = 1U;
                }

                if (StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ForceCapCe > 0U) {
                    if (StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ForceCapCe == 1U) {
                        IsCapCe = 1U;
                    } else {
                        IsCapCe = 0U;
                    }
                    if (StlCap_DebugOn == 1U) {
                        SvcLog_DBG(SVC_LOG_STL_CAP, "[RawCapStart] ForceCapCe: %d", StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ForceCapCe, 0U);
                    }
                }
                if (IsCapCe > 0U) {
                    UINT16 Pitch;
                    StlCapCfg[NumCapInstance].AuxDataNeeded = 1;
                    StlCapCfg[NumCapInstance].AuxDataBuf.Window.Width  = (UINT16)(StlCapCfg[NumCapInstance].DataBuf.Window.Width >> 2U);
                    Rval = AmbaDSP_GetCmprRawBufInfo(StlCapCfg[NumCapInstance].AuxDataBuf.Window.Width, StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].Cmpr, &RawWidth, &Pitch);
                    StlCapCfg[NumCapInstance].AuxDataBuf.Pitch = Pitch;
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[%u] AmbaDSP_GetCmprRawBufInfo failed, return 0x%x", __LINE__, Rval);
                    }
                    StlCapCfg[NumCapInstance].AuxDataBuf.Window.Height = (UINT16)(StlCapCfg[NumCapInstance].DataBuf.Window.Height);
                    StlCapCfg[NumCapInstance].AuxDataBuf.BaseAddr = StlCapCfg[NumCapInstance].DataBuf.BaseAddr +
                                                                ((ULONG)StlCapCfg[NumCapInstance].DataBuf.Pitch * (ULONG)StlCapCfg[NumCapInstance].DataBuf.Window.Height * (ULONG)pCfg->CapNum);
                }
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].StreamID = (UINT8)VinID;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].CapType = SVC_FRAME_TYPE_RAW;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].RawCmpr = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].Cmpr;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].CapOnly = pCfg->CapOnly;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].RestartLiveview = pCfg->StopLiveview;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].PicType = pCfg->PicType;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].SyncEncStart = pCfg->SyncEncStart;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].Index = StlCapCfg[NumCapInstance].Index;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].BufInfo.Pitch = StlCapCfg[NumCapInstance].DataBuf.Pitch;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].BufInfo.Window.Width = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].EffectW;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].BufInfo.Window.Height = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].EffectH;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].BufInfo.BaseAddr = StlCapCfg[NumCapInstance].DataBuf.BaseAddr;
                if ((((StlCapCfg[NumCapInstance].DataBuf.Pitch < SVC_STL_MAX_W)    && (StlCapCfg[NumCapInstance].DataBuf.Window.Height < SVC_STL_MAX_H)) &&
                     ((StlCapCfg[NumCapInstance].AuxDataBuf.Pitch < SVC_STL_MAX_W) && (StlCapCfg[NumCapInstance].AuxDataBuf.Window.Height < SVC_STL_MAX_H))) &&
                     (SVC_STL_MAX_CAP_NUM >= pCfg->CapNum)) {
                    StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].BufSize = pCfg->CapNum * (((UINT32)StlCapCfg[NumCapInstance].DataBuf.Pitch * (UINT32)StlCapCfg[NumCapInstance].DataBuf.Window.Height)
                                                                                        + ((UINT32)StlCapCfg[NumCapInstance].AuxDataBuf.Pitch * (UINT32)StlCapCfg[NumCapInstance].AuxDataBuf.Window.Height));
                }
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].Status = 1;

                if (StlCap_DebugOn == 1U) {
                    SvcStillLog("[RawCapStart] BaseAddr %p AuxDataBuf %p", StlCapCfg[NumCapInstance].DataBuf.BaseAddr, StlCapCfg[NumCapInstance].AuxDataBuf.BaseAddr, 0, 0, 0);
                }
                SvcLog_OK(SVC_LOG_STL_CAP, "cap w %u h %u", StlCapCfg[NumCapInstance].DataBuf.Window.Width, StlCapCfg[NumCapInstance].DataBuf.Window.Height);
                Rval = AmbaDSP_DataCapCfg(NumCapInstance, &StlCapCfg[NumCapInstance]);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_DataCapCfg failed, return 0x%x", Rval, 0);
                }
                StlCap.CapInfoId[NumCapInstance] = (UINT8)NumCapInstance;
                CapInstance[NumCapInstance] = NumCapInstance;
                StlCapCtrl[NumCapInstance].CapNum = pCfg->CapNum;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].CapNum = pCfg->CapNum;
                StlCap.CapInfo[NumCapInstance][StlCap.CapCfgWp[NumCapInstance]].AebNum = pCfg->AebNum;
                if (StlCap.CapCfgWp[NumCapInstance] == ((UINT8)NUM_CAP_CFG - 1U)) {
                    StlCap.CapCfgWp[NumCapInstance] = 0;
                } else {
                    StlCap.CapCfgWp[NumCapInstance] ++;
                }
                if (StlCap_DebugOn == 1U) {
                    SvcLog_DBG(SVC_LOG_STL_CAP, " CapNum %u NumSensorPerCap %u", pCfg->CapNum, NumSensorPerCap);
                }
                NumCap += (UINT16)((UINT16)pCfg->CapNum * NumSensorPerCap);
                for (SensorIdx = 0; SensorIdx < SVC_CAP_MAX_SENSOR_PER_VIN; SensorIdx ++) {
                    StlCap.CapCnt[NumCapInstance][SensorIdx] = 0;
                }
                NumCapInstance ++;
                StlCap.CapVinMask |= ((UINT32)1 << VinID);
            }
        }
    }

    if (pCfg->PicType == SVC_STL_TYPE_CLEAN_RAW) {
        StlCap_CleanRawDump = 1;
    } else {
        StlCap_CleanRawDump = 0;
    }

    if (Rval == OK) {
        if (pCfg->PicType == SVC_STL_TYPE_RAW) {
            Rval = SvcStillMux_WaitAvail();
        }
    }
    if (Rval == OK) {
        SvcStillProc_SetNumCap((UINT32)NumCap);
        /* dsp cap start */
        StlCap.NumCapInstance = NumCapInstance;
        Rval = AmbaDSP_DataCapCtrl(NumCapInstance, CapInstance, StlCapCtrl, NULL);
    }
    return Rval;
}

static UINT32 CheckYuvStream(const UINT32 DispNum, const UINT16 StreamID, const UINT16 NumCapStream, UINT16 *pNumQvOnlyStream, UINT8 *pCapFromEncStrm, AMBA_DSP_DATACAP_CFG_s *pStlCapCfg)
{
    const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
    UINT32 Rval;
    if (DispNum > StreamID) {
        Rval = SvcStillMux_SetExifByFov(pResCfg->DispStrm[StreamID].StrmCfg.ChanCfg[0].FovId, (UINT8)StreamID);
        if (Rval == OK) {
            pStlCapCfg[NumCapStream].DataBuf.Window.Width = pResCfg->DispStrm[StreamID].StrmCfg.Win.Width;
            pStlCapCfg[NumCapStream].DataBuf.Window.Height = pResCfg->DispStrm[StreamID].StrmCfg.Win.Height;
        }
        if (StlCap.PivQview.Enable == 1U) {
            UINT8 i;
            for (i = 0; i < StlCap.PivQview.NumStrm; i ++) {
                if (StreamID == StlCap.PivQview.StrmId[i]) {
                    if (StlCap.PivQview.QvOnly[i] == 1U) {
                        if (*pNumQvOnlyStream < SVC_MAX_PIV_STREAMS) {
                            *pNumQvOnlyStream = *pNumQvOnlyStream + 1U;
                        } else {
                            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] *pNumQvOnlyStream %u >= SVC_MAX_PIV_STREAMS", __LINE__, *pNumQvOnlyStream);
                        }
                    }
                    break;
                }
            }
        }
    } else {
        if ((DispNum <= AMBA_DSP_MAX_VOUT_NUM) && (pResCfg->RecNum <= (UINT32)CONFIG_ICAM_MAX_REC_STRM)) {
            if (((pResCfg->RecNum + DispNum) > StreamID) && (StreamID >= DispNum)) {
                UINT16 RecStrm = StreamID - (UINT16)DispNum;
                Rval = SvcStillMux_SetExifByFov(pResCfg->RecStrm[RecStrm].StrmCfg.ChanCfg[0].FovId, (UINT8)StreamID);
                if (Rval == OK) {
                    pStlCapCfg[NumCapStream].DataBuf.Window.Width = pResCfg->RecStrm[RecStrm].StrmCfg.Win.Width;
                    pStlCapCfg[NumCapStream].DataBuf.Window.Height = pResCfg->RecStrm[RecStrm].StrmCfg.Win.Height;
                    StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].StreamID = (UINT8)StreamID;
                    *pCapFromEncStrm = 1;
                }
            } else {
                SvcLog_NG(SVC_LOG_STL_CAP, "RecNum 0x%x StreamID %u out of limitation", pResCfg->RecNum, StreamID);
                Rval = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_STL_CAP, "RecNum 0x%x DispNum %u out of limitation", pResCfg->RecNum, DispNum);
            Rval = SVC_NG;
        }
    }
    return Rval;
}

static UINT32 YuvCapStart(const SVC_CAP_CFG_s *pCfg)
{
    UINT16 CapInstance[SVC_MAX_PIV_STREAMS];
    AMBA_DSP_DATACAP_CFG_s StlCapCfg[SVC_MAX_PIV_STREAMS];
    AMBA_DSP_DATACAP_CTRL_s StlCapCtrl[SVC_MAX_PIV_STREAMS];
    UINT32 Rval;
    UINT16 StreamID, NumCapStream = 0, NumQvOnlyStream = 0, CurCapInstance;
    UINT8 CapFromEncStrm = 0, IsTimelapse;
    const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
#ifndef CONFIG_SOC_H22
    UINT16 LastStreamID = 0;
#endif

    Rval = AmbaWrap_memset(StlCapCfg, 0x0, sizeof(StlCapCfg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(CapInstance, 0x0, sizeof(CapInstance));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = AmbaWrap_memset(StlCapCtrl, 0x0, sizeof(StlCapCtrl));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }

    if (pCfg->CapType == SVC_FRAME_TYPE_YUV) {
#if defined(CONFIG_SVC_ENABLE_VOUT_DEF_IMG)
        UINT32 DispNum = 0;
#else
        UINT32 DispNum = pResCfg->DispNum;
#endif
        if ((DispNum <= AMBA_DSP_MAX_VOUT_NUM) && (pResCfg->RecNum <= (UINT32)CONFIG_ICAM_MAX_REC_STRM)) {

            AmbaSvcWrap_MisraMemset(StlCap.CapInfoId, 255, sizeof(StlCap.CapInfoId));

            for (StreamID = 0; StreamID < (DispNum + pResCfg->RecNum); StreamID++) {
                if ((((UINT32)1 << StreamID) & pCfg->StrmMsk) != 0U) {

                    if ((((UINT32)1 << StreamID) & pCfg->TimelapseMsk) != 0U) {
                        IsTimelapse = 1U;
                    } else {
                        IsTimelapse = 0U;
                    }

                    CurCapInstance = StreamID;

                    SvcLog_OK(SVC_LOG_STL_CAP, "[AmbaDSP_DataCapCfg] CapInstance %u StreamID %u", CurCapInstance, StreamID);
                    if (NumCapStream >= (UINT16)SVC_MAX_PIV_STREAMS) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[AmbaDSP_DataCapCfg] NumCapStream %u > SVC_MAX_PIV_STREAMS %u", NumCapStream, SVC_MAX_PIV_STREAMS);
                        break;
                    }
                    StlCapCfg[NumCapStream].CapDataType = (pCfg->SyncEncStart == 1U)? DSP_DATACAP_TYPE_SYNCED_YUV: DSP_DATACAP_TYPE_YUV;
                    if (StlCap_DebugOn == 1U) {
                        SvcLog_OK(SVC_LOG_STL_CAP, "[AmbaDSP_DataCapCfg] CapDataType %u", StlCapCfg[NumCapStream].CapDataType, 0);
                    }
                    if (pCfg->SyncEncStart == 1U) {
                        StlCapCfg[NumCapStream].Index = StreamID - (UINT16)DispNum;
                    } else {
                        StlCapCfg[NumCapStream].Index = StreamID;
                    }
                    StlCapCfg[NumCapStream].AllocType = ALLOC_EXTERNAL_CYCLIC; /* MUST be ALLOC_EXTERNAL_DISTINCT and ALLOC_EXTERNAL_CYCLIC */
                    StlCapCfg[NumCapStream].BufNum = 1;
                    StlCapCfg[NumCapStream].OverFlowCtrl = 1;            /* 0 : Wait for Append, 1: Rounding */
                    if (StlCap_DebugOn == 1U) {
                        SvcLog_OK(SVC_LOG_STL_CAP, "[AmbaDSP_DataCapCfg] Index %u", StlCapCfg[NumCapStream].Index, 0);
                    }
                    StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].StreamID = StreamID;

                    Rval = CheckYuvStream(DispNum, StreamID, NumCapStream, &NumQvOnlyStream, &CapFromEncStrm, StlCapCfg);

                    if (Rval == SVC_OK) {
                        UINT32 Temp = SCAP_ALIGN(YUV_ALIGN_BYTE, (UINT32)StlCapCfg[NumCapStream].DataBuf.Window.Width);
                        if (Temp <= SVC_STL_MAX_W) {
                            StlCapCfg[NumCapStream].DataBuf.Pitch = (UINT16)Temp;
                        } else {
                            SvcLog_NG(SVC_LOG_STL_CAP, "[%u] Temp %u > SVC_STL_MAX_W", __LINE__, Temp);
                        }
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].PicType = pCfg->PicType;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].SyncEncStart = pCfg->SyncEncStart;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].CapFromEncStrm = CapFromEncStrm;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].CapOnly = pCfg->CapOnly;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].Index = StlCapCfg[NumCapStream].Index;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].BufInfo.Pitch = StlCapCfg[NumCapStream].DataBuf.Pitch;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].BufInfo.Window.Width = StlCapCfg[NumCapStream].DataBuf.Window.Width;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].BufInfo.Window.Height = StlCapCfg[NumCapStream].DataBuf.Window.Height;
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].Status = 1;
#ifdef CONFIG_SOC_H22
                        Temp = SCAP_ALIGN(16, (UINT32)StlCapCfg[NumCapStream].DataBuf.Window.Height);
                        if (Temp <= SVC_STL_MAX_W) {
                            StlCapCfg[NumCapStream].DataBuf.Window.Height = (UINT16)Temp;
                        } else {
                            SvcLog_NG(SVC_LOG_STL_CAP, "[%u] Temp %u > SVC_STL_MAX_W", __LINE__, Temp);
                        }
#endif
                        if (NumCapStream == 0U) {
                            StlCapCfg[NumCapStream].DataBuf.BaseAddr = pCfg->BufferAddr;
                        } else {
                            UINT32 BufSize;
#ifdef CONFIG_SOC_H22
                            BufSize = ((UINT32)StlCapCfg[NumCapStream - 1U].DataBuf.Pitch * (UINT32)StlCapCfg[NumCapStream - 1U].DataBuf.Window.Height) << 1U;
#else
                            UINT16 Pitch;
                            if (IsTimelapse == 0U) {
                                Rval = AmbaDSP_CalcStillYuvExtBufSize(LastStreamID, DSP_DATACAP_BUFTYPE_YUV, &Pitch, &BufSize);
                                if (Rval != OK) {
                                    SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_CalcStillYuvExtBufSize failed, return 0x%x", Rval, 0);
                                }
                            }
#endif
                            StlCapCfg[NumCapStream].DataBuf.BaseAddr = StlCapCfg[NumCapStream - 1U].DataBuf.BaseAddr + (ULONG)BufSize;
                        }
                        StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].BufInfo.BaseAddr = StlCapCfg[NumCapStream].DataBuf.BaseAddr;
                        if (StlCap_DebugOn == 1U) {
                            SvcStillLog("[YuvCapStart] BaseAddr %p", StlCapCfg[NumCapStream].DataBuf.BaseAddr, 0, 0, 0, 0);
                        }
                        SvcLog_OK(SVC_LOG_STL_CAP, "cap w %u h %u", StlCapCfg[NumCapStream].DataBuf.Window.Width, StlCapCfg[NumCapStream].DataBuf.Window.Height);
                        if (IsTimelapse == 0U) {
                            Rval = AmbaDSP_DataCapCfg(CurCapInstance, &StlCapCfg[NumCapStream]);
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_DataCapCfg failed, return 0x%x", Rval, 0);
                            }
                        }
                    }
                    StlCap.CapInfoId[CurCapInstance] = (UINT8)NumCapStream;
                    StlCap.CapInfo[NumCapStream][StlCap.CapCfgWp[NumCapStream]].CapNum = pCfg->CapNum;
                    if (StlCap.CapCfgWp[NumCapStream] == ((UINT8)NUM_CAP_CFG - 1U)) {
                        StlCap.CapCfgWp[NumCapStream] = 0;
                    } else {
                        StlCap.CapCfgWp[NumCapStream] ++;
                    }
                    NumCapStream ++;
                    SvcStillProc_SetNumCap((UINT32)NumCapStream - (UINT32)NumQvOnlyStream);
#ifndef CONFIG_SOC_H22
                    LastStreamID = StreamID;
#endif
                }
            }
        }
        if (Rval == OK) {
            if (pCfg->PicType == SVC_STL_TYPE_YUV) {
                Rval = SvcStillMux_WaitAvail();
            }
        }
        if (Rval == OK) {
            StlCap.NumCapInstance = NumCapStream;

            NumCapStream = 0U;
            for (StreamID = 0; StreamID < (DispNum + pResCfg->RecNum); StreamID++) {
                if ((((UINT32)1 << StreamID) & pCfg->StrmMsk) != 0U) {
                    if ((((UINT32)1 << StreamID) & pCfg->TimelapseMsk) == 0U) {
                        CapInstance[NumCapStream] = StreamID;
                        StlCapCtrl[NumCapStream].CapNum = pCfg->CapNum;
                        NumCapStream ++;
                    }
                }
            }

            Rval = SvcDSP_DataCapCtrl(NumCapStream, CapInstance, StlCapCtrl, NULL);
        }
    }
    return Rval;
}

static UINT32 ConfigSensorMode(const SVC_CAP_CFG_s *pCfg)
{
    AMBA_SENSOR_CHANNEL_s  SsChan;
    AMBA_SENSOR_CONFIG_s   SsCfg;
    UINT32 SensorID = 0, AllSensorID = 0, Rval, VinBit, VinBits = 0;
    UINT16 VinID;

#ifdef CONFIG_BSP_CV2DK
    Rval = AmbaWrap_memset(AllSensorID_Disable, 0xFF, sizeof(AllSensorID_Disable));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AllSensorID_Disable memset fail 0x%x", __LINE__, Rval);
    }
#endif

    Rval = AmbaWrap_memset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        for (VinID = 0; VinID < AMBA_DSP_MAX_VIN_NUM; VinID++) {
            VinBit = (UINT32)1 << VinID;
            if ((VinBit & pCfg->StrmMsk) != 0U) {
                UINT32 SensorIdx;
                SvcLog_DBG(SVC_LOG_STL_CAP, "VinID (%d) ChanMask (0x%8x)", VinID, StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanMask);
                for (SensorIdx = 0; SensorIdx < SVC_CAP_MAX_SENSOR_PER_VIN; SensorIdx ++) {
                    if (((UINT32)StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].ChanMask & ((UINT32)1 << SensorIdx)) != 0U) {
                        Rval = SvcResCfg_GetSensorIDOfSensorIdx(VinID, SensorIdx, &SensorID);
                        AllSensorID |= SensorID;
                        SvcLog_OK(SVC_LOG_STL_CAP, "SensorIdx (%u) SensorID (0x%x)", SensorIdx, SensorID);
                    }
                }
                if (Rval == OK) {
                    SvcLog_OK(SVC_LOG_STL_CAP, "AllSensorID (0x%8x)", AllSensorID, 0);
                    SsChan.VinID    = VinID;
                    SsChan.SensorID = AllSensorID;
#ifdef CONFIG_BSP_CV2DK
                    AllSensorID_Disable[VinID] = AllSensorID;
#endif
                    Rval = AmbaSensor_Init(&SsChan);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "AmbaSensor_Init failed, return 0x%x", Rval, 0);
                    } else {
                        Rval = AmbaSensor_Enable(&SsChan);
                        if (Rval != OK) {
                            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaSensor_Enable failed, return 0x%x", Rval, 0);
                        }
                    }
                }
                if (Rval == OK) {
                    Rval = AmbaWrap_memset(&SsCfg, 0, sizeof(AMBA_SENSOR_CONFIG_s));
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                    } else {
                        SsCfg.ModeID = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].CapSensorModeID;
                        SvcLog_OK(SVC_LOG_STL_CAP, "sensor ModeID (%d)", SsCfg.ModeID , 0);
                        Rval = AmbaSensor_Config(&SsChan, &SsCfg);
                        if (Rval != OK) {
                            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaSensor_Config failed, return 0x%x", Rval, 0);
                        }
                    }
                }
                if (Rval == OK) {
                    AMBA_DSP_VIN_SUB_CHAN_CFG_s SubChCfg;
                    AMBA_SENSOR_STATUS_INFO_s SensorStatus;
                    UINT16 VinW, VinH;
                    Rval = AmbaWrap_memset(&SubChCfg, 0, sizeof(SubChCfg));
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
                    } else {
                        UINT32 Temp;
                        Rval = AmbaSensor_GetStatus(&SsChan, &SensorStatus);
                        if (Rval != OK) {
                            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaSensor_GetStatus failed, return 0x%x", Rval, 0);
                        }
                        SubChCfg.SubChan.Index     = VinID;
                        SubChCfg.Option            = AMBA_DSP_VIN_CAP_OPT_PROG;
                        SubChCfg.ConCatNum         = 1U;
                        SubChCfg.IntcNum           = 1U;
                        VinW = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].CapW;
                        VinH = StlCap.Vin[VinID][StlCap.VinInfoRp[VinID]].CapH;
                        SubChCfg.CaptureWindow.Width = VinW;
                        SubChCfg.CaptureWindow.Height = VinH;
                        Temp = (((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Width - (UINT32)VinW) >> 1U) + SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartX;
                        if (Temp <= SVC_STL_MAX_W) {
                            SubChCfg.CaptureWindow.OffsetX = (UINT16)Temp;
                        } else {
                            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] Temp %u > SVC_STL_MAX_W", __LINE__, Temp);
                        }
                        Temp = (((UINT32)SensorStatus.ModeInfo.OutputInfo.RecordingPixels.Height - (UINT32)VinH) >> 1U) + SensorStatus.ModeInfo.OutputInfo.RecordingPixels.StartY;
                        if (Temp <= SVC_STL_MAX_H) {
                            SubChCfg.CaptureWindow.OffsetY = (UINT16)Temp;
                        } else {
                            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] Temp %u > SVC_STL_MAX_H", __LINE__, Temp);
                        }
                        Rval = AmbaDSP_LiveviewConfigVinCapture(VinID, 1, &SubChCfg);
                        if (Rval != OK) {
                            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_LiveviewConfigVinCapture failed, return 0x%x", Rval, 0);
                        } else {
                            VinBits |= VinBit;
                        }
                    }
                }
            }
        }
        if (Rval == OK) {
            SvcLiveview_SetStillVinPostBits(VinBits);
        }
    }
    return Rval;
}

static void QvTimeout(UINT32 VoutId)
{
    UINT32 Rval, Flag;
    if (VoutId <= NUM_VOUT_IDX) {
        Flag = (UINT32)QV_TIMEOUT_VOUT_0 << VoutId;
        Rval = AmbaKAL_EventFlagSet(&StlCap.QvFlag, Flag);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_EventFlagSet VoutId %u failed! return 0x%x", VoutId, Rval);
        }
    } else {
        SvcLog_NG(SVC_LOG_STL_CAP, "QvTimeout wrong VoutId %u", VoutId, 0);
    }
}

static void QvTimerStop(UINT8 VoutId)
{
    UINT32 Rval;

    Rval = AmbaKAL_TimerStop(&StlCap.QvInfo[VoutId].Timer);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_TimerStop failed! return %u", Rval, 0U);
    }
    if (SVC_OK == Rval) {
        Rval = AmbaKAL_TimerDelete(&StlCap.QvInfo[VoutId].Timer);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_TimerDelete failed! return %u", Rval, 0U);
        }
    }
    if (Rval == OK) {
        SVC_STL_CAP_EVENT_s CapEvent;
        StlCap.QvInfo[VoutId].Enable = 0;
        /* check if all qv is done */
#ifdef CONFIG_SOC_CV28
        /* delete qv task */
        CapEvent.Event = STL_CAP_EVENT_CAP_QV_STOP;
        Rval = AmbaKAL_MsgQueueSend(&StlCap.CapQue, &CapEvent, 1000);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_MsgQueueSend failed! return %u", Rval, 0U);
        }
#else
        {
            UINT8 i;
            for (i = 0; i < AMBA_DSP_MAX_VOUT_NUM; i ++) {
                if (StlCap.QvInfo[i].Enable == 1U) {
                    break;
                }
                /* delete qv task */
                if (i == (AMBA_DSP_MAX_VOUT_NUM - 1U)) {
                    CapEvent.Event = STL_CAP_EVENT_CAP_QV_STOP;
                    Rval = AmbaKAL_MsgQueueSend(&StlCap.CapQue, &CapEvent, 1000);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_MsgQueueSend failed! return %u", Rval, 0U);
                    }
                }
            }
        }
#endif
    }
}

static void QvStop(UINT8 VoutId)
{
    UINT32 Rval;
    AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s DefImg;
    Rval = AmbaWrap_memset(&DefImg, 0x0, sizeof(DefImg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    }
    Rval = SvcDisplay_SetDefImg(0, VoutId, &DefImg);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "SvcDisplay_SetDefImg failed! return 0x%x", Rval, 0U);
    } else {
        QvTimerStop(VoutId);
    }
}

static void* Qview_TaskEntry(void* EntryArg)
{
    ULONG  ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while ( ArgVal != 0xCafeU ) {
        UINT32 ActualFlags = 0, Rval;
        Rval = AmbaKAL_EventFlagGet(&StlCap.QvFlag, QV_TIMEOUT_VOUT_All, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, KAL_WAIT_FOREVER);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_CAP, "Event flag get failed with 0x%x", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_STL_CAP, "[QV] get flag 0x%x", ActualFlags, 0);
            if ((ActualFlags & QV_TIMEOUT_VOUT_0) == QV_TIMEOUT_VOUT_0) {
                SvcLog_OK(SVC_LOG_STL_CAP, "[QV] stop vout 0", 0, 0);
                QvStop(0);
            }
            if ((ActualFlags & QV_TIMEOUT_VOUT_1) == QV_TIMEOUT_VOUT_1) {
                SvcLog_OK(SVC_LOG_STL_CAP, "[QV] stop vout 1", 0, 0);
                QvStop(1);
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }
    return NULL;
}

static void QvTaskStop(void)
{
    UINT32 Rval;
    Rval = AmbaKAL_EventFlagDelete(&StlCap.QvFlag);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_EventFlagDelete failed! return %u", Rval, 0U);
    }
    if (Rval == SVC_OK) {
        Rval = SvcTask_Destroy(&StlCap.QvTask);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "SvcTask_Destroy for qview failed! return 0x%x", Rval, 0U);
        } else {
            SvcLog_OK(SVC_LOG_STL_CAP, "SvcTask_Destroy for qview successfully", 0, 0U);
        }
    }
    StlCap.QvTaskInit = 0;
    SvcStillCap_ClearStatus(SVC_STL_QV_BUSY);
}

static UINT32 QvTaskStart(void)
{
    static UINT8 QvTaskStack[QV_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char  FlgName[] = "QvFlag";
    UINT32 Rval;

    if (StlCap.QvTaskInit == 0U) {
        StlCap.QvTask.Priority    = StlCap.TaskCtrl.Priority + 1U;
        StlCap.QvTask.EntryFunc   = Qview_TaskEntry;
        StlCap.QvTask.EntryArg    = 0;
        StlCap.QvTask.pStackBase  = QvTaskStack;
        StlCap.QvTask.StackSize   = QV_TASK_STACK_SIZE;
        Rval = SvcTask_Create("QvTask", &StlCap.QvTask);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_STL_CAP, "SvcTask_Create for qview failed! return 0x%x", Rval, 0U);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagCreate(&StlCap.QvFlag, FlgName);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_CAP, "Event flag create failed with 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagClear(&StlCap.QvFlag, 0xFFFFFFFFU);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_CAP, "Event flag clear failed with 0x%x", Rval, 0U);
            }
        }
        StlCap.QvTaskInit = 1;
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

static void QvTimerStart(UINT8 VoutId)
{
    UINT32 Rval;
#ifdef CONFIG_SOC_CV28
    static char  TimerName[AMBA_DSP_MAX_VOUT_NUM][9] = {"QvTimer0"};
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    static char  TimerName[AMBA_DSP_MAX_VOUT_NUM][9] = {"QvTimer0", "QvTimer1", "QvTimer2"};
#else
    static char  TimerName[AMBA_DSP_MAX_VOUT_NUM][9] = {"QvTimer0", "QvTimer1"};
#endif

    Rval = QvTaskStart();
    if (Rval == SVC_OK) {
        Rval = AmbaKAL_TimerCreate(&StlCap.QvInfo[VoutId].Timer, TimerName[VoutId], QvTimeout,
                                   VoutId, SVC_QV_INTERVAL, SVC_QV_INTERVAL, AMBA_KAL_AUTO_START);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_TimerCreate failed! return %u", Rval, 0U);
        }
    }
}

static void QvStart(UINT8 VoutId)
{
    UINT32 Rval;
    AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s DefImg;
    Rval = AmbaWrap_memset(&DefImg, 0x0, sizeof(DefImg));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
    } else {
        DefImg.Pitch = StlCap.QvInfo[VoutId].Pitch;
        DefImg.BaseAddrY = StlCap.QvInfo[VoutId].AddrY;
        DefImg.BaseAddrUV = StlCap.QvInfo[VoutId].AddrUV;
        DefImg.DataFormat = AMBA_DSP_YUV420;
        Rval = SvcDisplay_SetDefImg(1, VoutId, &DefImg);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "SvcDisplay_SetDefImg failed! return 0x%x", Rval, 0U);
        }
    }
    if (Rval == OK) {
        QvTimerStart(VoutId);
    }
}

static void* SvcStillCap_TaskEntry(void* EntryArg)
{
    SVC_STL_CAP_EVENT_s CapEvent;
    UINT32 Rval;
    ULONG  ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while ( ArgVal != 0xCafeU ) {
        Rval = AmbaKAL_MsgQueueReceive(&StlCap.CapQue, &CapEvent, AMBA_KAL_WAIT_FOREVER);
        if (OK == Rval) {
            if (CapEvent.Event == STL_CAP_EVENT_CAP_START) {
                if (CapEvent.Cfg.CapType == SVC_FRAME_TYPE_RAW) {
                    //SvcLog_DBG(SVC_LOG_STL_CAP, "StopLiveview %u, pSvcLivStop %u", CapEvent.Cfg.StopLiveview, (UINT32)pSvcLivStop);
                    if ((CapEvent.Cfg.StopLiveview == 1U) && (pSvcLivStop != NULL)) {
                        Rval = pSvcLivStop();
                        if (Rval == OK) {
                            Rval = ConfigSensorMode(&CapEvent.Cfg);
                        }
                    }
                    if (Rval == OK) {
                        Rval = RawCapStart(&CapEvent.Cfg);
                    }
                    if (Rval == OK) {
                        Rval = AmbaKAL_GetSysTickCount(&StlCap.StartTime);
                        if (Rval != OK) {
                            SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
                        }
                    }
                } else {
                    if (CapEvent.Cfg.CapType == SVC_FRAME_TYPE_YUV) {
                        Rval = YuvCapStart(&CapEvent.Cfg);
                        if ((CapEvent.Cfg.SyncEncStart == 1U) && (CapEvent.Cfg.CapType == SVC_FRAME_TYPE_YUV)) {
                            if (Rval == OK) {
                                Rval = AmbaKAL_EventFlagSet(&StlCap.Flag, SVC_STL_CAP_SENT);
                                if (OK != Rval) {
                                    SvcLog_NG(SVC_LOG_STL_CAP, "Event flag set failed with 0x%x", Rval, 0U);
                                }
                            }
                        }
                        if (Rval == OK) {
                            Rval = AmbaKAL_GetSysTickCount(&StlCap.StartTime);
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_GetSysTickCount failed! return 0x%x", Rval, 0U);
                            }
                        }
                    }
                }
            } else {
                if (CapEvent.Event == STL_CAP_EVENT_CAP_QV_START) {
                    SvcLog_OK(SVC_LOG_STL_CAP, "[QV] start vout %u", CapEvent.QvVoutId, 0);
                    QvStart(CapEvent.QvVoutId);
                } else {
                    if (CapEvent.Event == STL_CAP_EVENT_CAP_QV_STOP) {
                        QvTaskStop();
                    }
                }
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

/**
* turn on/off debugging
* @param [in] On 1-on 0-off
*/
void SvcStillCap_Debug(UINT8 On)
{
    StlCap_DebugOn = On;
    SvcLog_OK(SVC_LOG_STL_CAP, "SvcStillCap_Debug %u", On, 0);
}

static UINT32 CheckSd(void)
{
    UINT32                 Rval = SVC_NG;
    AMBA_SD_CARD_STATUS_s  CardStatus;

    AmbaSvcWrap_MisraMemset(&CardStatus, 0, sizeof(CardStatus));
    if (AmbaSD_GetCardStatus(AMBA_SD_CHANNEL0, &CardStatus) == OK) {
        Rval = SVC_OK;
    } else {
        if (AmbaSD_GetCardStatus(AMBA_SD_CHANNEL1, &CardStatus) == OK) {
            Rval = SVC_OK;
        }
    }

    if (Rval == SVC_NG) {
        SvcLog_NG(SVC_LOG_STL_CAP, "sd card not found", 0, 0);
    } else {
        if (CardStatus.WriteProtect == 1U) {
            SvcLog_NG(SVC_LOG_STL_CAP, "sd card is write-protected", 0, 0);
            Rval = SVC_NG;
        } else {
            AMBA_FS_DRIVE_INFO_t DriveInfo;
            UINT64 FreeSize;
            AmbaSvcWrap_MisraMemset(&DriveInfo, 0, sizeof(DriveInfo));
            Rval = AmbaFS_GetDriveInfo(StlCap.StorageDrive, &DriveInfo);
            FreeSize = (UINT64)DriveInfo.BytesPerSector * (UINT64)DriveInfo.SectorsPerCluster * (UINT64)DriveInfo.EmptyClusters;
            if ((UINT64)StlCap.MinStorageSize > FreeSize) {
                SvcLog_NG(SVC_LOG_STL_CAP, "sd card free space shortage", 0, 0);
                Rval = SVC_NG;
            }
        }
    }
    return Rval;
}

static UINT32 PreCheckSd(const SVC_CAP_CTRL_s *pCapCtrl)
{
    UINT32 Rval;
    if ((pCapCtrl->Cfg.CapOnly == 1U) && (pCapCtrl->Cfg.PicType != SVC_STL_TYPE_YUV) && (pCapCtrl->Cfg.PicType != SVC_STL_TYPE_RAW)) {
        Rval = SVC_OK;
    } else {
        if (pCapCtrl->PivQview.Enable == 1U) {
            UINT8 StreamID, NumQvOnlyStream = 0, NumCapStream = 0;
            const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
#if defined(CONFIG_SVC_ENABLE_VOUT_DEF_IMG)
            UINT32 DispNum = 0;
#else
            UINT32 DispNum = pResCfg->DispNum;
#endif
            UINT8 i;

            if ((DispNum <= AMBA_DSP_MAX_VOUT_NUM) && (pResCfg->RecNum <= (UINT32)CONFIG_ICAM_MAX_REC_STRM)) {
                for (StreamID = 0; StreamID < (DispNum + pResCfg->RecNum); StreamID++) {
                    if ((((UINT32)1 << StreamID) & pCapCtrl->Cfg.StrmMsk) != 0U) {
                        for (i = 0; i < pCapCtrl->PivQview.NumStrm; i ++) {
                            if (StreamID == pCapCtrl->PivQview.StrmId[i]) {
                                if (pCapCtrl->PivQview.QvOnly[i] == 1U) {
                                    if (SVC_MAX_PIV_STREAMS > NumQvOnlyStream) {
                                        NumQvOnlyStream ++;
                                    }
                                    break;
                                }
                            }
                        }
                        if (SVC_MAX_PIV_STREAMS > NumCapStream) {
                            NumCapStream ++;
                        }
                    }
                }
            }
            if ((NumCapStream != NumQvOnlyStream) && (pCapCtrl->Cfg.PicType != SVC_STL_TYPE_NULL_WR)) {
                Rval = CheckSd();
            } else {
                Rval = SVC_OK;
            }
        } else {
            if (pCapCtrl->Cfg.PicType != SVC_STL_TYPE_NULL_WR) {
                Rval = CheckSd();
            } else {
                Rval = OK;
            }
        }
    }
    return Rval;
}

void SvcStillCap_SetQview(const AMBA_DSP_YUV_IMG_BUF_s *pYuv, UINT8 VoutId)
{
    if (VoutId >= AMBA_DSP_MAX_VOUT_NUM ) {
        SvcLog_NG(SVC_LOG_STL_CAP, "SvcStillCap_SetQview invalid VoutId %u", VoutId, 0u);
    } else if (StlCap.Qview.Enable != 1U) {
        SvcLog_NG(SVC_LOG_STL_CAP, "SvcStillCap_SetQview Qview is not enable", 0u, 0u);
    } else if (pYuv == NULL) {
        SvcLog_NG(SVC_LOG_STL_CAP, "SvcStillCap_SetQview Null pYuv", 0u, 0u);
    } else {
        UINT32 Rval;
        SVC_STL_CAP_EVENT_s CapEvent;

        StlCap.QvInfo[VoutId].Enable = 1;
        StlCap.QvInfo[VoutId].AddrY  = pYuv->BaseAddrY;
        StlCap.QvInfo[VoutId].AddrUV = pYuv->BaseAddrUV;
        StlCap.QvInfo[VoutId].Width  = pYuv->Window.Width;
        StlCap.QvInfo[VoutId].Height = pYuv->Window.Height;
        StlCap.QvInfo[VoutId].Pitch  = pYuv->Pitch;

        CapEvent.Event = STL_CAP_EVENT_CAP_QV_START;
        CapEvent.QvVoutId = VoutId;

        SvcStillCap_SetStatus(SVC_STL_QV_BUSY);
        Rval = AmbaKAL_MsgQueueSend(&StlCap.CapQue, &CapEvent, 1000);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "ln %u AmbaKAL_MsgQueueSend failed! return %u", __LINE__, Rval);
        }
    }
}

/**
* start still capture
* @param [in] pCapCtrl pointer to still capture setting
* @return 0-OK, 1-NG
*/
UINT32 SvcStillCap_CapStart(const SVC_CAP_CTRL_s *pCapCtrl)
{
    SVC_STL_CAP_EVENT_s CapEvent;
    UINT32 Rval;
    UINT32 Status = 0;

    SvcStillCap_GetStatus(&Status);
    if (Status != SVC_STL_INIT) {
        SvcLog_DBG(SVC_LOG_STL_CAP, "Warning: capture status 0x%0x != SVC_STL_INIT(0x%x)", Status, SVC_STL_INIT);
        Rval = OK;
    } else {
        Rval = PreCheckSd(pCapCtrl);

        if (Rval == SVC_OK) {
            if (pCapCtrl->Cfg.CapNum > (UINT32)NUM_CAP_CFG) {
                Rval = SVC_NG;
                SvcLog_NG(SVC_LOG_STL_CAP, "CapNum %u > NUM_CAP_CFG %u", pCapCtrl->Cfg.CapNum, NUM_CAP_CFG);
            }
        }
        if (Rval == SVC_OK) {
            if (pCapCtrl->Cfg.AebNum > 1U) {
                if (pCapCtrl->Cfg.AebNum != pCapCtrl->Cfg.CapNum) {
                    Rval = SVC_NG;
                    SvcLog_NG(SVC_LOG_STL_CAP, "AebNum %u != CapNum %u", pCapCtrl->Cfg.AebNum, pCapCtrl->Cfg.CapNum);
                }
                if (Rval == SVC_OK) {
                    if (pCapCtrl->Cfg.AebNum > SVC_STL_MAX_AEB_NUM) {
                        Rval = SVC_NG;
                        SvcLog_NG(SVC_LOG_STL_CAP, "AebNum %u > SVC_STL_MAX_AEB_NUM %u", pCapCtrl->Cfg.AebNum, SVC_STL_MAX_AEB_NUM);
                    }
                }
            }
        }
        if (Rval == SVC_OK) {
            SvcStillCap_SetStatus(SVC_STL_CAP_BUSY);
            Rval = AmbaWrap_memcpy(&CapEvent.Cfg, &pCapCtrl->Cfg, sizeof(SVC_CAP_CFG_s));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
            }

            Rval = AmbaWrap_memcpy(&StlCap.Qview, &pCapCtrl->Qview, sizeof(SVC_STILL_QVIEW_CFG_s));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
            } else {
                SvcLog_DBG(SVC_LOG_STL_CAP, "Qview Enable %u NumQview %u", StlCap.Qview.Enable, StlCap.Qview.NumQview);
            }

            Rval = AmbaWrap_memcpy(&StlCap.PivQview, &pCapCtrl->PivQview, sizeof(SVC_PIV_QVIEW_CFG_s));
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
            } else {
                if (pCapCtrl->Cfg.CapType == SVC_FRAME_TYPE_RAW) {
                    UINT32 VinID;
                    for (VinID = 0; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                        if (((UINT32)pCapCtrl->Cfg.StrmMsk & ((UINT32)1 << VinID)) != 0U) {
                            Rval = AmbaWrap_memcpy(&StlCap.Vin[VinID][StlCap.VinInfoWp[VinID]], &(pCapCtrl->Vin[VinID]), sizeof(SVC_STILL_CAP_VININFO_s));
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
                            } else {
                                if (StlCap.VinInfoWp[VinID] == ((UINT8)NUM_CAP_CFG - 1U)) {
                                    StlCap.VinInfoWp[VinID] = 0;
                                } else {
                                    StlCap.VinInfoWp[VinID] ++;
                                }
                            }
                        }
                    }
                }
                CapEvent.Event = STL_CAP_EVENT_CAP_START;
                Rval = AmbaKAL_MsgQueueSend(&StlCap.CapQue, &CapEvent, 1000);
                if ((pCapCtrl->Cfg.SyncEncStart == 1U) && (pCapCtrl->Cfg.CapType == SVC_FRAME_TYPE_YUV)) {
                    UINT32 ActualFlags = 0;
                    Rval = AmbaKAL_EventFlagGet(&StlCap.Flag, SVC_STL_CAP_SENT, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 1000);
                    if (OK != Rval) {
                        SvcLog_NG(SVC_LOG_STL_CAP, "Event flag get failed with 0x%x", Rval, 0U);
                    }
                }
            }
        }
    }
    return Rval;
}

/**
* initialize and create still capture task
* @param [in] pCfg configuration of capture task
* @return 0-OK, 1-NG
*/
UINT32 SvcStillCap_Create(const SVC_CAP_TSK_s *pCfg)
{
    UINT32       Rval;
    static UINT8 SvcStillCap[SVC_STL_CAP_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char  MsgQueName[] = "CapQue";
    static char  MutexName[] = "CapMutex";
    static char  FlgName[] = "StlCapFlg";

    if (StlCap_Created == 0U) {
        Rval = AmbaWrap_memset(&StlCap, 0x0, sizeof(StlCap));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        } else {
            /* task create */
            StlCap.TaskCtrl.Priority    = pCfg->Priority;
            StlCap.TaskCtrl.EntryFunc   = SvcStillCap_TaskEntry;
            StlCap.TaskCtrl.EntryArg    = 0U;
            StlCap.TaskCtrl.pStackBase  = SvcStillCap;
            StlCap.TaskCtrl.StackSize   = SVC_STL_CAP_STACK_SIZE;
            StlCap.TaskCtrl.CpuBits     = pCfg->CpuBits;
            StlCap.StorageDrive         = pCfg->StorageDrive;
            StlCap.MinStorageSize       = pCfg->MinStorageSize;

            Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STL_RAW_RDY, CapDataReady);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_EventHandlerRegister fail, return 0x%x", Rval, 0);
            }
            if (Rval == OK) {
                //SvcLog_DBG(SVC_LOG_STL_CAP, "CapQue 0x%x, size of event %u", (UINT32)&StlCap.CapQue, sizeof(SVC_STL_CAP_EVENT_s));
                //SvcLog_DBG(SVC_LOG_STL_CAP, "CapQueBuf 0x%x, size of queue %u", (UINT32)StlCap.CapQueBuf, sizeof(StlCap.CapQueBuf));
                Rval = AmbaKAL_MsgQueueCreate(&StlCap.CapQue, MsgQueName, (UINT32)sizeof(SVC_STL_CAP_EVENT_s), StlCap.CapQueBuf, (UINT32)sizeof(StlCap.CapQueBuf));
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "CapQue created failed! return 0x%x", Rval, 0U);
                }
            }
            if (SVC_OK == Rval) {
                Rval = AmbaKAL_MutexCreate(&StlCap.Mutex, MutexName);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_MutexCreate failed! return 0x%x", Rval, 0U);
                }
            }
            if (Rval == SVC_OK) {
                Rval = AmbaKAL_EventFlagCreate(&StlCap.Flag, FlgName);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "Event flag create failed with 0x%x", Rval, 0U);
                }
            }
            if (Rval == SVC_OK) {
                Rval = AmbaKAL_EventFlagClear(&StlCap.Flag, 0xFFFFFFFFU);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "Event flag clear failed with 0x%x", Rval, 0U);
                }
            }
            if (Rval == OK) {
                Rval = SvcTask_Create("SvcStillCap", &StlCap.TaskCtrl);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_STL_CAP, "SvcStillCap_TaskEntry created failed! return 0x%x", Rval, 0U);
                } else {
                    StlCap_Created = 1;
                    SvcStillCap_SetStatus(SVC_STL_INIT);
                }
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

/**
* de-initialize and delete still capture task
* @return 0-OK, 1-NG
*/
UINT32 SvcStillCap_Delete(void)
{
    UINT32 Rval;

    if (StlCap_Created == 1U) {
        Rval = SvcTask_Destroy(&StlCap.TaskCtrl);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_CAP, "SvcStillCap_TaskEntry delete failed! return 0x%x", Rval, 0U);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_EventFlagDelete(&StlCap.Flag);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "Flag delete failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueDelete(&StlCap.CapQue);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_CAP, "CapQue delete failed! return 0x%x", Rval, 0U);
            }
        }
        if (SVC_OK == Rval) {
            Rval = AmbaKAL_MutexDelete(&StlCap.Mutex);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_STL_CAP, "AmbaKAL_MutexDelete failed! return 0x%x", Rval, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STL_RAW_RDY, CapDataReady);
            if (Rval == SVC_OK) {
                StlCap_Created = 0;
            } else {
                SvcLog_NG(SVC_LOG_STL_CAP, "AmbaDSP_EventHandlerUnRegister fail, return 0x%x", Rval, 0);
            }
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}
