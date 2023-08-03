/**
 * @file AmbaIPC_SD.c
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
 * @details SD controller rpdev driver.
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaCache.h"
#include "AmbaSD_Def.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaSD.h"

#include "AmbaIPC_SD.h"
#include "AmbaMMU.h"
#include "libwrapc.h"

#include "AmbaMisraFix.h"

/* FIXME */
#define SD_CMD8     8   /* SEND_EXT_CSD          adtc                    R1  */
#define SD_ACMD13   13  /* SD_STATUS             adtc                    R1  */
#define SD_ACMD51   51  /* SEND_SCR              adtc                    R1  */

static AMBA_IPC_HANDLE SD_Channel;
int Rpdev_SdInited = 0;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_SdRpmsgAck
 *
 *  @Description:: Ack Linux that ThreadX is done.
 *
 *  @Input      ::
 *          HostID  : Host ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_SdRpmsgAck(int HostID)
{
    AMBA_RPDEV_SD_MSG_s SdCtrlCmd;

    memset(&SdCtrlCmd, 0x0, sizeof(SdCtrlCmd));
    SdCtrlCmd.Cmd = SD_RPMSG_ACK;
    SdCtrlCmd.Param = (UINT32) HostID;

    return AmbaIPC_Send(SD_Channel, &SdCtrlCmd, sizeof(SdCtrlCmd));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_SdInfoGet
 *
 *  @Description:: Get the local SD card info and send it to Linux.
 *
 *  @Input      ::
 *          pMsg    : Received rpmsg.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_SdInfoGet(AMBA_RPDEV_SD_MSG_s *pMsg)
{
    UINT32 HostID;
    AMBA_SD_CARD_STATUS_s CardStatus;
    AmbaRpdev_SdInfo_s *pSdInfo;
    ULONG VirtAddr;
    UINT32 OpCondition;

    AmbaMMU_PhysToVirt((ULONG)pMsg->Param, &VirtAddr);
    pSdInfo = (AmbaRpdev_SdInfo_s *)VirtAddr;
    HostID = pSdInfo->HostID;
    AmbaSD_GetCardStatus(HostID, &CardStatus);

    pSdInfo->IsInit     = !!CardStatus.CardIsInit;
    pSdInfo->IsSdmem    = (CardStatus.SdType == AMBA_SD_MEMORY)  ? 1 : 0;
    pSdInfo->IsMmc      = (CardStatus.SdType == AMBA_SD_eMMC)    ? 1 : 0;
    pSdInfo->IsSdio     = (CardStatus.SdType == AMBA_SD_IO_ONLY) ? 1 : 0;
    pSdInfo->BusWidth   = (UINT16) AmbaSD_Ctrl[HostID].CardInfo.MemCardInfo.BusWidth;
    pSdInfo->Hcs        = (UINT16) AmbaSD_Ctrl[HostID].CardRegs.OpCondition.Ccs;
    pSdInfo->Rca        = (UINT32) AmbaSD_Ctrl[HostID].CardRegs.RelativeCardAddr;
    AmbaMisra_TypeCast32(&OpCondition, &AmbaSD_Ctrl[HostID].CardRegs.OpCondition);
    pSdInfo->Ocr        =  OpCondition;
    pSdInfo->Clk        = (UINT32) AmbaSD_Ctrl[HostID].CardInfo.MemCardInfo.CurrentClock;

    AmbaCache_DataClean((ULONG)pSdInfo, sizeof(AmbaRpdev_SdInfo_s));

    return Rpdev_SdRpmsgAck(pSdInfo->HostID);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_SdRespGet
 *
 *  @Description:: Get the local SD command response info and send it to Linux.
 *
 *  @Input      ::
 *          pMsg    : Received rpmsg.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_SdRespGet(AMBA_RPDEV_SD_MSG_s *pMsg)
{
    UINT32 HostID;
    AmbaRpdev_SdResp_s *pSdResp;
    AMBA_SD_INFO_REG_s  *pCardRegs;
    ULONG VirtAddr;

    AmbaMMU_PhysToVirt((ULONG)pMsg->Param, &VirtAddr);
    pSdResp = (AmbaRpdev_SdResp_s *)VirtAddr;
    pSdResp->Ret = -1;

    HostID = pSdResp->HostID;

    pCardRegs = &AmbaSD_Ctrl[HostID].CardRegs;

    pSdResp->Ret = AmbaSD_GetResponse(HostID, pSdResp->OpCode, pSdResp->Resp);

    if (pSdResp->OpCode == SD_ACMD13) {
        void *pBuf = (void *) &pCardRegs->SdStatus;
        memcpy(pSdResp->Buf, pBuf, sizeof(AMBA_SD_INFO_SSR_REG_s));
    } else if (pSdResp->OpCode == SD_ACMD51) {
        void *pBuf = (void *) &pCardRegs->SdConfig;
        memcpy(pSdResp->Buf, pBuf, sizeof(AMBA_SD_INFO_SCR_REG_s));
    } else if (pSdResp->OpCode == SD_CMD8) {
        void *pBuf = (void *) &pCardRegs->ExtCsd;
        memcpy(pSdResp->Buf, pBuf, sizeof(AMBA_SD_INFO_EXT_CSD_REG_s));
    }

    AmbaCache_DataClean((ULONG)pSdResp, sizeof(AmbaRpdev_SdResp_s));

    return Rpdev_SdRpmsgAck(pSdResp->HostID);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_SdInsertDetect
 *
 *  @Description::
 *
 *  @Input      ::
 *          pMsg    : Received rpmsg.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_SdInsertDetect(AMBA_RPDEV_SD_MSG_s *pMsg)
{
    UNUSED(pMsg);
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Rpdev_SdEjecttDetect
 *
 *  @Description::
 *
 *  @Input      ::
 *          pMsg    : Received rpmsg.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Rpdev_SdEjecttDetect(AMBA_RPDEV_SD_MSG_s *pMsg)
{
    UNUSED(pMsg);
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_SdDetectChange
 *
 *  @Description:: Send a message that SD card has some changes to Linux .
 *
 *  @Input      ::
 *          HostID  : Host ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *         int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_SdDetectChange(int HostID)
{
    AMBA_RPDEV_SD_MSG_s SdCtrlCmd;

    if (!Rpdev_SdInited)
        return 0;

    memset(&SdCtrlCmd, 0x0, sizeof(SdCtrlCmd));
    SdCtrlCmd.Cmd = SD_DETECT_CHANGE;
    SdCtrlCmd.Param = (UINT32) HostID;

    return AmbaIPC_Send(SD_Channel, &SdCtrlCmd, sizeof(SdCtrlCmd));
}

/*-----------------------------------------------------------------------------------------------*\
 *  Call back function array for MsgHandler.
\*-----------------------------------------------------------------------------------------------*/
typedef int (*PROC_FUNC)(AMBA_RPDEV_SD_MSG_s *);
static PROC_FUNC ProcList[] = {
    Rpdev_SdInfoGet,
    Rpdev_SdRespGet,
    Rpdev_SdInsertDetect,
    Rpdev_SdEjecttDetect,
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
    AMBA_RPDEV_SD_MSG_s *pMsg = pMsgCtrl->pMsgData;

    UNUSED(IpcHandle);
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "cmd = [%d], param = [0x%08x]", pMsg->Cmd, pMsg->Param, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    switch (pMsg->Cmd) {
    case SD_INFO_GET:
        return ProcList[0](pMsg);
    case SD_RESP_GET:
        return ProcList[1](pMsg);
    case SD_DETECT_INSERT:
        return ProcList[2](pMsg);
    case SD_DETECT_EJECT:
        return ProcList[3](pMsg);
    default:
        return -1;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_SdInit
 *
 *  @Description:: SD rpmsg channel initialization
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_SdInit(void)
{
#ifdef CONFIG_OPENAMP
    SD_Channel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "AmbaRpdev_SD", MsgHandler);
#else
    SD_Channel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "AmbaRpdev_SD", MsgHandler);
#endif
    AmbaIPC_RegisterChannel(SD_Channel, NULL);
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "AmaIPC registered channel [sd]", NULL, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();

    Rpdev_SdInited = 1;
}
