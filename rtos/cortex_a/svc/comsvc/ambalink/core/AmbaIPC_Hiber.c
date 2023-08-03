/**
 * @file AmbaIPC_Hiber.c
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
 * @details Rpdev_Hiber is a rpmsg device to cowork with Linux to handle hibernation.
 *
 */
#include "AmbaIntrinsics.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaCache.h"
#include "AmbaNAND.h"
#include "AmbaNAND_OP.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaSD.h"
#include "AmbaSD_STD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaMMU.h"
#include "AmbaRTSL_NAND_BBM.h"
#include "AmbaSYS.h"
#include <AmbaWrap.h>

#include <AmbaIPC_LinkCtrl.h>
#include "AmbaIPC_Hiber.h"

//#define ENABLE_LINK_CTRL_DEBUG_MSG  1
#if defined(ENABLE_LINK_CTRL_DEBUG_MSG)
#define LINK_CTRL_DEBUG_MSG 1
#else
#define LINK_CTRL_DEBUG_MSG 0
#endif

static UINT64 HiberRpmsgSuspendBackupSize = 0u;
#define SET_RPMSG_SUSPEND_BACKUP_SIZE(Size)   (HiberRpmsgSuspendBackupSize += Size)
#define WriteWord(Addr, d)                    (*((volatile UINT32 *) (Addr)) = (d))
#define ReadWord(Addr)                        (*((volatile UINT32 *) (Addr)))
#define GET_RPMSG_SUSPEND_BACKUP_ADDR()       (AmbaLinkCtrl.RpmsgSuspBackupAddr + HiberRpmsgSuspendBackupSize)
#define GET_RPMSG_SUSPEND_BACKUP_SIZE()       (HiberRpmsgSuspendBackupSize)
#define LINUX_IS_HIBER_MODE()                 (LinuxSuspendMode == AMBA_LINK_SUSPEND_TO_DISK)

extern UINT64 LinuxKernelStart;
extern AMBA_KAL_EVENT_FLAG_t AmbaLinkEventFlag;
extern AMBA_LINK_CTRL_s AmbaLinkCtrl;

static AMBA_RPDEV_LINK_CTRL_OBJ_s *pLinkCtrlObj = NULL;
static AMBA_KAL_TASK_t HiberSuspendTask __attribute__((section(".bss.noinit")));

void (*AmbaIPC_LinkCtrlSuspendCallBack)(UINT32 m) = NULL;
UINT32 LinuxSuspendMode = 999u;

/* For IPC suspend task */
static AMBA_KAL_MSG_QUEUE_t *IpcSuspendQueue = NULL;
static UINT8 HiberStack[0x4000] GNU_SECTION_NOZEROINIT;

AMBA_IPC_HIBER_CTRL_s _Hiber_Ctrl;


