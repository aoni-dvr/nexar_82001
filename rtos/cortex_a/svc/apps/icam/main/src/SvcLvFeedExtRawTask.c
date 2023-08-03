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
#include "AmbaShell.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDspInt.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcLiveview.h"
#include "SvcResCfg.h"
#include "SvcTaskList.h"
#include "SvcLvFeedExtRawTask.h"

#define SVC_LV_FEED_EXT_RAW_TASK_STACK_SIZE    (0x10000U)
/* Internal Attribute */
typedef struct {
    UINT8               TaskEnable;
    SVC_TASK_CTRL_s     Task;
    char                TaskName[32U];
    UINT8               TaskStack[SVC_LV_FEED_EXT_RAW_TASK_STACK_SIZE];
    AMBA_DSP_INT_PORT_s IntPort;
} SVC_LV_FEED_EXT_RAW_TASK_CTRL_s;

static SVC_LV_FEED_EXT_RAW_TASK_CTRL_s LvFeedExtRawTaskCtrl[AMBA_DSP_MAX_VIN_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 LvFeedExtRawTask_Ctrl(UINT32 Enable);
static void*  LvFeedExtRawTask_Entry(void *EntryArg);

#define SVC_LOG_LV_FD_EX_RAW_TASK "LV_FD_EX_RAW_TASK"


UINT32 SvcLvFeedExtRawTask_Start(void)
{
    static UINT8 Init = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;

    UINT32 RetVal = SVC_OK;

    if (Init == 0U) {
        if (SVC_OK != AmbaWrap_memset(&LvFeedExtRawTaskCtrl, 0, sizeof(LvFeedExtRawTaskCtrl))) {
            SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "## SvcLiveviewTask_Init() failed with AmbaWrap_memset SVC_LV_FD_EX_RAW_TASK_CTRL_s", 0U, 0U);
        }
        Init = 1U;
    }

    if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxs, &FovNum)) {
        if (FovNum > 0U) {
            if (SVC_OK == LvFeedExtRawTask_Ctrl(1U /* Enable */)) {
                /* Do nothing here */
            } else {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Ctrl() enable failed", 0U, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    return RetVal;

}

UINT32 SvcLvFeedExtRawTask_Stop(void)
{
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;
    UINT32 RetVal = SVC_OK;

    if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxs, &FovNum)) {
        if (FovNum > 0U) {
            if (SVC_OK != LvFeedExtRawTask_Ctrl(0U /* Disable */)) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Ctrl() disable failed", 0U, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    return RetVal;
}


static UINT32 LvFeedExtRawTask_Ctrl(UINT32 Enable)
{
    UINT32 VinID, VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;
    UINT32 FovIdxsInVinID[AMBA_DSP_MAX_VIEWZONE_NUM], FovNumInVinID = 0U;
    UINT32 FovIdxsFromMem[AMBA_DSP_MAX_VIEWZONE_NUM], FovNumFromMem = 0U;
    UINT32 FovIdx = 0U;

    UINT32 RetVal = SVC_OK;

    SvcLog_DBG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Ctrl(%u) start", Enable, 0U);

    if (SVC_OK != SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
        SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Ctrl() err, SvcResCfg_GetVinNum() failed", 0U, 0U);
    }

    if (Enable > 0U) {
        UINT32 TaskCreate = 0U;
        for (UINT32 Idx = 0U; Idx < VinNum; Idx++) {
            VinID = VinIDs[Idx];

            if (SVC_OK == SvcResCfg_GetFovIdxsInVinID(VinID, FovIdxsInVinID, &FovNumInVinID)) {
                if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxsFromMem, &FovNumFromMem)) {
                    for (UINT32 i = 0U; i < FovNumInVinID; i++) {
                        UINT32 FovIdxInVinID = FovIdxsInVinID[i];
                        for (UINT32 j = 0U; j < FovNumFromMem; j++) {
                            UINT32 FovIdxFromMem = FovIdxsFromMem[j];
                            if (FovIdxInVinID == FovIdxFromMem) {
                                FovIdx = FovIdxInVinID;
                                TaskCreate = 1U;
                                break;
                            }
                        }
                        if (TaskCreate == 1U) {
                            break;
                        }
                    }
                }
            }
            if ((LvFeedExtRawTaskCtrl[VinID].TaskEnable == 0U) && (TaskCreate == 1U)) {
                /* Open Inerrupt port to receive the necessary interrupt */
                RetVal = AmbaDspInt_Open(&LvFeedExtRawTaskCtrl[VinID].IntPort);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "Vin[%u] LvFeedExtRawTask_Ctrl() err, AmbaDspInt_Open() failed with 0x%x", VinID, RetVal);
                } else {
                    /* Create the external data feeding task */
                    LvFeedExtRawTaskCtrl[VinID].Task.Priority   = SVC_LV_FEED_EXT_RAW_TASK_PRI;
                    LvFeedExtRawTaskCtrl[VinID].Task.EntryFunc  = LvFeedExtRawTask_Entry;
                    LvFeedExtRawTaskCtrl[VinID].Task.pStackBase = LvFeedExtRawTaskCtrl[VinID].TaskStack;
                    LvFeedExtRawTaskCtrl[VinID].Task.EntryArg   = FovIdx;
                    LvFeedExtRawTaskCtrl[VinID].Task.StackSize  = SVC_LV_FEED_EXT_RAW_TASK_STACK_SIZE;
                    LvFeedExtRawTaskCtrl[VinID].Task.CpuBits    = SVC_LV_FEED_EXT_RAW_TASK_CPU_BITS;

                    RetVal = SvcTask_Create("SvcLiveviewFeedExtDataTask", &LvFeedExtRawTaskCtrl[VinID].Task);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "Vin[%u] LvFeedExtRawTask_Ctrl() err, task create failed with 0x%x", VinID, RetVal);
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "Vin[%u] LvFeedExtRawTask_Ctrl() err, task has been created", VinID, 0U);
                RetVal = SVC_NG;
            }
        }
    } else {
        for (UINT32 i = 0U; i < VinNum; i++) {
            VinID = VinIDs[i];
            if (LvFeedExtRawTaskCtrl[VinID].TaskEnable != 0U) {
                /* Destroy the external data feeding task */
                RetVal = SvcTask_Destroy(&LvFeedExtRawTaskCtrl[VinID].Task);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "Vin[%u] LvFeedExtRawTask_Ctrl() err, task destroy failed with 0x%x", VinID, RetVal);
                } else {
                    /* Close the port */
                    RetVal = AmbaDspInt_Close(&LvFeedExtRawTaskCtrl[VinID].IntPort);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "Vin[%u] LvFeedExtRawTask_Ctrl() err, AmbaDspInt_Close() failed with 0x%x", VinID, RetVal);
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "Vin[%u] LvFeedExtRawTask_Ctrl() err, task doesn't exist", VinID, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    SvcLog_DBG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Ctrl(%u) Done", Enable, 0U);

    return RetVal;
}

