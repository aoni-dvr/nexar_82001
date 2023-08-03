/**
*  @file SvcStgMgr.c
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
*  @details svc storage manager
*
*/

#include "AmbaKAL.h"
#include "AmbaSD.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#ifdef CONFIG_BUILD_COMMON_SERVICE_DCF
#include "AmbaDCF.h"
#endif
#include "SvcErrCode.h"
#include "SvcStgMgr.h"
#ifdef CONFIG_BUILD_COMAMBA_CODEC_COMMON
#include "AmbaVfs.h"
#endif
#include "SvcTask.h"
#include "SvcWrap.h"
#include "SvcLog.h"

#define SVC_STG_MGR_ACT_DELCHK    (0U)
#define SVC_STG_MGR_ACT_UPDATE    (1U)

#define SVC_STG_MGR_OP_ADD        (0U)
#define SVC_STG_MGR_OP_SUB        (1U)
#define SVC_STG_MGR_OP_ASSIGN     (2U)

#define SVC_STG_MGR_LOG           "STGMGR"
#define SVC_STG_MGR_STACK_SIZE    (0x4000U)
#define SVC_STG_MGR_QUEUE_SIZE    (128U)
#define DRIVE_NUM                 (26U)

#define SVC_STG_MGR_FLG_UPDATE_DONE (0x1U)

typedef struct {
    char   Drive;
    UINT8  Action;
} STG_MGR_DATA_s;

static void*   StgMgr_TaskEntry(void* EntryArg);
static void    FreeSizeOp(UINT8 DriveNum, UINT8 Operator, UINT64 Value);
#ifdef CONFIG_BUILD_COMAMBA_CODEC_COMMON
static void    VfsWriteCallBack(const AMBA_VFS_FILE_s *pVfsFile, void *pParam);
#endif
static UINT32  GetDriveNumber(char Drive, UINT8 *DriveNumber);

static UINT64                       FreeSpaceArr[DRIVE_NUM] = {0U};
static UINT64                       SpaceThresholdArr[DRIVE_NUM] = {0U};
static STG_MGR_DATA_s               StgMgrMsgQue[SVC_STG_MGR_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MSG_QUEUE_t         StgMgrMsgQueId GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MUTEX_t             StgMgrFreeSpaceMutex GNU_SECTION_NOZEROINIT;
static DeletFileCB                  DeletFileCallBack = NULL;
static AMBA_KAL_EVENT_FLAG_t        StgMgrFlg GNU_SECTION_NOZEROINIT;

static void FreeSizeOp(UINT8 DriveNum, UINT8 Operator, UINT64 Value)
{
    if (SVC_OK != AmbaKAL_MutexTake(&StgMgrFreeSpaceMutex, AMBA_KAL_WAIT_FOREVER)) {
        SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_MutexTake failed!!", 0U, 0U);
    }

    if (SVC_STG_MGR_OP_ADD == Operator) {
        FreeSpaceArr[DriveNum] += Value;
    } else if (SVC_STG_MGR_OP_SUB == Operator) {
        FreeSpaceArr[DriveNum] -= Value;
    } else if (SVC_STG_MGR_OP_ASSIGN == Operator) {
        FreeSpaceArr[DriveNum] = Value;
    } else {
        SvcLog_NG(SVC_STG_MGR_LOG, "Operator(%u) not supported!!", (UINT32)Operator, 0U);
    }

    if (SVC_OK != AmbaKAL_MutexGive(&StgMgrFreeSpaceMutex)) {
        SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_MutexGive failed!!", 0U, 0U);
    }
}


static void* StgMgr_TaskEntry(void* EntryArg)
{
    UINT8                 Action, DriveNum = 0U, Update;
    UINT32                Rval = SVC_OK, Err;
    char                  Drive;
    UINT64                FileSize = 0U, FreeSize = 0U;
    STG_MGR_DATA_s        Info;
    AMBA_FS_DRIVE_INFO_t  DriveInfo;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(StgMgrMsgQue);

    while (Rval == SVC_OK) {
        Rval = AmbaKAL_MsgQueueReceive(&StgMgrMsgQueId, &Info, AMBA_KAL_WAIT_FOREVER);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_STG_MGR_LOG, "Wait msg queue failed!!", 0U, 0U);
            continue;
        }

        Drive  = Info.Drive;
        Action = Info.Action;
        Update = 0U;

        if (SVC_OK == GetDriveNumber(Drive, &DriveNum)) {
            if (Action == SVC_STG_MGR_ACT_DELCHK) {
                SvcStgMgr_GetFreeSize(Drive, &FreeSize);
                while (FreeSize < SpaceThresholdArr[DriveNum]) {
                    /* Delete oldest file */
                    if (SVC_OK != DeletFileCallBack(Drive, &FileSize)) {
                        //SvcLog_NG(SVC_STG_MGR_LOG, "DeleteOldestFile failed!!", 0U, 0U);
                        break;
                    } else {
                        FreeSizeOp(DriveNum, SVC_STG_MGR_OP_ADD, FileSize);
                    }
                    SvcStgMgr_GetFreeSize(Drive, &FreeSize);
                    Update = 1U;
                }
            }

            if ((Action == SVC_STG_MGR_ACT_UPDATE) || (Update == 1U)) {
                if (AMBA_FS_ERR_NONE != AmbaFS_GetDriveInfo(Drive, &DriveInfo)) {
                    FreeSizeOp(DriveNum, SVC_STG_MGR_OP_ASSIGN, 0ULL);
                    SvcLog_DBG(SVC_STG_MGR_LOG, "Reset Drive space to 0", 0U, 0U);
                } else {
                    FreeSize = (UINT64)DriveInfo.BytesPerSector * (UINT64)DriveInfo.SectorsPerCluster * (UINT64)DriveInfo.EmptyClusters;
                    FreeSizeOp(DriveNum, SVC_STG_MGR_OP_ASSIGN, FreeSize);
                    SvcLog_DBG(SVC_STG_MGR_LOG, "Drive free space = %u MB", (UINT32)((FreeSize / (UINT64)1024U) / (UINT64)1024U), 0U);
                }

                if (Action == SVC_STG_MGR_ACT_UPDATE) {
                    Err = AmbaKAL_EventFlagSet(&StgMgrFlg, SVC_STG_MGR_FLG_UPDATE_DONE);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_EventFlagSet failed!!", 0U, 0U);
                    }
                }
            }
        }
    }

    return NULL;
}

