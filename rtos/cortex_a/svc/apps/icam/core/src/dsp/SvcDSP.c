/**
 *  @file SvcDSP.c
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
 *  @details svc dsp control
 *
 */

#include "AmbaMisraFix.h"
#include "AmbaShell_Commands.h"
#include "AmbaCache.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcWrap.h"
#include "SvcPlat.h"
#include "SvcDSP.h"

#define SVC_LOG_DSP                 "DSP"

#define DSP_MSG_TASK_STACK_SIZE     (0x15000U)

#define SVCDSP_STATUS_IDLE    (0U)
#define SVCDSP_STATUS_RUN     (1U)
#ifdef CONFIG_ICAM_DSP_SUSPEND
#define SVCDSP_STATUS_SUSPEND (2U)
#endif

static UINT8   SvcDSP_Status = SVCDSP_STATUS_IDLE;

static UINT32  g_CleanBoot = SVCDSP_OP_INVALID;

#define SVCDSP_TASK_READY   (1U)
static AMBA_KAL_EVENT_FLAG_t SvcDspMsgTskFlag GNU_SECTION_NOZEROINIT;

static UINT32 SvcDSP_ErrEventHdlr(const void *pEventData)
{
    static UINT32 IsHitErrEvt[AMBA_DSP_ERROR_NUM] = {0U};
    AMBA_DSP_EVENT_ERROR_INFO_s *pErrInfo = NULL;

    AmbaMisra_TypeCast(&pErrInfo, &pEventData);
    if (pErrInfo != NULL) {
        if (pErrInfo->Type < AMBA_DSP_ERROR_NUM) {
            if (IsHitErrEvt[pErrInfo->Type] == 0U) {

#ifdef AMBA_DSP_ERROR_BITSINFO
                if (pErrInfo->Type == AMBA_DSP_ERROR_BITSINFO) {
                    IsHitErrEvt[pErrInfo->Type] = 1U;
                    SvcLog_NG(SVC_LOG_DSP, "## get dsp encoder bitstream error(0x%X)", pErrInfo->Info, 0U);
                } else
#endif
#ifdef AMBA_DSP_ERROR_ASSERT
                if (pErrInfo->Type == AMBA_DSP_ERROR_ASSERT) {
                    SvcLog_NG(SVC_LOG_DSP, "## get dsp assertion error(0x%X)", pErrInfo->Info, 0U);
                } else
#endif
#ifdef AMBA_DSP_ERROR_VIN_CMDREQ
                if (pErrInfo->Type == AMBA_DSP_ERROR_VIN_CMDREQ) {
                    IsHitErrEvt[pErrInfo->Type] = 1U;
#ifndef CONFIG_LINUX
                    SvcLog_NG(SVC_LOG_DSP, "## get dsp vin cmd timeout", 0U, 0U);
#endif
                } else
#endif
#ifdef AMBA_DSP_ERROR_VOUT_CMDREQ
                if (pErrInfo->Type == AMBA_DSP_ERROR_VOUT_CMDREQ) {
                    IsHitErrEvt[pErrInfo->Type] = 1U;
#ifndef CONFIG_LINUX
                    SvcLog_NG(SVC_LOG_DSP, "## get dsp vout cmd timeout", 0U, 0U);
#endif
                } else
#endif
                {
                    IsHitErrEvt[pErrInfo->Type] = 1U;
                    SvcLog_NG(SVC_LOG_DSP, "## get dsp error event. Type(%d), Info(0x%X)", pErrInfo->Type, pErrInfo->Info);
                }
            }
        }

        AmbaMisra_TouchUnused(pErrInfo);
    }

    return 0U;
}

static void* DspMsgTaskEntry(void* EntryArg)
{
    UINT32  Rval = SVC_OK, LoopU = 1U, WaitFlag, ActualFlags = 0U;
    const UINT32 *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    WaitFlag = AMBA_DSP_MESSAGE_ARRIVED;
    while (0U < LoopU) {
        Rval = AmbaDSP_MainWaitFlag(WaitFlag, &ActualFlags, 0xFFFFU);
        if (Rval == OK) {
            if (0U < (ActualFlags & AMBA_DSP_MESSAGE_ARRIVED)) {
                Rval = AmbaDSP_MainMsgParseEntry((*pArg));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_DSP, "## fail to parse dsp message, rval(%u)", Rval, 0U);
                }
            }
        } else {
        #if defined(CONFIG_ICAM_DSP_SUSPEND)
            Rval = AmbaKAL_EventFlagClear(&SvcDspMsgTskFlag, SVCDSP_TASK_READY);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_DSP, "dsp msg task flag clear fail! ErrCode(0x%X)", Rval, 0U);
            }
            Rval = AmbaKAL_EventFlagGet(&SvcDspMsgTskFlag, SVCDSP_TASK_READY, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_DSP, "dsp msg task flag get fail! ErrCode(0x%X)", Rval, 0U);
            }
        #else
            LoopU = 0U;
        #endif
            AmbaMisra_TouchUnused(&LoopU);
        }
    }

    SvcLog_NG("DspMsgTaskEntry", "wait dsp message timeout, rval(0x%X)", Rval, 0U);
