/**
*  @file SvcRecBlend.c
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
*  @details Definitions and Constants for record blend
*
*/
#include "AmbaKAL.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaUtility.h"
/* framework */
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
/* share */
#include "SvcRecMain.h"
#include "SvcRecBlend.h"

#define SVC_LOG_RBL                     "RBL"
#define SVC_REC_BLEND_STACK_SIZE        0x3000U

#define SVC_REC_BLEND_FLAG_START        1U
#define SVC_REC_BLEND_FLAG_IDLE         2U
#define SVC_REC_BLEND_FLAG_START_ACK    4U

#define SVC_REC_BLEND_WAIT_INTERVAL     500U


static AMBA_KAL_EVENT_FLAG_t         RecBlendEventFlag GNU_SECTION_NOZEROINIT;
static SVC_TASK_CTRL_s               TaskCtrl GNU_SECTION_NOZEROINIT;
static UINT32                        TaskCreate = 0U;
static UINT32                        IsInit     = 0U;
static UINT32                        StreamBits = 0U;
static AMBA_DSP_VIDEO_BLEND_CONFIG_s BlendCfg[CONFIG_ICAM_MAX_REC_STRM][SVC_REC_BLEND_NUM] GNU_SECTION_NOZEROINIT;
static RecBlendUpdBufCB              UpdateBufCB[CONFIG_ICAM_MAX_REC_STRM][SVC_REC_BLEND_NUM] GNU_SECTION_NOZEROINIT;

static void* SvcRecBlendTask(void* EntryArg);

static void SvcRecBlendInit(void)
{
    UINT32 i, j, Err;

    if ((TaskCreate == 0UL) && (IsInit == 0UL)) {

        Err = AmbaWrap_memset(BlendCfg, 0, sizeof(BlendCfg));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_RBL, "AmbaWrap_memset BlendCfg failed!(%u)", Err, 0U);
        }

        for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
            for (j = 0U; j < (UINT32)SVC_REC_BLEND_NUM; j++) {
                UpdateBufCB[i][j] = NULL;
            }
        }

        IsInit = 1U;
    }

}

