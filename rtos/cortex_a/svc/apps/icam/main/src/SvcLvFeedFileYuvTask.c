/**
 *  @file SvcLvFeedExtRawTask.c
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
 *  @details svc liveview feed external raw task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaShell.h"
#include "AmbaGDMA.h"
#include "AmbaCache.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaDspInt.h"

#include "AmbaVfs.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcMem.h"
#include "SvcVinSrc.h"
#include "SvcLiveview.h"
#include "SvcResCfg.h"
#include "SvcBuffer.h"
#include "SvcPlat.h"

#include "SvcTaskList.h"
#include "SvcBufMap.h"

#include "SvcLvFeedFileYuvTask.h"

#define SVC_LOG_FEEDYUV_TASK         "FEEDYUV_TASK"

typedef struct {
    UINT32                  Enable;
    UINT32                  CurIdx;
    UINT32                  BufNum;
    UINT32                  YuvType;
    AMBA_DSP_EXT_YUV_BUF_s  Buf[SVC_FEEDYUV_MAX_BUF];
} SVC_FEEDYUV_FOV_INFO_s;

typedef struct {
#define SVC_FEEDYUV_TASK_STACK_SIZE  (0x6000U)
    UINT32                  IsCreate;
    UINT32                  UpdateInterval;
    SVC_TASK_CTRL_s         UpdateTask;
    UINT8                   UpdateTaskStack[SVC_FEEDYUV_TASK_STACK_SIZE];

#define SVC_FEEDYUV_FLG_WAITFLAG (0x7FFFFUL)
    AMBA_KAL_EVENT_FLAG_t   Flag;
    SVC_FEEDYUV_FOV_INFO_s  FovInfo[AMBA_DSP_MAX_VIEWZONE_NUM];
} SVC_FEEDYUV_CTRL_s;


static SVC_FEEDYUV_CTRL_s   FeedCtrl GNU_SECTION_NOZEROINIT;

static void* UpdateTask_Entry(void* EntryArg)
{
    UINT32                    Err, LoopU = 1U, ActualFlag, i;
    SVC_FEEDYUV_FOV_INFO_s    *pFovInfo;
    UINT16                    FovId[AMBA_DSP_MAX_VIEWZONE_NUM], Num;
    AMBA_DSP_EXT_YUV_BUF_s    ExtBuf[AMBA_DSP_MAX_VIEWZONE_NUM];
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&LoopU);

    while (LoopU == 1U) {
        ActualFlag = 0U;
        Num        = 0U;

        Err = AmbaWrap_memset(&ExtBuf, 0, sizeof(ExtBuf));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
        }

        Err = AmbaKAL_EventFlagGet(&(FeedCtrl.Flag),
                                    SVC_FEEDYUV_FLG_WAITFLAG,
                                    AMBA_KAL_FLAGS_ANY,
                                    AMBA_KAL_FLAGS_CLEAR_NONE,
                                    &ActualFlag,
                                    AMBA_KAL_WAIT_FOREVER);
        if (Err == KAL_ERR_NONE) {
            for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                if ((ActualFlag & (1UL << i)) > 0U) {
                    pFovInfo = &(FeedCtrl.FovInfo[i]);

                    if (pFovInfo->Enable > 0U) {
                        Err = AmbaWrap_memcpy(&(ExtBuf[Num]), &(pFovInfo->Buf[pFovInfo->CurIdx]), sizeof(AMBA_DSP_EXT_YUV_BUF_s));
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memcpy failed %u", Err, 0U);
                        }
                        FovId[Num] = (UINT16)i;
                        Num++;

                        /* update buffer index */
                        pFovInfo->CurIdx++;
                        if (pFovInfo->CurIdx >= pFovInfo->BufNum) {
                            pFovInfo->CurIdx = 0U;
                        }
                    }
                }
            }

            /* update yuv buffer */
            Err = AmbaDSP_LiveviewFeedYuvData(Num, FovId, ExtBuf);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaDSP_LiveviewFeedYuvData failed %u", Err, 0U);
            }

            Err = AmbaKAL_TaskSleep(FeedCtrl.UpdateInterval);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_TaskSleep failed %u", Err, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_EventFlagGet SVC_FEEDYUV_FLG_WAITFLAG failed %u", Err, 0U);
            LoopU = 0U;
        }
    }

    return NULL;
}