#ifndef CONFIG_QNX
    AmbaPrint_Flush();
#endif

    return NULL;
}

static inline UINT32 FRAME_ALIGN(UINT32 AlignVal, UINT32 Size)
{
    UINT32 RetVal = 0U;

    if ((AlignVal >= 2U) && (Size > 0U)) {
        RetVal = ( Size + ( AlignVal - 1U ) ) & ( ~ ( AlignVal - 1U ) );
    }
    return RetVal;
}

/**
* status of dsp boot
* @return 0-RestartBoot, 1-CleanBoot
*/
UINT32 SvcDSP_IsCleanBoot(void)
{
    UINT32  Data0 = 0xFFFFFFFFU, Data1 = 0U;

    if (g_CleanBoot == SVCDSP_OP_INVALID) {
        if (AmbaDSP_GetStatus(DSP_STATUS_TYPE_OP_MODE, 0U, &Data0, &Data1) != OK) {
            SvcLog_DBG(SVC_LOG_DSP, "fail to get dsp status", 0U, 0U);
        }

        if (Data0 == 0U) {
            g_CleanBoot = SVCDSP_OP_DIRTYBOOT;
        } else {
            g_CleanBoot = SVCDSP_OP_CLEANBOOT;
        }
    }

    return g_CleanBoot;
}

/**
* status of dsp boot
* @return 0-NotBoot, 1-BootDone
*/
UINT8 SvcDSP_IsBootDone(void)
{
    UINT8 Rval = 0U;

    if (SvcDSP_Status > SVCDSP_STATUS_IDLE) {
        Rval = 1U;
    }

    return Rval;
}

/**
* create dsp message task
* @param [in] MsgTaskPriority priority of dsp task
* @param [in] MsgTaskCpuBits cpu bits of dsp task
* @return none
*/
void SvcDSP_DspMsgTaskCreate(UINT32 MsgTaskPriority, UINT32 MsgTaskCpuBits)
{
    /* create dsp msg task */
    static UINT32           TaskCreated = 0U;
    static char             SvcDspMsgTskName[] = "SvcDspMsgTsk";
    static SVC_TASK_CTRL_s  DspMsgTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8            DspMsgTaskStack[DSP_MSG_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    UINT32  Rval;

    if (TaskCreated == 0U) {
        AmbaSvcWrap_MisraMemset(&SvcDspMsgTskFlag, 0, sizeof(SvcDspMsgTskFlag));
        Rval = AmbaKAL_EventFlagCreate(&SvcDspMsgTskFlag, SvcDspMsgTskName);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_DSP, "dsp msg task flag isn't created. ErrCode(0x%X)", Rval, 0U);
        } else {
            Rval = AmbaKAL_EventFlagClear(&SvcDspMsgTskFlag, 0xFFFFFFFFU);
            Rval |= AmbaKAL_EventFlagSet(&SvcDspMsgTskFlag, SVCDSP_TASK_READY);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_DSP, "dsp msg task flag set fail. ErrCode(0x%X)", Rval, 0U);
            }
        }

        DspMsgTaskCtrl.Priority   = MsgTaskPriority;
        DspMsgTaskCtrl.EntryFunc  = DspMsgTaskEntry;
        DspMsgTaskCtrl.pStackBase = DspMsgTaskStack;
        DspMsgTaskCtrl.StackSize  = DSP_MSG_TASK_STACK_SIZE;
        DspMsgTaskCtrl.CpuBits    = MsgTaskCpuBits;
        DspMsgTaskCtrl.EntryArg   = 0U;

        Rval = SvcTask_Create("SvcDspMsgTask", &DspMsgTaskCtrl);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_DSP, "dsp_msg_task isn't created", 0U, 0U);
        } else {
            TaskCreated = 1U;
        }
    } else {
        Rval = AmbaKAL_EventFlagSet(&SvcDspMsgTskFlag, SVCDSP_TASK_READY);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_DSP, "dsp msg task flag set fail! ErrCode(0x%X)", Rval, 0U);
        }
    }
}