/**
* create RecBlend task
* @param [in] Priority task priority
* @param [in] CpuBits task cpu bits
* @return ErrorCode
*/
UINT32 SvcRecBlend_Create(UINT32 Priority, UINT32 CpuBits)
{
    UINT32         Rval = SVC_OK, Err;
    static UINT8   RecBlendStack[SVC_REC_BLEND_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char    FlagName[] = "SvcRecBlendFlag";

    if (TaskCreate == 0U) {
        /* flag create */
        Err = AmbaKAL_EventFlagCreate(&RecBlendEventFlag, FlagName);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RBL, "create flag failed!(%u)", Err, 0U);
            Rval = SVC_NG;
        }

        /* task create */
        TaskCtrl.Priority    = Priority;
        TaskCtrl.EntryFunc   = SvcRecBlendTask;
        TaskCtrl.EntryArg    = 0U;
        TaskCtrl.pStackBase  = RecBlendStack;
        TaskCtrl.StackSize   = SVC_REC_BLEND_STACK_SIZE;
        TaskCtrl.CpuBits     = CpuBits;

        Err = SvcTask_Create("RecBlend_Task", &TaskCtrl);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_RBL, "RecBlend_Task created failed!(%u)", Err, 0U);
            Rval = SVC_OK;
        }

        if (Rval == SVC_OK) {
            TaskCreate = 1U;
        }
    } else {
        SvcLog_NG(SVC_LOG_RBL, "the task is already existed", 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* destroy the task
* @return ErrorCode
*/
UINT32 SvcRecBlend_Destroy(void)
{
    UINT32 Rval = SVC_OK, Err, ActualFlags;

    if (TaskCreate == 1U) {
        /* wait task idle */
        Err = AmbaKAL_EventFlagGet(&RecBlendEventFlag, SVC_REC_BLEND_FLAG_IDLE, AMBA_KAL_FLAGS_ALL,
                                    AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RBL, "get flag failed!(%u)", Err, 0U);
        }

        /* task delete */
        Err = SvcTask_Destroy(&TaskCtrl);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RBL, "delete task failed!(%u)", Err, 0U);
            Rval = SVC_NG;
        }

        /* flag delete */
        Err = AmbaKAL_EventFlagDelete(&RecBlendEventFlag);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RBL, "delete flag failed!(%u)", Err, 0U);
            Rval = SVC_NG;
        }

        if (Rval == SVC_OK) {
            SvcLog_OK(SVC_LOG_RBL, "RecBlend_Task is destroyed", 0U, 0U);
            TaskCreate = 0U;
            IsInit     = 0U;
        }
    } else {
        SvcLog_NG(SVC_LOG_RBL, "RecBlend_Task is not existed", 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* config for blend area
* @param [in] StreamId stream index
* @param [in] BlendAreaIdx blend area index
* @param [in] pBlendConfig config
*/
void SvcRecBlend_Config(UINT16 StreamId, UINT16 BlendAreaIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendConfig)
{
    UINT32 Err;

    SvcRecBlendInit();

    if ((StreamId < (UINT16)CONFIG_ICAM_MAX_REC_STRM) && (BlendAreaIdx < (UINT16)SVC_REC_BLEND_NUM)) {
        Err = AmbaWrap_memcpy(&BlendCfg[StreamId][BlendAreaIdx], pBlendConfig, sizeof(AMBA_DSP_VIDEO_BLEND_CONFIG_s));
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_RBL, "AmbaWrap_memcpy failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_RBL, "invalid streamid/BlendAreaIdx(%u/%u)", StreamId, BlendAreaIdx);
    }
}

/**
* register callback function
* @param [in] StreamId stream index
* @param [in] BlendAreaIdx blend area index
* @param [in] pFunc callback function
*/
void SvcRecBlend_RegUpdBufCB(UINT16 StreamId, UINT16 BlendAreaIdx, RecBlendUpdBufCB pFunc)
{
    SvcRecBlendInit();

    if ((StreamId < (UINT16)CONFIG_ICAM_MAX_REC_STRM) && (BlendAreaIdx < (UINT16)SVC_REC_BLEND_NUM)) {
        UpdateBufCB[StreamId][BlendAreaIdx] = pFunc;
    } else {
        SvcLog_NG(SVC_LOG_RBL, "invalid streamid/BlendAreaIdx(%u/%u)", StreamId, BlendAreaIdx);
    }
}

/**
* start rec blend
* @param [in] StartBits stream bits
*/
void SvcRecBlend_Start(UINT32 StartBits)
{
    UINT32  Err, NumStrm = 0U, i, j, ActualFlags = 0U;
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];

    /* enable blend effect when start*/
    SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StartBits, &NumStrm, StrmIdxArr);
    for (i = 0U; i < NumStrm; i++) {
        for (j = 0U; j < SVC_REC_BLEND_NUM; j++) {
            if (BlendCfg[StrmIdxArr[i]][j].BlendYuvBuf.BaseAddrY != 0UL) {
                BlendCfg[StrmIdxArr[i]][j].Enable = 1U;
            }
        }
    }

    StreamBits |= StartBits;

    if (StreamBits != 0U) {
        Err = AmbaKAL_EventFlagSet(&RecBlendEventFlag, SVC_REC_BLEND_FLAG_START);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RBL, "set flag failed!(%u)", Err, 0U);
        }

        Err = AmbaKAL_EventFlagGet(&RecBlendEventFlag,
                                   SVC_REC_BLEND_FLAG_START_ACK,
                                   AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlags,
                                   5000U);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_RBL, "gat flag SVC_REC_BLEND_FLAG_START_ACK failed!(%u)", Err, 0U);
        }
    }
}

/**
* stop rec blend
* @param [in] StopBits stream bits
*/
void SvcRecBlend_Stop(UINT32 StopBits)
{
    UINT32  NumStrm = 0U, i, j, Err;
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];

    StreamBits &= (~StopBits);

    /* disable blend effect when stop*/
    SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StopBits, &NumStrm, StrmIdxArr);
    for (i = 0U; i < NumStrm; i++) {
        for (j = 0U; j < SVC_REC_BLEND_NUM; j++) {
            if (BlendCfg[StrmIdxArr[i]][j].Enable == 1U) {
                BlendCfg[StrmIdxArr[i]][j].Enable = 0U;
                Err = AmbaDSP_VideoEncControlBlend(1, &(StrmIdxArr[i]), &(BlendCfg[StrmIdxArr[i]][j]));
                if (SVC_OK != Err) {
                    SvcLog_NG(SVC_LOG_RBL, "control blend failed!(%u)", Err, 0U);
                }
            }
        }
    }
}

