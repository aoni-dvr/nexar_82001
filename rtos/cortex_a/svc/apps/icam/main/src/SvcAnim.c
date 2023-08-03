/**
 *  @file SvcAnim.c
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
 *  @details Svc Animation
 *
 */
#include <AmbaDSP_Event.h>
#include <AmbaDSP_Def.h>
#include <AmbaCalib_AVMIF.h>
#include <AmbaCache.h>
#include <AmbaCalibData.h>
#include <AmbaDSP_ImageUtility.h>
#include <AmbaFS.h>
#include "AmbaCT_AvmTunerIF.h"
#include "AmbaUtility.h"

#include "SvcAnim.h"
#include "SvcWrap.h"
#include "SvcAnimMemPool.h"
#include "SvcErrCode.h"
#include "SvcPlat.h"

#define SVC_ANIM_MAX_LZ4_DST                 (1U)
#define SVC_ANIM_MAX_LOAD_DATA               (255U)
#define SVC_ANIM_RING_BUF_COUNT              (5U)
#define SVC_ANIM_LAYOUT_INFO_COUNT           (SVC_ANIM_RING_BUF_COUNT)
#define SVC_ANIM_COMP_SEGMENT_SIZE           (16*1024)

#define SVC_ANIM_MAX_LOAD_DECMPR_TASK        (2U)
#define SVC_ANIM_MAX_MSG                     (128U)
#define SVC_ANIM_TASK_STACK_SIZE             (0x8000U)
#define SVC_ANIM_TASK_PRIORITY               (100U)
#define SVC_ANIM_TASK_CORE                   (0x2U)
#define SVC_ANIM_SCHDLR_TASK_STACK_SIZE      (0x8000U)
#define SVC_ANIM_SCHDLR_TASK_PRIORITY        (101U)
#define SVC_ANIM_SCHDLR_TASK_CORE            (0x2U)
#define SVC_ANIM_RENDER_TASK_STACK_SIZE      (0x8000U)
#define SVC_ANIM_RENDER_TASK_PRIORITY        (60U)
#define SVC_ANIM_RENDER_TASK_CORE            (0x1U)

#define SVC_ANIM_EVENT_FLAG_PLAY             (1U)
#define SVC_ANIM_EVENT_FLAG_LOAD_DECMPR_DONE (2U)
#define SVC_ANIM_EVENT_FLAG_OSD_DONE         (4U)

#define SVC_ANIM_CMD_LOAD                    (0U)
#define SVC_ANIM_CMD_DECOMPRESS              (1U)
#define SVC_ANIM_CMD_OSD_CHANGED_BUF_CB      (2U)
#define SVC_ANIM_CMD_OSD_DONE_CB             (3U)


typedef struct {
    UINT8 DispId; // Id in DispInfo array of SVC_ANIM_LAYOUT_INFO_s
    UINT8 ItemId; // Id in DecmprDone array of SVC_ANIM_LAYOUT_DISP_INFO_s
    UINT32 GroupId; // Id in LoadDone and DecmprDone array of SVC_ANIM_LAYOUT_DISP_INFO_s
} SVC_ANIM_MSG_UPDATE_LAYOUT_STATE_s;

typedef struct {
    UINT8 *Buffer;
    UINT8 ItemId;
    UINT8 ItemCount;
    UINT32 GroupId;
    UINT32 BufSize;
} SVC_ANIM_MSG_LOAD_CFG_s;

typedef struct {
    UINT8 Algo;
    UINT8 DecomprType;
    INT32 SegmentSize;
    INT32 DstNum;
    AMBA_LZ4_TBL_SIZE_s DstTableSize[SVC_ANIM_MAX_LZ4_DST];
    AMBA_LZ4_WIN_s DstWinInfo[SVC_ANIM_MAX_LZ4_DST];
    UINT8 *SrcAddr;
    UINT32 SrcSize;
    UINT8 *DstAddr;
    UINT32 DstSize;
} SVC_ANIM_MSG_DECMPR_CFG_s;

typedef union {
    SVC_ANIM_MSG_LOAD_CFG_s LoadCfg;
    SVC_ANIM_MSG_DECMPR_CFG_s DecmprCfg;
} SVC_ANIM_CMD_INFO_u;

typedef struct {
    UINT8 CmdType;
    UINT8 LayoutId;
    SVC_ANIM_MSG_UPDATE_LAYOUT_STATE_s UpdateLayoutState;
    SVC_ANIM_CMD_INFO_u CmdInfo;
} SVC_ANIM_MSG_s;

typedef struct {
    UINT8 LayoutId;
} SVC_ANIM_RENDER_MSG_s;

typedef struct {
    AMBA_KAL_MUTEX_t Mutex; // to make sure that config will not be modified during playing animation
    AMBA_KAL_MSG_QUEUE_t MsgQueue[SVC_ANIM_MAX_LOAD_DECMPR_TASK];
    AMBA_KAL_MSG_QUEUE_t RenderMsgQueue;
    SVC_ANIM_MSG_s MsgPool[SVC_ANIM_MAX_LOAD_DECMPR_TASK][SVC_ANIM_MAX_MSG];
    SVC_ANIM_RENDER_MSG_s RenderMsgPool[SVC_ANIM_MAX_MSG];
    AMBA_KAL_EVENT_FLAG_t AnimFlag;
    AMBA_KAL_TASK_t SchdlrTask;
    AMBA_KAL_TASK_t Task[SVC_ANIM_MAX_LOAD_DECMPR_TASK];
    AMBA_KAL_TASK_t RenderTask;
} SVC_ANIM_KAL_CFG_s;

typedef struct {
    UINT8 DispId;
    UINT8 GroupCount; // same as GroupCount in SVC_ANIM_DATA_INFO_s
    AMBA_CALIB_DATA_GROUP_DESC_s *GroupDesc[SVC_ANIM_MAX_GROUP];
    UINT32 GroupId[SVC_ANIM_MAX_GROUP]; // same as Id in SVC_ANIM_GROUP_DESC_s
    UINT8 ItemCount[SVC_ANIM_MAX_GROUP];
    UINT8 LoadDone[SVC_ANIM_MAX_GROUP];
    UINT8 DecmprDone[SVC_ANIM_MAX_GROUP][SVC_ANIM_MAX_ITEM];
    SVC_ANIM_DATA_DESC_s WarpDataDesc[SVC_ANIM_LIB_MAX_CHAN];
    SVC_ANIM_DATA_DESC_s BlendDataDesc[SVC_ANIM_LIB_MAX_VOUT][SVC_ANIM_LIB_MAX_CHAN];
    UINT8 *WarpAddr[SVC_ANIM_LIB_MAX_CHAN];
    UINT8 *BlendAddr[SVC_ANIM_LIB_MAX_VOUT][SVC_ANIM_LIB_MAX_CHAN];
} SVC_ANIM_LAYOUT_DISP_INFO_s;

typedef struct {
    UINT32 GroupId;
    UINT8 ItemId;
    UINT8 ItemCount;
    UINT8 *Buffer; // store the data of this team
} SVC_ANIM_LOAD_DATA_INFO_s;