/**
* boot dsp
* @param [in] pCfg configuration of dsp
* @param [in] MsgTaskPriority priority of dsp task
* @param [in] MsgTaskCpuBits cpu bits of dsp task
* @return 0-OK, 1-NG
*/
UINT32 SvcDSP_Boot(const AMBA_DSP_SYS_CONFIG_s *pCfg, UINT32 MsgTaskPriority, UINT32 MsgTaskCpuBits)
{
    static UINT8 IsRegErrEvtHdlr = 0U;
    UINT32      Rval;
    const void  *pLog;

    /* init dsp log */
    AmbaMisra_TypeCast(&pLog, &(pCfg->DebugLogDataAreaAddr));
    Rval = AmbaShell_CommandDspLogInit(pLog, pCfg->DebugLogDataAreaSize);
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_DSP, "## fail to configure dsp log", 0U, 0U);
    }

    if (OK == Rval) {
        if (IsRegErrEvtHdlr == 0U) {
            Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_ERROR, SvcDSP_ErrEventHdlr);
            if (Rval != 0U) {
                SvcLog_NG(SVC_LOG_DSP, "## fail to register dsp error event, rval(0x%X)", Rval, 0U);
            } else {
                IsRegErrEvtHdlr = 1U;
            }
        }

        SvcDSP_DspMsgTaskCreate(MsgTaskPriority, MsgTaskCpuBits);
    }

    if (OK == Rval) {
        /* boot dsp */
        Rval = AmbaDSP_MainInit(pCfg);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_DSP, "## fail to init dsp, return %u", Rval, 0U);
        } else {
            SvcDSP_Status = SVCDSP_STATUS_RUN;

            /* clear dsp alive flag */
            g_CleanBoot = SVCDSP_OP_DONTCARE;
        }
    }
    return Rval;
}

/**
* configuration of IDSP to VISORC message queue
* Not necessary for FlexiDAG
* @param [in] pCfg configuration of dsp
* @param [in] VpMsgBase message memory base of vpu
* @param [in] VpMSgSize message memory size of vpu
* @return none
*/
void SvcDSP_PackVecProcCfg(AMBA_DSP_SYS_CONFIG_s *pCfg, ULONG VpMsgBase, UINT32 VpMSgSize)
{
    if (0U < g_CleanBoot) {
    #if defined(CONFIG_THREADX) && !defined(CONFIG_THREADX64)
        extern UINT32 AmbaDSP_CalVpMsgBufferSize(const UINT32 *pNumMsgs, UINT32 *pMsgSize);

        static AMBA_DSP_MSG_BUFF_CTRL_s  g_VpMsgCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

        UINT32                    MsgSize, VpMsgNum;
        ULONG                     Addr;
        AMBA_DSP_MSG_BUFF_CTRL_s  *pCtrl = &g_VpMsgCtrl;

        AmbaSvcWrap_MisraMemset(pCtrl, 0, sizeof(AMBA_DSP_MSG_BUFF_CTRL_s));

        VpMsgNum = 1U;
        if (AmbaDSP_CalVpMsgBufferSize(&VpMsgNum, &MsgSize) == OK) {
            VpMsgNum = VpMSgSize / MsgSize;
        }
        pCtrl->MsgBufAddr  = (UINT32)VpMsgBase;
        pCtrl->MsgNum      = VpMsgNum;
        pCtrl->RP          = VpMsgNum - 1U;
        pCtrl->WP          = VpMsgNum - 1U;
        /* PureRtos flag */
        pCtrl->reserved[0] = 1U;

        AmbaMisra_TypeCast(&Addr, &pCtrl);
        pCfg->VpMsgCtrlBufAddr = Addr;
        if (SvcPlat_CacheClean(pCfg->VpMsgCtrlBufAddr, sizeof(AMBA_DSP_MSG_BUFF_CTRL_s)) != OK) {
            SvcLog_NG(SVC_LOG_DSP, "## something wrong with cache clean", 0U, 0U);
        }
    #else
        AmbaMisra_TouchUnused(pCfg);
        AmbaMisra_TouchUnused(&VpMsgBase);
        AmbaMisra_TouchUnused(&VpMSgSize);
    #endif
    }
}