static void* LvFeedExtRawTask_Entry(void *EntryArg)
{
    const ULONG *pArg;
    UINT32  VinID = 0U, FovIdx = 0U, FovIdxFrom = 0U, Valid = 0U;
    UINT32  Rval = SVC_OK;

    UINT32  Flag, ActualFlag, SwLvRawSeq = 0U;

    SVC_LIV_INFO_s LivInfo;
    const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();

    /* Get the input address to pointer pArg */
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    FovIdx = (*pArg);

    if (SVC_OK == SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID)) {
        /* Do nothing */
    }

    if (SVC_OK == SvcResCfg_GetFovIdxBypassFrom(FovIdx, &FovIdxFrom, &Valid)) {
        if (Valid > 0U) {
            /* Do nothing */
            SvcLog_DBG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Entry() Find FovIdx(%d) src is from FovIdx(%d)", FovIdx, FovIdxFrom);
        } else {
            SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Entry() err. Cannot find src fov", 0U, 0U);
        }
    }

    SvcLiveview_InfoGet(&LivInfo);

    while (VinID < AMBA_DSP_MAX_VIN_NUM) {
        Flag = SVC_DSP_VIN0_SOF << (2U * VinID);
        ActualFlag = 0U;
        if (Rval == AmbaDspInt_Take(&LvFeedExtRawTaskCtrl[VinID].IntPort, Flag, &ActualFlag, AMBA_KAL_WAIT_FOREVER)) {
            if ((ActualFlag & Flag) > 0U) {
                AMBA_DSP_LIVEVIEW_EXT_RAW_s ExtRaw = {0};
                UINT32 BufIdx;
                UINT8 VinRawCompr = 0;  /* No compression */
                UINT16 RawWidth, RawPitch;
                UINT16 ViewZoneId = (UINT16) FovIdx;
                UINT16 NumViewZone = 1U;

                (void)AmbaDSP_GetCmprRawBufInfo(pResCfg->FovCfg[FovIdx].RawWin.Width, VinRawCompr, &RawWidth, &RawPitch);

                ExtRaw.RawBuffer.Compressed     = VinRawCompr;
                ExtRaw.RawBuffer.Window.Width   = pResCfg->FovCfg[FovIdx].RawWin.Width;
                ExtRaw.RawBuffer.Window.Height  = pResCfg->FovCfg[FovIdx].RawWin.Height;
                ExtRaw.RawBuffer.Window.OffsetX = pResCfg->FovCfg[FovIdx].RawWin.OffsetX;
                ExtRaw.RawBuffer.Window.OffsetY = pResCfg->FovCfg[FovIdx].RawWin.OffsetY;
                BufIdx = (UINT32) (SwLvRawSeq % (UINT32) CONFIG_ICAM_LV_FEED_EXT_DATA_BUF_NUM);
                ExtRaw.RawBuffer.BaseAddr = LivInfo.pFeedExtDataTbl[FovIdxFrom].BufAddr[BufIdx];
                ExtRaw.RawBuffer.Pitch = RawPitch;
                ExtRaw.VinId  = (UINT16) VinID;
                ExtRaw.VinId |= (UINT16)((UINT32) 1U << 15U);   /* From VirtVin */

                ExtRaw.IsLast = 0U; //TBD
                ExtRaw.CapPts = 0U; //TBD
                ExtRaw.CapSequence = 0U; //TBD

                (void)AmbaDSP_LiveviewFeedRawData(NumViewZone, &ViewZoneId, &ExtRaw);
                SwLvRawSeq++;
            }
        } else {
            SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtRawTask_Entry() err, AmbaDspInt_Task() failed with 0x%x", Rval, 0U);
        }
    }
    return NULL;
}