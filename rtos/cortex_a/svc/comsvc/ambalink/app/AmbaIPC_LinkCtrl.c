/**
 * @file AmbaIPC_LinkCtrl.c
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
 * @details LinkCtrl is a RPMSG device to cowork with Linux for handling LinkCtrl commands, e.g. reboot, hibernation.
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaWrap.h>
#include <AmbaPrint.h>
#include <AmbaLink.h>
#include <AmbaLinkInternal.h>
#include <AmbaLinkPrivate.h>
#include <AmbaSYS.h>

#include <AmbaIPC_LinkCtrl.h>

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
#include <AmbaMMU.h>
#include <AmbaCache.h>
#include <AmbaIPC_Hiber.h>
#endif /* CONFIG_AMBALINK_RPMSG_HIBER */

//#define ENABLE_LINK_CTRL_DEBUG_MSG  1
#if defined(ENABLE_LINK_CTRL_DEBUG_MSG)
#define LINK_CTRL_DEBUG_MSG 1
#else
#define LINK_CTRL_DEBUG_MSG 0
#endif


#define RPDEV_SUSP_DEV_NAME     "link_ctrl"

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
extern UINT64 LinuxKernelStart;
extern AMBA_KAL_EVENT_FLAG_t AmbaLinkEventFlag;
#endif /* CONFIG_AMBALINK_RPMSG_HIBER */

static AMBA_RPDEV_LINK_CTRL_OBJ_s Rpdev_LinkCtrlObj __attribute__((section(".bss.noinit")));

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
/* For IPC suspend task */
static AMBA_KAL_MSG_QUEUE_t IpcSuspendQueue __attribute__((section(".bss.noinit")));
static int IpcSuspendQueueBase[16] __attribute__((section(".bss.noinit")));
#endif /* CONFIG_AMBALINK_RPMSG_HIBER */


#ifdef CONFIG_AMBALINK_RPMSG_HIBER
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_LinkCtrl_RpmsgAck
 *
 *  @Description:: Ack Linux that ThreadX is done.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *               int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
INT32 AmbaIPC_LinkCtrl_RpmsgAck(void)
{
    AMBA_RPDEV_LINK_CTRL_CMD_s LinkCtrlCtrlCmd;

    if (AmbaWrap_memset(&LinkCtrlCtrlCmd, 0x0, sizeof(LinkCtrlCtrlCmd))!= 0U) { }
    LinkCtrlCtrlCmd.Cmd = LINK_CTRL_CMD_SUSPEND_ACK;

    return AmbaIPC_Send(Rpdev_LinkCtrlObj.Channel, &LinkCtrlCtrlCmd, sizeof(LinkCtrlCtrlCmd));
}
#endif /* CONFIG_AMBALINK_RPMSG_HIBER */

#if 0 // LinkCtrl Wrapper
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_LinkCtrl_SetWiFiConf
 *
 *  @Description:: Set /proc/ambarella/wifi.conf
 *
 *  @Input      :: none
 *              Buf       : content of wifi.conf
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *               int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int Rpdev_LinkCtrl_SetWiFiConf(const char *Buf)
{
    int len = strlen(Buf);
    LINK_CTRL_WIFI_CONF_s LinkCtrlWiFiCmd;

    if (len > (int)sizeof(LinkCtrlWiFiCmd.Buf)) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Err: wifi.conf size %d too big", len, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return -1;
    }

    LinkCtrlWiFiCmd.Cmd = LINK_CTRL_CMD_SET_WIFI_CONF;
    strcpy(LinkCtrlWiFiCmd.Buf, Buf);
    return AmbaIPC_Send(Rpdev_LinkCtrlObj.Channel, &LinkCtrlWiFiCmd, sizeof(LinkCtrlWiFiCmd.Cmd) + len + 1);
}
#endif // LinkCtrl Wrapper

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LinkCtrl_RcvMsgHandler
 *
 *  @Description:: Handler for recieve messages for AmbaRpdev_LinkCtrl Channel.
 *
 *  @Input      ::
 *              IpcHandle       : IPC channel.
 *              pMsgCtrl        : Pointer to message control data structure.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static INT32 LinkCtrl_RcvMsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    const AMBA_RPDEV_LINK_CTRL_CMD_s *pCtrl;
    pCtrl = (AMBA_RPDEV_LINK_CTRL_CMD_s *) pMsgCtrl->pMsgData;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TouchUnused(IpcHandle);
#endif

#if LINK_CTRL_DEBUG_MSG == 1
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "cmd = %d, addr = 0x%x, len = %u", pCtrl->Cmd, (UINT32)(ULONG)pMsgCtrl->pMsgData, pMsgCtrl->Length, 0U, 0U);
    AmbaPrint_Flush();