#ifdef CONFIG_BUILD_COMAMBA_CODEC_COMMON
static void VfsWriteCallBack(const AMBA_VFS_FILE_s *pVfsFile, void *pParam)
{
    const UINT64 *pWriteSize;
    UINT8        DriveNumber = 0U;

    AmbaMisra_TouchUnused(pParam);
    AmbaMisra_TypeCast(&pWriteSize, &pParam);

    if (SVC_OK == GetDriveNumber(pVfsFile->Drive, &DriveNumber)) {
        FreeSizeOp(DriveNumber, SVC_STG_MGR_OP_SUB, *pWriteSize);
    } else {
        SvcLog_NG(SVC_STG_MGR_LOG, "VfsWriteCallBack failed!!", 0U, 0U);
    }

}
#endif

static UINT32 GetDriveNumber(char Drive, UINT8 *DriveNumber)
{
    UINT32 Rval = SVC_OK;
    UINT8  DriveNum = (UINT8)Drive;

    if ((DriveNum <= 90U) && (DriveNum >= 65U)) {
        *DriveNumber = DriveNum - 65U;
    } else if ((DriveNum <= 122U) && (DriveNum >= 97U)) {
        *DriveNumber = (DriveNum - 32U) - 65U;
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_STG_MGR_LOG, "The drive is not supported!!", 0U, 0U);
    }

    return Rval;
}


/**
* initialization of storage manager module
* @param [in] Priority task priority
* @param [in] CpuBits task cpu bits
* @param [in] CallBackFunc callback function of file delete
* @return 0-OK, 1-NG
*/
UINT32 SvcStgMgr_Init(UINT32 Priority, UINT32 CpuBits, DeletFileCB CallBackFunc)
{
    UINT32                  Rval = SVC_OK;
    static UINT8            StgMgrStack[SVC_STG_MGR_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_TASK_CTRL_s  TaskCtrl GNU_SECTION_NOZEROINIT;
    static char             MsgQueName[] = "StgMgr_MsgQue";
    static char             MutexName[]  = "StgMgr_Mutex";
    static char             FlgName[]    = "StgMgr_Flg";

    /* queue create */
    if (KAL_ERR_NONE != AmbaKAL_MsgQueueCreate(&StgMgrMsgQueId, MsgQueName, (UINT32)sizeof(STG_MGR_DATA_s),
                                               &StgMgrMsgQue[0], SVC_STG_MGR_QUEUE_SIZE * (UINT32)sizeof(STG_MGR_DATA_s))) {
        SvcLog_NG(SVC_STG_MGR_LOG, "StgMgr_MsgQue created failed!", 0U, 0U);
        Rval = SVC_NG;
    }

    /* flag create */
    if (KAL_ERR_NONE != AmbaKAL_EventFlagCreate(&StgMgrFlg, FlgName)) {
        SvcLog_NG(SVC_STG_MGR_LOG, "StgMgr_Flg created failed!", 0U, 0U);
        Rval = SVC_NG;
    }

    /* Mutex create */
    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&StgMgrFreeSpaceMutex, MutexName)) {
        SvcLog_NG(SVC_STG_MGR_LOG, "StgMgr_Mutex created failed!", 0U, 0U);
        Rval = SVC_NG;
    }

    if (NULL != CallBackFunc) {
        DeletFileCallBack = CallBackFunc;
    } else {
        SvcLog_NG(SVC_STG_MGR_LOG, "the call back function is null!", 0U, 0U);
        Rval = SVC_NG;
    }