/**
* size query of dsp frame buffer
* @param [in] pSetup description of dsp frame
* @param [out] pMemSize buffer size
* @return 0-OK, 1-NG
*/
UINT32 SvcDSP_QueryFrameBufSize(const SVC_DSP_FRAME_s *pSetup, UINT32 *pMemSize)
{
    UINT32 Rval = OK;
    UINT16 Pitch, RawWidth;
    if (pSetup->FrameType == SVC_FRAME_TYPE_RAW) {
        Rval = AmbaDSP_GetCmprRawBufInfo((UINT16)pSetup->FrameW, (UINT16)pSetup->RawCmpr, &RawWidth, &Pitch);
        if (Rval == OK) {
            *pMemSize = Pitch;
            if (pSetup->CeNeeded == 1U) {
                Rval = AmbaDSP_GetCmprRawBufInfo((UINT16)pSetup->FrameW >> 2U, (UINT16)pSetup->RawCmpr, &RawWidth, &Pitch); // for CE
                *pMemSize += Pitch;
            }
            *pMemSize *= (pSetup->FrameH * pSetup->FrameNum);
            SvcLog_OK(SVC_LOG_DSP, "[SvcDSP_QueryFrameBufSize] raw size %u", *pMemSize, 0);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_DSP, "[%u] AmbaDSP_GetCmprRawBufInfo failed, return %u", __LINE__, Rval);
                *pMemSize = 0;
            }
        } else {
            SvcLog_NG(SVC_LOG_DSP, "[%u] AmbaDSP_GetCmprRawBufInfo failed, return %u", __LINE__, Rval);
            *pMemSize = 0;
        }
    } else {
        *pMemSize = FRAME_ALIGN(64, pSetup->FrameW) * FRAME_ALIGN(32, pSetup->FrameH + 16U) * pSetup->FrameNum;
        *pMemSize = (*pMemSize * 3U) >> 1U; // 420
        SvcLog_OK(SVC_LOG_DSP, "[SvcDSP_QueryFrameBufSize] yuv size %u", *pMemSize, 0);
    }
    return Rval;
}

UINT32 SvcDSP_DataCapCtrl(UINT16 NumCapInstance, const UINT16 *pCapInstance, void *pDataCapCtrl, UINT64 *pAttachedRawSeq)
{
    static UINT32 SvcDataCapCtrlInit = 0U;
    static AMBA_KAL_MUTEX_t SvcDataCapCtrlMutx;
    static char SvcDataCapCtrlName[] = "SvcDataCapCtrl";

    UINT32 Rval = 0U;

    if (SvcDataCapCtrlInit == 0U) {
        AmbaSvcWrap_MisraMemset(&SvcDataCapCtrlMutx, 0, sizeof(SvcDataCapCtrlMutx));
        Rval = AmbaKAL_MutexCreate(&SvcDataCapCtrlMutx, SvcDataCapCtrlName);
        if (Rval != 0U) {
            SvcLog_NG(SVC_LOG_DSP, "[SvcDSP_DataCapCtrl] fail to create mutex. ErrCode(0x%08X)", Rval, 0);
        } else {
            SvcDataCapCtrlInit = 1U;
        }
    }

    Rval = AmbaKAL_MutexTake(&SvcDataCapCtrlMutx, 10000U);
    if (Rval != 0U) {
        SvcLog_NG(SVC_LOG_DSP, "[SvcDSP_DataCapCtrl] fail to take mutex. ErrCode(0x%08X)", Rval, 0);
    } else {
        AMBA_DSP_DATACAP_CTRL_s *pCtrl = NULL;

        AmbaMisra_TypeCast(&pCtrl, &pDataCapCtrl);

        Rval = AmbaDSP_DataCapCtrl(NumCapInstance, pCapInstance, pCtrl, pAttachedRawSeq);

        if (0U != AmbaKAL_MutexGive(&SvcDataCapCtrlMutx)) {
            SvcLog_NG(SVC_LOG_DSP, "[SvcDSP_DataCapCtrl] fail to give mutex. ErrCode(0x%08X)", Rval, 0);
        }
    }

    return Rval;
}

#ifdef CONFIG_ICAM_DSP_SUSPEND
UINT32 SvcDSP_Suspend(void)
{
    UINT32 Err, Rval = SVC_OK;

    if (SvcDSP_Status == SVCDSP_STATUS_RUN) {
        Err = AmbaDSP_MainSuspend();
        if (Err != DSP_ERR_NONE) {
            SvcLog_NG(SVC_LOG_DSP, "AmbaDSP_MainSuspend failed %u", Err, 0);
            Rval = SVC_NG;
        } else {
            SvcDSP_Status = SVCDSP_STATUS_SUSPEND;
        }
    } else {
        SvcLog_NG(SVC_LOG_DSP, "dsp is not running %u", SvcDSP_Status, 0);
        Rval = SVC_NG;
    }

    return Rval;
}

UINT32 SvcDSP_Resume(void)
{
    UINT32 Err, Rval = SVC_OK;

    if (SvcDSP_Status == SVCDSP_STATUS_SUSPEND) {
        Err = AmbaDSP_MainResume();
        if (Err != DSP_ERR_NONE) {
            SvcLog_NG(SVC_LOG_DSP, "AmbaDSP_MainResume failed %u", Err, 0);
            Rval = SVC_NG;
        } else {
            SvcDSP_Status = SVCDSP_STATUS_RUN;
        }
    } else {
        SvcLog_NG(SVC_LOG_DSP, "dsp is not in suspend mode %u", SvcDSP_Status, 0);
        Rval = SVC_NG;
    }

    return Rval;
}
#endif