/**
* create yuv file feed task.
* @param [in]  pCreate create info
* @return ErrorCode
*/
UINT32 SvcLvFeedFileYuvTask_TaskCreate(const SVC_FEEDYUV_CREATE_s *pCreate)
{
    UINT32               Rval = SVC_OK, Err, i, j;
    UINT32               FovSrc = 0U, MemNum = 0U, MemFovId[AMBA_DSP_MAX_VIEWZONE_NUM] = {0}, MemYuvType[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
    ULONG                BufBase = 0U, BufLimit = 0U;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
    static UINT8         IsInit = 0U;

    AmbaMisra_TouchUnused(&Rval);

    Err = AmbaWrap_memset(MemFovId, 0, sizeof(MemFovId));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
    }

    Err = AmbaWrap_memset(MemYuvType, 0, sizeof(MemYuvType));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
    }

    if (IsInit == 0U) {
        Err = AmbaWrap_memset(&FeedCtrl, 0, sizeof(FeedCtrl));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
        }
        IsInit = 1U;
    }

    if (FeedCtrl.IsCreate == 0U) {
        Err = AmbaWrap_memset(&FeedCtrl, 0, sizeof(FeedCtrl));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
        }

        for (i = 0U; i < pCfg->FovNum; i++) {
            Err = SvcResCfg_GetFovSrc(i, &FovSrc);
            if ((Err == SVC_OK) && ((FovSrc == SVC_VIN_SRC_MEM_YUV420) || (FovSrc == SVC_VIN_SRC_MEM_YUV422) || (FovSrc == SVC_VIN_SRC_MEM_DEC))) {
                if ((FovSrc == SVC_VIN_SRC_MEM_YUV420) || (FovSrc == SVC_VIN_SRC_MEM_DEC)) {
                    MemYuvType[MemNum] = AMBA_DSP_YUV420;
                } else {
                    MemYuvType[MemNum] = AMBA_DSP_YUV422;
                }
                MemFovId[MemNum] = i;
                MemNum++;
            }
        }

        if ((Rval == SVC_OK) && (MemNum > 0U)) {
            UINT32 BufSize = 0U;

            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_FEED_FILE_YUV, &BufBase, &BufSize);
            if (Err == SVC_OK) {
                BufLimit = BufBase + BufSize;
            } else {
                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcBuffer_Request SMEM_PF0_ID_FEED_FILE_YUV failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        /* fill the info for each fov buffer */
        if ((Rval == SVC_OK) && (MemNum > 0U)) {
            UINT32                  FovId, YuvType, Pitch, Height;
            AMBA_DSP_EXT_YUV_BUF_s  *pBuf;

            for (i = 0U; i < MemNum; i++) {
                for (j = 0U; j < (UINT32)SVC_FEEDYUV_MAX_BUF; j++) {
                    FovId   = MemFovId[i];
                    YuvType = MemYuvType[i];
                    pBuf    = &(FeedCtrl.FovInfo[FovId].Buf[j]);

                    FeedCtrl.FovInfo[FovId].YuvType = YuvType;

                    /* pitch should be 64 aligned, height should be 16 aligned*/
                    Pitch  = GetAlignedValU32((UINT32)pCfg->FovCfg[FovId].RawWin.Width, 64U);
                    Height = GetAlignedValU32((UINT32)pCfg->FovCfg[FovId].RawWin.Height, 16U);

                    pBuf->ExtYuvBuf.DataFmt        = (UINT8)YuvType;
                    pBuf->ExtYuvBuf.Pitch          = (UINT16)Pitch;
                    pBuf->ExtYuvBuf.Window.Width   = pCfg->FovCfg[FovId].RawWin.Width;
                    pBuf->ExtYuvBuf.Window.Height  = pCfg->FovCfg[FovId].RawWin.Height;
                    pBuf->ExtYuvBuf.Window.OffsetX = 0U;
                    pBuf->ExtYuvBuf.Window.OffsetY = 0U;
                    pBuf->pExtME1Buf               = NULL;
                    pBuf->pExtME0Buf               = NULL;
                    pBuf->ExtYuvBuf.BaseAddrY      = BufBase;
                    pBuf->ExtYuvBuf.BaseAddrUV     = BufBase + ((ULONG)Pitch * (ULONG)Height);

                    if (YuvType == AMBA_DSP_YUV420) {
                        BufBase += (((ULONG)Pitch * (ULONG)Height) * (ULONG)3U) / (ULONG)2U;
                    } else {
                        BufBase += (((ULONG)Pitch * (ULONG)Height) * (ULONG)2U);
                    }

                    if (BufBase > BufLimit) {
                        SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "Buffersize is not enough %u/%u", (UINT32)BufBase, (UINT32)BufLimit);
                        Rval = SVC_NG;
                        break;
                    }
                }

                if (Rval != SVC_OK) {
                    break;
                }
            }
        }

        /* Read yuv data from the files to the buffers */
        if ((Rval == SVC_OK) && (MemNum > 0U)) {
            const SVC_FEEDYUV_FOV_s         *pFov;
            SVC_FEEDYUV_FOV_INFO_s          *pFovInfo;
            const AMBA_DSP_YUV_IMG_BUF_s    *pImg;
            void                            *Addr;
            AMBA_VFS_FILE_s                 File;
            UINT32                          NumSuccess = 0U;
            UINT32                          UVBufSize;

            for (i = 0U; i < pCreate->FovNum; i++) {
                pFov     = &(pCreate->FovInfo[i]);
                pFovInfo = &(FeedCtrl.FovInfo[pFov->FovIdx]);

                pFovInfo->BufNum = pFov->BufNum;
                pFovInfo->Enable = 1U;
                pFovInfo->CurIdx = 0U;

                for (j = 0U; j < pFovInfo->BufNum; j++) {
                    pImg = &(pFovInfo->Buf[j].ExtYuvBuf);

                    if (pFovInfo->YuvType == AMBA_DSP_YUV420) {
                        UVBufSize = ((UINT32)pImg->Pitch * (UINT32)pImg->Window.Height) / 2UL;
                    } else {
                        UVBufSize = ((UINT32)pImg->Pitch * (UINT32)pImg->Window.Height);
                    }

                    if (pFov->FileName[j][0] != '\0') { /* read yuv data from file to buffer */
                        Err = AmbaVFS_Open(&(pFov->FileName[j][0]), "r", 0U, &File);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaVFS_Open failed %u", Err, 0U);
                            Rval = SVC_NG;
                        }

                        if (Rval == SVC_OK) {
                            /* Y */
                            Err = SvcPlat_CacheInvalidate(pImg->BaseAddrY, (UINT32)pImg->Pitch * (UINT32)pImg->Window.Height);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcPlat_CacheInvalidate failed %u", Err, 0U);
                            }

                            AmbaMisra_TypeCast(&Addr, &(pImg->BaseAddrY));
                            Err = AmbaVFS_Read(Addr, (UINT32)pImg->Pitch * (UINT32)pImg->Window.Height, 1U, &File, &NumSuccess);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaVFS_Read failed %u", Err, 0U);
                                Rval = SVC_NG;
                            }

                            Err = SvcPlat_CacheClean(pImg->BaseAddrY, (UINT32)pImg->Pitch * (UINT32)pImg->Window.Height);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcPlat_CacheClean failed %u", Err, 0U);
                            }

                            /* UV */
                            Err = SvcPlat_CacheInvalidate(pImg->BaseAddrUV, UVBufSize);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcPlat_CacheInvalidate failed %u", Err, 0U);
                            }

                            AmbaMisra_TypeCast(&Addr, &(pImg->BaseAddrUV));
                            Err = AmbaVFS_Read(Addr, UVBufSize, 1U, &File, &NumSuccess);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaVFS_Read failed %u", Err, 0U);
                                Rval = SVC_NG;
                            }

                            Err = SvcPlat_CacheClean(pImg->BaseAddrUV, UVBufSize);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcPlat_CacheClean failed %u", Err, 0U);
                            }

                            Err = AmbaVFS_Close(&File);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaVFS_Close failed %u", Err, 0U);
                                Rval = SVC_NG;
                            }

                            Err = AmbaKAL_TaskSleep(500UL);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_TaskSleep failed %u", Err, 0U);
                            }
                        }
                    } else { /* set buffer to the default yuv value */
                        #define DEFAULT_Y_VAL  (0)
                        #define DEFAULT_UV_VAL (128)

                        /* Y */
                        AmbaMisra_TypeCast(&Addr, &(pImg->BaseAddrY));
                        Err = AmbaWrap_memset(Addr, DEFAULT_Y_VAL, (UINT32)pImg->Pitch * (UINT32)pImg->Window.Height);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
                        }
                        Err = SvcPlat_CacheClean(pImg->BaseAddrY, (UINT32)pImg->Pitch * (UINT32)pImg->Window.Height);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcPlat_CacheClean failed %u", Err, 0U);
                        }

                        /* UV */
                        AmbaMisra_TypeCast(&Addr, &(pImg->BaseAddrUV));
                        Err = AmbaWrap_memset(Addr, DEFAULT_UV_VAL - ((INT32)j * 64), UVBufSize);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
                        }
                        Err = SvcPlat_CacheClean(pImg->BaseAddrUV, UVBufSize);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcPlat_CacheClean failed %u", Err, 0U);
                        }
                    }
                }
            }
        }

        /* create event flag */
        if ((Rval == SVC_OK) && (MemNum > 0U)) {
            static char FlgName[] = "LvFeedYuv";

            Err = AmbaKAL_EventFlagCreate(&(FeedCtrl.Flag), FlgName);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_EventFlagCreate failed", Err, 0U);
                Rval = SVC_NG;
            }

            Err = AmbaKAL_EventFlagClear(&(FeedCtrl.Flag), SVC_FEEDYUV_FLG_WAITFLAG);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_EventFlagClear failed", Err, 0U);
                Rval = SVC_NG;
            }
        }

        /* create the task */
        if ((Rval == SVC_OK) && (MemNum > 0U)) {
            FeedCtrl.UpdateTask.EntryFunc  = UpdateTask_Entry;
            FeedCtrl.UpdateTask.pStackBase = FeedCtrl.UpdateTaskStack;
            FeedCtrl.UpdateTask.EntryArg   = 0U;
            FeedCtrl.UpdateTask.StackSize  = sizeof(FeedCtrl.UpdateTaskStack);
            FeedCtrl.UpdateTask.Priority   = SVC_LV_FD_FILE_YUV_CP_TASK_PRI;
            FeedCtrl.UpdateTask.CpuBits    = SVC_LV_FD_FILE_YUV_TASK_CPU_BITS;

            Err = SvcTask_Create("SvcLvFeedFile", &(FeedCtrl.UpdateTask));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcTask_Create failed", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if ((Rval == SVC_OK) && (MemNum > 0U)) {
            FeedCtrl.IsCreate       = 1U;
            FeedCtrl.UpdateInterval = pCreate->UpdateInterval;
        }
    } else {
        SvcLog_DBG(SVC_LOG_FEEDYUV_TASK, "the task is already created", 0U, 0U);
    }

    return Rval;

}