typedef struct {
    UINT8 DispCount;
    UINT8 LoadDataCount;
    UINT8 RenderDone;
    UINT8 *OsdAddr[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_LOAD_DATA_INFO_s LoadDataInfo[SVC_ANIM_MAX_LOAD_DATA];
    SVC_ANIM_INDEX_s *Index[SVC_ANIM_LIB_MAX_DISP];
    SVC_ANIM_LAYOUT_DISP_INFO_s DispInfo[SVC_ANIM_LIB_MAX_DISP];
} SVC_ANIM_LAYOUT_INFO_s;

typedef struct {
    UINT8 VoutCount;
    UINT8 LayoutInfoWp;
    UINT16 PlayLayoutCount;
    UINT32 CalibDataHdlr;
    UINT8 ChannelId[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_CHAN];
    UINT16 ViewZoneId[SVC_ANIM_LIB_MAX_DISP][SVC_ANIM_LIB_MAX_CHAN];
    SVC_ANIM_LIB_OFFSET_s DispOffset[SVC_ANIM_LIB_MAX_DISP];
    SVC_ANIM_LIB_VOUT_CFG_s VoutCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_LIB_OSD_CFG_s OsdCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_LIB_YUV_STRM_CFG_s YuvStrmCfg[SVC_ANIM_LIB_MAX_VOUT];
    SVC_ANIM_EVENT_CALLBACK_f EventCb;
    SVC_ANIM_OSD_BUF_CHANGE_CALLBACK_f OsdChangeCb;
    SVC_ANIM_OSD_DONE_CALLBACK_f OsdDoneCb;
    SVC_ANIM_KAL_CFG_s KalCfg;
    SVC_ANIM_HEADER_s AnimHeader;
    SVC_ANIM_LAYOUT_INFO_s LayoutInfo[SVC_ANIM_LAYOUT_INFO_COUNT];
    SVC_ANIM_PLAY_CFG_s PlayCfg;
} SVC_ANIM_MGR_s;

typedef struct {
    UINT8 BufferId;
    UINT32 LoadRingBufSize;
    UINT32 BlendRingBufSize;
    UINT32 OsdRingBufSize;
    UINT32 UpdateGeoWorkRingBufSize;
    UINT32 UpdateGeoOutRingBufSize;
    UINT32 RawDataSize;
    UINT32 GroupDescSize;
    UINT8 *LoadBufWp;
    UINT8 *LoadRingBuf[SVC_ANIM_RING_BUF_COUNT];
    UINT8 *BlendBufWp;
    UINT8 *BlendRingBuf[SVC_ANIM_RING_BUF_COUNT];
    UINT8 *OsdBufWp[SVC_ANIM_LIB_MAX_VOUT];
    UINT8 *OsdRingBuf[SVC_ANIM_LIB_MAX_VOUT][SVC_ANIM_RING_BUF_COUNT];
    UINT8 *UpdateGeoWorkBufWp;
    UINT8 *UpdateGeoWorkRingBuf[SVC_ANIM_RING_BUF_COUNT];
    UINT8 *UpdateGeoOutBufWp;
    UINT8 *UpdateGeoOutRingBuf[SVC_ANIM_RING_BUF_COUNT];
    UINT8 *RawData;
    UINT8 *GroupDesc;
} SVC_ANIM_BUF_MGR_s;

static SVC_ANIM_MGR_s g_AnimMgr GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;
static SVC_ANIM_BUF_MGR_s g_BufMgr GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

static inline UINT16 U16MAX(UINT16 a, UINT16 b)       {return ((a > b) ? a : b);}
static inline UINT16 U16ALIGN(UINT16 x, UINT16 Align) {return ((x + (Align - 1U)) & ~(Align - 1U));}
static inline UINT32 U32ALIGN(UINT32 x, UINT32 Align) {return ((x + (Align - 1U)) & ~(Align - 1U));}

UINT32 SvcAnim_GetWorkBufSize(const SVC_ANIM_GET_BUF_CFG_s *Cfg, UINT32 *CacheWorkBufSize, UINT32 *NonCacheWorkBufSize)
{
    UINT32 Rval = SVC_OK;
    if (Cfg == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Cfg is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (CacheWorkBufSize == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: CacheWorkBufSize is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else if (NonCacheWorkBufSize == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: NonCacheWorkBufSize is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT8 i;
        UINT32 OsdBufSize = 0U;
        const SVC_ANIM_BUF_MGR_s *pBufMgr = &g_BufMgr;
        const SVC_ANIM_MGR_s *pAnimMgr = &g_AnimMgr;
        void *Ptr;
        UINT32 Tmp;
        AmbaMisra_TypeCast(&Ptr, &pBufMgr);
        AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(SVC_ANIM_BUF_MGR_s));
        AmbaMisra_TypeCast(&Ptr, &pAnimMgr);
        AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(SVC_ANIM_MGR_s));
        *CacheWorkBufSize = 0U;
        *NonCacheWorkBufSize = 0U;
        for (i = 0U; i < Cfg->VoutCount; i++) {
            /* osd buf is required to be cache-aligned */
            Tmp = (UINT32)Cfg->OsdBufPitch[i];
            Tmp*= Cfg->OsdBufHeight[i];
            OsdBufSize += Tmp;
            OsdBufSize = U32ALIGN(OsdBufSize, AMBA_CACHE_LINE_SIZE);
        }
        *NonCacheWorkBufSize += (OsdBufSize * SVC_ANIM_RING_BUF_COUNT);
        g_BufMgr.OsdRingBufSize = OsdBufSize;
        Rval = AmbaCalibData_Create(AMBA_CALIB_DATA_MODE_READ, &g_AnimMgr.CalibDataHdlr);
        if (Rval == SVC_OK) {
            UINT8 *U8Ptr;
            const SVC_ANIM_HEADER_s *pAnimHeader = &g_AnimMgr.AnimHeader;
            AmbaMisra_TypeCast(&U8Ptr, &pAnimHeader);
            Rval = AmbaCalibData_GetUdta(g_AnimMgr.CalibDataHdlr, 0U, U8Ptr, sizeof(SVC_ANIM_HEADER_s));
            if (Rval == SVC_OK) {
                UINT16 j;
                UINT32 CompRingBufSize = 0U, MaxCompSize[SVC_ANIM_LIB_MAX_DISP] = {0U};
                UINT32 WarpRingBufSize = 0U, WarpSize, MaxWarpSize[SVC_ANIM_LIB_MAX_DISP] = {0U};
                UINT32 BlendRingBufSize = 0U, BlendSize, MaxBlendSize[SVC_ANIM_LIB_MAX_DISP] = {0U};
                UINT32 UpdateGeoWorkRingBufSize = 0U, UpdateGeoOutRingBufSize = 0U;
                for (i = 0U; i < pAnimHeader->DispCount; i++) {
                    for (j = 0U; j < pAnimHeader->ViewCount[i]; j++) {
                        UINT8 k;
                        /* Comp size */
                        if (pAnimHeader->CompSize[i][j] > MaxCompSize[i]) {
                            MaxCompSize[i] = pAnimHeader->CompSize[i][j];
                        }
                        /* Warp size */
                        WarpSize = 0U;
                        for (k = 0U; k < pAnimHeader->WarpHeader[i][j].WarpCount; k++) {
                            WarpSize += pAnimHeader->WarpHeader[i][j].HorGridNum[k] * pAnimHeader->WarpHeader[i][j].VerGridNum[k] * sizeof(AMBA_CAL_GRID_POINT_s);
                        }
                        if (WarpSize > MaxWarpSize[i]) {
                            MaxWarpSize[i] = WarpSize;
                        }
                        /* Blend size */
                        BlendSize = 0U;
                        for (k = 0U; k < pAnimHeader->BlendHeader[i][j].BlendCount; k++) {
                            Tmp = (UINT32)pAnimHeader->BlendHeader[i][j].Width[k];
                            Tmp*= pAnimHeader->BlendHeader[i][j].Height[k] * sizeof(UINT8);
                            BlendSize += Tmp;
                        }
                        if (BlendSize > MaxBlendSize[i]) {
                            MaxBlendSize[i] = BlendSize;
                        }
                    }
                }
                /* Calculate ring buf size */
                for (i = 0U; i < pAnimHeader->DispCount; i++) {
                    CompRingBufSize += MaxCompSize[i];
                    CompRingBufSize = U32ALIGN(CompRingBufSize, AMBA_CACHE_LINE_SIZE);
                    WarpRingBufSize += MaxWarpSize[i];
                    WarpRingBufSize = U32ALIGN(WarpRingBufSize, AMBA_CACHE_LINE_SIZE);
                    BlendRingBufSize += MaxBlendSize[i];
                    BlendRingBufSize = U32ALIGN(BlendRingBufSize, AMBA_CACHE_LINE_SIZE);
                }
                g_BufMgr.LoadRingBufSize = CompRingBufSize + WarpRingBufSize;
                g_BufMgr.BlendRingBufSize = BlendRingBufSize;
                {
                    /* Consider update geo working and out buffer */
                    AMBA_IK_QUERY_CALC_GEO_BUF_SIZE_s BufSizeInfo = {0U};
                    Rval = AmbaIK_QueryCalcGeoBufSize(&BufSizeInfo);
                    if (Rval == SVC_OK) {
                        UpdateGeoWorkRingBufSize = (BufSizeInfo.WorkBufSize * SVC_ANIM_LIB_MAX_VIEWZONE);
                        UpdateGeoWorkRingBufSize = U32ALIGN(UpdateGeoWorkRingBufSize, AMBA_CACHE_LINE_SIZE);
                        g_BufMgr.UpdateGeoWorkRingBufSize = UpdateGeoWorkRingBufSize;
                        UpdateGeoOutRingBufSize = (BufSizeInfo.OutBufSize * SVC_ANIM_LIB_MAX_VIEWZONE);
                        UpdateGeoOutRingBufSize = U32ALIGN(UpdateGeoOutRingBufSize, AMBA_CACHE_LINE_SIZE);
                        g_BufMgr.UpdateGeoOutRingBufSize = UpdateGeoOutRingBufSize;
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetWorkBufSize: AmbaIK_QueryCalcGeoBufSize() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                }
                if (Rval == SVC_OK) {
                    /* Calculate required buf size */
                    *CacheWorkBufSize += ((CompRingBufSize + WarpRingBufSize + UpdateGeoWorkRingBufSize + UpdateGeoOutRingBufSize) * SVC_ANIM_RING_BUF_COUNT);
                    *NonCacheWorkBufSize += (BlendRingBufSize * SVC_ANIM_RING_BUF_COUNT);
                    {
                        /* Calculate stack size of tasks */
                        UINT32 StackSize;
                        StackSize = U32ALIGN(SVC_ANIM_TASK_STACK_SIZE, AMBA_CACHE_LINE_SIZE);
                        *CacheWorkBufSize += StackSize;
                        StackSize = U32ALIGN(SVC_ANIM_TASK_STACK_SIZE, AMBA_CACHE_LINE_SIZE);
                        *CacheWorkBufSize += StackSize;
                        StackSize = U32ALIGN(SVC_ANIM_SCHDLR_TASK_STACK_SIZE, AMBA_CACHE_LINE_SIZE);
                        *CacheWorkBufSize += StackSize;
                        StackSize = U32ALIGN(SVC_ANIM_RENDER_TASK_STACK_SIZE, AMBA_CACHE_LINE_SIZE);
                        *CacheWorkBufSize += StackSize;
                    }
                    {
                        UINT16 ViewCount = 0U;
                        UINT32 RawDataSize;
                        UINT32 GroupDescSize;
                        /* Raw data */
                        for (i = 0U; i < pAnimHeader->DispCount; i++) {
                            ViewCount += pAnimHeader->ViewCount[i];
                        }
                        RawDataSize = ViewCount * sizeof(SVC_ANIM_INDEX_s);
                        RawDataSize = U32ALIGN(RawDataSize, AMBA_CACHE_LINE_SIZE);
                        *CacheWorkBufSize += RawDataSize;
                        g_BufMgr.RawDataSize = RawDataSize;
                        /* Group desc */
                        GroupDescSize = pAnimHeader->GroupCount * sizeof(AMBA_CALIB_DATA_GROUP_DESC_s);
                        GroupDescSize = U32ALIGN(GroupDescSize, AMBA_CACHE_LINE_SIZE);
                        *CacheWorkBufSize += GroupDescSize;
                        g_BufMgr.GroupDescSize = GroupDescSize;
                    }

                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetWorkBufSize: AmbaCalibData_GetUdta() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetWorkBufSize: AmbaCalibData_Create() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 SvcAnim_GetDispatchTaskId(UINT8 *DispatchTaskId)
{
    UINT8 i;
    UINT32 Rval, MaxNumAvailable = 0U;
    for (i = 0U; i < SVC_ANIM_MAX_LOAD_DECMPR_TASK; i++) {
        AMBA_KAL_MSG_QUEUE_INFO_s MsgQueueInfo;
        Rval = AmbaKAL_MsgQueueQuery(&g_AnimMgr.KalCfg.MsgQueue[i], &MsgQueueInfo);
        if (Rval == SVC_OK) {
            if (MaxNumAvailable < MsgQueueInfo.NumAvailable) {
                /* Dispatch jobs based on the job number of the task */
                *DispatchTaskId = i;
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetDispatchTaskId(%u): AmbaKAL_MsgQueueQuery() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
            break;
        }
    }
    return Rval;
}

static void* SvcAnim_SchdlrTask(void* Info)
{
    UINT32 Rval, Ret;
    AmbaMisra_TouchUnused(&Info);
    /* Read raw data */
    /* TODO */ // should know the groupId for RawData
    Rval = SvcAnimLib_Load(g_AnimMgr.CalibDataHdlr, /*(g_AnimMgr.CalibDataHdlr->GroupNum - 1U)*/1U, 0U, 1U, g_BufMgr.RawData, g_BufMgr.RawDataSize);
    if (Rval == SVC_OK) {
        /* Get group desc */
        UINT32 i;
        ULONG  DescAddr;
        AMBA_CALIB_DATA_GROUP_DESC_s *Desc;
        AmbaMisra_TypeCast(&Desc, &g_BufMgr.GroupDesc);
        for (i = 0U; i < g_AnimMgr.AnimHeader.GroupCount; i++) {
            Rval = AmbaCalibData_GetDesc(g_AnimMgr.CalibDataHdlr, i, Desc);
            if (Rval == SVC_OK) {
                AmbaMisra_TypeCast(&DescAddr, &Desc);
                DescAddr += sizeof(AMBA_CALIB_DATA_GROUP_DESC_s);
                AmbaMisra_TypeCast(&Desc, &DescAddr);
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: AmbaCalibData_GetDesc() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                break;
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: SvcAnimLib_Load() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
    }
    if (Rval == SVC_OK) {
        for (;;) {
            UINT32 Flag;
            Rval = AmbaKAL_EventFlagGet(&g_AnimMgr.KalCfg.AnimFlag, SVC_ANIM_EVENT_FLAG_PLAY, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &Flag, AMBA_KAL_WAIT_FOREVER);
            if (Rval == SVC_OK) {
                /* Take mutex */
                Rval = AmbaKAL_MutexTake(&g_AnimMgr.KalCfg.Mutex, AMBA_KAL_WAIT_FOREVER);
                if (Rval == SVC_OK) {
                    UINT16 PlayId = 0U;
                    while (PlayId < g_AnimMgr.PlayLayoutCount) {
                        UINT8 i, j, k, Wp = 0U, LayoutId = g_AnimMgr.LayoutInfoWp;
                        UINT16 ViewId;
                        ULONG RawDataAddr, GroupDescAddr;
                        SVC_ANIM_INDEX_s *Index = NULL;
                        AMBA_CALIB_DATA_GROUP_DESC_s *Desc;
                        void *Ptr;
                        const SVC_ANIM_LAYOUT_INFO_s *pLayoutInfo = &g_AnimMgr.LayoutInfo[LayoutId];
                        AmbaMisra_TypeCast(&Ptr, &pLayoutInfo);
                        AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(SVC_ANIM_LAYOUT_INFO_s));
                        for (i = 0U; i < g_AnimMgr.AnimHeader.DispCount; i++) {
                            if (g_AnimMgr.PlayCfg.PlayCount[i] > PlayId) {
                                g_AnimMgr.LayoutInfo[LayoutId].DispCount += 1U;
                                /* Fill group count in LayoutInfo */
                                g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].DispId = i;
                                ViewId = g_AnimMgr.PlayCfg.PlayList[i][PlayId];
                                /* Search index */
                                AmbaMisra_TypeCast(&RawDataAddr, &g_BufMgr.RawData);
                                for (j = 0U; j < i; j++) {
                                    RawDataAddr += (g_AnimMgr.AnimHeader.ViewCount[j] * sizeof(SVC_ANIM_INDEX_s));
                                }
                                RawDataAddr += (ViewId * sizeof(SVC_ANIM_INDEX_s));
                                AmbaMisra_TypeCast(&Index, &RawDataAddr);
                                g_AnimMgr.LayoutInfo[LayoutId].Index[Wp] = Index;
                                g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].GroupCount = Index->DataInfo.GroupCount;
                                for (j = 0U; j < g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].GroupCount; j++) {
                                    g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].GroupId[j] = Index->DataInfo.GroupDesc[j].Id;
                                    /* Fill item count in LayoutInfo */
                                    AmbaMisra_TypeCast(&GroupDescAddr, &g_BufMgr.GroupDesc);
                                    GroupDescAddr += (Index->DataInfo.GroupDesc[j].Id * sizeof(AMBA_CALIB_DATA_GROUP_DESC_s));
                                    AmbaMisra_TypeCast(&Desc, &GroupDescAddr);
                                    g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].GroupDesc[j] = Desc;
                                    g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].ItemCount[j] = Desc->Count;
                                    /*
                                     *  Set DecmprDone as 1 if it's uncompressed for the item of the group
                                     *  because it doesn't need to decompress for the item.
                                     */
                                    for (k = 0U; k < g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].ItemCount[j]; k++) {
                                        if (Desc->ItemInfo[k].Compressed == AMBA_CALIB_DATA_CMPR_ALGO_NONE) {
                                            g_AnimMgr.LayoutInfo[LayoutId].DispInfo[Wp].DecmprDone[j][k] = 1U;
                                        }
                                    }
                                }
                                Wp++;
                            }
                        }
                        /* Clear SVC_ANIM_EVENT_FLAG_LOAD_DECMPR_DONE of previous layout */
                        Rval = AmbaKAL_EventFlagClear(&g_AnimMgr.KalCfg.AnimFlag, SVC_ANIM_EVENT_FLAG_LOAD_DECMPR_DONE);
                        if (Rval == SVC_OK) {
                            UINT8 DispatchTaskId, PreOsdCbTaskId = 0U;
                            /* Dispatch jobs */
                            if (g_AnimMgr.OsdChangeCb != NULL) {
                                /* Pre osd callback */
                                SVC_ANIM_MSG_s Msg;
                                Msg.CmdType = SVC_ANIM_CMD_OSD_CHANGED_BUF_CB;
                                Msg.LayoutId = LayoutId;
                                /* Send job msg */
                                Rval = SvcAnim_GetDispatchTaskId(&DispatchTaskId);
                                if (Rval == SVC_OK) {
                                    Rval = AmbaKAL_MsgQueueSend(&g_AnimMgr.KalCfg.MsgQueue[DispatchTaskId], &Msg, AMBA_KAL_WAIT_FOREVER);
                                    if (Rval == SVC_OK) {
                                        /* For post osd callback */
                                        PreOsdCbTaskId = DispatchTaskId;
                                    } else {
                                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): AmbaKAL_MsgQueueSend() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                    }
                                } else {
                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): SvcAnim_GetDispatchTaskId() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                }
                            }
                            if (Rval == SVC_OK) {
                                /* Load */
                                for (i = 0U; i < g_AnimMgr.LayoutInfo[LayoutId].DispCount; i++) {
                                    for (j = 0U; j < g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].GroupCount; j++) {
                                        if (g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].LoadDone[j] != 1U) {
                                            /* Load the whole group at once */
                                            SVC_ANIM_MSG_s Msg;
                                            UINT32 LoadSize = 0U;
                                            Msg.CmdType = SVC_ANIM_CMD_LOAD;
                                            Msg.LayoutId = LayoutId;
                                            Msg.CmdInfo.LoadCfg.GroupId = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].GroupId[j];
                                            Msg.CmdInfo.LoadCfg.ItemId = 0U;
                                            Desc = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].GroupDesc[j];
                                            Msg.CmdInfo.LoadCfg.ItemCount = Desc->Count;
                                            Msg.CmdInfo.LoadCfg.Buffer = g_BufMgr.LoadBufWp;
                                            for (k = Msg.CmdInfo.LoadCfg.ItemId; k < (Msg.CmdInfo.LoadCfg.ItemId + Msg.CmdInfo.LoadCfg.ItemCount); k++) {
                                                LoadSize += Desc->ItemInfo[k].Size;
                                            }
                                            Msg.CmdInfo.LoadCfg.BufSize = LoadSize;
                                            /* Fill config for layout state update */
                                            Msg.UpdateLayoutState.DispId = i;
                                            Msg.UpdateLayoutState.GroupId = j;
                                            /* Send job msg */
                                            Rval = SvcAnim_GetDispatchTaskId(&DispatchTaskId);
                                            if (Rval == SVC_OK) {
                                                Rval = AmbaKAL_MsgQueueSend(&g_AnimMgr.KalCfg.MsgQueue[DispatchTaskId], &Msg, AMBA_KAL_WAIT_FOREVER);
                                                if (Rval == SVC_OK) {
                                                    ULONG LoadBufAddr;
                                                    UINT32 LoadDataId = g_AnimMgr.LayoutInfo[LayoutId].LoadDataCount;
                                                    g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[LoadDataId].GroupId = Msg.CmdInfo.LoadCfg.GroupId;
                                                    g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[LoadDataId].ItemId = Msg.CmdInfo.LoadCfg.ItemId;
                                                    g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[LoadDataId].ItemCount = Msg.CmdInfo.LoadCfg.ItemCount;
                                                    g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[LoadDataId].Buffer = Msg.CmdInfo.LoadCfg.Buffer;
                                                    g_AnimMgr.LayoutInfo[LayoutId].LoadDataCount++;
                                                    /* Move Wp of load buffer */
                                                    AmbaMisra_TypeCast(&LoadBufAddr, &g_BufMgr.LoadBufWp);
                                                    LoadBufAddr += U32ALIGN(Msg.CmdInfo.LoadCfg.BufSize, AMBA_CACHE_LINE_SIZE);
                                                    AmbaMisra_TypeCast(&g_BufMgr.LoadBufWp, &LoadBufAddr);
                                                } else {
                                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): AmbaKAL_MsgQueueSend() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                                }
                                            } else {
                                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): SvcAnim_GetDispatchTaskId() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                            }
                                        }
                                    }
                                }
                            }
                            if (Rval == SVC_OK) {
                                /* Decompress */
                                UINT8 m;
                                for (i = 0U; i < g_AnimMgr.LayoutInfo[LayoutId].DispCount; i++) {
                                    for (j = 0U; j < g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].GroupCount; j++) {
                                        for (k = 0U; k < g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].ItemCount[j]; k++) {
                                            if (g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].DecmprDone[j][k] != 1U) {
                                                UINT32 DstSize = 0U;
                                                SVC_ANIM_MSG_s Msg;
                                                Msg.CmdType = SVC_ANIM_CMD_DECOMPRESS;
                                                Msg.LayoutId = LayoutId;
                                                Desc = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].GroupDesc[j];
                                                Msg.CmdInfo.DecmprCfg.Algo = Desc->ItemInfo[k].Compressed;
                                                if (Msg.CmdInfo.DecmprCfg.Algo == AMBA_CALIB_DATA_CMPR_ALGO_LZ4) {
                                                    /* Assume that all elements in one item are the same data type and there's only one element if it's osd-type. */
                                                    if ((Desc->ItemInfo[k].Count == 1U) && (Desc->ItemInfo[k].ElementInfo[0].Type == AMBA_CALIB_DATA_TYPE_OSD)) {
                                                        UINT8 DispId = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].DispId, VoutId, Found = 0U;
                                                        Msg.CmdInfo.DecmprCfg.DecomprType = AMBA_CALIB_DATA_DECMPR_LZ4_WINDOW;
                                                        Index = g_AnimMgr.LayoutInfo[LayoutId].Index[i];
                                                        /*
                                                         *  Assumption:
                                                         *  All VoutId in the VoutId array use the same data, then use the same osd buffer.
                                                         *  So we can just use index 0 to find corresponding osd buffer size.
                                                         *  TODO:
                                                         *  Actually, same data may be used for different osd buffer.
                                                         *  Ex: two osd buffer of different size includes the same data of the same size
                                                         */
                                                        VoutId = Index->DataInfo.GroupDesc[j].ItemInfo[k].ElementInfo[0].VoutId;
                                                        for (m = 0U; m < g_AnimMgr.VoutCount; m++) {
                                                            if (g_AnimMgr.VoutCfg[m].VoutId == VoutId) {
                                                                Found = 1U;
                                                                break;
                                                            }
                                                        }
                                                        if (Found == 1U) {
                                                            UINT16 VoutActWinOffsetX, VoutActWinOffsetY;
                                                            Msg.CmdInfo.DecmprCfg.DstTableSize[0].Width = g_AnimMgr.OsdCfg[m].BufPitch;
                                                            Msg.CmdInfo.DecmprCfg.DstTableSize[0].Height = g_AnimMgr.OsdCfg[m].BufHeight;
                                                            VoutActWinOffsetX = g_AnimMgr.VoutCfg[m].VoutWin.Width - g_AnimMgr.VoutCfg[m].YuvWidth;
                                                            VoutActWinOffsetX = (UINT16)VoutActWinOffsetX / 2U;
                                                            VoutActWinOffsetY = g_AnimMgr.VoutCfg[m].VoutWin.Height - g_AnimMgr.VoutCfg[m].YuvHeight;
                                                            VoutActWinOffsetY = (UINT16)VoutActWinOffsetY / 2U;
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].StartX = (UINT32)g_AnimMgr.DispOffset[DispId].OffsetX;
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].StartX += Desc->ItemInfo[k].ElementInfo[0].Osd.OsdWindow.OffsetX;
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].StartX += VoutActWinOffsetX;
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].StartY = (UINT32)g_AnimMgr.DispOffset[DispId].OffsetY;
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].StartY += Desc->ItemInfo[k].ElementInfo[0].Osd.OsdWindow.OffsetY;
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].StartY += VoutActWinOffsetY;
                                                            /* Osd size */
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].Width = Desc->ItemInfo[k].ElementInfo[0].Osd.OsdWindow.Width;
                                                            Msg.CmdInfo.DecmprCfg.DstWinInfo[0].Height = Desc->ItemInfo[k].ElementInfo[0].Osd.OsdWindow.Height;
                                                        } else {
                                                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): VoutId is not found!", __LINE__, 0U, 0U, 0U, 0U);
                                                            Rval = SVC_NG;
                                                        }
                                                    } else {
                                                        Msg.CmdInfo.DecmprCfg.DecomprType = AMBA_CALIB_DATA_DECMPR_LZ4_LINEAR;
                                                    }
                                                } else {
                                                    Rval = SVC_NG;
                                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: Compressed algo type (%u) is valid!", Msg.CmdInfo.DecmprCfg.Algo, 0U, 0U, 0U, 0U);
                                                }
                                                if (Rval == SVC_OK) {
                                                    UINT8 n, StartItemId = 0U, EndItemId, Found = 0U;
                                                    ULONG Addr;
                                                    Msg.CmdInfo.DecmprCfg.SegmentSize = SVC_ANIM_COMP_SEGMENT_SIZE;
                                                    Msg.CmdInfo.DecmprCfg.DstNum = 1;
                                                    /* Search for src data */
                                                    for (m = 0U; m < g_AnimMgr.LayoutInfo[LayoutId].LoadDataCount; m++) {
                                                        if (g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].GroupId[j] == g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[m].GroupId) {
                                                            StartItemId = g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[m].ItemId;
                                                            EndItemId = StartItemId + g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[m].ItemCount - 1U;
                                                            if ((k >= StartItemId) && (k <= EndItemId)) {
                                                                Found = 1U;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    if (Found == 1U) {
                                                        AmbaMisra_TypeCast(&Addr, &g_AnimMgr.LayoutInfo[LayoutId].LoadDataInfo[m].Buffer);
                                                        for (n = StartItemId; n < k; n++) {
                                                            Addr += Desc->ItemInfo[n].Size;
                                                        }
                                                        AmbaMisra_TypeCast(&Msg.CmdInfo.DecmprCfg.SrcAddr, &Addr);
                                                        Msg.CmdInfo.DecmprCfg.SrcSize = Desc->ItemInfo[k].Size;
                                                        for (m = 0U; m < Desc->ItemInfo[k].Count; m++) {
                                                            DstSize += Desc->ItemInfo[k].ElementInfo[m].Size;
                                                        }
                                                        /* Assume that all elements in one item are the same data type. */
                                                        if (Desc->ItemInfo[k].ElementInfo[0].Type == AMBA_CALIB_DATA_TYPE_BLEND) {
                                                            ULONG BlendBufAddr;
                                                            Msg.CmdInfo.DecmprCfg.DstAddr = g_BufMgr.BlendBufWp;
                                                            /* Move Wp of blend buffer */
                                                            AmbaMisra_TypeCast(&BlendBufAddr, &g_BufMgr.BlendBufWp);
                                                            BlendBufAddr += U32ALIGN(DstSize, AMBA_CACHE_LINE_SIZE);
                                                            AmbaMisra_TypeCast(&g_BufMgr.BlendBufWp, &BlendBufAddr);
                                                        } else if (Desc->ItemInfo[k].ElementInfo[0].Type == AMBA_CALIB_DATA_TYPE_OSD) {
                                                            UINT8 VoutId = Index->DataInfo.GroupDesc[j].ItemInfo[k].ElementInfo[0].VoutId;
                                                            Found = 0U;
                                                            for (m = 0U; m < g_AnimMgr.VoutCount; m++) {
                                                                if (g_AnimMgr.VoutCfg[m].VoutId == VoutId) {
                                                                    Found = 1U;
                                                                    break;
                                                                }
                                                            }
                                                            if (Found == 1U) {
                                                                Msg.CmdInfo.DecmprCfg.DstAddr = g_BufMgr.OsdBufWp[m];
                                                            } else {
                                                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): VoutId is not found!", __LINE__, 0U, 0U, 0U, 0U);
                                                                Rval = SVC_NG;
                                                            }
                                                        } else {
                                                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: Data type (%u) is not supported to decompress!", Desc->ItemInfo[k].ElementInfo[0].Type, 0U, 0U, 0U, 0U);
                                                            Rval = SVC_NG;
                                                        }
                                                        Msg.CmdInfo.DecmprCfg.DstSize = DstSize;
                                                        /* Fill config for layout state update */
                                                        Msg.UpdateLayoutState.DispId = i;
                                                        Msg.UpdateLayoutState.GroupId = j;
                                                        Msg.UpdateLayoutState.ItemId = k;
                                                        if (Rval == SVC_OK) {
                                                            /* Send job msg */
                                                            Rval = SvcAnim_GetDispatchTaskId(&DispatchTaskId);
                                                            if (Rval == SVC_OK) {
                                                                Rval = AmbaKAL_MsgQueueSend(&g_AnimMgr.KalCfg.MsgQueue[DispatchTaskId], &Msg, AMBA_KAL_WAIT_FOREVER);
                                                                if (Rval != SVC_OK) {
                                                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): AmbaKAL_MsgQueueSend() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                                                }
                                                            } else {
                                                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): SvcAnim_GetDispatchTaskId() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                                            }
                                                        }
                                                    } else {
                                                        Rval = SVC_NG;
                                                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: Src data for this decompress process dosen't exist!", 0U, 0U, 0U, 0U, 0U);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if ((Rval == SVC_OK) && (g_AnimMgr.OsdDoneCb != NULL)) {
                                /* Make sure that all osd data have been loaded or decompressed */
                                Rval = AmbaKAL_EventFlagGet(&g_AnimMgr.KalCfg.AnimFlag, SVC_ANIM_EVENT_FLAG_LOAD_DECMPR_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &Flag, AMBA_KAL_WAIT_FOREVER);
                                if (Rval == SVC_OK) {
                                    /* Post osd callback */
                                    SVC_ANIM_MSG_s Msg;
                                    Msg.CmdType = SVC_ANIM_CMD_OSD_DONE_CB;
                                    Msg.LayoutId = LayoutId;
                                        /* Dispatched task id is same as task which takes over pre osd callback */
                                        Rval = AmbaKAL_MsgQueueSend(&g_AnimMgr.KalCfg.MsgQueue[PreOsdCbTaskId], &Msg, AMBA_KAL_WAIT_FOREVER);
                                        if (Rval != SVC_OK) {
                                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): AmbaKAL_MsgQueueSend() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                        }
                                } else {
                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: AmbaKAL_EventFlagGet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                                }
                            }
                            if (Rval == SVC_OK) {
                                /* Render */
                                if (g_AnimMgr.LayoutInfo[LayoutId].RenderDone != 1U) {
                                    SVC_ANIM_RENDER_MSG_s Msg;
                                    Rval = AmbaKAL_MsgQueueSend(&g_AnimMgr.KalCfg.RenderMsgQueue, &Msg, AMBA_KAL_WAIT_FOREVER);
                                    if (Rval != SVC_OK) {
                                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask(%u): AmbaKAL_MsgQueueSend() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                    }
                                }
                            }
                            if (Rval == SVC_OK) {
                                /* Move to next layout and next ring buffer */
                                PlayId++;
                                g_AnimMgr.LayoutInfoWp++;
                                g_BufMgr.BufferId = (g_BufMgr.BufferId + 1U) % SVC_ANIM_RING_BUF_COUNT;
                                g_BufMgr.LoadBufWp = g_BufMgr.LoadRingBuf[g_BufMgr.BufferId];
                                g_BufMgr.BlendBufWp = g_BufMgr.BlendRingBuf[g_BufMgr.BufferId];
                                /*for (i = 0U; i < g_AnimMgr.VoutCount; i++) {
                                    g_BufMgr.OsdBufWp[i] = g_BufMgr.OsdRingBuf[i][g_BufMgr.BufferId];
                                }*/
                                g_BufMgr.UpdateGeoWorkBufWp = g_BufMgr.UpdateGeoWorkRingBuf[g_BufMgr.BufferId];
                                g_BufMgr.UpdateGeoOutBufWp = g_BufMgr.UpdateGeoOutRingBuf[g_BufMgr.BufferId];
                            }
                        } else {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: AmbaKAL_EventFlagClear() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                        }
                    }
                    /* Give mutex */
                    Ret = AmbaKAL_MutexGive(&g_AnimMgr.KalCfg.Mutex);
                    if (Ret != SVC_OK) {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_ConfigDisplay: AmbaKAL_MutexGive() failed! Ret = %u", Ret, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: AmbaKAL_MutexTake() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SchdlrTask: AmbaKAL_EventFlagGet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
            if (Rval != SVC_OK) {
                break;
            }
        }
    }

    return NULL;
}