#endif
    switch (pCtrl->Cmd) {
#ifdef CONFIG_AMBALINK_RPMSG_HIBER
    case LINK_CTRL_CMD_SUSPEND_PREPARE:
        //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "LINK_CTRL_CMD_SUSPEND_PREPARE", NULL, NULL, NULL, NULL, NULL);
        //AmbaPrint_Flush();
        if (AmbaKAL_EventFlagSet(&AmbaLinkEventFlag, AMBALINK_SUSPEND_START) != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaKAL_EventFlagSet failed!(0x2)", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        AmbaIPC_HiberPrepare((void *)(ULONG) pCtrl->Param1);
        break;
    case LINK_CTRL_CMD_SUSPEND_DONE:
        //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "LINK_CTRL_CMD_SUSPEND_DONE", NULL, NULL, NULL, NULL, NULL);
        //AmbaPrint_Flush();
        LinuxKernelStart = pCtrl->Param2;
        AmbaKAL_MsgQueueSend(&IpcSuspendQueue, &pCtrl->Param1, AMBA_KAL_WAIT_FOREVER);
        break;
    case LINK_CTRL_CMD_SUSPEND_EXIT:
        if (AmbaKAL_EventFlagClear(&AmbaLinkEventFlag, AMBALINK_SUSPEND_START) != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaKAL_EventFlagClear failed!(0x2)", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }

        if (AmbaKAL_EventFlagSet(&AmbaLinkEventFlag, AMBALINK_BOOT_IPC_READY) != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaKAL_EventFlagSet failed!(0x2)", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }

        //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "LINK_CTRL_CMD_SUSPEND_EXIT", NULL, NULL, NULL, NULL, NULL);
        //AmbaPrint_Flush();
        break;
    case LINK_CTRL_CMD_GET_MEM_INFO:
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "LINK_CTRL_CMD_GET_MEM_INFO", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();

        do {
            UINT32 VirtAddr;
            extern int Amp_NetFifo_GetMemInfo(UINT64 type, UINT64 meminfo) __attribute__((weak));

            AmbaMMU_PhysToVirt(((ULONG)(pCtrl->Param2 & 0x00000000FFFFFFFF)), (ULONG *)&VirtAddr);

            if (Amp_NetFifo_GetMemInfo) {
                UINT64  intptr_VirtAddr = ((UINT64)((UINT32)VirtAddr) & 0x00000000FFFFFFFF);
                Amp_NetFifo_GetMemInfo(pCtrl->Param1, intptr_VirtAddr);
            } else {
                if (AmbaWrap_memset((void *)VirtAddr, 0, 32)!= 0U) { }
            }

            AmbaCache_DataClean((UINT32)VirtAddr, 32);
            AmbaIPC_LinkCtrl_RpmsgAck();
        } while(0);

        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "LINK_CTRL_CMD_GET_MEM_INFO", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        break;
#endif /* CONFIG_AMBALINK_RPMSG_HIBER */

    case (UINT32)LINK_CTRL_CMD_REBOOT:
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "LINK_CTRL_CMD_REBOOT", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        (void)AmbaSYS_Reboot();
        break;
    default:
        // no action
        break;
    }

    return 0;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_LinkCtrl_Init
 *
 *  @Description:: Initialize for LinkCtrl device.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_LinkCtrl_Init(void)
{
#ifdef CONFIG_AMBALINK_RPMSG_HIBER
    UINT32 err;
#endif

    AMBA_RPDEV_LINK_CTRL_OBJ_s *pLinkCtrlObj = &Rpdev_LinkCtrlObj;
    if (AmbaWrap_memset(pLinkCtrlObj, 0x0, sizeof(*pLinkCtrlObj))!= 0U) { }

    pLinkCtrlObj->pName = RPDEV_SUSP_DEV_NAME;
#ifdef CONFIG_OPENAMP
    pLinkCtrlObj->Channel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, pLinkCtrlObj->pName, LinkCtrl_RcvMsgHandler);
#else
    pLinkCtrlObj->Channel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, pLinkCtrlObj->pName, LinkCtrl_RcvMsgHandler);
#endif
    if (pLinkCtrlObj->Channel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    } else {
        if (AmbaIPC_RegisterChannel(pLinkCtrlObj->Channel, NULL)!= 0){ }

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
        if (AmbaWrap_memset(&IpcSuspendQueue, 0, sizeof(IpcSuspendQueue))!= 0U) { }
        if (AmbaWrap_memset(IpcSuspendQueueBase, 0, sizeof(IpcSuspendQueueBase))!= 0U) { }
        err = AmbaKAL_MsgQueueCreate(&IpcSuspendQueue, "IpcSuspendQueue", sizeof(INT32), IpcSuspendQueueBase, sizeof(IpcSuspendQueueBase));
        if (err != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create IpcSuspendQueue!", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        } else {
            err = AmbaIPC_HiberInit(pLinkCtrlObj, &IpcSuspendQueue);
            if (err != OK) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to AmbaIPC_HiberInit()!", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
            }
        }
#endif // CONFIG_AMBALINK_RPMSG_HIBER
    }
}


