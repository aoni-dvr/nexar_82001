/**
 * @file AmbaIPC_Vfs.c
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
 * @details For threadx file system access from Linux through RPMSG
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
//#include "AmbaSysCtrl.h"
#include "AmbaINT.h"
#include "AmbaFS.h"
#include "AmbaShell.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
#include "AmbaIPC_Lock.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_Vfs.h"
#include "AmbaCache.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#if !defined(CONFIG_CC_USESTD)
#include "libwrapc.h"
#else
#include <string.h>
#include <stdlib.h>
#endif


/* Handler pass message to be handled by another command */
#define VFS_ERR_NEXT_CMD (1)

#define tolower(c)          ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
#define VFS_MAX_PAGES       32
#define VFS_QUEUE_SIZE      16
#define VFS_SHADOW_BUF_SIZE (4096*VFS_MAX_PAGES)
/* the big scatter-gether buffers to speed up read/write */
static char sg_buf[2][VFS_SHADOW_BUF_SIZE] __attribute__((aligned(0x1000))) __attribute__((section(".bss.noinit")));
AMBA_KAL_EVENT_FLAG_t sg_buf_flags __attribute__((section(".bss.noinit")));

static AMBA_IPC_HANDLE Channel __attribute__((section(".bss.noinit")));
AMBA_VFS_OP_s VFSOperation __attribute__((section(".bss.noinit")));

#ifndef CONFIG_AMBALINK_RPMSG_SIZE
#define CONFIG_AMBALINK_RPMSG_SIZE      8
#endif

/* The real VFS Messaging buffer */
static char vfs_msg[VFS_QUEUE_SIZE][CONFIG_AMBALINK_RPMSG_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));

/* for RPMSG task */
static AMBA_KAL_MSG_QUEUE_t rpmsg_queue __attribute__((section(".bss.noinit")));
static int rpmsg_queue_base[VFS_QUEUE_SIZE] __attribute__((section(".bss.noinit")));

/* for file operation task */
static AMBA_KAL_MSG_QUEUE_t ops_queue __attribute__((section(".bss.noinit")));
static int ops_queue_base[VFS_QUEUE_SIZE] __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t ops_task __attribute__((section(".bss.noinit")));

/* for scatter-gather task */
static AMBA_KAL_MSG_QUEUE_t sg_queue __attribute__((section(".bss.noinit")));
static int sg_queue_base[VFS_QUEUE_SIZE] __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t sg_task __attribute__((section(".bss.noinit")));
static UINT8 VfsStack[0x4000] GNU_SECTION_NOZEROINIT;
static UINT8 SgStack[0x4000] GNU_SECTION_NOZEROINIT;

static inline void AmbaIPC_Vfs_clrbss(void)
{
    memset(&Channel, 0, sizeof(Channel));
    memset(&VFSOperation, 0, sizeof(VFSOperation));

    memset(sg_buf, 0, sizeof(sg_buf));
    memset(&sg_buf_flags, 0, sizeof(sg_buf_flags));

    memset(vfs_msg, 0, sizeof(vfs_msg));

    memset(&rpmsg_queue, 0, sizeof(rpmsg_queue));
    memset(rpmsg_queue_base, 0, sizeof(rpmsg_queue_base));

    memset(&ops_queue, 0, sizeof(ops_queue));
    memset(ops_queue_base, 0, sizeof(ops_queue_base));
    memset(&ops_task, 0, sizeof(ops_task));

    memset(&sg_queue, 0, sizeof(sg_queue));
    memset(sg_queue_base, 0, sizeof(sg_queue_base));
    memset(&sg_task, 0, sizeof(sg_task));
}


/* support for CFS */


/**
 * File Mode in CFS
 */
typedef enum AMBA_UT_CFS_FILE_MODE_e_ {
    AMBA_UT_CFS_FILE_MODE_READ_ONLY = 0,    /**< "r" */
    AMBA_UT_CFS_FILE_MODE_READ_WRITE,       /**< "r+" */
    AMBA_UT_CFS_FILE_MODE_WRITE_ONLY,       /**< "w" */
    AMBA_UT_CFS_FILE_MODE_WRITE_READ,       /**< "w+" */
    AMBA_UT_CFS_FILE_MODE_APPEND_ONLY,      /**< "a" */
    AMBA_UT_CFS_FILE_MODE_APPEND_READ,      /**< "a+" */
    AMBA_UT_CFS_FILE_MODE_MAX               /**< max */
} AMBA_UT_CFS_FILE_MODE_e;

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  Ascii2Unicode
 *
 *  @Description::  Convert ASCII code to Unicode.
 *
