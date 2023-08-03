/**
 * @file AmbaIPC_Clock.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details Clock rpdev driver.
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaCache.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_Clock.h"
#include "AmbaMMU.h"
#include "libwrapc.h"

#ifdef AMBAIPC_CLK_DEBUG
#define IPC_CLK_MSG     1
#else
#define IPC_CLK_MSG     0
#endif

static AMBA_IPC_HANDLE Clock_Channel;
static AMBA_KAL_EVENT_FLAG_t ClockFlag;
int Rpdev_ClockInited = 0;
extern AMBA_KAL_EVENT_FLAG_t AmbaLinkEventFlag;

/*-----------------------------------------------------------------------------------------------*\
 *  Call back function array for Rpdev_ClockGet.
\*-----------------------------------------------------------------------------------------------*/
typedef UINT32 (*CLK_GET_FUNC)(void);
static CLK_GET_FUNC ClkGetList[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

/*-----------------------------------------------------------------------------------------------*\
 *  Call back function array for Rpdev_ClockSet.
\*-----------------------------------------------------------------------------------------------*/
typedef int (*CLK_SET_FUNC)(UINT32);
static CLK_SET_FUNC ClkSetList[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_ClockRpmsgAckLinux
 *
 *  @Description:: Ack Linux that ThreadX is done.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_ClockRpmsgAckLinux(void)
{
    AMBA_RPDEV_CLK_MSG_s ClkCtrlCmd;

    memset(&ClkCtrlCmd, 0x0, sizeof(ClkCtrlCmd));
    ClkCtrlCmd.Cmd = CLK_RPMSG_ACK_LINUX;

    return AmbaIPC_Send(Clock_Channel, &ClkCtrlCmd, sizeof(ClkCtrlCmd));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_ClockRpmsgAckThreadX
 *
 *  @Description:: Ack ThreadX that Linux is done.
 *
 *  @Input      ::
 *          pMsg    : Received rpmsg.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       int : OK(0)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_ClockRpmsgAckThreadX(AMBA_RPDEV_CLK_MSG_s *pMsg)
{
    UNUSED(pMsg);
    AmbaKAL_EventFlagSet(&ClockFlag, 0x1);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_ClockGet
 *
 *  @Description:: Get the clock and send it to Linux.
 *
 *  @Input      ::
 *          pMsg    : Received rpmsg.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_ClockGet(AMBA_RPDEV_CLK_MSG_s *pMsg)
{
    AmbaRpdev_Clock_s *pClk;
    ULONG VirtAddr;

    AmbaMMU_PhysToVirt((ULONG)pMsg->Param, &VirtAddr);
    pClk = (AmbaRpdev_Clock_s *)VirtAddr;
    if(IPC_CLK_MSG) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "get %d clock!\n", pClk->ClockIndex, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    if (ClkGetList[pClk->ClockIndex] != NULL) {
        pClk->Rate = ClkGetList[pClk->ClockIndex]();
    } else {
        pClk->Rate = 0;
    }

    AmbaCache_DataClean((ULONG)pClk, sizeof(AmbaRpdev_Clock_s));

    return Rpdev_ClockRpmsgAckLinux();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_ClockSet
 *
 *  @Description:: Set clock and ack Linux.
 *
 *  @Input      ::
 *          pMsg    : Received rpmsg.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_ClockSet(AMBA_RPDEV_CLK_MSG_s *pMsg)
{
    AmbaRpdev_Clock_s *pClk;
    ULONG VirtAddr;

    AmbaMMU_PhysToVirt((ULONG)pMsg->Param, &VirtAddr);
    pClk = (AmbaRpdev_Clock_s *)VirtAddr;
    if(IPC_CLK_MSG) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "set %d clock!\n", pClk->ClockIndex, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    if (pClk->ClockIndex == AMBALINK_CLK_AHB || pClk->ClockIndex == AMBALINK_CLK_APB ||
        pClk->ClockIndex == AMBALINK_CLK_AXI) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "set %d clock is not allowed!\n", pClk->ClockIndex, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    } else {
        if (ClkSetList[pClk->ClockIndex] != NULL) {
            if (ClkSetList[pClk->ClockIndex](pClk->Rate) < 0) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "set %d clock failed!\n", pClk->ClockIndex, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
        }
    }

    return Rpdev_ClockRpmsgAckLinux();
}

/*-----------------------------------------------------------------------------------------------*\
 *  Call back function array for MsgHandler.
\*-----------------------------------------------------------------------------------------------*/
typedef int (*CLK_PROC_FUNC)(AMBA_RPDEV_CLK_MSG_s *);
static CLK_PROC_FUNC ProcList[] = {
    Rpdev_ClockGet,
    Rpdev_ClockSet,
    Rpdev_ClockRpmsgAckThreadX,
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MsgHandler
 *
 *  @Description:: Channel message handler
 *
 *  @Input      ::
 *          IpcHandle   : IPC channel
 *          pMsgCtrl    : Pointer to message control data structure.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    AMBA_RPDEV_CLK_MSG_s *pMsg = pMsgCtrl->pMsgData;

    UNUSED(IpcHandle);
    if(IPC_CLK_MSG) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "cmd = [%d], param = [0x%08x]", pMsg->Cmd, pMsg->Param, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    switch (pMsg->Cmd) {
    case CLK_GET:
        return ProcList[0](pMsg);
    case CLK_SET:
        return ProcList[1](pMsg);
    case CLK_RPMSG_ACK_THREADX:
        return ProcList[2](pMsg);
    default:
        return -1;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_ClockChangedPreNotify
 *
 *  @Description:: Pre-Notify Linux that clock will be changed.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *         int : OK(0)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_ClockChangedPreNotify(void)
{
    UINT32 Dummy;
    AMBA_RPDEV_CLK_MSG_s ClkCtrlCmd;
    int Status;
    UINT32 ActFlg;

    Status = AmbaKAL_EventFlagGet(&AmbaLinkEventFlag, AMBALINK_BOOT_IPC_READY,
                                   1U, 0U, &ActFlg, 0);

    if (!Rpdev_ClockInited || (Status != OK)) {
        if(IPC_CLK_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: rpmsg is not ready!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        return 0;
    }

    memset(&ClkCtrlCmd, 0x0, sizeof(ClkCtrlCmd));
    ClkCtrlCmd.Cmd = CLK_CHANGED_PRE_NOTIFY;

    AmbaIPC_Send(Clock_Channel, &ClkCtrlCmd, sizeof(ClkCtrlCmd));

    /*
     * We will disable BOSS in Linux (gidle = 1 and state = suspended).
     * This is used to prevent Linux is scheduled
     * while timer is suspended during clock is changing.
     */

    AmbaKAL_EventFlagGet(&ClockFlag, 0x1, 1U, 1U, &Dummy,
                          AMBA_KAL_WAIT_FOREVER);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_ClockChangedPostNotify
 *
 *  @Description:: Post-Notify Linux that clock is changed.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        int : OK(0)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_ClockChangedPostNotify(void)
{
    UINT32 Dummy;
    AMBA_RPDEV_CLK_MSG_s ClkCtrlCmd;
    int Status;
    UINT32 ActFlg;

    Status = AmbaKAL_EventFlagGet(&AmbaLinkEventFlag, AMBALINK_BOOT_IPC_READY,
                                   1U, 0U, &ActFlg, 0);

    if (!Rpdev_ClockInited || (Status != OK)) {
        if(IPC_CLK_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: rpmsg is not ready!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        return 0;
    }

    memset(&ClkCtrlCmd, 0x0, sizeof(ClkCtrlCmd));
    ClkCtrlCmd.Cmd = CLK_CHANGED_POST_NOTIFY;

    AmbaIPC_Send(Clock_Channel, &ClkCtrlCmd, sizeof(ClkCtrlCmd));

    AmbaKAL_EventFlagGet(&ClockFlag, 0x1, 1U, 1U, &Dummy,
                          AMBA_KAL_WAIT_FOREVER);

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_ClockInit
 *
 *  @Description:: Clock rpmsg channel initialization
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_ClockInit(void)
{
#ifdef CONFIG_OPENAMP
    Clock_Channel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "AmbaRpdev_CLK", MsgHandler);
#else
    Clock_Channel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "AmbaRpdev_CLK", MsgHandler);
#endif
    if (Clock_Channel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }

    AmbaIPC_RegisterChannel(Clock_Channel, NULL);
    if(IPC_CLK_MSG) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "AmaIPC registered channel [clock]", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    AmbaKAL_EventFlagCreate(&ClockFlag,"ClockFlag");

    Rpdev_ClockInited = 1;
}