#ifdef CONFIG_BUILD_COMAMBA_CODEC_COMMON
    /* Register vfs callback function */
    AmbaVFS_RegCbFunc((UINT32)AMBA_VFS_CB_TYPE_WRITE, VfsWriteCallBack);
#endif

    /* task create */
    if (SVC_OK == Rval) {
        TaskCtrl.Priority    = Priority;
        TaskCtrl.EntryFunc   = StgMgr_TaskEntry;
        TaskCtrl.EntryArg    = 0U;
        TaskCtrl.pStackBase  = StgMgrStack;
        TaskCtrl.StackSize   = SVC_STG_MGR_STACK_SIZE;
        TaskCtrl.CpuBits     = CpuBits;

        if (SVC_OK != SvcTask_Create("StgMgr_Task", &TaskCtrl)) {
            SvcLog_NG(SVC_STG_MGR_LOG, "StgMgr_Task created failed!", 0U, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

/**
* free space get of storage
* @param [in] Drive drive of storage
* @param [out] pFreeSize size of free space
* @return none
*/
void SvcStgMgr_GetFreeSize(char Drive, UINT64 *pFreeSize)
{
    UINT8 DriveNumber = 0U;

    if (SVC_OK == GetDriveNumber(Drive, &DriveNumber)) {
        if (SVC_OK != AmbaKAL_MutexTake(&StgMgrFreeSpaceMutex, AMBA_KAL_WAIT_FOREVER)) {
            SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_MutexTake failed!!", 0U, 0U);
        }

        *pFreeSize = FreeSpaceArr[DriveNumber];

        if (SVC_OK != AmbaKAL_MutexGive(&StgMgrFreeSpaceMutex)) {
            SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_MutexGive failed!!", 0U, 0U);
        }

    } else {
        SvcLog_NG(SVC_STG_MGR_LOG, "SvcStgMgr_GetFreeSize failed!!", 0U, 0U);
    }
}

/**
* configure threshold of free space
* @param [in] Drive drive of storage
* @param [in] FreeSizeThr threshold of free space
* @return 0-OK, 1-NG
*/
UINT32 SvcStgMgr_SetFreeSizeThr(char Drive, UINT64 FreeSizeThr)
{
    UINT32 Rval = SVC_OK;
    UINT8  DriveNumber = 0U;

    if (SVC_OK == GetDriveNumber(Drive, &DriveNumber)) {
        SpaceThresholdArr[DriveNumber] = FreeSizeThr;
    } else {
        SvcLog_NG(SVC_STG_MGR_LOG, "SvcStgMgr_SetFreeSizeThr failed!!", 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* do checking of free space
* @param [in] Drive drive of storage
* @return none
*/
void SvcStgMgr_TrigDelCheck(char Drive)
{
    STG_MGR_DATA_s  Info;
    UINT8           DriveNumber = 0U;

    if (SVC_OK == GetDriveNumber(Drive, &DriveNumber)) {
        Info.Drive     = Drive;
        Info.Action    = SVC_STG_MGR_ACT_DELCHK;
        if (KAL_ERR_NONE != AmbaKAL_MsgQueueSend(&StgMgrMsgQueId, &Info, AMBA_KAL_NO_WAIT)) {
            SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_MsgQueueSend failed!!", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_STG_MGR_LOG, "SvcStgMgr_FreeSizeGuar failed!!", 0U, 0U);
    }
}

/**
* do checking of free space with storage device
* @param [in] Drive task priority
* @param [in] TimeOut wait update done timeout
* @return none
*/
void SvcStgMgr_TrigUpdFreeSize(char Drive, UINT32 TimeOut)
{
    STG_MGR_DATA_s  Info;
    UINT8           DriveNumber = 0U;
    UINT32          Err, ActualFlag = 0U;

    if (SVC_OK == GetDriveNumber(Drive, &DriveNumber)) {
        Info.Drive     = Drive;
        Info.Action    = SVC_STG_MGR_ACT_UPDATE;

        Err = AmbaKAL_EventFlagClear(&StgMgrFlg, SVC_STG_MGR_FLG_UPDATE_DONE);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_EventFlagClear failed!!", 0U, 0U);
        }

        Err = AmbaKAL_MsgQueueSend(&StgMgrMsgQueId, &Info, AMBA_KAL_NO_WAIT);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_STG_MGR_LOG, "AmbaKAL_MsgQueueSend failed!!", 0U, 0U);
        }

        if (TimeOut > 0U) {
            Err = AmbaKAL_EventFlagGet(&StgMgrFlg,
                                       SVC_STG_MGR_FLG_UPDATE_DONE,
                                       AMBA_KAL_FLAGS_ALL,
                                       AMBA_KAL_FLAGS_CLEAR_AUTO,
                                       &ActualFlag,
                                       TimeOut);
            if (Err != KAL_ERR_NONE) {
                SvcLog_DBG(SVC_STG_MGR_LOG, "get SVC_STG_MGR_FLG_UPDATE_DONE timeout", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_STG_MGR_LOG, "SvcStgMgr_UpdateFreeSize failed!!", 0U, 0U);
    }
}