\*----------------------------------------------------------------------------*/
static void Ascii2Unicode(char *AsciiString, char *UnicodeString)
{
    //if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
    //    AmbaLink_Ascii2Unicode(AsciiString, (UINT16 *)UnicodeString);
    //} else {
        strcpy(UnicodeString, AsciiString);
    //}
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  Unicode2Ascii
 *
 *  @Description::  Convert Unicode to ASCII code .
 *
\*----------------------------------------------------------------------------*/
static void Unicode2Ascii(char *UnicodeString, char *AsciiString)
{
    //if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
    //    AmbaLink_Unicode2Ascii((const UINT16 *)UnicodeString, AsciiString);
    //} else {
        strcpy(AsciiString, UnicodeString);
    //}
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RegisterVFSOperation
 *
 *  @Description::  Register the file operation for AmbaFS use.
 *
 *  @param [in] Operation VFS Operation to be registered.
 *  @return 0 if success
 *  @todo Input check
 *
\*----------------------------------------------------------------------------*/
int AmbaIPC_RegisterVFSOperation(AMBA_VFS_OP_s *Operation)
{
    int err = -1;

    if (Operation) {
        memcpy(&VFSOperation, Operation, sizeof(AMBA_VFS_OP_s));
        err = 0;
    }
    return err;
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  MakeTime
 *
 *  @Description::  Convert time/date to epoc time
 *
\*----------------------------------------------------------------------------*/
unsigned int MakeTime(int time, int date)
{
    unsigned int year, mon, day, hour, min, sec;

    sec  = (time & 0x1f) * 2;
    min  = (time >> 5) & 0x3f;
    hour = (time >> 11) & 0x1f;
    day  = (date & 0x1f);
    mon  = ((date >> 5) & 0xf);
    year = ((date >> 9) & 0x7f) + 1980;

    /* 1..12 -> 11,12,1..10 */
    if (0 >= (int) (mon -= 2)) {
        mon += 12;      /* Puts Feb last since it has leap day */
        year -= 1;
    }

    return ((((unsigned long)
              (year/4 - year/100 + year/400 + 367*mon/12 + day) +
               year*365 - 719499
             )*24 + hour /* now have hours */
           )*60 + min /* now have minutes */
         )*60 + sec; /* finally seconds */
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ConvertTimeStamp
 *
 *  @Description::  Conver the time to rtos timestamp
 *
\*----------------------------------------------------------------------------*/
static void ConvertTimeStamp(AMBA_IPC_VFS_TIMESTMP* linux_time, AMBA_FS_SYS_TIME_s* time, AMBA_FS_SYS_DATE_s* date)
{
    date->SysYear = linux_time->year + 1900;
    date->SysMonth = linux_time->month + 1;
    date->SysDay = linux_time->day;

    time->SysHour = linux_time->hour;
    time->SysMin = linux_time->min;
    time->SysSec = linux_time->sec;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AddDtaEntry
 *
 *  @Description::  Add a Dta entry to msg
 *
\*----------------------------------------------------------------------------*/
static AMBA_IPC_VFS_STAT_s* AddDtaEntry(
            AMBA_FS_DTA_t *dta,
            AMBA_IPC_VFS_MSG_s *msg,
            AMBA_IPC_VFS_STAT_s *stat)
{
    char *next_stat;
    int len;

    UNUSED(msg);
    /* populate the reply structure with entry info */
    stat->size = dta->FileSize;
    stat->type = (dta->Attribute & ATTR_DIR) ?
        AMBA_IPC_VFS_STAT_DIR : AMBA_IPC_VFS_STAT_FILE;
    if (dta->LongName[0]) {
        Unicode2Ascii((char *)dta->LongName, stat->name);
        //strcpy(stat->name, dta->LongName);
    } else {
        Unicode2Ascii((char *)dta->FileName, stat->name);
        //strcpy(stat->name, dta->FileName);
    }
    stat->statp = (UINT64)(ULONG) dta;
    stat->atime = MakeTime(dta->ATime, dta->ADate);
    stat->ctime = MakeTime(dta->CTime, dta->CDate);
    stat->mtime = MakeTime(dta->Time, dta->Date);

    len = offsetof(AMBA_IPC_VFS_STAT_s, name) + strlen(stat->name) + 1;
    len = (len + 7) & ~7;
    next_stat = (char*)stat + len;
    return (AMBA_IPC_VFS_STAT_s*)next_stat;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcLsInit
 *
 *  @Description::  process LS_INIT command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcLsInit(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_FS_DTA_t *fs_dta;
    char *path = (char*)msg->parameter;
    char Buf[512];
    AMBA_IPC_VFS_STAT_s *stat = (AMBA_IPC_VFS_STAT_s *)msg->parameter;
    int i = 0, ret;

    fs_dta = AmbaLink_Malloc(MEM_POOL_FS_DTA_BUF, sizeof(AMBA_FS_DTA_t));
    if (!fs_dta) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcLsInit out of memory", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return 0;
    }

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcLsInit %s ", path, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "%p", fs_dta, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    Ascii2Unicode(path, Buf);
    ret = VFSOperation.FirstDirEnt(Buf, ATTR_ALL, fs_dta);
    if (ret == 0) {
        stat = AddDtaEntry(fs_dta, msg, stat);
        for (i = 1; i < msg->flag; i++) {
            ret = VFSOperation.NextDirEnt(fs_dta);
            if (ret == 0)
                stat = AddDtaEntry(fs_dta, msg, stat);
            else
                break;
        }
    } else {
        ret = AmbaFS_GetError();
        //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcLsInit: %d", ret, 0U, 0U, 0U, 0U);
        //AmbaPrint_Flush();
        if (ret != FS_ERR_0002) {
            // not no file entry, set error to linux.
            stat->type = -1;
        }

        stat->statp = (UINT64)(ULONG) fs_dta;
        stat++;
    }

    msg->flag = i;
    AmbaIPC_Send(Channel, msg, (INT32) ((ULONG) stat - (ULONG) msg));

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcLsNext
 *
 *  @Description::  process LS_NEXT command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcLsNext(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_FS_DTA_t *fs_dta;
    AMBA_IPC_VFS_STAT_s *stat = (AMBA_IPC_VFS_STAT_s *)msg->parameter;
    int i, ret;

    fs_dta = (AMBA_FS_DTA_t*)(ULONG) stat->statp;
    if (!fs_dta) {
        return 0;
    }

    for (i = 0; i < msg->flag; i++) {
        ret = VFSOperation.NextDirEnt(fs_dta);
        if (ret == 0)
            stat = AddDtaEntry(fs_dta, msg, stat);
        else
            break;
   }

    msg->flag = i;
    AmbaIPC_Send(Channel, msg, (UINT32) ((ULONG) stat - (ULONG) msg));

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcLsExit
 *
 *  @Description::  process LS_EXIT command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcLsExit(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_FS_DTA_t *fs_dta;
    AMBA_IPC_VFS_STAT_s *stat = (AMBA_IPC_VFS_STAT_s *)msg->parameter;

    fs_dta = (AMBA_FS_DTA_t*)(ULONG)stat->statp;
    if (fs_dta) {
        AmbaLink_Free(MEM_POOL_FS_DTA_BUF, fs_dta);
    }

    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcLsExit %p", fs_dta, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcStat
 *
 *  @Description::  process STAT command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcStat(AMBA_IPC_VFS_MSG_s *msg)
{
    /* we are only instrested in one file */
    msg->flag = 1;

    if (!ProcLsInit(msg)) {
        ProcLsExit(msg);
    }
    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcQuickStat
 *
 *  @Description::  process STAT command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcQuickStat(AMBA_IPC_VFS_MSG_s *msg)
{
    extern UINT32 AmbaIPC_OWNER_IS_LOCAL(UINT32 id);
    AMBA_IPC_VFS_QUICK_STAT_s *qstat;
    char *path = (char*)&(msg->parameter[1]);
    char Drive = (char) tolower((int) path[0]);
    UINT32 Owner = 0, CpsrBackup;
    char Buf[512];
    AMBA_FS_DTA_t *fs_dta;
    int ret;
    ULONG VirtAddr;

    AmbaMMU_PhysToVirt((ULONG)msg->parameter[0], &VirtAddr);
    qstat = (AMBA_IPC_VFS_QUICK_STAT_s *)VirtAddr;
    fs_dta = (AMBA_FS_DTA_t *)AmbaLink_Malloc(MEM_POOL_FS_DTA_BUF, sizeof(AMBA_FS_DTA_t));

    if (!fs_dta) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcQuickStat out of memory", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return 0;
    }

    if (Drive == 'a' || Drive == 'b') {
        if (AmbaIPC_OWNER_IS_LOCAL(AMBA_IPC_MUTEX_NAND))
            Owner = 1;
    } else if (Drive == 'c') {
        if (AmbaIPC_OWNER_IS_LOCAL(AMBA_IPC_MUTEX_SD0))
            Owner = 1;
    } else {
            Owner = 0;
    }

    if (Owner == 1) {
        // Mutex owner is ThreadX.
        //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcQuickStat %s ", path, NULL, NULL, NULL, NULL);
        //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "%p", qstat, 0U, 0U, 0U, 0U);
        //AmbaPrint_Flush();
        Ascii2Unicode(path, Buf);
        ret = VFSOperation.FirstDirEnt(Buf, ATTR_ALL, fs_dta);
        if (ret == 0) {
            qstat->type = (fs_dta->Attribute & ATTR_DIR) ? AMBA_IPC_VFS_STAT_DIR : AMBA_IPC_VFS_STAT_FILE;
        } else {
            qstat->type = AMBA_IPC_VFS_STAT_NULL;
        }
    } else {
        qstat->type = AMBA_IPC_VFS_STAT_NULL;
    }

    AmbaKAL_CriticalSectionEnter(&CpsrBackup);
    /* Make write magic and cache handling be atomic. */
    /* Then wake up Linux immediately to speed up the "ls" operation. */
    qstat->magic = AMBAFS_QSTAT_MAGIC;
    //AmbaCache_DataFlush((UINT32)&qstat->statp, 64);

    AmbaKAL_CriticalSectionExit(CpsrBackup);

    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcQuickStat type = 0x%x magic = 0x%x", qstat->type, qstat->magic, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    if (fs_dta) {
        AmbaLink_Free(MEM_POOL_FS_DTA_BUF, fs_dta);
    }

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcOpen
 *
 *  @Description::  process OPEN command, return a file handle
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcOpen(AMBA_IPC_VFS_MSG_s *msg)
{
    char *path = (char*)&msg->parameter[1];
    char *mode;
    char Buf[512];
    AMBA_FS_FILE *fp;
    int Err = 0;
    int cfs = 0;
    AMBA_VFS_FILE_PARAM_s cfs_param;

    mode = (char *) &(msg->parameter[0]);

    Ascii2Unicode(path, Buf);

    if (VFSOperation.fopen != NULL) { //use CFS
        cfs = 1;
        VFSOperation.GetFileParam(&cfs_param);
        strcpy(cfs_param.Filename, Buf);

        if(!strcmp(mode, "r")) {
            cfs_param.Mode = AMBA_UT_CFS_FILE_MODE_READ_ONLY;
        } else if (!strcmp(mode, "r+")) {
            cfs_param.Mode = AMBA_UT_CFS_FILE_MODE_READ_WRITE;
        } else if (!strcmp(mode, "w")) {
            cfs_param.Mode = AMBA_UT_CFS_FILE_MODE_WRITE_ONLY;
        } else if (!strcmp(mode, "w+")) {
            cfs_param.Mode = AMBA_UT_CFS_FILE_MODE_WRITE_READ;
        } else if (!strcmp(mode, "a")) {
            cfs_param.Mode = AMBA_UT_CFS_FILE_MODE_APPEND_ONLY;
        } else if (!strcmp(mode, "a+")) {
            cfs_param.Mode = AMBA_UT_CFS_FILE_MODE_APPEND_READ;
        }
    }

    if(cfs == 0)
        AmbaFS_FileOpen(Buf, mode, &fp);
    else
        fp = VFSOperation.fopen(&cfs_param);

    if (fp == NULL) {
        Err = AmbaFS_GetError();
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "file open error (%d)", Err, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcOpen %s mode %s, -->", path, mode, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "%p", fp, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    msg->parameter[0] = (UINT64)(ULONG) fp;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFS_MSG_s)+8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcClose
 *
 *  @Description::  process CLOSE command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcClose(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_FS_FILE *fp;

    fp = (AMBA_FS_FILE *)(ULONG)msg->parameter[0];
    VFSOperation.fclose(fp);
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFS_MSG_s));
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcClose %p", fp, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcReadGetBuffer
 *
 *  @Description::  only get the big buffer for READ command
 *      * First try to get a big buffer
 *      * Then send that buffer to sg task which reads and copies it to target pages
 *      * Note we assume that there are no more than VFS_MAX_PAGES pages in
 *        total and there is no hole in those pages.
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcReadGetBuffer(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_IPC_VFS_IO_s *io = (AMBA_IPC_VFS_IO_s*)msg->parameter;
    int  idx;
    UINT32 flags;

    if (io->total > VFS_MAX_PAGES) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VFS: Too much pages(%d) to read", io->total, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return 0;
    }

    /* grab a free shadow buffer */
    idx = AmbaKAL_EventFlagGet(&sg_buf_flags, 0x3, 0U, 0U, &flags,
                           AMBA_KAL_WAIT_FOREVER);
    if (idx != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagGet failed (%d)", idx, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    msg->flag = (flags & 0x1) ? 0 : 1;

    idx = AmbaKAL_EventFlagClear(&sg_buf_flags, 1<<msg->flag);
    if (idx != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagClear failed (%d)", idx, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    /* send the shadow buffer to sg task to fill the page */
    msg->cmd = AMBA_IPC_VFS_CMD_SCATTER;
    idx = AmbaKAL_MsgQueueSend(&ops_queue, &msg, AMBA_KAL_WAIT_FOREVER);
    if (idx != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_MsgQueueSend failed (%d)", idx, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
    }
    /* return non-zero here since the msg is sent explicitly to sg task */
    return VFS_ERR_NEXT_CMD;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcReadScatter
 *
 *  @Description::  Read to big buffer and copy data from big buffer to pages
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcReadScatter(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_IPC_VFS_IO_s *io = (AMBA_IPC_VFS_IO_s*)msg->parameter;
    AMBA_IPC_VFS_BH_s *bh = io->bh;
    AMBA_FS_FILE      *fp = (AMBA_FS_FILE *)(ULONG) io->fp;
    char *shadow;
    int  idx, bytes, Rval, pos, splice_len = 0;
    ULONG VirtAddr;
    UINT64 offset;

    pos = msg->flag & 0x1;
    shadow = sg_buf[pos];

    for (idx = io->total - 1; idx >= 0; idx--)
        splice_len += bh[idx].len;

    VFSOperation.ftell(fp, &offset);
    if ((int)offset != bh->offset) {
        Rval = VFSOperation.fseek(fp, (UINT64)bh->offset, PF_SEEK_SET);
        if (Rval != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "fseek failed (%d)", Rval, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            msg->flag = 0xFF;
            goto out;
        }
    }

    /* read the bytes into the shadow buffer first */
    for(idx = 0; idx < splice_len; ) {
        VFSOperation.fread(shadow+idx, 1,
                                         (splice_len - idx), fp, &bytes);
        if (bytes <= 0) {
            if (bytes != 0) {
                UINT64 FilePos;

                AmbaFS_FileTell(fp, &FilePos);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcReadScatter error(%d), %d", bytes, (int)FilePos, 0U, 0U, 0U);
                AmbaPrint_Flush();
                msg->flag = 0xFF;
                goto out;
            }
            break;
        }
        idx += bytes;
    }

    for (idx = 0; idx < io->total; idx++) {
        AmbaMMU_PhysToVirt((ULONG)bh[idx].addr, &VirtAddr);
        memcpy((void *)VirtAddr, shadow, bh[idx].len);
        shadow += bh[idx].len;
    }

out:
    AmbaIPC_Send(Channel, msg, msg->len);
    idx = AmbaKAL_EventFlagSet(&sg_buf_flags, 1<<pos);
    if (idx != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagClear failed (%d)", idx, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcGather
 *
 *  @Description::  process WRITE command, gather all data into shadow buffer
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcGather(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_IPC_VFS_IO_s *io = (AMBA_IPC_VFS_IO_s*)msg->parameter;
    AMBA_IPC_VFS_BH_s *bh = io->bh;
    char *shadow;
    int  idx, splice_len;
    UINT32 flags;
    ULONG VirtAddr;

    if (io->total > VFS_MAX_PAGES) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "VFS: Too much pages(%d) to write", io->total, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        return 0;
    }

    /* grab a free shadow buffer */
    idx = AmbaKAL_EventFlagGet(&sg_buf_flags, 0x3, 0U, 0U, &flags,
                       AMBA_KAL_WAIT_FOREVER);
    if (idx != OK){
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagClear failed (%d)", idx, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    msg->flag = (flags & 0x1) ? 0 : 1;
    shadow = sg_buf[msg->flag];

    idx = AmbaKAL_EventFlagClear(&sg_buf_flags, 1<<msg->flag);
    if (idx != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagClear failed (%d)", idx, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    /* check if we can gather the write */
    for (idx = flags = 0; idx < io->total - 1; idx++) {
        if (bh[idx].offset + bh[idx].len != bh[idx+1].offset) {
            flags = 1;
            break;
        }
    }

    if (flags) {
        msg->flag |= 0x80;
    } else {
        for (idx = splice_len = 0; idx < io->total; idx++) {
            AmbaMMU_PhysToVirt((ULONG)bh[idx].addr, &VirtAddr);
            memcpy(shadow + splice_len,(void *) VirtAddr, bh[idx].len);
            splice_len += bh[idx].len;
        }
    }

    /* send the msg to ops task to perform the write */
    msg->cmd = AMBA_IPC_VFS_CMD_DOWRITE;
    idx = AmbaKAL_MsgQueueSend(&ops_queue, &msg, AMBA_KAL_WAIT_FOREVER);
    if (idx != OK) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_MsgQueueSend failed (%d)", idx, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
    }
    /* return non-zero here since the msg is sent explicitly to sg task */
    return VFS_ERR_NEXT_CMD;
}

 /*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcWrite
 *
 *  @Description::  Write to fs
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcWrite(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_IPC_VFS_IO_s *io = (AMBA_IPC_VFS_IO_s*)(ULONG)msg->parameter;
    AMBA_IPC_VFS_BH_s *bh = io->bh, *end;
    AMBA_FS_FILE      *fp = (AMBA_FS_FILE *)(ULONG)io->fp;
    char *shadow;
    int  idx, splice_len, sg_idx, is_gather;
    ULONG VirtAddr;
    UINT32 bytes;
    UINT64 offset;

    sg_idx = msg->flag & 0x1;
    is_gather = !(msg->flag & 0x80);
    shadow = sg_buf[sg_idx];

    if (is_gather) {
        for (idx = splice_len = 0; idx < io->total; idx++) {
            splice_len += bh[idx].len;
        }

        VFSOperation.ftell(fp, &offset);
        if ((int)offset != bh->offset)
            VFSOperation.fseek(fp, (UINT64)bh->offset, PF_SEEK_SET);

        for(idx = 0; idx < splice_len; ) {
            VFSOperation.fwrite(shadow+idx, 1,
                                             (splice_len - idx), fp, &bytes);
            if (bytes <= 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcWrite error(%d), %p %d", bytes, (UINT32)(ULONG)fp, (UINT32)(ULONG)bh->offset, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }
            idx += bytes;
        }
    } else {
        for (end = bh + io->total; bh < end; bh++) {
            VFSOperation.ftell(fp, &offset);
            if ((int)offset != bh->offset)
                VFSOperation.fseek(fp, (UINT64)bh->offset, PF_SEEK_SET);
            AmbaMMU_PhysToVirt((ULONG) bh->addr, &VirtAddr);
            VFSOperation.fwrite((void *)VirtAddr, 1, bh->len, fp, &bytes);
            if (bytes <= 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcWrite error(%d), %p %d", bytes, (UINT32)(ULONG)fp, (UINT32)(ULONG)bh->offset, 0U, 0U);
                AmbaPrint_Flush();
                break;
            } else if ((int)bytes != bh->len) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcWrite error (bytes != bh->len) %d, %d", bytes, bh->len, 0U, 0U, 0U);
                AmbaPrint_Flush();
                break;
            }
        }
    }

    VFSOperation.FSync(fp);

    idx = AmbaKAL_EventFlagSet(&sg_buf_flags, 1<<sg_idx);
    if (idx != OK) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_EventFlagClear failed (%d)!", idx, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    AmbaIPC_Send(Channel, msg, msg->len);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcCreate
 *
 *  @Description::  process CREATE command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcCreate(AMBA_IPC_VFS_MSG_s *msg)
{
    char *path;
    char Buf[512];
    AMBA_FS_FILE *fp;
    AMBA_VFS_FILE_PARAM_s cfs_param;
    int cfs = 0;

    path = (char*)msg->parameter;
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcCreate %s", path, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();

    Ascii2Unicode(path, Buf);
    if(VFSOperation.fopen != NULL){
        cfs = 1;
        VFSOperation.GetFileParam(&cfs_param);
        strcpy(cfs_param.Filename, Buf);
        cfs_param.Mode = AMBA_UT_CFS_FILE_MODE_WRITE_ONLY;
    }

    if(cfs == 0)
        AmbaFS_FileOpen(Buf, "w", &fp);
    else
        fp = VFSOperation.fopen(&cfs_param);

    VFSOperation.fclose(fp);
    return ProcStat(msg);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcDelete
 *
 *  @Description::  process DELETE command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcDelete(AMBA_IPC_VFS_MSG_s *msg)
{
    char *path;
    char Buf[512];

    path = (char*)msg->parameter;
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcDelete %s", path, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();
    Ascii2Unicode(path, Buf);
    VFSOperation.remove(Buf);
    return ProcStat(msg);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcMakeDir
 *
 *  @Description::  process MKDIR command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcMakeDir(AMBA_IPC_VFS_MSG_s *msg)
{
    char *path;
    char Buf[512];

    path = (char*)msg->parameter;
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcMakeDir %s", path, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();
    Ascii2Unicode(path, Buf);
    VFSOperation.Mkdir(Buf);
    return ProcStat(msg);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcRemoveDir
 *
 *  @Description::  process RMDIR command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcRemoveDir(AMBA_IPC_VFS_MSG_s *msg)
{
    char *path;
    char Buf[512];

    path = (char*)msg->parameter;
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcRemoveDir %s", path, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();
    Ascii2Unicode(path, Buf);
    VFSOperation.Rmdir(Buf);
    return ProcStat(msg);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcRename
 *
 *  @Description::  process RENAME command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcRename(AMBA_IPC_VFS_MSG_s *msg)
{
    char *old_path, *new_path;
    AMBA_FS_FILE_INFO_s Stat;
    int Rval;

    new_path = (char*)msg->parameter;
    old_path = new_path + strlen(new_path) + 1;
    // Following syntax of Linux "rename" system call,
    // one must delete dst if it exists
    Rval = VFSOperation.Stat(new_path, &Stat);
    if (!Rval) {
        if (Stat.Attr & ATTR_DIR)
            Rval = VFSOperation.Rmdir(new_path);
        else
            Rval = VFSOperation.remove(new_path);

        if (Rval) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "rename %s to %s, fail to remove destination", old_path, new_path, NULL, NULL, NULL);
            AmbaPrint_Flush();
            goto rename_exit;
        }
    }

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcRename %s-->%s", old_path, new_path, NULL, NULL, NULL);
    //AmbaPrint_Flush();
    Rval = VFSOperation.Move(old_path, new_path);

rename_exit:
    if (Rval != OK) {
        AMBA_IPC_VFS_STAT_s *stat = (AMBA_IPC_VFS_STAT_s*)msg->parameter;
        stat->type = AMBA_IPC_VFS_STAT_NULL;
        AmbaIPC_Send(Channel, msg,
            sizeof(AMBA_IPC_VFS_STAT_s)+sizeof(AMBA_IPC_VFS_MSG_s));
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "!!! Rename failed !!!", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return 0;
    }

    return ProcStat(msg);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcMount
 *
 *  @Description::  process Mount command
 *
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcMount(AMBA_IPC_VFS_MSG_s *msg)
{
    msg->flag = 1;  //Always mount successfully

    return AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFS_MSG_s));
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcUmount
 *
 *  @Description::  process Umount command
 *      Nothing is necessary currently for this command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcUmount(AMBA_IPC_VFS_MSG_s *msg)
{
    msg->flag = 1;
    return AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFS_MSG_s));
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcVolSize
 *
 *  @Description::  process Volume_Size command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcVolSize(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_FS_DRIVE_INFO_t DevInfo;
    int Rval;
    char Drive = (char) msg->parameter[0];

    Rval = VFSOperation.GetDev(Drive, &DevInfo);
    if (Rval != 0) {
        msg->flag = 1;  /* failure */
        memset(msg->parameter, 0x0, 4 * sizeof(UINT64));
    } else {
        msg->flag = 0;  /* success */
        msg->parameter[0] = DevInfo.TotalClusters;                /* Total clusters */
        msg->parameter[1] = DevInfo.EmptyClusters;                /* Unused clusters */
        msg->parameter[2] = DevInfo.SectorsPerCluster * DevInfo.BytesPerSector;  /* Cluster size in bytes */
        msg->parameter[3] = DevInfo.FormatType;                /* Format type defined in ThreadX side */
    }

    return AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFS_MSG_s) + 4 * sizeof(UINT64));
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcSetTime
 *
 *  @Description::  process Set timestamp command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcSetTime(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_IPC_VFS_STAT_TIMESTMP_s *stat = (AMBA_IPC_VFS_STAT_TIMESTMP_s *)msg->parameter;
    AMBA_FS_TIMESTMP_s stamp;
    char *path = (char *)stat->name;
    int Err, ret = 0;
    char Buf[512];

    memset(&stamp, 0, sizeof(AMBA_FS_TIMESTMP_s));
    ConvertTimeStamp(&stat->ctime, &stamp.Createtime, &stamp.CreateDate);
    ConvertTimeStamp(&stat->mtime, &stamp.ModifiedTime, &stamp.ModifiedDate);
    ConvertTimeStamp(&stat->atime, &stamp.AccessTime, &stamp.AccessDate);

    Ascii2Unicode(path, Buf);
    Err = AmbaFS_SetTimeStamp(Buf, &stamp);

    if(Err) {
        Err = AmbaFS_GetError();
        /*
         * Set timestamp will fail if the file is open already.
         * Ignore this error.
         */
        if (Err == FS_ERR_000D) {
            ret = 0;
        } else {
            ret = Err;
            /* hide error print */
            //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Error code %d", Err, 0U, 0U, 0U, 0U);
            //AmbaPrint_Flush();
        }
    }

    msg->parameter[0] = ret;
    return AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFS_MSG_s) + 8);
}

static int ProcSetSize(AMBA_IPC_VFS_MSG_s *msg)
{
    AMBA_IPC_VFS_STAT_SIZE_s *stat = (AMBA_IPC_VFS_STAT_SIZE_s *)msg->parameter;
    char *path = (char *)stat->name;
    int Err, ret = 0;
    char Buf[512];

    Ascii2Unicode(path, Buf);
    Err = AmbaFS_FileTruncate(Buf, stat->size);

    if(Err) {
        Err = AmbaFS_GetError();
        /*
         * Set timestamp will fail if the file is open already.
         * Ignore this error.
         */
        if (Err == FS_ERR_000D) {
            ret = 0;
        } else {
            ret = Err;
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Error code %d", Err, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    msg->parameter[0] = ret;
    return AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFS_MSG_s) + 8);
}

typedef int (*PROC_FUNC)(AMBA_IPC_VFS_MSG_s *);
static PROC_FUNC proc_list[] = {
    ProcLsInit,
    ProcLsNext,
    ProcLsExit,
    ProcStat,
    ProcOpen,
    ProcClose,
    ProcReadGetBuffer,
    ProcGather, /* AMBA_IPC_VFS_CMD_WRITE */
    ProcCreate,
    ProcDelete,
    ProcMakeDir,
    ProcRemoveDir,
    ProcRename,
    ProcMount,
    ProcUmount,
    ProcReadScatter,
    ProcWrite,
    ProcVolSize,
    ProcQuickStat,
    ProcSetTime,
    ProcSetSize,
};

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcMsg
 *
 *  @Description::  Process VFS msg
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static void *ProcMsg(void *parm)
{
    AMBA_KAL_MSG_QUEUE_t *queue = (AMBA_KAL_MSG_QUEUE_t*)parm;
    AMBA_IPC_VFS_MSG_s *msg;
    int cmd, ret;

    while (1) {
        if (AmbaKAL_MsgQueueReceive(queue, &msg, AMBA_KAL_WAIT_FOREVER))
            continue;

        cmd = msg->cmd;
        if (cmd >= 0 && cmd < (int)(sizeof(proc_list)/sizeof(proc_list[0]))) {
            ret = proc_list[cmd](msg);
        } else {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Ignore unknown proc %d", cmd, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            ret = 0;
        }

        /*  FIXME: Only proc exection succeeds, return vfs_msg back to rpmsg_queue.
         *         If proc execution fails, vfs_msg will become fewer.
         */
        if (!ret)
            AmbaKAL_MsgQueueSend(&rpmsg_queue, &msg, AMBA_KAL_WAIT_FOREVER);
        else {
            if (ret != VFS_ERR_NEXT_CMD) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Failed to handle proc %d", cmd, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            } else {
                // Note: The final command should return 0 to make msg be queued.
            }
        }
    }
    return NULL;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RpmsgCB
 *
 *  @Description::  relay incomding VFS msg to target task
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int RpmsgCB(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    AMBA_IPC_VFS_MSG_s *msg;
    AMBA_IPC_VFS_MSG_s *dst;

    UNUSED(IpcHandle);
    AmbaKAL_MsgQueueReceive(&rpmsg_queue, &dst, AMBA_KAL_WAIT_FOREVER);
    msg = pMsgCtrl->pMsgData;
    msg->len = pMsgCtrl->Length;
    memcpy(dst, msg, msg->len);

    if (msg->cmd == AMBA_IPC_VFS_CMD_WRITE || msg->cmd == AMBA_IPC_VFS_CMD_READ)
        AmbaKAL_MsgQueueSend(&sg_queue, &dst, AMBA_KAL_WAIT_FOREVER);
    else
        AmbaKAL_MsgQueueSend(&ops_queue, &dst, AMBA_KAL_WAIT_FOREVER);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  InitVFSOperation
 *
 *  @Description::  Init VFS File Operation
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int InitVFSOperation(void)
{
    VFSOperation.fopen = NULL;
    VFSOperation.fclose = (UINT32 (*)(void *))AmbaFS_FileClose;
    VFSOperation.fread = (UINT32 (*)(void *, UINT32, UINT32, void *, UINT32 *))AmbaFS_FileRead;
    VFSOperation.fwrite = (UINT32 (*)(void *, UINT32, UINT32, void *, UINT32 *))AmbaFS_FileWrite;
    VFSOperation.fseek = (UINT32 (*)(void *, INT64, INT32))AmbaFS_FileSeek;
    VFSOperation.ftell = (UINT32 (*)(void *, UINT64 *))AmbaFS_FileTell;
    VFSOperation.FSync = (UINT32 (*)(void *))AmbaFS_FileSync;
    VFSOperation.fappend = (UINT32 (*)(void *, UINT64, UINT64 *))AmbaFS_ClusterAppend;
    VFSOperation.feof = (UINT32 (*)(void *))AmbaFS_FileEof;
    VFSOperation.remove = AmbaFS_Remove;
    VFSOperation.Move = AmbaFS_Move;
    VFSOperation.Stat = (UINT32 (*)(const char *, void *))AmbaFS_GetFileInfo;
    VFSOperation.Chmod = AmbaFS_ChangeFileMode;
    VFSOperation.FirstDirEnt = (UINT32 (*)(const char *, UINT8, void *))AmbaFS_FileSearchFirst;
    VFSOperation.NextDirEnt = (UINT32 (*)(void *))AmbaFS_FileSearchNext;
    VFSOperation.Combine = AmbaFS_FileCombine;
    VFSOperation.Divide = AmbaFS_FileDivide;
    VFSOperation.Cinsert = AmbaFS_ClusterInsert;
    VFSOperation.Cdelete = AmbaFS_ClusterDelete;
    VFSOperation.Mkdir = AmbaFS_MakeDir;
    VFSOperation.Rmdir = AmbaFS_RemoveDir;
    VFSOperation.GetDev = (UINT32 (*)(char, void *))AmbaFS_GetDriveInfo;
    VFSOperation.Format = AmbaFS_Format;
    VFSOperation.Sync = AmbaFS_Sync;

    return 0;
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_VfsInit
 *
 *  @Description::  Init VFS module
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_VfsInit(void)
{
    int status = 0;
    AMBA_LINK_TASK_CTRL_s *pTask;

    /* Variable initialization */
    AmbaIPC_Vfs_clrbss();

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_VFS_OPS];

     /* Initialize the VFS file operation */
    InitVFSOperation();

    if (AmbaKAL_MsgQueueCreate(&rpmsg_queue, "rpmsg_queue", sizeof(int), rpmsg_queue_base, sizeof(rpmsg_queue_base))) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create VFS RPMSG queue", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    } else {
        void *addr;
        for (status = 0; status < VFS_QUEUE_SIZE; status++) {
            addr = (void *)(ULONG) vfs_msg[status];
            AmbaKAL_MsgQueueSend(&rpmsg_queue, &addr, AMBA_KAL_WAIT_FOREVER);
        }
    }

    if (AmbaKAL_MsgQueueCreate(&ops_queue, "ops_queue", sizeof(int),ops_queue_base, sizeof(ops_queue_base))) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create VFS operation queue", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    if (AmbaKAL_MsgQueueCreate(&sg_queue, "sg_queue", sizeof(int),sg_queue_base, sizeof(sg_queue_base))) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create VFS scatter-gather queue", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    if (AmbaKAL_TaskCreate(&ops_task,
                           pTask->TaskName,
                           pTask->Priority,
                           ProcMsg,
                           (void *) &ops_queue,
                           (void *) &(VfsStack[0]),
                           (UINT32)sizeof(VfsStack),
                           0U)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create VFS operation task", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    status = AmbaKAL_TaskSetSmpAffinity(&ops_task, (pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
    if (status) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity is failed! (0x%x)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    status = AmbaKAL_TaskResume(&ops_task);
    if (status) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskResume is failed! (%d)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_VFS_SG];

    if (AmbaKAL_TaskCreate(&sg_task,
                           pTask->TaskName,
                           pTask->Priority,
                           ProcMsg,
                           (void *) &sg_queue,
                           (void *) &(SgStack[0]),
                           (UINT32)sizeof(SgStack),
                           0U)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create VFS scatter-gather task", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    status = AmbaKAL_TaskSetSmpAffinity(&sg_task, (pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
    if (status) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity is failed! (0x%x)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    status = AmbaKAL_TaskResume(&sg_task);
    if (status) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskResume is failed! (%d)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    if (AmbaKAL_EventFlagCreate(&sg_buf_flags,"SgBufFlags")) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create VFS scatter-gather event flags", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    } else {
        AmbaKAL_EventFlagSet(&sg_buf_flags, 0x3);
    }

#ifdef CONFIG_OPENAMP
    Channel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "aipc_vfs", RpmsgCB);
#else
    Channel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "aipc_vfs", RpmsgCB);
#endif
    if (Channel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }
    AmbaIPC_RegisterChannel(Channel, NULL);

}