static UINT32 SvcAnim_UpdateLayoutState(UINT8 CmdType, UINT8 LayoutId, UINT8 DispId, UINT32 GroupId, UINT8 ItemId)
{
    UINT32 Rval = SVC_OK;
    if (CmdType == SVC_ANIM_CMD_LOAD) {
        g_AnimMgr.LayoutInfo[LayoutId].DispInfo[DispId].LoadDone[GroupId] = 1U;
    } else if (CmdType == SVC_ANIM_CMD_DECOMPRESS) {
        g_AnimMgr.LayoutInfo[LayoutId].DispInfo[DispId].DecmprDone[GroupId][ItemId] = 1U;
    } else {
        Rval = SVC_NG;
        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_UpdateLayoutState: CmdType (%u) is invalid!", CmdType, 0U, 0U, 0U, 0U);
    }
    if (Rval == SVC_OK) {
        UINT8 i, j, k, Done = 1U;
        for (i = 0U; i < g_AnimMgr.LayoutInfo[LayoutId].DispCount; i++) {
            for (j = 0U; j < g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].GroupCount; j++) {
                if (Done == 1U) { // There's no need to check if previous group has something to do.
                    /* Check if this group has been loaded */
                    if (g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].LoadDone[j] == 0U) {
                        Done = 0U; // it still needs to load data
                    }
                    /*
                     *  Check if all items in this group have been decompressed.
                     *  There's no need to check if this group has not been loaded yet.
                     */
                    if (Done == 1U) {
                        for (k = 0U; k < g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].ItemCount[j]; k++) {
                            if (g_AnimMgr.LayoutInfo[LayoutId].DispInfo[i].DecmprDone[j][k] == 0U) {
                                Done = 0U; // it still needs to decompress data
                            }
                        }
                    }
                }
            }
            if (Done == 0U) {
                break; // one break for MISRA C
            }
        }
        if (Done == 1U) {
            Rval = AmbaKAL_EventFlagSet(&g_AnimMgr.KalCfg.AnimFlag, SVC_ANIM_EVENT_FLAG_LOAD_DECMPR_DONE);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_UpdateLayoutState: AmbaKAL_EventFlagSet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Rval;
}