static void* SvcRecBlendTask(void* EntryArg)
{
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM], AreaIdx;
    UINT32  ActualFlags = 0U, Err = OK, NumStrm = 0U, i, Action = 0U, IsStart;

    AmbaMisra_TouchUnused(EntryArg);

    SvcLog_OK(SVC_LOG_RBL, "SvcRecBlendTask created successfully!", 0U, 0U);

    while (Err == OK) {
        Err = AmbaKAL_EventFlagSet(&RecBlendEventFlag, SVC_REC_BLEND_FLAG_IDLE);
        if (Err != OK) {
            SvcLog_NG(SVC_LOG_RBL, "set flag failed!(%u)", Err, 0U);
            continue;
        }

        Err = AmbaKAL_EventFlagGet(&RecBlendEventFlag,
                                   SVC_REC_BLEND_FLAG_START,
                                   AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlags,
                                   AMBA_KAL_WAIT_FOREVER);
        if (Err != OK) {
            SvcLog_NG(SVC_LOG_RBL, "get flag failed!(%u)", Err, 0U);
            continue;
        }

        Err = AmbaKAL_EventFlagClear(&RecBlendEventFlag, SVC_REC_BLEND_FLAG_IDLE);
        if (Err != OK) {
            SvcLog_NG(SVC_LOG_RBL, "clear flag failed!(%u)", Err, 0U);
            continue;
        }
        SvcLog_OK(SVC_LOG_RBL, "SvcRecBlendTask start!", 0U, 0U);

        IsStart = 0U;

        while (Err == OK) {
            SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StreamBits, &NumStrm, StrmIdxArr);
            if (NumStrm == 0U) {
                Err = AmbaKAL_EventFlagClear(&RecBlendEventFlag, SVC_REC_BLEND_FLAG_START | SVC_REC_BLEND_FLAG_START_ACK);
                if (Err != OK) {
                    SvcLog_NG(SVC_LOG_RBL, "clear flag failed!(%u)", Err, 0U);
                }
                SvcLog_OK(SVC_LOG_RBL, "SvcRecBlendTask stop!", 0U, 0U);
                break;
            }

            for (i = 0U; i < NumStrm; i++) {
                for (AreaIdx = 0U; AreaIdx < (UINT16)SVC_REC_BLEND_NUM; AreaIdx++) {
                    if (UpdateBufCB[StrmIdxArr[i]][AreaIdx] != NULL) {
                        Action = 0U;
                        UpdateBufCB[StrmIdxArr[i]][AreaIdx](StrmIdxArr[i], AreaIdx, &Action);
                        if (Action == SVC_REC_BLEND_ACTION_UPDATE_BUF) {
                            if (BlendCfg[StrmIdxArr[i]][AreaIdx].Enable == 1U) {
                                Err = AmbaDSP_VideoEncControlBlend(1, &(StrmIdxArr[i]), &(BlendCfg[StrmIdxArr[i]][AreaIdx]));
                                if (SVC_OK != Err) {
                                    SvcLog_NG(SVC_LOG_RBL, "control blend failed!(%u)", Err, 0U);
                                }
                            }
                        }
                    }
                }
            }

            if (IsStart == 0U) {
                Err = AmbaKAL_EventFlagSet(&RecBlendEventFlag, SVC_REC_BLEND_FLAG_START_ACK);
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_RBL, "set flag failed!(%u)", Err, 0U);
                }

                IsStart = 1U;
            }

            Err = AmbaKAL_TaskSleep(SVC_REC_BLEND_WAIT_INTERVAL);
            if (OK != Err) {
                SvcLog_NG(SVC_LOG_RBL, "AmbaKAL_TaskSleep failed!(%u)", Err, 0U);
            }
        }
    }

    return NULL;
}