/**
* delete yuv file feed task.
* @return ErrorCode
*/
UINT32 SvcLvFeedFileYuvTask_TaskDelete(void)
{
    UINT32 Err, Rval = SVC_OK;

    AmbaMisra_TouchUnused(&Rval);

    if (FeedCtrl.IsCreate == 1U) {
        Err = SvcTask_Destroy(&(FeedCtrl.UpdateTask));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "SvcTask_Destroy failed", Err, 0U);
            Rval = SVC_NG;
        }

        Err = AmbaKAL_EventFlagDelete(&(FeedCtrl.Flag));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_EventFlagDelete failed", Err, 0U);
            Rval = SVC_NG;
        }

        if (Rval == SVC_OK) {
            FeedCtrl.IsCreate = 0U;
        }
    }

    return Rval;
}

/**
* start yuv update to dsp.
* @param [in]  FovBits fov bits
* @return ErrorCode
*/
UINT32 SvcLvFeedFileYuvTask_Start(UINT32 FovBits)
{
    UINT32 Err, Rval = SVC_OK;

    AmbaMisra_TouchUnused(&Rval);

    if (FeedCtrl.IsCreate == 1U) {
        Err = AmbaKAL_EventFlagSet(&(FeedCtrl.Flag), FovBits);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_EventFlagSet failed", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

/**
* stop yuv update to dsp.
* @param [in]  FovBits fov bits
* @return ErrorCode
*/
UINT32 SvcLvFeedFileYuvTask_Stop(UINT32 FovBits)
{
    UINT32 Err, Rval = SVC_OK;

    AmbaMisra_TouchUnused(&Rval);

    if (FeedCtrl.IsCreate == 1U) {
        Err = AmbaKAL_EventFlagClear(&(FeedCtrl.Flag), FovBits);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaKAL_EventFlagClear failed", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

/**
* memory size evaluation
* @param [out]  pWorkSize return size
*/
void SvcLvFeedFileYuvTask_MemSizeQuery(UINT32 *pWorkSize)
{
    UINT32               Rval = SVC_OK, Err, i;
    UINT32               FovSrc = 0U, MemNum = 0U, Rsize = 0U;
    UINT32               MemFovId[AMBA_DSP_MAX_VIEWZONE_NUM] = {0}, MemYuvType[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(&Rval);

    Err = AmbaWrap_memset(MemFovId, 0, sizeof(MemFovId));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
    }

    Err = AmbaWrap_memset(MemYuvType, 0, sizeof(MemYuvType));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_FEEDYUV_TASK, "AmbaWrap_memset failed %u", Err, 0U);
    }

    for (i = 0U; i < pCfg->FovNum; i++) {
        Err = SvcResCfg_GetFovSrc(i, &FovSrc);
        if ((Err == SVC_OK) && ((FovSrc == SVC_VIN_SRC_MEM_YUV420) || (FovSrc == SVC_VIN_SRC_MEM_YUV422) || (FovSrc == SVC_VIN_SRC_MEM_DEC))) {
            if ((FovSrc == SVC_VIN_SRC_MEM_YUV420) || (FovSrc == SVC_VIN_SRC_MEM_DEC)) {
                MemYuvType[MemNum] = AMBA_DSP_YUV420;
            } else {
                MemYuvType[MemNum] = AMBA_DSP_YUV422;
            }
            MemFovId[MemNum] = i;
            MemNum++;
        }
    }

    if ((Rval == SVC_OK) && (MemNum > 0U)) {
        UINT32 FovId, YuvType, Pitch, Height, Size;

        for (i = 0U; i < MemNum; i++) {
            FovId   = MemFovId[i];
            YuvType = MemYuvType[i];

            /* pitch should be 64 aligned, height should be 16 aligned*/
            Pitch  = GetAlignedValU32((UINT32)pCfg->FovCfg[FovId].RawWin.Width, 64U);
            Height = GetAlignedValU32((UINT32)pCfg->FovCfg[FovId].RawWin.Height, 16U);

            Size   = Pitch * Height;
            /* YUV420: 1.5*size, YUV422: 2*size */
            if (YuvType == AMBA_DSP_YUV420) {
                Size   = (Size * 3U) / 2U;
            } else {
                Size   = (Size * 2U);
            }
            Size   = Size * (UINT32)SVC_FEEDYUV_MAX_BUF;

            Rsize += Size;
        }
    }

    *pWorkSize = Rsize;
}