static UINT32 SvcAnim_FillEventInfo(UINT8 LayoutId, SVC_ANIM_EVENT_INFO_s *EventInfo)
{
    UINT32 Rval = SVC_OK;
    if (EventInfo == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: EventInfo is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT8 i, j, k;
        EventInfo->VoutCount = g_AnimMgr.VoutCount;
        for (i = 0U; i < EventInfo->VoutCount; i++) {
            EventInfo->VoutId[i] = g_AnimMgr.VoutCfg[i].VoutId;
            EventInfo->OsdInfo[i].DispCount = g_AnimMgr.LayoutInfo[LayoutId].DispCount;
            EventInfo->OsdInfo[i].Buffer = g_BufMgr.OsdBufWp[i];
            EventInfo->OsdInfo[i].BufPitch = g_AnimMgr.OsdCfg[i].BufPitch;
            EventInfo->OsdInfo[i].BufWidth = g_AnimMgr.OsdCfg[i].BufWidth;
            EventInfo->OsdInfo[i].BufHeight = g_AnimMgr.OsdCfg[i].BufHeight;
            for (j = 0U; j < EventInfo->OsdInfo[i].DispCount; j++) {
                if (Rval == SVC_OK) {
                    UINT8 DispId = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[j].DispId;
                    const SVC_ANIM_INDEX_s *Index = g_AnimMgr.LayoutInfo[LayoutId].Index[j];
                    EventInfo->OsdInfo[i].OsdCount[j] = Index->DataInfo.OsdCount;
                    for (k = 0U; k < EventInfo->OsdInfo[i].OsdCount[j]; k++) {
                        if (Rval == SVC_OK) {
                            UINT32 GroupId = Index->DataInfo.OsdDataDesc[k].GroupId;
                            UINT8 ItemId = Index->DataInfo.OsdDataDesc[k].ItemId;
                            UINT8 ElementId = Index->DataInfo.OsdDataDesc[k].ElementId;
                            const AMBA_CALIB_DATA_GROUP_DESC_s *Desc = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[j].GroupDesc[GroupId];
                            if (Desc->ItemInfo[ItemId].ElementInfo[ElementId].Type == AMBA_CALIB_DATA_TYPE_OSD) {
                                /*
                                 *  Offsets in desc are the offset in disp window,
                                 *  so we need to add the offset of disp window in the layout.
                                 */
                                EventInfo->OsdInfo[i].Window[j][k].OffsetX = g_AnimMgr.DispOffset[DispId].OffsetX +
                                                                            Desc->ItemInfo[ItemId].ElementInfo[ElementId].Osd.OsdWindow.OffsetX;
                                EventInfo->OsdInfo[i].Window[j][k].OffsetY = g_AnimMgr.DispOffset[DispId].OffsetY +
                                                                            Desc->ItemInfo[ItemId].ElementInfo[ElementId].Osd.OsdWindow.OffsetY;
                                EventInfo->OsdInfo[i].Window[j][k].Width = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Osd.OsdWindow.Width;
                                EventInfo->OsdInfo[i].Window[j][k].Height = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Osd.OsdWindow.Height;
                            } else {
                                Rval = SVC_NG;
                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_FillEventInfo: Element type (%u) is invalid!", Desc->ItemInfo[ItemId].ElementInfo[ElementId].Type, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                }
            }
            if (Rval != SVC_OK) {
                break;
            }
        }
    }
    return Rval;
}

static void* SvcAnim_LoadDecmprTask(void* EntryArg)
{
    ULONG  info, *pInfo;
    SVC_ANIM_MSG_s Msg;

    AmbaMisra_TypeCast(&pInfo, &EntryArg);
    info = (*pInfo);

    for (;;) {
        UINT32 Rval = AmbaKAL_MsgQueueReceive(&g_AnimMgr.KalCfg.MsgQueue[info], &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Rval == SVC_OK) {
            UINT8 LayoutId = Msg.LayoutId;
            if (Msg.CmdType == SVC_ANIM_CMD_LOAD) {
                Rval = SvcAnimLib_Load(g_AnimMgr.CalibDataHdlr, Msg.CmdInfo.LoadCfg.GroupId, Msg.CmdInfo.LoadCfg.ItemId, Msg.CmdInfo.LoadCfg.ItemCount, Msg.CmdInfo.LoadCfg.Buffer, Msg.CmdInfo.LoadCfg.BufSize);
                if (Rval == SVC_OK) {
                    Rval = SvcAnim_UpdateLayoutState(Msg.CmdType, LayoutId, Msg.UpdateLayoutState.DispId, Msg.UpdateLayoutState.GroupId, Msg.UpdateLayoutState.ItemId);
                    if (Rval == SVC_OK) {
                        /* Assign addr for each element data */
                        UINT8 i, LayoutDispId = Msg.UpdateLayoutState.DispId;
                        UINT32 LayoutGroupId = Msg.UpdateLayoutState.GroupId;
                        const AMBA_CALIB_DATA_GROUP_DESC_s *Desc = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].GroupDesc[LayoutGroupId];
                        /* Assume that all warp data of the disp are stored in the same item */
                        for (i = Msg.CmdInfo.LoadCfg.ItemId; i < (Msg.CmdInfo.LoadCfg.ItemId + Msg.CmdInfo.LoadCfg.ItemCount); i++) {
                            if (Rval == SVC_OK) {
                                if (Desc->ItemInfo[i].ElementInfo[0].Type == AMBA_CALIB_DATA_TYPE_WARP) {
                                    const UINT8 *ItemBuf;
                                    UINT8 j;
                                    ULONG TmpAddr;
                                    const SVC_ANIM_INDEX_s *Index = g_AnimMgr.LayoutInfo[LayoutId].Index[LayoutDispId];
                                    /* Move to the start point of the item first */
                                    AmbaMisra_TypeCast(&TmpAddr, &Msg.CmdInfo.LoadCfg.Buffer);
                                    for (j = Msg.CmdInfo.LoadCfg.ItemId; j < i; j++) {
                                        TmpAddr += Desc->ItemInfo[j].Size;
                                    }
                                    AmbaMisra_TypeCast(&ItemBuf, &TmpAddr);
                                    /* Assign warp addr based on each element */
                                    for (j = 0U; j < Index->DataInfo.GroupDesc[LayoutGroupId].ItemInfo[i].ElementCount; j++) {
                                        if (Rval == SVC_OK) {
                                            UINT8 k, DataId = Index->DataInfo.GroupDesc[LayoutGroupId].ItemInfo[i].ElementInfo[j].DataId;
                                            AmbaMisra_TypeCast(&TmpAddr, &ItemBuf);
                                            for (k = 0U; k < j; k++) {
                                                TmpAddr += Desc->ItemInfo[i].ElementInfo[k].Size;
                                            }
                                            /* The count of warp addr is same as element count */
                                            if (Index->WarpEnable[DataId] == 1U) {
                                                g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].WarpDataDesc[DataId].GroupId = Index->DataInfo.GroupDesc[LayoutGroupId].Id;
                                                g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].WarpDataDesc[DataId].ItemId = i;
                                                g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].WarpDataDesc[DataId].ElementId = j;
                                                AmbaMisra_TypeCast(&g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].WarpAddr[DataId], &TmpAddr);
                                            } else {
                                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: WarpEnable and DataId are inconsistent!", 0U, 0U, 0U, 0U, 0U);
                                                Rval = SVC_NG;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: SvcAnim_UpdateLayoutState() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: SvcAnimLib_Load() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else if (Msg.CmdType == SVC_ANIM_CMD_DECOMPRESS) {
                UINT32 i;
                AMBA_CALIB_DATA_DECMPR_INFO_s DecmprInfo;
                DecmprInfo.Lz4Info.DecmprType = Msg.CmdInfo.DecmprCfg.DecomprType;
                DecmprInfo.Lz4Info.SegmentSize = (UINT16)Msg.CmdInfo.DecmprCfg.SegmentSize;
                DecmprInfo.Lz4Info.DstNum = (UINT16)Msg.CmdInfo.DecmprCfg.DstNum;
                for (i = 0U; i < (UINT32)DecmprInfo.Lz4Info.DstNum; i++) {
                    DecmprInfo.Lz4Info.DstTableSize[i] = Msg.CmdInfo.DecmprCfg.DstTableSize[i];
                    DecmprInfo.Lz4Info.DstWinInfo[i] = Msg.CmdInfo.DecmprCfg.DstWinInfo[i];
                }
                Rval = SvcAnimLib_Decompress(Msg.CmdInfo.DecmprCfg.Algo, &DecmprInfo, Msg.CmdInfo.DecmprCfg.SrcAddr, Msg.CmdInfo.DecmprCfg.SrcSize, Msg.CmdInfo.DecmprCfg.DstAddr, Msg.CmdInfo.DecmprCfg.DstSize);
                if (Rval == SVC_OK) {
                    Rval = SvcAnim_UpdateLayoutState(Msg.CmdType, LayoutId, Msg.UpdateLayoutState.DispId, Msg.UpdateLayoutState.GroupId, Msg.UpdateLayoutState.ItemId);
                    if (Rval == SVC_OK) {
                        /* Assign addr for each element data */
                        UINT32 j;
                        UINT8 ItemId = Msg.UpdateLayoutState.ItemId, LayoutDispId = Msg.UpdateLayoutState.DispId;
                        UINT32 LayoutGroupId = Msg.UpdateLayoutState.GroupId;
                        const AMBA_CALIB_DATA_GROUP_DESC_s *Desc = g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].GroupDesc[LayoutGroupId];
                        const SVC_ANIM_INDEX_s *Index = g_AnimMgr.LayoutInfo[LayoutId].Index[LayoutDispId];
                        /*
                         *  Assume that all elements in the item are same type,
                         *  so we can use the type of first element to decide what the type of this item is.
                         */
                        if (Desc->ItemInfo[ItemId].ElementInfo[0].Type == AMBA_CALIB_DATA_TYPE_BLEND) {
                            UINT8 VoutId, DataId, Found = 0U;
                            ULONG TmpAddr;
                            for (i = 0U; i < Desc->ItemInfo[ItemId].Count; i++) {
                                AmbaMisra_TypeCast(&TmpAddr, &Msg.CmdInfo.DecmprCfg.DstAddr);
                                for (j = 0U; j < i; j++) {
                                    TmpAddr += Desc->ItemInfo[ItemId].ElementInfo[j].Size;
                                }
                                /*
                                 *  Check which vout that this data element relates to.
                                 *  Assume that all elements in the item are for the same vout.
                                 */
                                VoutId = Index->DataInfo.GroupDesc[LayoutGroupId].ItemInfo[ItemId].ElementInfo[i].VoutId;
                                for (j = 0U; j < g_AnimMgr.VoutCount; j++) {
                                    if (g_AnimMgr.VoutCfg[j].VoutId == VoutId) {
                                        Found = 1U;
                                        break;
                                    }
                                }
                                if (Found == 1U) {
                                    DataId = Index->DataInfo.GroupDesc[LayoutGroupId].ItemInfo[ItemId].ElementInfo[i].DataId;
                                    if (Index->BlendType[DataId] != 0U) {
                                        g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].BlendDataDesc[j][DataId].GroupId = Index->DataInfo.GroupDesc[LayoutGroupId].Id;
                                        g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].BlendDataDesc[j][DataId].ItemId = ItemId;
                                        g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].BlendDataDesc[j][DataId].ElementId = (UINT8)i;
                                        AmbaMisra_TypeCast(&g_AnimMgr.LayoutInfo[LayoutId].DispInfo[LayoutDispId].BlendAddr[j][DataId], &TmpAddr);
                                    } else {
                                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: BlendType and DataId are inconsistent!", 0U, 0U, 0U, 0U, 0U);
                                        Rval = SVC_NG;
                                    }
                                } else {
                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask(%u): VoutId is not found!", __LINE__, 0U, 0U, 0U, 0U);
                                    Rval = SVC_NG;
                                }
                                if (Rval != SVC_OK) {
                                    break;
                                }
                            }
                        } else if (Desc->ItemInfo[ItemId].ElementInfo[0].Type == AMBA_CALIB_DATA_TYPE_OSD) {
                            UINT8 VoutId = Index->DataInfo.GroupDesc[LayoutGroupId].ItemInfo[ItemId].ElementInfo[0].VoutId, Found = 0U;
                            for (i = 0U; i < g_AnimMgr.VoutCount; i++) {
                                if (g_AnimMgr.VoutCfg[i].VoutId == VoutId) {
                                    Found = 1U;
                                    break;
                                }
                            }
                            if (Found == 1U) {
                                g_AnimMgr.LayoutInfo[LayoutId].OsdAddr[i] = Msg.CmdInfo.DecmprCfg.DstAddr;
                            } else {
                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask(%u): VoutId is not found!", __LINE__, 0U, 0U, 0U, 0U);
                                Rval = SVC_NG;
                            }
                        } else {
                            Rval = SVC_NG;
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: Element type (%u) is invalid!", Desc->ItemInfo[ItemId].ElementInfo[0].Type, 0U, 0U, 0U, 0U);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: SvcAnim_UpdateLayoutState() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: SvcAnimLib_Decompress() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else if (Msg.CmdType == SVC_ANIM_CMD_OSD_CHANGED_BUF_CB) {
                SVC_ANIM_EVENT_INFO_s EventInfo;
                Rval = SvcAnim_FillEventInfo(LayoutId, &EventInfo);
                if (Rval == SVC_OK) {
                    Rval = g_AnimMgr.OsdChangeCb(&EventInfo);
                    if (Rval != SVC_OK) {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: OsdChangeCb() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: SvcAnim_FillEventInfo() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else if (Msg.CmdType == SVC_ANIM_CMD_OSD_DONE_CB) {
                SVC_ANIM_EVENT_INFO_s EventInfo;
                Rval = SvcAnim_FillEventInfo(LayoutId, &EventInfo);
                if (Rval == SVC_OK) {
                    Rval = g_AnimMgr.OsdDoneCb(&EventInfo);
                    if (Rval == SVC_OK) {
                        static UINT8 OsdDoneFlagId = 0U;
                        UINT8 DefaultDataFlag = 0U;
                        if (g_AnimMgr.LayoutInfo[LayoutId].RenderDone == 1U) {
                            DefaultDataFlag = 1U;
                        }
                        if (DefaultDataFlag == 0U) {
                            UINT32 TmpFlag = (UINT32)SVC_ANIM_EVENT_FLAG_OSD_DONE << OsdDoneFlagId;
                            /* This process is for render task, and it's not required for default data. */
                            Rval = AmbaKAL_EventFlagSet(&g_AnimMgr.KalCfg.AnimFlag, TmpFlag);
                            if (Rval == SVC_OK) {
                                OsdDoneFlagId = (OsdDoneFlagId + 1U) % SVC_ANIM_RING_BUF_COUNT;
                            } else {
                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: AmbaKAL_EventFlagSet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                            }
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: OsdDoneCb() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: SvcAnim_FillEventInfo() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: Cmd (%u) is invalid!", Msg.CmdType, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_LoadDecmprTask: AmbaKAL_MsgQueueReceive() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
        if (Rval != SVC_OK) {
            break;
        }
    }

    return NULL;
}

static void* SvcAnim_RenderTask(void* Info)
{
    SVC_ANIM_RENDER_MSG_s Msg;
    UINT32 Rval = AmbaKAL_MsgQueueReceive(&g_AnimMgr.KalCfg.RenderMsgQueue, &Msg, AMBA_KAL_WAIT_FOREVER);
    AmbaMisra_TouchUnused(&Info);
    if (Rval == SVC_OK) {
        static UINT8 OsdDoneFlagId = 0U;
        UINT32 Flag, RegFlag;
        /* Make sure that osd is done */
        RegFlag = (UINT32)SVC_ANIM_EVENT_FLAG_OSD_DONE << OsdDoneFlagId;
        Rval = AmbaKAL_EventFlagGet(&g_AnimMgr.KalCfg.AnimFlag, RegFlag, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_AUTO, &Flag, AMBA_KAL_WAIT_FOREVER);
        if (Rval == SVC_OK) {
            OsdDoneFlagId = (OsdDoneFlagId + 1U) % SVC_ANIM_RING_BUF_COUNT;
            /* Sync job */
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_RenderTask: AmbaKAL_EventFlagGet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_RenderTask: AmbaKAL_MsgQueueReceive() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
    }
    return NULL;
}

UINT32 SvcAnim_SyncJobRdyHandler(const void *EventInfo)
{
    UINT32 Rval = SVC_OK;
    const AMBA_DSP_LV_SYNC_JOB_INFO_s *pInfo = NULL;
    AmbaMisra_TypeCast(&pInfo, &EventInfo);
    if (pInfo->Status == 0U) {
        AmbaPrint_PrintUInt5("SvcAnim_SyncJobRdyHandler: Sync job %u is done!", pInfo->JobId, 0U, 0U, 0U, 0U);
    } else if (pInfo->Status == 1U) {
        AmbaPrint_PrintUInt5("SvcAnim_SyncJobRdyHandler: Sync job %u is discarded!", pInfo->JobId, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_SyncJobRdyHandler: Sync job %u is not defined!", pInfo->JobId, 0U, 0U, 0U, 0U);
        Rval = SVC_NG;
    }
    if (Rval == SVC_OK) {
    }
    return Rval;
}

UINT32 SvcAnim_Init(const SVC_ANIM_INIT_CFG_s *Cfg)
{
    UINT32 Rval = SVC_OK;
    if (Cfg == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Cfg is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT8 i, j;
        static char MsgQueueName[SVC_ANIM_MAX_LOAD_DECMPR_TASK][SVC_ANIM_MAX_NAME_LENGTH] = {"SvcAnimMsgQueue0", "SvcAnimMsgQueue1"};
        /* Fill init config */
        g_AnimMgr.VoutCount = Cfg->VoutCount;
        for (i = 0U; i < SVC_ANIM_LIB_MAX_DISP; i++) {
            for (j = 0U; j < SVC_ANIM_LIB_MAX_CHAN; j++) {
                g_AnimMgr.ChannelId[i][j] = Cfg->ChannelId[i][j];
                g_AnimMgr.ViewZoneId[i][j] = Cfg->ViewZoneId[i][j];
            }
        }
        for (i = 0U; i < SVC_ANIM_LIB_MAX_DISP; i++) {
            g_AnimMgr.DispOffset[i] = Cfg->DispOffset[i];
        }
        for (i = 0U; i < SVC_ANIM_LIB_MAX_VOUT; i++) {
            g_AnimMgr.VoutCfg[i] = Cfg->VoutCfg[i];
            g_AnimMgr.OsdCfg[i] = Cfg->OsdCfg[i];
            g_AnimMgr.YuvStrmCfg[i] = Cfg->YuvStrmCfg[i];
        }
        g_AnimMgr.EventCb = Cfg->EventCb;
        g_AnimMgr.OsdChangeCb = Cfg->OsdChangeCb;
        g_AnimMgr.OsdDoneCb = Cfg->OsdDoneCb;
        /* Create msg queue, event flag and mutex */
        for (i = 0U; i < SVC_ANIM_MAX_LOAD_DECMPR_TASK; i++) {
            Rval = AmbaKAL_MsgQueueCreate(&g_AnimMgr.KalCfg.MsgQueue[i], MsgQueueName[i], sizeof(SVC_ANIM_MSG_s), g_AnimMgr.KalCfg.MsgPool[i], SVC_ANIM_MAX_MSG * sizeof(SVC_ANIM_MSG_s));
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: AmbaKAL_MsgQueueCreate(%u) failed! Rval = %u", i, Rval, 0U, 0U, 0U);
                break;
            }
        }
        if (Rval == SVC_OK) {
            static char RenderMsgQueueName[SVC_ANIM_MAX_NAME_LENGTH] = "SvcAnimRenderMsgQueue";
            Rval = AmbaKAL_MsgQueueCreate(&g_AnimMgr.KalCfg.RenderMsgQueue, RenderMsgQueueName, sizeof(SVC_ANIM_RENDER_MSG_s), g_AnimMgr.KalCfg.RenderMsgPool, SVC_ANIM_MAX_MSG * sizeof(SVC_ANIM_RENDER_MSG_s));
            if (Rval == SVC_OK) {
                static char AnimFlagName[SVC_ANIM_MAX_NAME_LENGTH] = "SvcAnimFlag";
                Rval = AmbaKAL_EventFlagCreate(&g_AnimMgr.KalCfg.AnimFlag, AnimFlagName);
                if (Rval == SVC_OK) {
                    Rval = AmbaKAL_EventFlagClear(&g_AnimMgr.KalCfg.AnimFlag, 0xFFFFFFFFU);
                    if (Rval != SVC_OK) {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: AmbaKAL_EventFlagClear() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: AmbaKAL_EventFlagCreate() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: AmbaKAL_MsgQueueCreate(RenderMsgQueue) failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        }
        if (Rval == SVC_OK) {
            static char CfgMutexName[SVC_ANIM_MAX_NAME_LENGTH] = "SvcAnimCfgMutex";
            Rval = AmbaKAL_MutexCreate(&g_AnimMgr.KalCfg.Mutex, CfgMutexName);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_MutexCreate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
            }
        }
        if (Rval == SVC_OK) {
            SVC_ANIM_MEM_POOL_INIT_CFG_s MemPoolInitCfg;
            MemPoolInitCfg.MemPoolNum = SVC_ANIM_MEM_POOL_REGION_NUM;
            AmbaMisra_TypeCast(&MemPoolInitCfg.MemPool[SVC_ANIM_MEM_POOL_REGION_CACHE].BufferAddr, &Cfg->CacheWorkBuf);
            MemPoolInitCfg.MemPool[SVC_ANIM_MEM_POOL_REGION_CACHE].BufferSize = Cfg->CacheWorkBufSize;
            AmbaMisra_TypeCast(&MemPoolInitCfg.MemPool[SVC_ANIM_MEM_POOL_REGION_NONCACHE].BufferAddr, &Cfg->NonCacheWorkBuf);
            MemPoolInitCfg.MemPool[SVC_ANIM_MEM_POOL_REGION_NONCACHE].BufferSize = Cfg->NonCacheWorkBufSize;
            Rval = SvcAnimMemPool_Init(&MemPoolInitCfg);
            if (Rval == SVC_OK) {
                /* Memory allocation */
                /* Osd buffer (non-cache) */
                UINT32 OsdBufSize;
                void *Buf, *BufferRaw;
                for (i = 0U; i < Cfg->VoutCount; i++) {
                    OsdBufSize = (UINT32)Cfg->OsdCfg[i].BufPitch;
                    OsdBufSize*= Cfg->OsdCfg[i].BufHeight;
                    for (j = 0U; j < SVC_ANIM_RING_BUF_COUNT; j++) {
                        Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_NONCACHE, AMBA_CACHE_LINE_SIZE, &Buf, &BufferRaw, OsdBufSize);
                        if (Rval == SVC_OK) {
                            AmbaSvcWrap_MisraMemset(Buf, 0, OsdBufSize); // clear osd buf first
                            AmbaMisra_TypeCast(&g_BufMgr.OsdRingBuf[i][j], &Buf);
                        } else {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate(%u, %u) failed! Size = %u, Rval = %u", __LINE__, i, j, OsdBufSize, Rval);
                            break;
                        }
                    }
                }
                if (Rval == SVC_OK) {
                    /* Load ring buffer (cache) */
                    for (i = 0U; i < SVC_ANIM_RING_BUF_COUNT; i++) {
                        Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &Buf, &BufferRaw, g_BufMgr.LoadRingBufSize);
                        if (Rval == SVC_OK) {
                            AmbaMisra_TypeCast(&g_BufMgr.LoadRingBuf[i], &Buf);
                        } else {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate(%u) failed! Size = %u, Rval = %u", __LINE__, i, g_BufMgr.LoadRingBufSize, Rval, 0U);
                            break;
                        }
                    }
                }
                if (Rval == SVC_OK) {
                    /* Blend ring buffer (non-cache) */
                    for (i = 0U; i < SVC_ANIM_RING_BUF_COUNT; i++) {
                        Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_NONCACHE, AMBA_CACHE_LINE_SIZE, &Buf, &BufferRaw, g_BufMgr.BlendRingBufSize);
                        if (Rval == SVC_OK) {
                            AmbaMisra_TypeCast(&g_BufMgr.BlendRingBuf[i], &Buf);
                        } else {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate(%u) failed! Size = %u, Rval = %u", __LINE__, i, g_BufMgr.BlendRingBufSize, Rval, 0U);
                            break;
                        }
                    }
                }
                if (Rval == SVC_OK) {
                    /* Update geo working ring buffer (cache) */
                    for (i = 0U; i < SVC_ANIM_RING_BUF_COUNT; i++) {
                        Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &Buf, &BufferRaw, g_BufMgr.UpdateGeoWorkRingBufSize);
                        if (Rval == SVC_OK) {
                            AmbaMisra_TypeCast(&g_BufMgr.UpdateGeoWorkRingBuf[i], &Buf);
                        } else {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate(%u) failed! Size = %u, Rval = %u", __LINE__, i, g_BufMgr.UpdateGeoWorkRingBufSize, Rval, 0U);
                            break;
                        }
                    }
                }
                if (Rval == SVC_OK) {
                    /* Update geo output ring buffer (cache) */
                    for (i = 0U; i < SVC_ANIM_RING_BUF_COUNT; i++) {
                        Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &Buf, &BufferRaw, g_BufMgr.UpdateGeoOutRingBufSize);
                        if (Rval == SVC_OK) {
                            AmbaMisra_TypeCast(&g_BufMgr.UpdateGeoOutRingBuf[i], &Buf);
                        } else {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate(%u) failed! Size = %u, Rval = %u", __LINE__, i, g_BufMgr.UpdateGeoOutRingBufSize, Rval, 0U);
                            break;
                        }
                    }
                }
                if (Rval == SVC_OK) {
                    /* Raw data (cache) */
                    Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &Buf, &BufferRaw, g_BufMgr.RawDataSize);
                    if (Rval == SVC_OK) {
                        AmbaMisra_TypeCast(&g_BufMgr.RawData, &Buf);
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                    }
                }
                if (Rval == SVC_OK) {
                    /* Group desc (cache) */
                    Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &Buf, &BufferRaw, g_BufMgr.GroupDescSize);
                    if (Rval == SVC_OK) {
                        AmbaMisra_TypeCast(&g_BufMgr.GroupDesc, &Buf);
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                    }
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: SvcAnimMemPool_Init() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        }
        if (Rval == SVC_OK) {
            /* Create scheduler task */
            static char SchdlrTaskName[SVC_ANIM_MAX_NAME_LENGTH] = "SvcAnimSchdlrTask";
            void *StackBase, *BufferRaw;
            Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &StackBase, &BufferRaw, SVC_ANIM_SCHDLR_TASK_STACK_SIZE);
            if (Rval == SVC_OK) {
                Rval = AmbaKAL_TaskCreate(&g_AnimMgr.KalCfg.SchdlrTask,     /* pTask */
                                            SchdlrTaskName,                           /* pTaskName */
                                            SVC_ANIM_SCHDLR_TASK_PRIORITY,      /* Priority */
                                            SvcAnim_SchdlrTask,                 /* void (*EntryFunction)(UINT32) */
                                            0U,                                 /* EntryArg */
                                            StackBase,                          /* pStackBase */
                                            SVC_ANIM_SCHDLR_TASK_STACK_SIZE,    /* StackByteSize */
                                            0U);                               /* AutoStart */
                if (Rval != SVC_OK) {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskCreate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                } else {
                    Rval = AmbaKAL_TaskSetSmpAffinity(&g_AnimMgr.KalCfg.SchdlrTask, SVC_ANIM_SCHDLR_TASK_CORE);
                    if (Rval != SVC_OK) {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskSetSmpAffinity() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                    } else {
                        Rval = AmbaKAL_TaskResume(&g_AnimMgr.KalCfg.SchdlrTask);
                        if (Rval != SVC_OK) {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskResume() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                        }
                    }
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
            }
            if (Rval == SVC_OK) {
                /* Create load & decompress task */
                for (i = 0U; i < SVC_ANIM_MAX_LOAD_DECMPR_TASK; i++) {
                    static char AnimTaskName[SVC_ANIM_MAX_LOAD_DECMPR_TASK][SVC_ANIM_MAX_NAME_LENGTH] = {"SvcAnimTask0", "SvcAnimTask1"};
                    static ULONG  TaskArg[SVC_ANIM_MAX_LOAD_DECMPR_TASK];

                    Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &StackBase, &BufferRaw, SVC_ANIM_TASK_STACK_SIZE);
                    if (Rval == SVC_OK) {
                        TaskArg[i] = i;

                        Rval = AmbaKAL_TaskCreate(&g_AnimMgr.KalCfg.Task[i],        /* pTask */
                                                    AnimTaskName[i],                        /* pTaskName */
                                                    SVC_ANIM_TASK_PRIORITY,             /* Priority */
                                                    SvcAnim_LoadDecmprTask,             /* void (*EntryFunction)(UINT32) */
                                                    &(TaskArg[i]),                                  /* EntryArg */
                                                    StackBase,                          /* pStackBase */
                                                    SVC_ANIM_TASK_STACK_SIZE,           /* StackByteSize */
                                                    0U);                               /* AutoStart */
                        if (Rval != SVC_OK) {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskCreate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                        } else {
                            Rval = AmbaKAL_TaskSetSmpAffinity(&g_AnimMgr.KalCfg.Task[i], SVC_ANIM_TASK_CORE);
                            if (Rval != SVC_OK) {
                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskSetSmpAffinity() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                            } else {
                                Rval = AmbaKAL_TaskResume(&g_AnimMgr.KalCfg.Task[i]);
                                if (Rval != SVC_OK) {
                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskResume() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                                }
                            }
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                    }
                }
            }
            if (Rval == SVC_OK) {
                /* Create render task */
                static char RenderTaskName[SVC_ANIM_MAX_NAME_LENGTH] = "SvcAnimRenderTask";
                Rval = SvcAnimMemPool_Allocate(SVC_ANIM_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &StackBase, &BufferRaw, SVC_ANIM_RENDER_TASK_STACK_SIZE);
                if (Rval == SVC_OK) {
                    Rval = AmbaKAL_TaskCreate(&g_AnimMgr.KalCfg.RenderTask,     /* pTask */
                                                RenderTaskName,                           /* pTaskName */
                                                SVC_ANIM_RENDER_TASK_PRIORITY,      /* Priority */
                                                SvcAnim_RenderTask,                 /* void (*EntryFunction)(UINT32) */
                                                0U,                                 /* EntryArg */
                                                StackBase,                          /* pStackBase */
                                                SVC_ANIM_RENDER_TASK_STACK_SIZE,    /* StackByteSize */
                                                0U);                               /* AutoStart */
                    if (Rval != SVC_OK) {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskCreate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                    } else {
                        Rval = AmbaKAL_TaskSetSmpAffinity(&g_AnimMgr.KalCfg.RenderTask, SVC_ANIM_RENDER_TASK_CORE);
                        if (Rval != SVC_OK) {
                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskSetSmpAffinity() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                        } else {
                            Rval = AmbaKAL_TaskResume(&g_AnimMgr.KalCfg.RenderTask);
                            if (Rval != SVC_OK) {
                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): AmbaKAL_TaskResume() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                            }
                        }
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init(%u): SvcAnimMemPool_Allocate() failed! Rval = %u", __LINE__, Rval, 0U, 0U, 0U);
                }
            }
        }
        if (Rval == SVC_OK) {
            SVC_ANIM_LIB_SYS_CFG_s AnimLibSysCfg;
            /* Init Wp of all buffer */
            UINT8 BufferId = g_BufMgr.BufferId;
            g_BufMgr.LoadBufWp = g_BufMgr.LoadRingBuf[BufferId];
            g_BufMgr.BlendBufWp = g_BufMgr.BlendRingBuf[BufferId];
            for (i = 0U; i < Cfg->VoutCount; i++) {
                g_BufMgr.OsdBufWp[i] = g_BufMgr.OsdRingBuf[i][BufferId];
            }
            g_BufMgr.UpdateGeoWorkBufWp = g_BufMgr.UpdateGeoWorkRingBuf[BufferId];
            g_BufMgr.UpdateGeoOutBufWp = g_BufMgr.UpdateGeoOutRingBuf[BufferId];
            /* Init anim lib */
            AnimLibSysCfg.VoutCount = Cfg->VoutCount;
            for (i = 0U; i < SVC_ANIM_LIB_MAX_VOUT; i++) {
                AnimLibSysCfg.VoutCfg[i] = Cfg->VoutCfg[i];
                AnimLibSysCfg.OsdCfg[i] = Cfg->OsdCfg[i];
                AnimLibSysCfg.YuvStrmCfg[i] = Cfg->YuvStrmCfg[i];
            }
            Rval = SvcAnimLib_Init(&AnimLibSysCfg);
            if (Rval == SVC_OK) {
                /* Register sync job ready callback */
                Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_SYNC_JOB_RDY, SvcAnim_SyncJobRdyHandler);
                if (Rval == SVC_OK) {
                    /* Trigger for default data */
                    g_AnimMgr.PlayLayoutCount = 1U;
                    g_AnimMgr.LayoutInfo[0].RenderDone = 1U; // No need to render for liveview
                    g_AnimMgr.PlayCfg.FrameRateDivisor = 1U;
                    for (i = 0U; i < Cfg->DefaultDataCfg.DispCount; i++) {
                        UINT8 DispId = Cfg->DefaultDataCfg.DispId[i];
                        g_AnimMgr.PlayCfg.PlayCount[DispId] = 1U;
                        g_AnimMgr.PlayCfg.PlayList[DispId][0] = Cfg->DefaultDataCfg.ViewId[i];
                    }
                    Rval =AmbaKAL_EventFlagSet(&g_AnimMgr.KalCfg.AnimFlag, SVC_ANIM_EVENT_FLAG_PLAY);
                    if (Rval != SVC_OK) {
                        AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: AmbaKAL_EventFlagSet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: AmbaDSP_EventHandlerRegister() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Init: SvcAnimLib_Init() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Rval;
}

UINT32 SvcAnim_GetDefaultData(SVC_ANIM_DEFAULT_DATA_s *DefaultData)
{
    UINT32 Rval = SVC_OK;
    if (DefaultData == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: DefaultData is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        UINT32 Flag;
        /* Wait osd buf of index 0 done */
        Rval = AmbaKAL_EventFlagGet(&g_AnimMgr.KalCfg.AnimFlag, SVC_ANIM_EVENT_FLAG_OSD_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &Flag, AMBA_KAL_WAIT_FOREVER);
        if (Rval == SVC_OK) {
            UINT8 i, j, k, TotalChanCount = 0U;
            UINT16 MaxViewZoneId = 0U;
            SVC_ANIM_INDEX_s *Index;
            void *Ptr;
            AmbaMisra_TypeCast(&Ptr, &DefaultData);
            AmbaSvcWrap_MisraMemset(Ptr, 0, sizeof(SVC_ANIM_DEFAULT_DATA_s));
            DefaultData->VoutCount = g_AnimMgr.VoutCount;
            for (i = 0U; i < DefaultData->VoutCount; i++) {
                DefaultData->VoutId[i] = g_AnimMgr.VoutCfg[i].VoutId;
                DefaultData->OsdBuf[i] = g_AnimMgr.LayoutInfo[0].OsdAddr[i];
                for (j = 0U; j < g_AnimMgr.LayoutInfo[0].DispCount; j++) {
                    Index = g_AnimMgr.LayoutInfo[0].Index[j];
                    TotalChanCount += Index->ChanCount;
                }
                DefaultData->LvStrmCfg[i].NumChan = TotalChanCount;
            }
            for (i = 0U; i < g_AnimMgr.LayoutInfo[0].DispCount; i++) {
                if (Rval == SVC_OK) {
                    UINT8 DispId = g_AnimMgr.LayoutInfo[0].DispInfo[i].DispId;
                    UINT16 ViewZoneId;
                    Index = g_AnimMgr.LayoutInfo[0].Index[i];
                    for (j = 0U; j < Index->ChanCount; j++) {
                        if (MaxViewZoneId < g_AnimMgr.ViewZoneId[i][j]) {
                            MaxViewZoneId = g_AnimMgr.ViewZoneId[i][j];
                        }
                        ViewZoneId = g_AnimMgr.ViewZoneId[DispId][j];
                        DefaultData->WarpEnable[ViewZoneId] = Index->WarpEnable[j];
                        if (Index->WarpEnable[j] == 1U) {
                            /* Search corresponding group desc of warp data */
                            UINT8 ItemId = g_AnimMgr.LayoutInfo[0].DispInfo[i].WarpDataDesc[j].ItemId, Found = 0U;
                            UINT8 ElementId = g_AnimMgr.LayoutInfo[0].DispInfo[i].WarpDataDesc[j].ElementId;
                            UINT32 GroupId = g_AnimMgr.LayoutInfo[0].DispInfo[i].WarpDataDesc[j].GroupId;
                            const AMBA_CALIB_DATA_GROUP_DESC_s *Desc;
                            for (k = 0U; k < g_AnimMgr.LayoutInfo[0].DispInfo[i].GroupCount; k++) {
                                if (GroupId == g_AnimMgr.LayoutInfo[0].DispInfo[i].GroupId[k]) {
                                    Found = 1U;
                                    break;
                                }
                            }
                            if (Found == 1U) {
                                Desc = g_AnimMgr.LayoutInfo[0].DispInfo[i].GroupDesc[k];
                                if (Desc->ItemInfo[ItemId].ElementInfo[ElementId].Type == AMBA_CALIB_DATA_TYPE_WARP) {
                                    /* Fill warp info */
                                    DefaultData->IkWarpInfo[ViewZoneId].Version = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Version;
                                    DefaultData->IkWarpInfo[ViewZoneId].HorGridNum = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.HorGridNum;
                                    DefaultData->IkWarpInfo[ViewZoneId].VerGridNum = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.VerGridNum;
                                    DefaultData->IkWarpInfo[ViewZoneId].TileWidthExp = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.TileWidth;
                                    DefaultData->IkWarpInfo[ViewZoneId].TileHeightExp = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.TileHeight;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.StartX = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.StartX;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.StartY = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.StartY;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.Width = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.Width;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.Height = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.Height;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.HSubSample.FactorDen = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.HSubSampleFactorDen;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.HSubSample.FactorNum = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.HSubSampleFactorNum;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.VSubSample.FactorDen = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.VSubSampleFactorDen;
                                    DefaultData->IkWarpInfo[ViewZoneId].VinSensorGeo.VSubSample.FactorNum = Desc->ItemInfo[ItemId].ElementInfo[ElementId].Warp.Data.CalibSensorGeo.VSubSampleFactorNum;
                                    DefaultData->IkWarpInfo[ViewZoneId].Enb_2StageCompensation = 1U;
                                    AmbaMisra_TypeCast(&DefaultData->IkWarpInfo[ViewZoneId].pWarp, &g_AnimMgr.LayoutInfo[0].DispInfo[i].WarpAddr[j]);
                                } else {
                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetDefaultData(%u): Data type (%u) is not expected!", __LINE__, Desc->ItemInfo[ItemId].ElementInfo[ElementId].Type, 0U, 0U, 0U);
                                    Rval = SVC_NG;
                                }
                            } else {
                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetDefaultData(%u): GroupId is not found!", __LINE__, 0U, 0U, 0U, 0U);
                                Rval = SVC_NG;
                            }
                        }
                        if (Rval != SVC_OK) {
                            break;
                        }
                    }
                }
            }
            if (Rval == SVC_OK) {
                DefaultData->ViewZoneCount = (UINT8)(MaxViewZoneId + 1U);
                /* Fill chan info of lv stream */
                for (i = 0U; i < g_AnimMgr.LayoutInfo[0].DispCount; i++) {
                    if (Rval == SVC_OK) {
                        UINT8 DispId = g_AnimMgr.LayoutInfo[0].DispInfo[i].DispId;
                        UINT16 ViewZoneId;
                        Index = g_AnimMgr.LayoutInfo[0].Index[i];
                        for (j = 0U; j < Index->VoutCount; j++) {
                            if (Rval == SVC_OK) {
                                for (k = 0U; k < Index->ChanCount; k++) {
                                    UINT8 ChanId = g_AnimMgr.ChannelId[DispId][k];
                                    ViewZoneId = g_AnimMgr.ViewZoneId[DispId][k];
                                    DefaultData->ChanInfo[j][ChanId].ViewZoneId = ViewZoneId;
                                    DefaultData->ChanInfo[j][ChanId].ROI = g_AnimMgr.YuvStrmCfg[j].Roi[ViewZoneId];
                                    DefaultData->ChanInfo[j][ChanId].Window = Index->RenderCfg[j][k].ChanInfo.Window;
                                    /* Need to add the offset of the disp window */
                                    DefaultData->ChanInfo[j][ChanId].Window.OffsetX += g_AnimMgr.DispOffset[DispId].OffsetX;
                                    DefaultData->ChanInfo[j][ChanId].Window.OffsetY += g_AnimMgr.DispOffset[DispId].OffsetY;
                                    DefaultData->ChanInfo[j][ChanId].RotateFlip = Index->RenderCfg[j][k].ChanInfo.RotateFlip;
                                    DefaultData->MaxChanWin[j][ViewZoneId].Width = U16MAX(DefaultData->ChanInfo[j][ChanId].Window.Width, DefaultData->ChanInfo[j][ChanId].Window.Height);
                                    DefaultData->MaxChanWin[j][ViewZoneId].Height = DefaultData->MaxChanWin[j][ViewZoneId].Width;
                                    if (Index->BlendType[k] != 0U) {
                                        UINT8 ItemId = g_AnimMgr.LayoutInfo[0].DispInfo[i].BlendDataDesc[j][k].ItemId, m, Found = 0U;
                                        UINT8 ElementId = g_AnimMgr.LayoutInfo[0].DispInfo[i].BlendDataDesc[j][k].ElementId;
                                        UINT32 GroupId = g_AnimMgr.LayoutInfo[0].DispInfo[i].BlendDataDesc[j][k].GroupId;
                                        const AMBA_CALIB_DATA_GROUP_DESC_s *Desc;
                                        for (m = 0U; m < g_AnimMgr.LayoutInfo[0].DispInfo[i].GroupCount; m++) {
                                            if (GroupId == g_AnimMgr.LayoutInfo[0].DispInfo[i].GroupId[m]) {
                                                Found = 1U;
                                                break;
                                            }
                                        }
                                        if (Found == 1U) {
                                            Desc = g_AnimMgr.LayoutInfo[0].DispInfo[i].GroupDesc[m];
                                            if (Desc->ItemInfo[ItemId].ElementInfo[ElementId].Type == AMBA_CALIB_DATA_TYPE_BLEND) {
                                                AMBA_DSP_BUF_s *BlendData = &Index->RenderCfg[j][k].LumaAlphaBuf;
                                                DefaultData->ChanInfo[j][ChanId].BlendNum = Index->RenderCfg[j][k].ChanInfo.BlendNum;
                                                AmbaMisra_TypeCast(&DefaultData->ChanInfo[j][ChanId].LumaAlphaTable, &BlendData);
                                                BlendData->Window = DefaultData->ChanInfo[j][ChanId].Window;
                                                BlendData->Window.Width = (UINT16)Desc->ItemInfo[ItemId].ElementInfo[ElementId].Blend.Width;
                                                BlendData->Window.Height = (UINT16)Desc->ItemInfo[ItemId].ElementInfo[ElementId].Blend.Height;
                                                BlendData->Pitch = U16ALIGN(BlendData->Window.Width, AMBA_CACHE_LINE_SIZE);
                                                AmbaMisra_TypeCast(&BlendData->BaseAddr, &g_AnimMgr.LayoutInfo[0].DispInfo[i].BlendAddr[j][k]);
                                                Rval = SvcPlat_CacheClean(BlendData->BaseAddr, (UINT32)BlendData->Pitch * BlendData->Window.Height);
                                                if (Rval != SVC_OK) {
                                                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetDefaultData: SvcPlat_CacheClean() failed!, Rval = %u", Rval, 0U, 0U, 0U, 0U);
                                                }
                                            } else {
                                                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetDefaultData(%u): Data type (%u) is not expected!", __LINE__, Desc->ItemInfo[ItemId].ElementInfo[ElementId].Type, 0U, 0U, 0U);
                                                Rval = SVC_NG;
                                            }
                                        } else {
                                            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetDefaultData(%u): GroupId is not found!", __LINE__, 0U, 0U, 0U, 0U);
                                            Rval = SVC_NG;
                                        }
                                    } else {
                                        DefaultData->ChanInfo[j][ChanId].BlendNum = 0U;
                                    }
                                    if (Rval != SVC_OK) {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                /* Fill lv stream config */
                for (i = 0U; i < g_AnimMgr.VoutCount; i++) {
                    DefaultData->LvStrmCfg[i].StreamId = g_AnimMgr.YuvStrmCfg[i].YuvStrmId;
                    DefaultData->LvStrmCfg[i].Purpose = g_AnimMgr.YuvStrmCfg[i].Purpose;
                    DefaultData->LvStrmCfg[i].DestVout = (UINT16)1U << g_AnimMgr.VoutCfg[i].VoutId;
                    DefaultData->LvStrmCfg[i].DestEnc = g_AnimMgr.YuvStrmCfg[i].EncDest;
                    DefaultData->LvStrmCfg[i].Width = g_AnimMgr.VoutCfg[i].YuvWidth;
                    DefaultData->LvStrmCfg[i].Height = g_AnimMgr.VoutCfg[i].YuvHeight;
                    DefaultData->LvStrmCfg[i].MaxWidth = U16MAX(DefaultData->LvStrmCfg[i].Width, DefaultData->LvStrmCfg[i].Height);
                    DefaultData->LvStrmCfg[i].MaxHeight = U16MAX(DefaultData->LvStrmCfg[i].Width, DefaultData->LvStrmCfg[i].Height);
                    DefaultData->LvStrmCfg[i].StreamBuf.AllocType = ALLOC_INTERNAL;
                    DefaultData->LvStrmCfg[i].StreamBuf.BufNum = 0U;
                    DefaultData->LvStrmCfg[i].StreamBuf.pYuvBufTbl = NULL;
                    DefaultData->LvStrmCfg[i].MaxChanNum = DefaultData->LvStrmCfg[i].NumChan;
                    DefaultData->LvStrmCfg[i].pChanCfg = DefaultData->ChanInfo[i];
                    DefaultData->LvStrmCfg[i].pMaxChanWin = DefaultData->MaxChanWin[i];
                    for (j = 0U; j < DefaultData->LvStrmCfg[i].NumChan; j++) {
                        DefaultData->LvStrmCfg[i].MaxChanBitMask |= ((UINT32)1U << DefaultData->LvStrmCfg[i].pChanCfg[j].ViewZoneId);
                    }
                }
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_GetDefaultData: AmbaKAL_EventFlagGet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

UINT32 SvcAnim_Play(const SVC_ANIM_PLAY_CFG_s *Cfg)
{
    UINT32 Rval = SVC_OK;
    if (Cfg == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Cfg is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        /* Take mutex */
        Rval = AmbaKAL_MutexTake(&g_AnimMgr.KalCfg.Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == SVC_OK) {
            UINT8 i;
            UINT16 MaxPlayCount = 0U;
            g_AnimMgr.PlayCfg = *Cfg;
            for (i = 0U; i < SVC_ANIM_LIB_MAX_DISP; i++) {
                if (MaxPlayCount < g_AnimMgr.PlayCfg.PlayCount[i]) {
                    MaxPlayCount = g_AnimMgr.PlayCfg.PlayCount[i];
                }
            }
            g_AnimMgr.PlayLayoutCount = MaxPlayCount;
            Rval = AmbaKAL_EventFlagSet(&g_AnimMgr.KalCfg.AnimFlag, SVC_ANIM_EVENT_FLAG_PLAY);
            if (Rval == SVC_OK) {
                /* Give mutex */
                Rval = AmbaKAL_MutexGive(&g_AnimMgr.KalCfg.Mutex);
                if (Rval != SVC_OK) {
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Play: AmbaKAL_MutexGive() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Play: AmbaKAL_EventFlagSet() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_Play: AmbaKAL_MutexTake() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

UINT32 SvcAnim_ConfigDisplay(const SVC_ANIM_DISPLAY_CFG_s *Cfg)
{
    UINT32 Rval = SVC_OK;
    if (Cfg == NULL) {
        AmbaPrint_PrintStr5("[ERROR] %s: Cfg is NULL!", __func__, NULL, NULL, NULL, NULL);
        Rval = SVC_NG;
    } else {
        /* Take mutex */
        Rval = AmbaKAL_MutexTake(&g_AnimMgr.KalCfg.Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == SVC_OK) {
            UINT8 i, j;
            for (i = 0U; i < SVC_ANIM_LIB_MAX_DISP; i++) {
                for (j = 0U; j < SVC_ANIM_LIB_MAX_CHAN; j++) {
                    g_AnimMgr.ChannelId[i][j] = Cfg->ChannelId[i][j];
                    g_AnimMgr.ViewZoneId[i][j] = Cfg->ViewZoneId[i][j];
                }
                g_AnimMgr.DispOffset[i] = Cfg->Offset[i];
            }
            /* Give mutex */
            Rval = AmbaKAL_MutexGive(&g_AnimMgr.KalCfg.Mutex);
            if (Rval != SVC_OK) {
                AmbaPrint_PrintUInt5("[ERROR] SvcAnim_ConfigDisplay: AmbaKAL_MutexGive() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] SvcAnim_ConfigDisplay: AmbaKAL_MutexTake() failed! Rval = %u", Rval, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}