void AmbaIPC_HiberResetSuspendBackupSize(void)
{
    HiberRpmsgSuspendBackupSize = 0u;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_LinkCtrlSuspendLinux
 *
 *  @Description:: suspend Linux according to SuspendMode.
 *
 *  @Input      ::
 *              SuspendMode     : The value of SuspendMode.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_LinkCtrlSuspendLinux(UINT32 SuspendMode)
{
    AMBA_RPDEV_LINK_CTRL_CMD_s LinkCtrlCtrlCmd;
    extern int Rpdev_ClockInited;

    /* Disable Rpdev_Clock if linux will be suspended. */
    Rpdev_ClockInited = 0;
    if (AmbaKAL_EventFlagClear(&AmbaLinkEventFlag, AMBALINK_BOOT_IPC_READY) != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaKAL_EventFlagClear failed!(0x2)", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    if (AmbaWrap_memset(&LinkCtrlCtrlCmd, 0x0, sizeof(LinkCtrlCtrlCmd))!= 0U) { }
    LinkCtrlCtrlCmd.Cmd = LINK_CTRL_CMD_SUSPEND;
    LinkCtrlCtrlCmd.Param1 = SuspendMode;
    LinuxSuspendMode = SuspendMode;

    return AmbaIPC_Send(pLinkCtrlObj->Channel, &LinkCtrlCtrlCmd, sizeof(LinkCtrlCtrlCmd));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_LinkCtrlWaitSuspend
 *
 *  @Description:: Return if Linux is suspended.
 *
 *  @Input      ::
 *      TimeOutMs:   event flag time out in mini-seconds.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(<0)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_LinkCtrlWaitSuspend(UINT32 TimeOutMs)
{
    int Status;
    UINT32 ActFlg;

    Status = AmbaKAL_EventFlagGet(&AmbaLinkEventFlag, AMBALINK_SUSPEND_DONE,
                                   1U, 1U, &ActFlg, TimeOutMs);
    if (Status) {
        AmbaIPC_LinkCtrl_RpmsgAck();
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagGet failed!(%d)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return Status;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_LinkCtrlResumeLinux
 *
 *  @Description::  Resume Linux according to SuspendMode.
 *
 *  @Input      ::
 *              SuspendMode     : The value of SuspendMode.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_LinkCtrlResumeLinux(UINT32 SuspendMode)
{
    int Rval = -1;
    extern int Rpdev_ClockInited;

    if (SuspendMode == AMBA_LINK_SUSPEND_TO_DISK) {
        /* Load hibernation image from storage. */
        Rval = AmbaIPC_HiberResume(0);

        /* Wake up remote CPU */
        AmbaIPC_HiberWakeupRemote();

        /* Ack Linux by Rpmsg */
        AmbaIPC_HiberRestoreRpmsgFinish();
    } else if (SuspendMode <= AMBA_LINK_SUSPEND_TO_RAM) {
        Rval = AmbaIPC_HiberReturn(SuspendMode);

        /* No need to restore rpmsg info if return from dram. */
        /* Wait a while so linux can wait for a rpmsg ack. */
        AmbaKAL_TaskSleep(300);

        /* Ack Linux by Rpmsg */
        AmbaIPC_HiberRestoreRpmsgFinish();
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "invalid argument %d", SuspendMode, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    /* Re-enable Rpdev_Clock if linux is resumed. */
    Rpdev_ClockInited = 1;

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_LinkCtrlWaitResumeLinux
 *
 *  @Description:: Return if Linux is suspended.
 *
 *  @Input      ::
 *      TimeOutMs:   event flag time out in mini-seconds.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(<0)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_LinkCtrlWaitResumeLinux(UINT32 TimeOutMs)
{
    int Status;
    UINT32 ActFlg;

    Status = AmbaKAL_EventFlagGet(&AmbaLinkEventFlag, AMBALINK_BOOT_IPC_READY,
                                   1U, 0U, &ActFlg, TimeOutMs);
    if (Status) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagGet failed!(%d)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return Status;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IpcHiberSuspendHandler
 *
 *  @Description:: Handler for suspend and hibernation service.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void *IpcHiberSuspendHandler(void *Param)
{
    UINT32 Msg;

    UNUSED(Param);
    while (1) {
        if (AmbaKAL_MsgQueueReceive(IpcSuspendQueue, &Msg, AMBA_KAL_WAIT_FOREVER))
            continue;

        AmbaIPC_HiberSuspend(Msg);
    }

    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 * Definition for Hiber.
\*-----------------------------------------------------------------------------------------------*/

//#define ENABLE_HIBER_DEBUG_MSG  1
#if defined(ENABLE_HIBER_DEBUG_MSG)
#define HIBER_DEBUG_MSG     1
#else
#define HIBER_DEBUG_MSG     0
#endif

static AMBA_HIBER_s AmbaHiber;
static AMBA_HIBER_s *pAmbaHiber = &AmbaHiber;
static UINT32 RpmsgInfoBlk;
static UINT32 CurrentPage = 0;

int (*AmbaHiber_InitCallBack)(void) = NULL;
extern int AmbaLink_ReadPartitionInfo(int, UINT32, AMBA_PARTITION_ENTRY_s *);
extern int AmbaLink_ErasePartition(int PartFlag, UINT32 PartID, UINT32 TimeOut);

/*-----------------------------------------------------------------------------------------------*\
 * Internal functions.
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_Valid
 *
 *  @Description:: Check hibernation image is valid or not.
 *
 *  @Input      ::
 *              pAossInfo       : Pointer to AOSS information data structure.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_Valid(AMBA_HIBER_AOSS_INFO_u *pAossInfo)
{
    int Rval = 0;

    if (pAossInfo->Full.Magic != AMBA_HIBER_AOSS_MAGIC) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "wrong magic 0x%08x", pAossInfo->Full.Magic, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        Rval = -1;
        goto Hiber_Valid_Exit;
    }

    if (pAossInfo->Full.CopyPages >= pAossInfo->Full.TotalPages) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "too many pages [0x%08x:0x%08x]", pAossInfo->Full.CopyPages, pAossInfo->Full.TotalPages, 0U, 0U, 0U);
        AmbaPrint_Flush();
        Rval = -1;
        goto Hiber_Valid_Exit;
    }

#if 0
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAossInfo->Full.Param.Data[0] = 0x%x", pAossInfo->Full.Param.Data[0], 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAossInfo->Full.Param.Data[1] = 0x%x", pAossInfo->Full.Param.Data[1], 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAossInfo->Full.Param.Data[2] = 0x%x", pAossInfo->Full.Param.Data[2], 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAossInfo->Full.Param.Data[3] = 0x%x", pAossInfo->Full.Param.Data[3], 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAossInfo->Full.Magic] = 0x%x",        pAossInfo->Full.Magic, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAossInfo->Full.CopyPages = 0x%x",     pAossInfo->Full.CopyPages, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAossInfo->Full.TotalPages = 0x%x",    pAossInfo->Full.TotalPages, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
#endif

Hiber_Valid_Exit:

    return Rval;
}

static int Hiber_NandWriteHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                             AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0, OpPages;

    OpPages = AossSize / pPartInfo->MainSize;

    if(OpPages > (pPartInfo->PagesPerBlock / 2)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "OpPages(%d) > (pPartInfo->PagesPerBlock / 2)(%d)", OpPages, (pPartInfo->PagesPerBlock / 2), 0U, 0U, 0U);
        AmbaPrint_Flush();
        return -1;
    }

    for (Block = pPartInfo->StartBlock; Block < pPartInfo->EndBlock; Block++) {
        if (AmbaNAND_GetBlkMark(Block) != AMBA_NAND_BLK_GOOD) {
            continue;
        }

        Rval = AmbaNandOp_EraseBlock(Block, 5000);
        if (Rval != OK) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_EraseBlock failed. <Block %d>\n", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Try next Block...\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            continue;
        }

        Rval = AmbaNandOp_Program(Block * pPartInfo->PagesPerBlock, OpPages,
                                    (UINT8 *) pAossInfo, NULL, 5000);
        if (Rval != OK) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Program failed. <Block %d>", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            continue;
        } else {
            break;
        }
    }
    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_NandWriteDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                               AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{

    int Rval = 0;
    UINT32 Page, OpPages, PrePageIndex;
    UINT32 Block = 0;
    UINT32 PrePageOffset, PageOffset = 0;
    UINT32 PageIndex = 0;
    UINT32 TotalSize = 0;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    UINT32 VirtAddr;

    UNUSED(pAossInfo);
    UNUSED(AossSize);
    /* Write the data to the next block of header and header2 block. */
    Block = DataStartBlk + 1;
    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    for (; Block < pPartInfo->EndBlock; Block++) {
        if (AmbaNAND_GetBlkMark(Block) != AMBA_NAND_BLK_GOOD) {
            continue;
        }

        Rval = AmbaNandOp_EraseBlock(Block, 5000);
        if (Rval != OK) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_EraseBlock failed. <Block %d>\n", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Try next Block...\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            continue;
        }

        PrePageOffset = PageOffset;
        PrePageIndex = PageIndex;
        OpPages = (pPageInfo[PageIndex].Size - PageOffset) / pPartInfo->MainSize;

        if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
            OpPages++;

        if (OpPages > pPartInfo->PagesPerBlock)
            OpPages = pPartInfo->PagesPerBlock;

        for (Page = 0; Page < pPartInfo->PagesPerBlock;) {
            UINT32 PageAddr = (Block * pPartInfo->PagesPerBlock) + Page;
            Rval = AmbaNandOp_Program(PageAddr, OpPages,
                        (UINT8 *)(ULONG) (pPageInfo[PageIndex].Dst + PageOffset),
                        NULL, 5000);

            if (Rval != OK) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Program failed. <Block %d, Page %d, Pages %d>", Block, Page, OpPages, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            PageOffset += OpPages * pPartInfo->MainSize;
            Page += OpPages;

            if (PageOffset >= pPageInfo[PageIndex].Size) {
                TotalSize += pPageInfo[PageIndex].Size;
                if(LINK_CTRL_DEBUG_MSG) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Write 0x%08x, size 0x%08x.", pPageInfo[PageIndex].Dst, pPageInfo[PageIndex].Size, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                }
                PageIndex++;

                if (PageIndex > pAossInfo->Full.CopyPages)
                    goto Hiber_WriteNandDataPage_Exit;

                PageOffset = 0;
                OpPages = pPageInfo[PageIndex].Size / pPartInfo->MainSize;

                if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
                    OpPages++;

                if ((Page + OpPages) > pPartInfo->PagesPerBlock)
                    OpPages = pPartInfo->PagesPerBlock - Page;
            }
        }

        if (Rval < 0) {
            PageOffset = PrePageOffset;
            PageIndex = PrePageIndex;
            continue;
        }
    }
Hiber_WriteNandDataPage_Exit:

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

    if (Rval == 0) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Write %d bytes to block %d:%d.", TotalSize, DataStartBlk, Block, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_NandWriteHeader2(UINT32 HeaderBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0;
    UINT32 PageAddr;
    UINT32 OpPages = AossSize / pPartInfo->MainSize;

    if(OpPages > (pPartInfo->PagesPerBlock / 2)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "OpPages(%d) > (pPartInfo->PagesPerBlock / 2)(%d)", OpPages, (pPartInfo->PagesPerBlock / 2), 0U, 0U, 0U);
        AmbaPrint_Flush();
        return -1;
    }

    PageAddr = (HeaderBlk * pPartInfo->PagesPerBlock) + OpPages;

    if (AmbaNAND_GetBlkMark(HeaderBlk) != AMBA_NAND_BLK_GOOD) {
        /* Should not be here! */
        goto Hiber_NandWriteHeader2_Exit;
    }

    Rval = AmbaNandOp_Program(PageAddr, OpPages, (UINT8 *) pAossInfo, NULL, 5000);
    if (Rval != OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Program failed. <Block %d>", Block, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        AmbaNandOp_EraseBlock(HeaderBlk, 5000);
    }

Hiber_NandWriteHeader2_Exit:

    return Rval;
}
static int Hiber_SaveNandRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                               AMBA_HIBER_PART_INFO_s *pPartInfo,
                               UINT8 *pData, UINT32 Size)
{
    int Rval = 0;
    UINT32 Page, Pages;

    UNUSED(pAossInfo);
    Page = 0;
    Pages = Size / pPartInfo->MainSize;

    if (Size % pPartInfo->MainSize)
        Pages++;

    for (; RpmsgInfoBlk < pPartInfo->EndBlock; RpmsgInfoBlk++) {
        if (CurrentPage == 0) {
            if (AmbaNAND_GetBlkMark(RpmsgInfoBlk) != AMBA_NAND_BLK_GOOD) {
                continue;
            }
            Rval = AmbaNandOp_EraseBlock(RpmsgInfoBlk, 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_EraseBlock failed. <Block %d>", RpmsgInfoBlk, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Try next Block...", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
                continue;
            }
        }

        for (; Page < Pages; Page++) {
            UINT32 PageAddr = (RpmsgInfoBlk * pPartInfo->PagesPerBlock) + CurrentPage;
            Rval = AmbaNandOp_Program(PageAddr, 1, pData, NULL, 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Program failed. <Block %d, Page %d>", RpmsgInfoBlk, CurrentPage, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            pData += pPartInfo->MainSize;

            if (++CurrentPage == pPartInfo->PagesPerBlock) {
                CurrentPage = 0;
                break;
            }
        }
        if (Page == Pages)
            break;
    }

    if (Page != Pages) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: not enough space!!.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

Done:

    return Rval;
}

static int Hiber_NandReadHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                            AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0;
    UINT32 OpPages;

    OpPages = AossSize / pPartInfo->MainSize;
    if(OpPages > (pPartInfo->PagesPerBlock / 2)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "OpPages(%d) > (pPartInfo->PagesPerBlock / 2)(%d)", OpPages, (pPartInfo->PagesPerBlock / 2), 0U, 0U, 0U);
        AmbaPrint_Flush();
        return -1;
    }

    for (Block = pPartInfo->StartBlock; Block < pPartInfo->EndBlock; Block++) {
        UINT32 PageAddr = (Block * pPartInfo->PagesPerBlock) + 0;
        if (AmbaNAND_GetBlkMark(Block) != AMBA_NAND_BLK_GOOD) {
            continue;
        }

        Rval = AmbaNandOp_Read(PageAddr, OpPages, (UINT8 *) pAossInfo, NULL, 5000);
        if (Rval != OK) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Read failed. <Block: %d>", Block, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            continue;
        } else {
            break;
        }
    }

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_NandReadHeader2(AMBA_HIBER_PART_INFO_s *pPartInfo,
                            AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 Block, UINT32 OpPages)
{
    int Rval;
    UINT32 PageAddr = (Block * pPartInfo->PagesPerBlock) + OpPages;
    Rval = AmbaNandOp_Read(PageAddr, OpPages, (UINT8 *) pAossInfo, NULL, 5000);
    if (Rval != OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Read failed. <Block %d>", Block, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        goto HiberNand_ReadHeader2_Exit;
    }

HiberNand_ReadHeader2_Exit:

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_CheckNandReadHiberImgCrc(AMBA_HIBER_PART_INFO_s *pPartInfo,
                                AMBA_HIBER_AOSS_INFO_u *pAossInfo)
{
    AMBA_NAND_COMMON_INFO_s *pAmbaNAND_DevInfo = AmbaNAND_GetCommonInfo();
    int Rval;
    UINT32 Block = pAossInfo->Full.Param.HiberInfo.HeaderBlkNum;
    UINT32 Page, OpPages, BufSize, RemainSize, AddCrcSize;
    UINT32 ImgCrc = AMBA_CRC32_INIT_VALUE;
    void *pBuf;

    BufSize = 0x8000;
    RemainSize = pAossInfo->Full.Param.HiberInfo.ImgSize;
    pBuf = (UINT8 *) AmbaLink_Malloc(MEM_POOL_CRC_BUF, BufSize);
    OpPages = BufSize / pPartInfo->MainSize;
    /* Move to data block. */
    Block += 1;
    for (; Block < pPartInfo->EndBlock; Block++) {
        if (AmbaNAND_GetBlkMark(Block) != AMBA_NAND_BLK_GOOD) {
            continue;
        }

        for (Page = 0; Page < pPartInfo->PagesPerBlock; Page += OpPages) {
            UINT32 PageAddr = (Block * pAmbaNAND_DevInfo->BlockPageSize) + Page;
            Rval = AmbaNandOp_Read(PageAddr, OpPages, pBuf, NULL, 5000);
            if (Rval != OK) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Read failed. <Block %d, Page %d, Pages %d>", Block, Page, OpPages, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            if (RemainSize >= BufSize)
                AddCrcSize = BufSize;
            else
                AddCrcSize = RemainSize;

            RemainSize -= AddCrcSize;

            ImgCrc = AmbaUtility_Crc32Add((const void *) pBuf, AddCrcSize, ImgCrc);
            if (RemainSize == 0)
                goto Done;
        }
    }
Done:
    AmbaLink_Free(MEM_POOL_CRC_BUF,pBuf);
    return ImgCrc;
}

static int Hiber_NandReadDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Page, OpPages, PrePageIndex;
    UINT32 Block = DataStartBlk;
    UINT32 PageOffset = 0, PrePageOffset;
    UINT32 PageIndex = 0;
    UINT32 TotalSize = 0;
    UINT8 *pTmp, *pDst;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    UINT32 VirtAddr;

    UNUSED(AossSize);
    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    pTmp = (UINT8 *) AmbaLink_Malloc(MEM_POOL_NAND_TMP_BUF, pPartInfo->MainSize);

    /* Read the data from the next block of header and header2 block. */
    Block = DataStartBlk + 1;

    for (; Block < pPartInfo->EndBlock; Block++) {
        if (AmbaNAND_GetBlkMark(Block) != AMBA_NAND_BLK_GOOD) {
            continue;
        }

        PrePageOffset = PageOffset;
        PrePageIndex = PageIndex;
        OpPages = (pPageInfo[PageIndex].Size - PageOffset) / pPartInfo->MainSize;

        if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
            OpPages++;

        if (OpPages > pPartInfo->PagesPerBlock)
            OpPages = pPartInfo->PagesPerBlock;

        for (Page = 0; Page < pPartInfo->PagesPerBlock;) {
            UINT32 PageAddr = (Block * pPartInfo->PagesPerBlock) + Page;
            if (OpPages == 1 && pPageInfo[PageIndex].Size < pPartInfo->MainSize) {
                pDst = pTmp;
            } else {
                pDst = (UINT8 *)(ULONG)(pPageInfo[PageIndex].Src + PageOffset);
            }

            Rval = AmbaNandOp_Read(PageAddr, OpPages, pDst, NULL, 5000);
            if(LINK_CTRL_DEBUG_MSG) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read %d pages at block %d:%d into 0x%08x/0x%08x",
                                        OpPages, Block, Page,
                                        (UINT32)(pPageInfo[PageIndex].Dst + PageOffset),
                                        (UINT32)(pPageInfo[PageIndex].Src + PageOffset));
                AmbaPrint_Flush();
            }
            if (Rval != OK) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Read failed. <Block %d, Page %d, Pages %d>", Block, Page, OpPages, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            if (OpPages == 1 && pPageInfo[PageIndex].Size < pPartInfo->MainSize) {
                (void)AmbaWrap_memcpy((UINT8 *)(ULONG)(pPageInfo[PageIndex].Src + PageOffset),
                    pDst, pPageInfo[PageIndex].Size);
            }

            PageOffset += OpPages * pPartInfo->MainSize;
            Page += OpPages;

            if (PageOffset >= pPageInfo[PageIndex].Size) {
                TotalSize += pPageInfo[PageIndex].Size;
                if(LINK_CTRL_DEBUG_MSG) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read 0x%08x, size 0x%08x.", pPageInfo[PageIndex].Size, pPageInfo[PageIndex].Size, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                }
                PageIndex++;
                if (PageIndex > pAossInfo->Full.CopyPages)
                    goto Hiber_ReadNandDataPage_Exit;

                PageOffset = 0;
                OpPages = pPageInfo[PageIndex].Size / pPartInfo->MainSize;

                if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
                    OpPages++;

                if ((Page + OpPages) > pPartInfo->PagesPerBlock)
                    OpPages = pPartInfo->PagesPerBlock - Page;
            }
        }

        if (Rval < 0) {
            PageOffset = PrePageOffset;
            PageIndex = PrePageIndex;
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Try next Block...", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }
Hiber_ReadNandDataPage_Exit:

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

    if (Rval == 0) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read %d bytes from block %d:%d.", TotalSize, DataStartBlk, Block, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    AmbaLink_Free(MEM_POOL_NAND_TMP_BUF, pTmp);

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_RestoreNandRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                                  AMBA_HIBER_PART_INFO_s *pPartInfo,
                                  UINT8 *pData, UINT32 Size)
{
    int Rval = 0;
    UINT32 Page, Pages;

    UNUSED(pAossInfo);
    Page = 0;
    Pages = Size / pPartInfo->MainSize;
    if (Size % pPartInfo->MainSize)
        Pages++;

    for (; RpmsgInfoBlk < pPartInfo->EndBlock; RpmsgInfoBlk++) {
        if (CurrentPage == 0) {
            if (AmbaNAND_GetBlkMark(RpmsgInfoBlk) != AMBA_NAND_BLK_GOOD) {
                continue;
            }
        }

        for (; Page < Pages; Page++) {
            UINT32 PageAddr = (RpmsgInfoBlk * pPartInfo->PagesPerBlock) + CurrentPage;
            Rval = AmbaNandOp_Read(PageAddr, 1, pData, NULL, 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNandOp_Read failed. <Block %d, Page %d>", RpmsgInfoBlk, CurrentPage, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            pData += pPartInfo->MainSize;

            if (++CurrentPage == pPartInfo->PagesPerBlock) {
                CurrentPage = 0;
                break;
            }
        }

        if (Page == Pages)
            break;
    }

    if (Page != Pages) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: not enough space!!.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

Done:
    return Rval;
}

static int Hiber_EmmcWriteHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                             AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 Block = 0, OpPages;
    int Rval = 0;

    OpPages = AossSize / pPartInfo->MainSize;
    SecConfig.NumSector   = OpPages;
    SecConfig.StartSector = pPartInfo->StartBlock;
    SecConfig.pDataBuf    = (void *)pAossInfo;
    Rval = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (Rval < 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_WriteSector failed. <Sector %d, Sectors %d>", Block, OpPages, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    Block = pPartInfo->StartBlock;

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_EmmcWriteDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                               AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 OpPages;
    UINT32 Block = 0;
    UINT32 PageIndex = 0;
    UINT32 TotalSize = 0;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 VirtAddr;

    UNUSED(AossSize);
    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    /* Write the data to the next sector of header and header2 sector. */
    Block = DataStartBlk + ((pAmbaHiber->AossSize / pPartInfo->MainSize) * 2);
    for (PageIndex = 0; PageIndex <= pAossInfo->Full.CopyPages; PageIndex++) {
        OpPages = pPageInfo[PageIndex].Size / pPartInfo->MainSize;
        if (pPageInfo[PageIndex].Size % pPartInfo->MainSize)
            OpPages++;

        SecConfig.NumSector   = OpPages;
        SecConfig.StartSector = Block;
        SecConfig.pDataBuf    = (void *)(ULONG)(pPageInfo[PageIndex].Dst);
        Rval = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
        if (Rval < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_WriteSector failed. <Sector %d, Sectors %d>", Block, OpPages, 0U, 0U, 0U);
            AmbaPrint_Flush();
            break;
        }
        Block += OpPages;
        TotalSize += pPageInfo[PageIndex].Size;
    }

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

    if (Rval == 0) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Write %d bytes from block %d:%d.", TotalSize, DataStartBlk, Block, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_EmmcWriteHeader2(UINT32 HeaderBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 OpPages;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    OpPages = AossSize / pPartInfo->MainSize;

    HeaderBlk += (pAmbaHiber->AossSize / pPartInfo->MainSize);
    SecConfig.NumSector   = OpPages;
    SecConfig.StartSector = HeaderBlk;
    SecConfig.pDataBuf    = (void *)pAossInfo;
    Rval = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (Rval < 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_WriteSector failed. <Sector %d, Sectors %d>", HeaderBlk, OpPages, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return Rval;
}

static int Hiber_SaveEmmcRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                               AMBA_HIBER_PART_INFO_s *pPartInfo,
                               UINT8 *pData, UINT32 Size)
{
    int Rval = 0;
    UINT32 Pages;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    UNUSED(pAossInfo);
    Pages = Size / pPartInfo->MainSize;
    if (Size % pPartInfo->MainSize)
        Pages++;

    if ((RpmsgInfoBlk + Pages) >= pPartInfo->EndBlock) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: not enough space!!.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }
    SecConfig.NumSector   = Pages;
    SecConfig.StartSector = RpmsgInfoBlk;
    SecConfig.pDataBuf    = (void *)pData;
    Rval = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (Rval < 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_WriteSector failed. <Sector %d, Sectors %d>", RpmsgInfoBlk, Pages, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    RpmsgInfoBlk += Pages;

Done:
    return Rval;
}

static int Hiber_EmmcReadHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                            AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0;
    UINT32 OpPages;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    OpPages = AossSize / pPartInfo->MainSize;
    Block = pPartInfo->StartBlock;

    SecConfig.NumSector   = OpPages;
    SecConfig.StartSector = Block;
    SecConfig.pDataBuf    = (void *)pAossInfo;
    Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (Rval < 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_ReadSector failed. <Sector %d, Sectors %d>", 0U, OpPages, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_EmmcReadHeader2(AMBA_HIBER_PART_INFO_s *pPartInfo,
                            AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 Block, UINT32 OpPages)
{
    int Rval;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    UNUSED(pPartInfo);
    SecConfig.NumSector   = OpPages;
    SecConfig.StartSector = (Block + OpPages);
    SecConfig.pDataBuf    = (void *)pAossInfo;
    Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (Rval < 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_ReadSector failed. <Sector %d, Sectors %d>", Block, OpPages, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_CheckEmmcReadHiberImgCrc(AMBA_HIBER_PART_INFO_s *pPartInfo, AMBA_HIBER_AOSS_INFO_u *pAossInfo)
{
    int Rval;
    UINT32 Block;
    UINT32 OpPages, BufSize, RemainSize, AddCrcSize;
    UINT32 ImgCrc = AMBA_CRC32_INIT_VALUE;
    void *pBuf;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    BufSize = 0x8000;
    RemainSize = pAossInfo->Full.Param.HiberInfo.ImgSize;
    pBuf = (UINT8 *) AmbaLink_Malloc(MEM_POOL_CRC_BUF, BufSize);
    OpPages = BufSize / pPartInfo->MainSize;

    /* Move to data sector. */
    Block = pPartInfo->StartBlock + ((pAmbaHiber->AossSize / pPartInfo->MainSize) * 2);

    do {
        SecConfig.NumSector   = OpPages;
        SecConfig.StartSector = Block;
        SecConfig.pDataBuf    = (void *)pBuf;
        Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
        if (Rval < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_ReadSector failed. <Sector %d, Sectors %d>", Block, OpPages, 0U, 0U, 0U);
            AmbaPrint_Flush();
            break;
        }

        if (RemainSize >= BufSize)
            AddCrcSize = BufSize;
        else
            AddCrcSize = RemainSize;

        RemainSize -= AddCrcSize;

        ImgCrc = AmbaUtility_Crc32Add((const void *) pBuf, AddCrcSize, ImgCrc);

        Block += OpPages;
    } while (RemainSize != 0);

    AmbaLink_Free(MEM_POOL_CRC_BUF, pBuf);
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Hiber ImgCrc  = 0x%x, Rval = %d", ImgCrc, Rval, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    return ImgCrc;
}
static int Hiber_EmmcReadDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 OpPages;
    UINT32 PageOffset = 0;
    UINT32 Block;
    UINT32 PageIndex = 0;
    UINT32 TotalSize = 0;
    UINT8 *pTmp, *pDst;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 VirtAddr;

    UNUSED(AossSize);
    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    pTmp = (UINT8 *) AmbaLink_Malloc(MEM_POOL_EMMC_TMP_BUF, pPartInfo->MainSize);

    /* Read the data from the next sector of header and header2 sector. */
    Block = DataStartBlk + ((pAmbaHiber->AossSize / pPartInfo->MainSize) * 2);

    for (PageIndex = 0; PageIndex <= pAossInfo->Full.CopyPages; PageIndex++) {

        OpPages = pPageInfo[PageIndex].Size / pPartInfo->MainSize;

        if (OpPages != 0) {
            pDst = (UINT8 *)(ULONG)(pPageInfo[PageIndex].Src);
            SecConfig.NumSector   = OpPages;
            SecConfig.StartSector = Block;
            SecConfig.pDataBuf    = (void *)pDst;
            Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_ReadSector failed. <Sector %d, Sectors %d>", Block, OpPages, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            PageOffset = OpPages * pPartInfo->MainSize;
        }

        if ((pPageInfo[PageIndex].Size) % pPartInfo->MainSize) {
            Block += OpPages;

            OpPages = 1;
            pDst = pTmp;
            SecConfig.NumSector   = OpPages;
            SecConfig.StartSector = Block;
            SecConfig.pDataBuf    = (void *)pDst;
            Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_ReadSector failed. <Sector %d, Sectors %d>", Block, OpPages, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }
            (void)AmbaWrap_memcpy((UINT8 *)(ULONG)(pPageInfo[PageIndex].Src + PageOffset),
                    pDst, pPageInfo[PageIndex].Size - PageOffset);
        }

        PageOffset = 0;
        Block += OpPages;
        TotalSize += pPageInfo[PageIndex].Size;
    }

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

    if (Rval == 0) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read %d bytes from block %d:%d.", TotalSize, DataStartBlk, Block, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    AmbaLink_Free(MEM_POOL_EMMC_TMP_BUF, pTmp);

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_RestoreEmmcRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                                  AMBA_HIBER_PART_INFO_s *pPartInfo,
                                  UINT8 *pData, UINT32 Size)
{
    int Rval = 0;
    UINT32 Pages;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    UNUSED(pAossInfo);
    Pages = Size / pPartInfo->MainSize;
    if (Size % pPartInfo->MainSize)
        Pages++;

    if ((RpmsgInfoBlk + Pages) >= pPartInfo->EndBlock) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: not enough space!!.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    SecConfig.NumSector   = Pages;
    SecConfig.StartSector = RpmsgInfoBlk;
    SecConfig.pDataBuf    = (void *)pData;
    Rval = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (Rval < 0) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSD_ReadSector failed. <Sector %d, Sectors %d>", RpmsgInfoBlk, Pages, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    RpmsgInfoBlk += Pages;
Done:
    return Rval;
}

static int Hiber_SpiNorWriteHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                             AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0, OpPages;
    UINT32 Addr, BlkSize, Offset;
    UINT8 *pPtr = (UINT8 *) pAossInfo;

    BlkSize = pPartInfo->PagesPerBlock * pPartInfo->MainSize;
    Offset = 0;

    OpPages = AossSize / pPartInfo->MainSize;
    for (Block = pPartInfo->StartBlock; Block < pPartInfo->EndBlock; Block++) {
        Addr = Block * pPartInfo->MainSize * pPartInfo->PagesPerBlock;

        Rval = AmbaSpiNOR_Erase(Addr, BlkSize, 5000);
        if (Rval < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Erase failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }

        if (OpPages > pPartInfo->PagesPerBlock) {
            Rval = AmbaSpiNOR_Program(Addr, BlkSize, (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Program failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }

            OpPages -= pPartInfo->PagesPerBlock;
            Offset += BlkSize;
        } else {
            Rval = AmbaSpiNOR_Program(Addr, OpPages * pPartInfo->MainSize,
                                        (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Program failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            break;
        }
    }

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_SpiNorWriteDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                               AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Page, OpPages;
    UINT32 Block = 0;
    UINT32 PageOffset = 0, PrePageOffset;
    UINT32 PageIndex = 0, PrePageIndex;
    UINT32 TotalSize = 0;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    UINT32 BlkAddr, PageAddr, BlkSize;
    UINT32 VirtAddr;

    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    BlkSize = pPartInfo->PagesPerBlock * pPartInfo->MainSize;

    UNUSED(pAossInfo);
    UNUSED(AossSize);
    /* Write the data to the next block of header and header2 block. */
    Block = DataStartBlk + (pAmbaHiber->AossSize / BlkSize) + 1;

    for (; Block < pPartInfo->EndBlock; Block++) {
        BlkAddr = Block * BlkSize;
        Rval = AmbaSpiNOR_Erase(BlkAddr, BlkSize, 5000);
        if (Rval < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Erase failed. <addr 0x%08x>", BlkAddr, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
                   break;
        }
        PrePageOffset = PageOffset;
        PrePageIndex = PageIndex;
        OpPages = (pPageInfo[PageIndex].Size - PageOffset) / pPartInfo->MainSize;

        if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
            OpPages++;

        if (OpPages > pPartInfo->PagesPerBlock)
            OpPages = pPartInfo->PagesPerBlock;

        for (Page = 0; Page < pPartInfo->PagesPerBlock;) {
            PageAddr = BlkAddr + Page * pPartInfo->MainSize;
            Rval = AmbaSpiNOR_Program(PageAddr, OpPages * pPartInfo->MainSize,
                    (UINT8 *)(ULONG)(pPageInfo[PageIndex].Dst + PageOffset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNOR_Program failed. <addr 0x%08x>", PageAddr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            PageOffset += OpPages * pPartInfo->MainSize;
            Page += OpPages;

            if (PageOffset >= pPageInfo[PageIndex].Size) {
                TotalSize += pPageInfo[PageIndex].Size;
                if(LINK_CTRL_DEBUG_MSG) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Write 0x%08x, size 0x%08x.", pPageInfo[PageIndex].Dst, pPageInfo[PageIndex].Size, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                }
                PageIndex++;

                if (PageIndex > pAossInfo->Full.CopyPages)
                    goto Hiber_WriteDataPage_Exit;

                PageOffset = 0;
                OpPages = pPageInfo[PageIndex].Size / pPartInfo->MainSize;

                if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
                    OpPages++;

                if ((Page + OpPages) > pPartInfo->PagesPerBlock)
                    OpPages = pPartInfo->PagesPerBlock - Page;
            }
        }

        if (Rval < 0) {
            PageOffset = PrePageOffset;
            PageIndex = PrePageIndex;
            continue;
        }
    }
Hiber_WriteDataPage_Exit:

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

    return (Rval < 0) ? Rval : (int)Block;
}
static int Hiber_SpiNorWriteHeader2(UINT32 HeaderBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0;
    UINT32 OpPages;
    UINT32 Addr, BlkSize, Offset;
    UINT8 *pPtr = (UINT8 *) pAossInfo;

    OpPages = AossSize / pPartInfo->MainSize;
    BlkSize = pPartInfo->PagesPerBlock * pPartInfo->MainSize;
    Block = HeaderBlk + 1;
    Offset = 0;

    for (; Block < pPartInfo->EndBlock; Block++) {
        Addr = Block * BlkSize;

        Rval = AmbaSpiNOR_Erase(Addr, BlkSize, 5000);
        if (Rval < 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNOR_Erase failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }

        if (OpPages > pPartInfo->PagesPerBlock) {
            Rval = AmbaSpiNOR_Program(Addr, BlkSize, (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNOR_Program failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }

            OpPages -= pPartInfo->PagesPerBlock;
            Offset += BlkSize;
        } else {
            Rval = AmbaSpiNOR_Program(Addr, OpPages * pPartInfo->MainSize,
                                    (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaNOR_Program failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            break;
        }
    }
    return Rval;
}

static int Hiber_SaveSpiNorRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                               AMBA_HIBER_PART_INFO_s *pPartInfo,
                               UINT8 *pData, UINT32 Size)
{
    UINT32 Addr, BlkSize;
    UINT32 Page, Pages;
    int Rval = 0;

    UNUSED(pAossInfo);
    Page = 0;
    Pages = Size / pPartInfo->MainSize;
    if (Size % pPartInfo->MainSize)
        Pages++;

    BlkSize = pPartInfo->MainSize * pPartInfo->PagesPerBlock;

    for (; RpmsgInfoBlk < pPartInfo->EndBlock; RpmsgInfoBlk++) {
        if (CurrentPage == 0) {
            Rval = AmbaSpiNOR_Erase(RpmsgInfoBlk * BlkSize, BlkSize, 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Erase failed. <addr 0x%08x>", RpmsgInfoBlk * BlkSize, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }
        }

        for (; Page < Pages; Page++) {
            Addr = RpmsgInfoBlk * BlkSize + CurrentPage * pPartInfo->MainSize;
            Rval = AmbaSpiNOR_Program(Addr, pPartInfo->MainSize, (UINT8 *) pData, 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Program failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            pData += pPartInfo->MainSize;

            if (++CurrentPage == pPartInfo->PagesPerBlock) {
                CurrentPage = 0;
                break;
            }
        }

        if (Page == Pages)
            break;
    }

    if (Page != Pages) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: not enough space!!.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

Done:
    return Rval;
}

static int Hiber_SpiNorReadHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                            AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Addr, Offset, BlkSize;
    UINT32 Block = 0;
    UINT8 *pPtr = (UINT8 *) pAossInfo;
    UINT32 OpPages;

    OpPages = AossSize / pPartInfo->MainSize;
    Offset = 0;
    BlkSize = pPartInfo->PagesPerBlock * pPartInfo->MainSize;

    for (Block = pPartInfo->StartBlock; Block < pPartInfo->EndBlock; Block++) {
        Addr = Block * BlkSize;

        if (OpPages > pPartInfo->PagesPerBlock) {
            Rval = AmbaSpiNOR_Readbyte(Addr, BlkSize, (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }

            OpPages -= pPartInfo->PagesPerBlock;
            Offset += BlkSize;
        } else {
            Rval = AmbaSpiNOR_Readbyte(Addr, OpPages * pPartInfo->MainSize,
                                    (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            break;
        }
    }
    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_SpiNorReadHeader2(AMBA_HIBER_PART_INFO_s *pPartInfo,
                            AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 Block, UINT32 OpPages)
{
    int Rval = 0;
    UINT32 Addr, Offset, BlkSize, Blk2;
    UINT8 *pPtr = (UINT8 *) pAossInfo;

    Offset = 0;
    BlkSize = pPartInfo->PagesPerBlock * pPartInfo->MainSize;
    Blk2 = Block + 1;

    for (; Blk2  < pPartInfo->EndBlock; Blk2++) {
        Addr = Blk2  * BlkSize;

        if (OpPages > pPartInfo->PagesPerBlock) {
            Rval = AmbaSpiNOR_Readbyte(Addr, BlkSize, (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }

            OpPages -= pPartInfo->PagesPerBlock;
            Offset += BlkSize;
        } else {
            Rval = AmbaSpiNOR_Readbyte(Addr, OpPages * pPartInfo->MainSize,
                                        (UINT8 *) (pPtr + Offset), 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            break;
        }
    }
    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_CheckSpiNorReadHiberImgCrc(AMBA_HIBER_PART_INFO_s *pPartInfo,
                                        AMBA_HIBER_AOSS_INFO_u *pAossInfo)
{
    int Rval;
    UINT32 Block = pAossInfo->Full.Param.HiberInfo.HeaderBlkNum;
    UINT32 Page, OpPages, BufSize, RemainSize, AddCrcSize;
    UINT32 ImgCrc = AMBA_CRC32_INIT_VALUE;
    UINT32 Addr, BlkSize;
    void *pBuf;

    BufSize = 0x8000;
    RemainSize = pAossInfo->Full.Param.HiberInfo.ImgSize;
    pBuf = (UINT8 *) AmbaLink_Malloc(MEM_POOL_CRC_BUF, BufSize);
    OpPages = BufSize / pPartInfo->MainSize;


    BlkSize = pPartInfo->PagesPerBlock * pPartInfo->MainSize;

    /* Move to data block. */
    Block += (pAmbaHiber->AossSize / BlkSize) + 1;

    for (; Block < pPartInfo->EndBlock; Block++) {
        for (Page = 0; Page < pPartInfo->PagesPerBlock; Page += OpPages) {
            Addr = Block * BlkSize + Page * pPartInfo->MainSize;

            Rval = AmbaSpiNOR_Readbyte(Addr, OpPages * pPartInfo->MainSize,
                                    pBuf, 5000);

            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            if (RemainSize >= BufSize)
                AddCrcSize = BufSize;
            else
                AddCrcSize = RemainSize;

            RemainSize -= AddCrcSize;

            ImgCrc = AmbaUtility_Crc32Add((const void *) pBuf, AddCrcSize, ImgCrc);
            if (RemainSize == 0)
                goto Done;
        }
    }

Done:
    AmbaLink_Free(MEM_POOL_CRC_BUF, pBuf);
    return ImgCrc;
}
static int Hiber_SpiNorReadDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Page, OpPages, PrePageIndex;
    UINT32 Block = DataStartBlk;
    UINT32 BlkAddr, PageAddr, BlkSize;
    UINT32 PageOffset = 0, PrePageOffset;
    UINT32 PageIndex = 0;
    UINT32 TotalSize = 0;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    UINT8 *pTmp, *pDst;
    UINT32 VirtAddr;

    UNUSED(AossSize);
    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    pTmp = (UINT8 *) AmbaLink_Malloc(MEM_POOL_SPI_NOR_TMP_BUF, pPartInfo->MainSize);
    BlkSize = pPartInfo->MainSize * pPartInfo->PagesPerBlock;
    /* Read the data from the next block of header and header2 block. */
    Block = DataStartBlk + (pAmbaHiber->AossSize / BlkSize) + 1;

    for (; Block < pPartInfo->EndBlock; Block++) {
        BlkAddr = Block * BlkSize;

        PrePageOffset = PageOffset;
        PrePageIndex = PageIndex;
        OpPages = (pPageInfo[PageIndex].Size - PageOffset) / pPartInfo->MainSize;

        if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
            OpPages++;

        if (OpPages > pPartInfo->PagesPerBlock)
            OpPages = pPartInfo->PagesPerBlock;

        for (Page = 0; Page < pPartInfo->PagesPerBlock;) {
            PageAddr = BlkAddr + Page * pPartInfo->MainSize;

            if (OpPages == 1 && pPageInfo[PageIndex].Size < pPartInfo->MainSize) {
                pDst = pTmp;
            } else {
                pDst = (UINT8 *)(ULONG)(pPageInfo[PageIndex].Src + PageOffset);
            }

            Rval = AmbaSpiNOR_Readbyte(PageAddr, OpPages * pPartInfo->MainSize,
                                    pDst, 5000);

            if(LINK_CTRL_DEBUG_MSG) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read %d pages at block %d:%d into 0x%08x/0x%08x",
                                        OpPages, Block, Page,
                                        (UINT32)(pPageInfo[PageIndex].Dst + PageOffset),
                                        (UINT32)(pPageInfo[PageIndex].Src + PageOffset));
                AmbaPrint_Flush();
            }

            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <addr 0x%08x>", PageAddr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            if (OpPages == 1 && pPageInfo[PageIndex].Size < pPartInfo->MainSize) {
                (void)AmbaWrap_memcpy((UINT8 *)(ULONG)(pPageInfo[PageIndex].Src + PageOffset),
                        pDst, pPageInfo[PageIndex].Size);
            }

            PageOffset += OpPages * pPartInfo->MainSize;
            Page += OpPages;

            if (PageOffset >= pPageInfo[PageIndex].Size) {
                TotalSize += pPageInfo[PageIndex].Size;
                if(LINK_CTRL_DEBUG_MSG) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read 0x%08x, size 0x%08x.", pPageInfo[PageIndex].Size, pPageInfo[PageIndex].Size, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                }
                PageIndex++;
                if (PageIndex > pAossInfo->Full.CopyPages)
                    goto Hiber_ReadDataPage_Exit;

                PageOffset = 0;
                OpPages = pPageInfo[PageIndex].Size / pPartInfo->MainSize;

                if ((pPageInfo[PageIndex].Size - PageOffset) % pPartInfo->MainSize)
                    OpPages++;

                if ((Page + OpPages) > pPartInfo->PagesPerBlock)
                    OpPages = pPartInfo->PagesPerBlock - Page;
            }
        }

        if (Rval < 0) {
            PageOffset = PrePageOffset;
            PageIndex = PrePageIndex;
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Try next Block...", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

Hiber_ReadDataPage_Exit:

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

    if (Rval == 0) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read %d bytes from block %d:%d.", TotalSize, DataStartBlk, Block, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    AmbaLink_Free(MEM_POOL_SPI_NOR_TMP_BUF, pTmp);

    return (Rval < 0) ? Rval : (int)Block;
}

static int Hiber_RestoreSpiNorRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                                  AMBA_HIBER_PART_INFO_s *pPartInfo,
                                  UINT8 *pData, UINT32 Size)
{
    UINT32 Addr, BlkSize;
    UINT32 Page, Pages;
    int Rval = 0;

    UNUSED(pAossInfo);
    Page = 0;
    Pages = Size / pPartInfo->MainSize;
    if (Size % pPartInfo->MainSize)
        Pages++;

    BlkSize = pPartInfo->MainSize * pPartInfo->PagesPerBlock;

    for (; RpmsgInfoBlk < pPartInfo->EndBlock; RpmsgInfoBlk++) {
        for (; Page < Pages; Page++) {
            Addr = RpmsgInfoBlk * BlkSize + CurrentPage * pPartInfo->MainSize;

            Rval = AmbaSpiNOR_Readbyte(Addr, pPartInfo->MainSize, pData, 5000);
            if (Rval < 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaSpiNOR_Readbyte failed. <addr 0x%08x>", Addr, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }

            pData += pPartInfo->MainSize;

            if (++CurrentPage == pPartInfo->PagesPerBlock) {
                CurrentPage = 0;
                break;
            }
        }

        if (Page == Pages)
            break;
    }

    if (Page != Pages) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: not enough space!!.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

Done:
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_WriteHeader
 *
 *  @Description:: Write the hibernation image header to storage.
 *
 *  @Input      ::
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              AossSize        : Size of AOSS.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_WriteHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                             AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
//    UINT32 OpPages;
    AMBA_PARTITION_ENTRY_s PartInfo;

    AmbaLink_ReadPartitionInfo(1, AMBA_USER_PARTITION_LINUX_KERNEL, &PartInfo);
    pAossInfo->Full.Param.Crc.Lnx = PartInfo.ImageCRC32;
    AmbaLink_ReadPartitionInfo(1, AMBA_USER_PARTITION_LINUX_ROOT_FS, &PartInfo);
    pAossInfo->Full.Param.Crc.Rfs = PartInfo.ImageCRC32;

    /* Save the hibernation resume address. */
    pAossInfo->Full.Param.Crc.Rsv0 = LinuxKernelStart;

    AmbaCache_DataClean((UINT32)pAossInfo, AossSize);

//    OpPages = AossSize / pPartInfo->MainSize;

    if (_Hiber_Ctrl.HiberWriteHeader) {
        Rval = _Hiber_Ctrl.HiberWriteHeader(pPartInfo, pAossInfo, AossSize);
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Can't find HiberWriteHeader function.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_WriteDataPage
 *
 *  @Description:: Write the hibernation image data to storage.
 *
 *  @Input      ::
 *              DataStartBlk: Start block of data image.
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              AossSize        : Size of AOSS.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_WriteDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                               AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0;

    if (_Hiber_Ctrl.HiberWriteDataPage) {
        Rval = _Hiber_Ctrl.HiberWriteDataPage(DataStartBlk, pPartInfo, pAossInfo, AossSize);
    }
    else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Can't find HiberWriteDataPage function.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    if (Rval > 0) {
        Block = Rval;
        Rval = 0;
    }

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

Done:
    return (Rval < 0) ? Rval : (int)Block;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_CalcWriteHiberImgCrcInfo
 *
 *  @Description:: Calculation for hibernation image size and crc
 *
 *  @Input      ::
 *      pAossInfo   : Pointer to AOSS information data structure.
 *
 *  @Output     ::
 *                  pAossInfo->Full.Param.HiberInfo.ImgSize
 *                  pAossInfo->Full.Param.HiberInfo.ImgCrc
 *
 *  @Return     ::  none
\*-----------------------------------------------------------------------------------------------*/
static void Hiber_CalcWriteHiberImgCrcInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo)
{
    UINT32 i, ImgSize, ImgCrc;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    UINT32 VirtAddr;

    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    ImgSize = 0;
    for (i = 0; i <= pAossInfo->Full.CopyPages; i++) {
        ImgSize += pPageInfo[i].Size;
    }

    ImgCrc = AMBA_CRC32_INIT_VALUE;
    for (i = 0; i <= pAossInfo->Full.CopyPages; i++) {
        AmbaMMU_PhysToVirt((ULONG)pPageInfo[i].Dst, (ULONG *)&VirtAddr);
        ImgCrc = AmbaUtility_Crc32Add((const void *)VirtAddr, pPageInfo[i].Size, ImgCrc);
    }

    ImgCrc = AmbaUtility_Crc32Finalize(ImgCrc);

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "----- Hiber_CalcWriteHiberImgCrcInfo -----", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Hiber ImgSize = %d", ImgSize, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Hiber ImgCrc = %d", ImgCrc, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    pAossInfo->Full.Param.HiberInfo.ImgSize = ImgSize;
    pAossInfo->Full.Param.HiberInfo.ImgCrc = ImgCrc;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_WriteHeader2
 *
 *  @Description:: Write the hibernation image header2 to storage.
 *
 *  @Input      ::
 *              HeaderBlk       : Header block.
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              AossSize        : Size of AOSS.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_WriteHeader2(UINT32 HeaderBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;

    // Set HiberInfo
    Hiber_CalcWriteHiberImgCrcInfo(pAossInfo);
    pAossInfo->Full.Param.HiberInfo.Magic = AMBA_HIBER_SAVING_MAGIC;
    pAossInfo->Full.Param.HiberInfo.HeaderBlkNum = HeaderBlk;
    AmbaCache_DataClean((UINT32)pAossInfo, AossSize);

    if (_Hiber_Ctrl.HiberWriteHeader2) {
        Rval = _Hiber_Ctrl.HiberWriteHeader2(HeaderBlk, pPartInfo, pAossInfo, AossSize);
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Can't find HiberWriteHeader2 function.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_Save
 *
 *  @Description:: Save the hibernation image to storage.
 *
 *  @Input      ::
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              AossSize        : Size of AOSS.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_Save(AMBA_HIBER_PART_INFO_s *pPartInfo,
                      AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 HeaderBlk;

    if ((pPartInfo->StartBlock == 0) || (pPartInfo->StartBlock == pPartInfo->EndBlock)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "partition infos error [%d:%d]!", pPartInfo->StartBlock, pPartInfo->EndBlock, 0U, 0U, 0U);
        AmbaPrint_Flush();
        Rval = -1;
        goto HiberNand_Save_Exit;
    }

    Rval = Hiber_WriteHeader(pPartInfo, pAossInfo, AossSize);
    if ((Rval < (int)pPartInfo->StartBlock) || (Rval >= (int)pPartInfo->EndBlock)) {
        Rval = -1;
        goto HiberNand_Save_Exit;
    }

    HeaderBlk = Rval;

    /* We will reserve some space for header2 in the following function. */
    Rval = Hiber_WriteDataPage(HeaderBlk, pPartInfo, pAossInfo, AossSize);
    if ((Rval < (int)HeaderBlk) || (Rval >= (int)pPartInfo->EndBlock)) {
        Rval = -1;
        goto HiberNand_Save_Exit;
    }

    RpmsgInfoBlk = Rval + 1;

    Rval = Hiber_WriteHeader2(HeaderBlk, pPartInfo, pAossInfo, AossSize);

HiberNand_Save_Exit:

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_CheckReadHiberImgCrc
 *
 *  @Description:: Validate hibernation image crc from storage.
 *
 *  @Input      ::
 *      pPartInfo   : Pointer to hibernation partition information data structure.
 *      pAossInfo   : Pointer to AOSS information data structure.
 *
 *  @Output     ::  none
 *
 *  @Return     ::
 *          int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_CheckReadHiberImgCrc(AMBA_HIBER_PART_INFO_s *pPartInfo, AMBA_HIBER_AOSS_INFO_u *pAossInfo)
{
    int Rval;
    UINT32 ImgCrc;

    if (_Hiber_Ctrl.HiberCheckCrc) {
        ImgCrc = _Hiber_Ctrl.HiberCheckCrc(pPartInfo, pAossInfo);
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Can't find the HiberCheckCrc function", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    ImgCrc = AmbaUtility_Crc32Finalize(ImgCrc);

    if (ImgCrc == pAossInfo->Full.Param.HiberInfo.ImgCrc)
        Rval = 0;
    else
        Rval = -1;

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Hiber ImgCrc  = 0x%x, Rval = %d", ImgCrc, Rval, 0U, 0U, 0U);
    //AmbaPrint_Flush();
Done:
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_ReadHeader
 *
 *  @Description:: Read the hibernation image header from storage.
 *
 *  @Input      ::
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              AossSize        : Size of AOSS.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_ReadHeader(AMBA_HIBER_PART_INFO_s *pPartInfo,
                            AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0,BootMode;
    UINT32 OpPages, t1, t2;
    AMBA_PARTITION_ENTRY_s LnxPartInfo;
    AMBA_PARTITION_ENTRY_s RfsPartInfo;

    OpPages = AossSize / pPartInfo->MainSize;
    AmbaSYS_GetBootMode(&BootMode);
    if (_Hiber_Ctrl.HiberReadHeader) {
        Rval = _Hiber_Ctrl.HiberReadHeader(pPartInfo, pAossInfo, AossSize);
    }
    else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Can't find the HiberReadHeader function", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto HiberNand_ReadHeader_Exit;
    }

    if (Rval > 0) {
        Block = Rval;
    }

    AmbaCache_DataInvalidate((UINT32)pAossInfo, AossSize);

    if (Block >= pPartInfo->EndBlock) {
        if(HIBER_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Block(%d) >= pPartInfo->EndBlock.(%d)", Block, pPartInfo->EndBlock, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        Rval = -1;
        goto HiberNand_ReadHeader_Exit;
    }

    AmbaLink_ReadPartitionInfo(1, AMBA_USER_PARTITION_LINUX_KERNEL, &LnxPartInfo);
    AmbaLink_ReadPartitionInfo(1, AMBA_USER_PARTITION_LINUX_ROOT_FS, &RfsPartInfo);

    if ((pAossInfo->Full.Param.Crc.Lnx != LnxPartInfo.ImageCRC32) ||
        (pAossInfo->Full.Param.Crc.Rfs != RfsPartInfo.ImageCRC32)) {
        if(HIBER_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "wrong magic12 0x%08x:0x%08x.", pAossInfo->Simple.Info[2], pAossInfo->Simple.Info[3], 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        Rval = -1;
        goto HiberNand_ReadHeader_Exit;
    }

    /* Restore the hibernation resume address. */
    LinuxKernelStart = pAossInfo->Full.Param.Crc.Rsv0;
    AmbaCache_DataClean((UINT32)&LinuxKernelStart, sizeof(LinuxKernelStart));

    if (_Hiber_Ctrl.HiberReadHeader2) {
        Rval = _Hiber_Ctrl.HiberReadHeader2(pPartInfo, pAossInfo, Block, OpPages);
    }
    else {
        Rval = -1;
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Can't find the HiberReadHeader2 function.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto HiberNand_ReadHeader_Exit;
    }
    AmbaCache_DataInvalidate((UINT32)pAossInfo, AossSize);

    // check hibernatin magic and block number
    if ((pAossInfo->Full.Param.HiberInfo.Magic != AMBA_HIBER_SAVING_MAGIC) ||
        (pAossInfo->Full.Param.HiberInfo.HeaderBlkNum != Block)) {
        if(HIBER_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "wrong magic3b 0x%08x:0x%08x.", pAossInfo->Full.Param.HiberInfo.Magic, pAossInfo->Full.Param.HiberInfo.HeaderBlkNum, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        Rval = -1;
        goto HiberNand_ReadHeader_Exit;
    }

#if 0
    // Check hibernation image size and crc
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "----- Hiber image info in AossInfo -----", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Hiber ImgSize = %d", pAossInfo->Full.Param.HiberInfo.ImgSize, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Hiber ImgCrc = %d", pAossInfo->Full.Param.HiberInfo.ImgCrc, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
#endif

    if (AmbaLinkCtrl.AmbaHiberImgCheck) {
        // Check hiber image crc from nand.
        AmbaKAL_GetSysTickCount(&t1);
        Rval = Hiber_CheckReadHiberImgCrc(pPartInfo, pAossInfo);
        AmbaKAL_GetSysTickCount(&t2);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Hibernation Image Crc is invalid, erase hiber partition", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            AmbaLink_ErasePartition(1, AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG, 5000);
        } else {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Checking hibernation Image is valid in %d ms", t2 - t1, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

HiberNand_ReadHeader_Exit:

    return (Rval < 0) ? Rval : (int)Block;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_ReadDataPage
 *
 *  @Description:: Read the hibernation image data from storage.
 *
 *  @Input      ::
 *              DataStartBlk: Start block of data image.
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              AossSize        : Size of AOSS.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_ReadDataPage(UINT32 DataStartBlk, AMBA_HIBER_PART_INFO_s *pPartInfo,
                              AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 Block = 0;

    if (_Hiber_Ctrl.HiberReadDataPage)
        Rval = _Hiber_Ctrl.HiberReadDataPage(DataStartBlk, pPartInfo, pAossInfo, AossSize);
    else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s Can't find the HiberReadDataPage function.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
        goto Done;
    }

    if(Rval > 0) {
        Block = Rval;
        Rval = 0;
    }

    if (Block >= pPartInfo->EndBlock) {
        Rval = -1;
    }

    if (Rval == 0) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Read from block %d:%d.", DataStartBlk, Block, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

Done:
    return (Rval < 0) ? Rval : (int)Block;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_Restore
 *
 *  @Description:: Restore the hibernation image from storage.
 *
 *  @Input      ::
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              AossSize        : Size of AOSS.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_Restore(AMBA_HIBER_PART_INFO_s *pPartInfo,
                         AMBA_HIBER_AOSS_INFO_u *pAossInfo, UINT32 AossSize)
{
    int Rval = 0;
    UINT32 HeaderBlk;

    if ((pPartInfo->StartBlock == 0) || (pPartInfo->StartBlock == pPartInfo->EndBlock)) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "partition infos error [%d:%d]!", pPartInfo->StartBlock, pPartInfo->EndBlock, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        Rval = -1;
        goto Hiber_Restore_Exit;
    }

    Rval = Hiber_ReadHeader(pPartInfo, pAossInfo, AossSize);
    if ((Rval < (int)pPartInfo->StartBlock) || (Rval >= (int)pPartInfo->EndBlock)) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Hiber_ReadHeader error!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        Rval = -1;
        goto Hiber_Restore_Exit;
    }

    HeaderBlk = Rval;

    Rval = Hiber_Valid(pAossInfo);
    if (Rval) {
        if(LINK_CTRL_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Hiber_Valid error!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        goto Hiber_Restore_Exit;
    }

    Rval = Hiber_ReadDataPage(HeaderBlk, pPartInfo, pAossInfo, AossSize);
    if ((Rval < (int)HeaderBlk) || (Rval >= (int)pPartInfo->EndBlock)) {
        Rval = -1;
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Hiber_ReadDataPage error!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto Hiber_Restore_Exit;
    }

    RpmsgInfoBlk = Rval + 1;

    Rval = 0;

Hiber_Restore_Exit:

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_GetPartInfo
 *
 *  @Description:: Get the hibernation partition information from storage.
 *
 *  @Input      ::
 *              pAossInfo       : Pointer to AOSS information data structure.
 *              pPartInfo       : Pointer to hibernation partition information data structure.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_GetPartInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                             AMBA_HIBER_PART_INFO_s *pPartInfo)
{
    int Rval = 0;
    AMBA_PARTITION_ENTRY_s HiberPartInfo;
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo;
    UINT32 BootMode;

    UNUSED(pAossInfo);
    AmbaSYS_GetBootMode(&BootMode);
    AmbaLink_ReadPartitionInfo(1, AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG,
                               &HiberPartInfo);

    if (BootMode == AMBA_SYS_BOOT_FROM_NAND) {
        AMBA_NAND_COMMON_INFO_s *pAmbaNAND_DevInfo = AmbaNAND_GetCommonInfo();
        pPartInfo->MainSize = pAmbaNAND_DevInfo->MainByteSize;
        pPartInfo->PagesPerBlock = pAmbaNAND_DevInfo->BlockPageSize;
    }
    else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NAND) {
        AMBA_NAND_COMMON_INFO_s *pAmbaNAND_DevInfo = AmbaNAND_GetCommonInfo();
        pPartInfo->MainSize = pAmbaNAND_DevInfo->MainByteSize;
        pPartInfo->PagesPerBlock = pAmbaNAND_DevInfo->BlockPageSize;
    }

    else if (BootMode == AMBA_SYS_BOOT_FROM_EMMC) {
        pPartInfo->MainSize = 512;
        pPartInfo->PagesPerBlock = 1;
    } else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
        pNorDevInfo = AmbaSpiNOR_GetDevInfo();
        pPartInfo->MainSize = pNorDevInfo->PageSize;
        pPartInfo->PagesPerBlock = pNorDevInfo->EraseBlockSize /
                                    pNorDevInfo->PageSize;
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Boot device unrecogznized!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    pPartInfo->StartBlock = HiberPartInfo.StartBlkAddr;
    pPartInfo->EndBlock = HiberPartInfo.StartBlkAddr + HiberPartInfo.BlkCount;

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_Return
 *
 *  @Description:: Return from the hibernated status.
 *
 *  @Input      ::
 *              pAossInfo       : Pointer to AOSS information data structure.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_Return(AMBA_HIBER_AOSS_INFO_u *pAossInfo)
{
    int Rval = 0;
    int i;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    UINT32 VirtAddr;

    Rval = Hiber_Valid(pAossInfo);
    if (Rval)
        goto Hiber_Return_Exit;

    if (pAossInfo->Full.CopyPages == 0)
        goto Hiber_Return_Exit;

    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    for (i = 0; i <= (int)pAossInfo->Full.CopyPages; i++) {
#ifndef AMBA_KAL_NO_SMP
        /* In SMP BOSS, the cache is synced by SCU. */
        /* We can't invalidate the cache otherwise the data may be missing. */
        AmbaCache_DataInvalidate((ULONG)pPageInfo[i].Dst, pPageInfo[i].Size);
#endif
        (void)AmbaWrap_memcpy((void *)(ULONG)pPageInfo[i].Src,
                (void *)(ULONG)pPageInfo[i].Dst, pPageInfo[i].Size);

        AmbaCache_DataClean((ULONG)pPageInfo[i].Src, pPageInfo[i].Size);
        if(HIBER_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "copy from 0x%08x to 0x%08x, size 0x%08x.", pPageInfo[i].Dst, pPageInfo[i].Src, pPageInfo[i].Size, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "TotalPages = %d, CopyPages = %d", pAossInfo->Full.TotalPages, pAossInfo->Full.CopyPages, 0U, 0U, 0U);
    AmbaPrint_Flush();

Hiber_Return_Exit:

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_SaveRpmsgInfo
 *
 *  @Description:: Return from the hibernated status.
 *
 *  @Input      ::
 *              pAossInfo       : Pointer to AOSS information data structure.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_SaveRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                               AMBA_HIBER_PART_INFO_s *pPartInfo,
                               UINT8 *pData, UINT32 Size)
{
    int Rval = 0;

    if (_Hiber_Ctrl.HiberSaveRpmsg) {
        Rval = _Hiber_Ctrl.HiberSaveRpmsg(pAossInfo, pPartInfo, pData, Size);
    }
    else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Can't find the HiberSaveRpmsg function.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Hiber_RestoreRpmsgInfo
 *
 *  @Description:: Return from the hibernated status.
 *
 *  @Input      ::
 *              pAossInfo       : Pointer to AOSS information data structure.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *              int : OK(0)/NG(< 0)
\*-----------------------------------------------------------------------------------------------*/
static int Hiber_RestoreRpmsgInfo(AMBA_HIBER_AOSS_INFO_u *pAossInfo,
                                  AMBA_HIBER_PART_INFO_s *pPartInfo,
                                  UINT8 *pData, UINT32 Size)
{
    int Rval = 0;

    if (_Hiber_Ctrl.HiberRestoreRpmsg)
        Rval = _Hiber_Ctrl.HiberRestoreRpmsg(pAossInfo, pPartInfo, pData, Size);
    else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Can't find the HiberRestoreRpmsg function.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * Exported functions.
\*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberInitNVMCtrl
 *
 *  @Description:: Register the NVM function for read/write hibernation.
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_HiberInitNVMCtrl(void)
{
    UINT32 BootMode;

    AmbaSYS_GetBootMode(&BootMode);

    if (BootMode == AMBA_SYS_BOOT_FROM_NAND) {
        _Hiber_Ctrl.HiberWriteHeader       = Hiber_NandWriteHeader;
        _Hiber_Ctrl.HiberWriteHeader2      = Hiber_NandWriteHeader2;
        _Hiber_Ctrl.HiberWriteDataPage     = Hiber_NandWriteDataPage;
        _Hiber_Ctrl.HiberSaveRpmsg         = Hiber_SaveNandRpmsgInfo;
        _Hiber_Ctrl.HiberReadHeader        = Hiber_NandReadHeader;
        _Hiber_Ctrl.HiberReadHeader2       = Hiber_NandReadHeader2;
        _Hiber_Ctrl.HiberCheckCrc          = Hiber_CheckNandReadHiberImgCrc;
        _Hiber_Ctrl.HiberReadDataPage      = Hiber_NandReadDataPage;
        _Hiber_Ctrl.HiberRestoreRpmsg      = Hiber_RestoreNandRpmsgInfo;
    }
    else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NAND) {
        _Hiber_Ctrl.HiberWriteHeader       = Hiber_NandWriteHeader;
        _Hiber_Ctrl.HiberWriteHeader2      = Hiber_NandWriteHeader2;
        _Hiber_Ctrl.HiberWriteDataPage     = Hiber_NandWriteDataPage;
        _Hiber_Ctrl.HiberSaveRpmsg         = Hiber_SaveNandRpmsgInfo;
        _Hiber_Ctrl.HiberReadHeader        = Hiber_NandReadHeader;
        _Hiber_Ctrl.HiberReadHeader2       = Hiber_NandReadHeader2;
        _Hiber_Ctrl.HiberCheckCrc          = Hiber_CheckNandReadHiberImgCrc;
        _Hiber_Ctrl.HiberReadDataPage      = Hiber_NandReadDataPage;
        _Hiber_Ctrl.HiberRestoreRpmsg      = Hiber_RestoreNandRpmsgInfo;
    }
    else if (BootMode == AMBA_SYS_BOOT_FROM_EMMC) {
        _Hiber_Ctrl.HiberWriteHeader       = Hiber_EmmcWriteHeader;
        _Hiber_Ctrl.HiberWriteHeader2      = Hiber_EmmcWriteHeader2;
        _Hiber_Ctrl.HiberWriteDataPage     = Hiber_EmmcWriteDataPage;
        _Hiber_Ctrl.HiberSaveRpmsg         = Hiber_SaveEmmcRpmsgInfo;
        _Hiber_Ctrl.HiberReadHeader        = Hiber_EmmcReadHeader;
        _Hiber_Ctrl.HiberReadHeader2       = Hiber_EmmcReadHeader2;
        _Hiber_Ctrl.HiberCheckCrc          = Hiber_CheckEmmcReadHiberImgCrc;
        _Hiber_Ctrl.HiberReadDataPage      = Hiber_EmmcReadDataPage;
        _Hiber_Ctrl.HiberRestoreRpmsg      = Hiber_RestoreEmmcRpmsgInfo;
    } else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
        _Hiber_Ctrl.HiberWriteHeader       = Hiber_SpiNorWriteHeader;
        _Hiber_Ctrl.HiberWriteHeader2      = Hiber_SpiNorWriteHeader2;
        _Hiber_Ctrl.HiberWriteDataPage     = Hiber_SpiNorWriteDataPage;
        _Hiber_Ctrl.HiberSaveRpmsg         = Hiber_SaveSpiNorRpmsgInfo;
        _Hiber_Ctrl.HiberReadHeader        = Hiber_SpiNorReadHeader;
        _Hiber_Ctrl.HiberReadHeader2       = Hiber_SpiNorReadHeader2;
        _Hiber_Ctrl.HiberCheckCrc          = Hiber_CheckSpiNorReadHiberImgCrc;
        _Hiber_Ctrl.HiberReadDataPage      = Hiber_SpiNorReadDataPage;
        _Hiber_Ctrl.HiberRestoreRpmsg      = Hiber_RestoreSpiNorRpmsgInfo;
    }
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberSaveRpmsgInfo
 *
 *  @Description:: Save the rpmsg info.
 *
 *  @Input      ::
 *      pData   :  Address of pInfo.
 *              Size    :  Size of data.
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_HiberSaveRpmsgInfo(UINT8 *pData, UINT32 Size)
{
    int Rval = 0;
    AMBA_HIBER_AOSS_INFO_u *pAossInfo;
    AMBA_HIBER_PART_INFO_s PartInfo;

    if (LINUX_IS_SUSPEND_TO_RAM_MODE()) {
        if (AmbaWrap_memcpy((void *)(ULONG)GET_RPMSG_SUSPEND_BACKUP_ADDR(), pData, Size)!= 0U) { }

        AmbaCache_DataClean((ULONG)GET_RPMSG_SUSPEND_BACKUP_ADDR(), Size);

        SET_RPMSG_SUSPEND_BACKUP_SIZE(Size);
        //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "@@@ addr = 0x%x, size = %d, total write = %d", GET_RPMSG_SUSPEND_BACKUP_ADDR() - Size, Size, GET_RPMSG_SUSPEND_BACKUP_SIZE(), 0U, 0U);
        //AmbaPrint_Flush();
        return OK;
    }

    pAossInfo = (AMBA_HIBER_AOSS_INFO_u *)(ULONG)pAmbaHiber->AossAddr;
    AmbaCache_DataInvalidate((UINT32) pAossInfo, pAmbaHiber->AossSize);

    Rval = Hiber_GetPartInfo(pAossInfo, &PartInfo);
    if (Rval) {
        goto AmbaIPC_HiberSaveRpmsgInfo_Exit;
    }

    Rval = Hiber_SaveRpmsgInfo(pAossInfo, &PartInfo, pData, Size);
    if (Rval < 0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Hiber_SaveRpmsgInfo failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto AmbaIPC_HiberSaveRpmsgInfo_Exit;
    }

    if (RpmsgInfoBlk == PartInfo.EndBlock) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Not enough blocks for hibernation!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
    }

AmbaIPC_HiberSaveRpmsgInfo_Exit:

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberRestoreRpmsgInfo
 *
 *  @Description:: Restore the rpmsg info.
 *
 *  @Input      ::
 *      pData   :  Address of pInfo.
 *              Size    :  Size of data.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_HiberRestoreRpmsgInfo(UINT8 *pData, UINT32 Size)
{
    int Rval;
    AMBA_HIBER_AOSS_INFO_u *pAossInfo;
    AMBA_HIBER_PART_INFO_s PartInfo;

    if (AmbaLinkBootType == AMBALINK_WARM_BOOT) {
        if (AmbaWrap_memcpy(pData, (void const *)(ULONG)GET_RPMSG_SUSPEND_BACKUP_ADDR(), Size)!= 0U) { }
        SET_RPMSG_SUSPEND_BACKUP_SIZE(Size);
        //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "@@@ addr = 0x%x, size = %d, total read = %d", GET_RPMSG_SUSPEND_BACKUP_ADDR() - Size, Size, GET_RPMSG_SUSPEND_BACKUP_SIZE(), 0U, 0U);
        //AmbaPrint_Flush();
        return OK;
    }

    pAossInfo = (AMBA_HIBER_AOSS_INFO_u *)(ULONG)pAmbaHiber->AossAddr;
    AmbaCache_DataInvalidate((UINT32) pAossInfo, pAmbaHiber->AossSize);

    Rval = Hiber_GetPartInfo(pAossInfo, &PartInfo);
    if (Rval) {
        goto AmbaIPC_HiberRestoreRpmsgInfo;
    }

    Rval = Hiber_RestoreRpmsgInfo(pAossInfo, &PartInfo, pData, Size);
    if (Rval < 0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Hiber_RestoreRpmsgInfo failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto AmbaIPC_HiberRestoreRpmsgInfo;
    }

    if (RpmsgInfoBlk == PartInfo.EndBlock) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Not enough blocks for hibernation!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = -1;
    }

AmbaIPC_HiberRestoreRpmsgInfo:

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberRestoreRpmsgFinish
 *
 *  @Description:: Ack Linux rpmsg info restore is done.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_HiberRestoreRpmsgFinish(void)
{
    AmbaIPC_LinkCtrl_RpmsgAck();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberPrepare
 *
 *  @Description:: Prepare AMBA_HIBER_AOSS_INFO_s and run Aoss_Init() to hibernate.
 *
 *  @Input      ::
 *      pInfo   :  Address of pInfo.
 *
 *  @Output     ::
 *      pInfo   :  Output the address of AMBA_HIBER_AOSS_INFO_s to pInfo.
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_HiberPrepare(void *pPrepareInfo)
{
    AMBA_HIBER_AOSS_INFO_u *pAossInfo;
    UINT32 *pTmp = pPrepareInfo;
    UINT32 PhysAddr;

    if(pAmbaHiber->AossSize < (sizeof(AMBA_HIBER_AOSS_INFO_u) + sizeof(AMBA_HIBER_PAGE_INFO_s))) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "pAmbaHiber->AossSize < (sizeof(AMBA_HIBER_AOSS_INFO_u)(%d) + sizeof(AMBA_HIBER_PAGE_INFO_s)(%d))", pAmbaHiber->AossSize, sizeof(AMBA_HIBER_AOSS_INFO_u), sizeof(AMBA_HIBER_PAGE_INFO_s), 0U, 0U);
        AmbaPrint_Flush();
        return -1;
    }

    pAossInfo = (AMBA_HIBER_AOSS_INFO_u *)(ULONG) pAmbaHiber->AossAddr;
    pAossInfo->Full.Magic = AMBA_HIBER_AOSS_MAGIC;
    pAossInfo->Full.TotalPages = (pAmbaHiber->AossSize -
        sizeof(AMBA_HIBER_AOSS_INFO_u)) / sizeof(AMBA_HIBER_PAGE_INFO_s);
    pAossInfo->Full.CopyPages = 0;
    AmbaMMU_VirtToPhys((ULONG)(pAmbaHiber->AossAddr + sizeof(AMBA_HIBER_AOSS_INFO_u)), (ULONG *)&PhysAddr);
    pAossInfo->Full.pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)PhysAddr;
    AmbaMMU_VirtToPhys((ULONG) pAossInfo, (ULONG *)&PhysAddr);
    *pTmp = PhysAddr;

    return AmbaIPC_LinkCtrl_RpmsgAck();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberSuspend
 *
 *  @Description:: Store AMBA_HIBER_AOSS_INFO_s and Linux Page data to storage devcie.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_HiberSuspend(UINT32 Mode)
{
    int i, Rval = 0;
    AMBA_HIBER_AOSS_INFO_u *pAossInfo;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    AMBA_HIBER_PART_INFO_s PartInfo;
    extern unsigned long secure_monitor_call(UINT32 FuncID, ...);
    UINT32 VirtAddr;

    UNUSED(Mode);
    if (LINUX_IS_STANDBY_MODE()) {
        goto AmbaIPC_RpmsgSuspend;
    }

    pAossInfo = (AMBA_HIBER_AOSS_INFO_u *)(ULONG) pAmbaHiber->AossAddr;
    LinuxSuspendMode = pAossInfo->Simple.Info[0];

    Rval = Hiber_Valid(pAossInfo);
    if (Rval) {
        goto AmbaIPC_HiberSuspend_Exit;
    }

    AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
    pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
    if (pAossInfo->Full.CopyPages == 0 && pPageInfo[0].Size == 0) {
        Rval = -1;
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: CopyPages and Size are all empty!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        goto AmbaIPC_HiberSuspend_Exit;
    }

    for (i = 0; i <= (int)pAossInfo->Full.CopyPages; i++) {
        AmbaMMU_PhysToVirt((ULONG)pPageInfo[i].Src, (ULONG *)&VirtAddr);
        pPageInfo[i].Src = VirtAddr;
        AmbaMMU_PhysToVirt((ULONG)pPageInfo[i].Dst, (ULONG *)&VirtAddr);
        pPageInfo[i].Dst = VirtAddr;
    }

    if (LINUX_IS_SUSPEND_TO_DISK_MODE()) {
        Rval = Hiber_GetPartInfo(pAossInfo, &PartInfo);
        if (Rval) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Hiber_GetPartInfo failed!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            goto AmbaIPC_HiberSuspend_Exit;
        }

        Rval = Hiber_Save(&PartInfo, pAossInfo, pAmbaHiber->AossSize);
        if (Rval < 0) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Hiber_Save failed!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }

        CurrentPage = 0;
    }

AmbaIPC_RpmsgSuspend:
    AmbaIPC_RpmsgSuspend();

    if (LINUX_IS_STANDBY_MODE()) {
        for (i = 0; i < 10; i++) {
            Rval = secure_monitor_call(AMBA_SIP_LINUX_SUSPEND_DONE, 3, 0, 0);
            if (Rval) {
                //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "CPU3 killed", __func__, NULL, NULL, NULL, NULL);
                //AmbaPrint_Flush();
                break;
            }

            AmbaKAL_TaskSleep(100);
        }

        if (i == 10) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Suspend Linux failed?\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            return;
        }
    }

AmbaIPC_HiberSuspend_Exit:
    if (AmbaKAL_EventFlagSet(&AmbaLinkEventFlag, AMBALINK_SUSPEND_DONE) != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaKAL_EventFlagSet failed!(0x2)", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    if (AmbaIPC_LinkCtrlSuspendCallBack)
        AmbaIPC_LinkCtrlSuspendCallBack(LinuxSuspendMode);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberResume
 *
 *  @Description:: Resume the Linux from hibernated image.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                 int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_HiberResume(UINT32 Flag)
{
    int Rval = 0;
    //int i;
    AMBA_HIBER_AOSS_INFO_u *pAossInfo = NULL;
    AMBA_HIBER_PAGE_INFO_s *pPageInfo;
    AMBA_HIBER_PART_INFO_s PartInfo;
    UINT32 VirtAddr;

    UNUSED(Flag);
    pAossInfo = (AMBA_HIBER_AOSS_INFO_u *)(ULONG)pAmbaHiber->AossAddr;

    Rval = Hiber_GetPartInfo(pAossInfo, &PartInfo);
    if (Rval) {
        goto AmbaIPC_HiberResume_Exit;
    }

    Rval = Hiber_Restore(&PartInfo, pAossInfo, pAmbaHiber->AossSize);

    if (Rval == 0) {
        AmbaCache_DataClean((UINT32) pAossInfo, pAmbaHiber->AossSize);
        AmbaMMU_PhysToVirt((ULONG) pAossInfo->Full.pPageInfo, (ULONG *)&VirtAddr);
        pPageInfo = (AMBA_HIBER_PAGE_INFO_s *)VirtAddr;
        if (pAossInfo->Full.CopyPages == 0 && pPageInfo[0].Size == 0) {
            Rval = -1;
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: CopyPages and Size are all empty!", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            goto AmbaIPC_HiberResume_Exit;
        }
#if 0
        for (i = 0; i <= pAossInfo->Full.CopyPages; i++) {
            AmbaCache_DataClean((UINT32) pPageInfo[i].Src, pPageInfo[i].Size);
        }
#endif

        AmbaLinkBootType = AMBALINK_HIBER_BOOT;
        AmbaCache_DataClean((UINT32)&AmbaLinkBootType, sizeof(AmbaLinkBootType));
    } else {
        AmbaLinkBootType = AMBALINK_COLD_BOOT;
        AmbaCache_DataClean((UINT32)&AmbaLinkBootType, sizeof(AmbaLinkBootType));
    }

    CurrentPage = 0;

AmbaIPC_HiberResume_Exit:
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberWakeupRemote
 *
 *  @Description:: Wake up remote Os.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_HiberWakeupRemote(void)
{
    /* bit5 core0, bit6 core1, bit18 core2, bit19 core3*/
    /* Just release the primary core (3) of Linux. */
    WriteWord(AXI_CONFIG_PHYS_BASE_ADDR + 0x28,
        ReadWord(AXI_CONFIG_PHYS_BASE_ADDR + 0x28) | (1 << 19));

    WriteWord(AXI_CONFIG_PHYS_BASE_ADDR + 0x28,
        ReadWord(AXI_CONFIG_PHYS_BASE_ADDR + 0x28) & ~(1 << 19));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberReturn
 *
 *  @Description:: Return from the hibernated status.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_HiberReturn(UINT32 SuspendMode)
{
    int Rval = 0;
    AMBA_HIBER_AOSS_INFO_u *pAossInfo;

    if(HIBER_DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
    }
    AmbaLinkBootType = AMBALINK_WARM_BOOT;

    if (SuspendMode != AMBA_LINK_SUSPEND_TO_DISK) {
        goto WakeupRemote;
    }

    pAossInfo = (AMBA_HIBER_AOSS_INFO_u *)(ULONG)pAmbaHiber->AossAddr;
    AmbaCache_DataInvalidate((UINT32) pAossInfo, pAmbaHiber->AossSize);
    Rval = Hiber_Return(pAossInfo);
    if (Rval)
        goto AmbaIPC_HiberReturn_Exit;

WakeupRemote:
    AmbaIPC_HiberWakeupRemote();

AmbaIPC_HiberReturn_Exit:
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberWipeout
 *
 *  @Description:: Erase the hibernation image.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *               int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_HiberWipeout(UINT32 Flag)
{
    int Rval;
    UNUSED(Flag);

    Rval = AmbaLink_ErasePartition(1, AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG, 5000);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberDump
 *
 *  @Description:: Dump the hibernation image to storage device.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
int AmbaIPC_HiberDump(const char *Path)
{
    int Rval = 0;

    UNUSED(Path);
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberBootSetup
 *
 *  @Description:: Setup the AOSS information for hibernation boot.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_HiberBootSetup(void)
{
    pAmbaHiber->AossAddr = AmbaLinkCtrl.AmbaLinkAossAddr;
    pAmbaHiber->AossSize = 0x10000;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIPC_HiberInit
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaIPC_HiberInit(AMBA_RPDEV_LINK_CTRL_OBJ_s *pObj, AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    UINT32 Rval = OK;
    AMBA_LINK_TASK_CTRL_s *pTask;

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_HIBER];

    if ((pObj != NULL) && (pMsgQueue != NULL)) {
        pLinkCtrlObj = pObj;
        IpcSuspendQueue = pMsgQueue;
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to set IpcSuspendQueue!", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Rval = KAL_ERR_ARG;
    }

    // TODO: Create task by LinkCtrl app.
    if (Rval == OK) {
        if (AmbaWrap_memset(&HiberSuspendTask, 0, sizeof(HiberSuspendTask))!= 0U) { }
        Rval = AmbaKAL_TaskCreate(&HiberSuspendTask,
                                  pTask->TaskName,
                                  pTask->Priority,
                                  IpcHiberSuspendHandler,
                                  NULL,
                                  (void *) &(HiberStack[0]),
                                  (UINT32)sizeof(HiberStack),
                                  0U);
    }
    if (Rval != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Can't not create HiberSuspendHandler task.\n", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    } else {
        Rval = AmbaKAL_TaskSetSmpAffinity(&HiberSuspendTask,(pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
        if (Rval != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity is failed! (0x%x)", Rval, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        } else {
            Rval = AmbaKAL_TaskResume(&HiberSuspendTask);
            if (Rval!= OK) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskResume is failed! (%d)", Rval, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
        }
    }

    if (AmbaHiber_InitCallBack != NULL)
        AmbaHiber_InitCallBack();

    return Rval;
}

