/**
 * @file AmbaIPC_FuseVfs.c
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
 * @details For threadx file system access from Linux user space through RPMSG
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "AmbaShell.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_FuseVfs.h"
#include "AmbaUtility.h"
#include "AmbaMMU.h"
#include "libwrapc.h"
#include "AmbaCSL_TMR.h"

//#define ENABLE_FUSEVFS_DEBUG_MSG       1
#ifdef ENABLE_FUSEVFS_DEBUG_MSG
#define DEBUG_MSG           1
#else
#define DEBUG_MSG           0
#endif
#define VFS_QUEUE_SIZE      16

extern AMBA_VFS_OP_s VFSOperation;
extern int InitVFSOperation(void);
static AMBA_IPC_HANDLE Channel;

#ifndef CONFIG_AMBALINK_RPMSG_SIZE
#define CONFIG_AMBALINK_RPMSG_SIZE      8
#endif

/* The real VFS Messaging buffer */
static char vfs_msg[VFS_QUEUE_SIZE][CONFIG_AMBALINK_RPMSG_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE)));

/* for RPMSG task */
static AMBA_KAL_MSG_QUEUE_t rpmsg_queue;
static int rpmsg_queue_base[VFS_QUEUE_SIZE];

/* for file operation task */
static AMBA_KAL_MSG_QUEUE_t ops_queue;
static int ops_queue_base[VFS_QUEUE_SIZE];
static AMBA_KAL_TASK_t ops_task;
static UINT8 FuseVfsStack[0x4000] GNU_SECTION_NOZEROINIT;

extern unsigned int MakeTime(int time, int date);

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
 *  @RoutineName::  ProcOpen
 *
 *  @Description::  process OPEN command, return a file handle
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcOpen(AMBA_IPC_VFFS_MSG_s *msg)
{
    AMBA_IPC_VFFS_OPEN_s *open_arg = (AMBA_IPC_VFFS_OPEN_s *)msg->parameter;
    char *path = (char*) open_arg->file;
    char *mode;
    char Buf[512];
    AMBA_FS_FILE *fp;
    AMBA_VFS_FILE_PARAM_s cfs_param;
    int Err = 0;
    int cfs = 0;

    mode = (char *) &(open_arg->mode);
    Ascii2Unicode(path, Buf);
    if(DEBUG_MSG) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "ProcOpen %s mode %s, -->", path, mode, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "%p", (UINT32)fp, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    if(VFSOperation.fopen != NULL) {
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
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "file open error (%d)", Err, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }


    msg->parameter[0] = (UINT64)(ULONG)fp;
    msg->parameter[1] = Err;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 16);

    return Err;
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
static int ProcClose(AMBA_IPC_VFFS_MSG_s *msg)
{
    AMBA_FS_FILE *fp;

    fp = (AMBA_FS_FILE*)(ULONG)(msg->parameter[0]);
    VFSOperation.fclose(fp);
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s));

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcRead
 *
 *  @Description::  Read data
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcRead(AMBA_IPC_VFFS_MSG_s *msg)
{
    AMBA_IPC_VFS_IO_s *io = (AMBA_IPC_VFS_IO_s*)msg->parameter;
    AMBA_FS_FILE      *fp = io->fp;
    char *shadow;
    int splice_len = io->total;
    int idx, bytes;
    UINT32 VirtAddr;

    AmbaMMU_PhysToVirt((ULONG)io->buf, (ULONG *)&VirtAddr);
    shadow = (char *)VirtAddr;
    if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: read file fp ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "0x%u", (UINT32)fp, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
    }
    /* read the bytes into the shadow buffer first */
    for(idx = 0; idx < splice_len; ) {
        VFSOperation.fread(shadow+idx, 1, splice_len - idx, fp, &bytes);
        if (bytes <= 0) {
            if (bytes != 0) {
                if(DEBUG_MSG) {
                    UINT64 offset;

                    VFSOperation.ftell(fp, &offset);
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcRead error(%d), %d", bytes, (int)offset, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                }
            }
            break;
        }
        idx += bytes;
    }

    msg->parameter[0] = idx;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
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
static int ProcWrite(AMBA_IPC_VFFS_MSG_s *msg)
{
    AMBA_IPC_VFS_IO_s *io = (AMBA_IPC_VFS_IO_s*)msg->parameter;
    AMBA_FS_FILE      *fp = io->fp;
    char *shadow;
    int splice_len = io->total;
    int idx;
    UINT32 VirtAddr,bytes;

    AmbaMMU_PhysToVirt((ULONG)io->buf, (ULONG *)&VirtAddr);
    shadow = (char *)VirtAddr;
    /* write the bytes into the shadow buffer first */
    for(idx = 0; idx < splice_len; ) {
        VFSOperation.fwrite(shadow+idx, 1, splice_len - idx, fp, &bytes);
        if (bytes <= 0) {
            if (bytes != 0) {
                if(DEBUG_MSG) {
                    UINT64 offset;

                    VFSOperation.ftell(fp, &offset);
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "ProcWrite error(%d), %d", bytes, (int)offset, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                }
            }
            break;
        }
        idx += bytes;
    }

    msg->parameter[0] = idx;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

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
static int ProcStat(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal;
    AMBA_FS_DTA_t dta;
    char Buf[512];
    AMBA_IPC_VFFS_STAT_s *pStat;
    char *filename;
    int len, root_dir = 0;
    UINT32 VirtAddr;

    AmbaMMU_PhysToVirt((ULONG)msg->parameter[0], (ULONG *)&VirtAddr);
    pStat = (AMBA_IPC_VFFS_STAT_s *)VirtAddr;
    filename = (char *)&msg->parameter[1];
    len = strlen(filename);

    /* replace c:\ with c:\* */
    if(!strcmp(&filename[len-2], ":\\")) {
        strcat(filename, "*");
        root_dir = 1;
    }

    Ascii2Unicode(filename, Buf);

    rVal = VFSOperation.FirstDirEnt(Buf, ATTR_ALL, &dta);
    if(rVal) {
        rVal = AmbaFS_GetError();
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "failed, filename %s and error code is %d", (UINT32)&msg->parameter[1], rVal, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
        goto done;
    }

    pStat->fs_type = 0;
    pStat->fstfz = dta.FileSize;
    pStat->fstact = pStat->fstut = pStat->fstct = MakeTime(dta.Time, dta.Date);
    pStat->fstad = pStat->fstud = pStat->fstcd = dta.Date;

    if(root_dir) {
        /* correct the attribute of c:\ */
        pStat->fstat = ATTR_DIR;
    } else {
        pStat->fstat = dta.Attribute;
    }

done:
    pStat->rval = rVal;
    AmbaIPC_Send(Channel, msg, msg->len);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcSeek
 *
 *  @Description::  process seek command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcSeek(AMBA_IPC_VFFS_MSG_s *msg)
{
    AMBA_IPC_VFFS_SEEK_s *arg = (AMBA_IPC_VFFS_SEEK_s*)msg->parameter;
    int rVal;

    rVal = VFSOperation.fseek((void *)arg->fp, arg->offset, arg->origin);
    if(rVal) {
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: seek file failed", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

    msg->parameter[0] = rVal;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcRemove
 *
 *  @Description::  process unlink command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcRemove(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal;
    char Buf[512];

    Ascii2Unicode((char *)msg->parameter, Buf);
    rVal = VFSOperation.remove(Buf);
    if(rVal) {
        rVal = AmbaFS_GetError();
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "remove %s failed, rval is %d", (UINT32)msg->parameter, rVal, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    msg->parameter[0] = rVal;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcFSFirst
 *
 *  @Description::  process fsfirst command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcFSFirst(AMBA_IPC_VFFS_MSG_s *msg)
{
    AMBA_IPC_VFFS_FSFIRST_s *arg = (AMBA_IPC_VFFS_FSFIRST_s*)msg->parameter;
    int rVal;
    AMBA_FS_DTA_t *dta;
    AMBA_IPC_VFFS_FSFIND_s *fsfind;
    char Buf[512];
    char *filename;
//    int len;
    UINT32 VirtAddr;

//    len = strlen((char *) arg->path);
    filename = (char *)arg->path;
    AmbaMMU_PhysToVirt((ULONG)arg->res, (ULONG *)&VirtAddr);
    fsfind = (AMBA_IPC_VFFS_FSFIND_s *) VirtAddr;
    Ascii2Unicode(filename, Buf);
    dta = AmbaLink_Malloc(MEM_POOL_FS_DTA_BUF, sizeof(AMBA_FS_DTA_t));

    if(DEBUG_MSG) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s dst %s", __func__, filename, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "failed, filename %s and error code is %d", (UINT32)&msg->parameter[1], rVal, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    rVal = VFSOperation.FirstDirEnt(Buf, ATTR_ALL, dta);
    fsfind->rval = rVal;

    if(rVal) {
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s failed", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        fsfind->dta = NULL;
        fsfind->Time = 0;
        fsfind->Date = fsfind->FileSize = fsfind->Attribute = 0;
        memset(fsfind->FileName, 0x0, VFFS_SHORT_NAME_LEN);
        memset(fsfind->LongName, 0x0, VFFS_LONG_NAME_LEN);

        if(dta) {
            AmbaLink_Free(MEM_POOL_FS_DTA_BUF, dta);
        }

    } else {
        fsfind->dta = (void *)dta;
        fsfind->Time = dta->Time;
        fsfind->Date = dta->Date;
        fsfind->FileSize = dta->FileSize;
        fsfind->Attribute = dta->Attribute;
        Unicode2Ascii((char *)dta->FileName, fsfind->FileName);
        Unicode2Ascii((char *)dta->LongName, fsfind->LongName);
    }

    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s));

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcFSNext
 *
 *  @Description::  process fsnext command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcFSNext(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal;
    AMBA_FS_DTA_t *dta;
    AMBA_IPC_VFFS_FSFIND_s *fsfind;
    UINT32 VirtAddr;

    dta = (AMBA_FS_DTA_t *)(ULONG)msg->parameter[0];
    AmbaMMU_PhysToVirt((ULONG)msg->parameter[1], (ULONG *)&VirtAddr);
    fsfind = (AMBA_IPC_VFFS_FSFIND_s *)VirtAddr;
    rVal = VFSOperation.NextDirEnt(dta);
    fsfind->rval = rVal;

    if(rVal) {
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s failed", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
        if(dta)
            AmbaLink_Free(MEM_POOL_FS_DTA_BUF, dta);

        fsfind->dta = 0;
        fsfind->Time = 0;
        fsfind->Date = fsfind->FileSize = fsfind->Attribute = 0;
        memset(fsfind->FileName, 0x0, VFFS_SHORT_NAME_LEN);
        memset(fsfind->LongName, 0x0, VFFS_LONG_NAME_LEN);
    } else {
        fsfind->dta = (void *)dta;
        fsfind->Time = dta->Time;
        fsfind->Date = dta->Date;
        fsfind->FileSize = dta->FileSize;
        fsfind->Attribute = dta->Attribute;
        Unicode2Ascii((char *) dta->FileName, fsfind->FileName);
        Unicode2Ascii((char *) dta->LongName, fsfind->LongName);
    }

    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcChmod
 *
 *  @Description::  process chmod command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcChmod(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal, attr;
    char Buf[512];
    AMBA_FS_FILE_INFO_s Stat;
    int mode_u, mode_g, mode_o;
    unsigned int write_enable = 0;

    attr = msg->parameter[0];
    Ascii2Unicode((char *)&msg->parameter[1], Buf);

    rVal = VFSOperation.Stat(Buf, &Stat);
    if(rVal) {
        goto done;
    }

    mode_u = attr / 100;
    mode_g = (attr - (mode_u * 100)) / 10;
    mode_o = attr - (mode_u * 100) - (mode_g * 10);

    /* FIXME : Only support read only en/disable. */
    write_enable = (mode_u & 0x2) & (mode_g & 0x2) & (mode_o & 0x2);
    if (write_enable) {
        Stat.Attr &= (~ATTR_RDONLY);
    } else {
        Stat.Attr |= ATTR_RDONLY;
    }

    rVal = VFSOperation.Chmod(Buf, Stat.Attr);

    if(rVal) {
        rVal = AmbaFS_GetError();
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "failed and error code is %d", rVal, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

done:
    msg->parameter[0] = rVal;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcGetDev
 *
 *  @Description::  process getdev command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcGetDev(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal;
    AMBA_IPC_VFFS_GETDEV_s *vffs_dev_info;
    AMBA_FS_DRIVE_INFO_t DevInf;
    char Buf[512];
    UINT32 VirtAddr;

    AmbaMMU_PhysToVirt((ULONG)msg->parameter[0], (ULONG *)&VirtAddr);
    vffs_dev_info = (AMBA_IPC_VFFS_GETDEV_s *)VirtAddr;
    Ascii2Unicode((char *)&msg->parameter[1], Buf);

    rVal = VFSOperation.GetDev(Buf[0], &DevInf);
    if(rVal) {
        goto done;
    }

    vffs_dev_info->cls = DevInf.TotalClusters;
    vffs_dev_info->ecl = DevInf.EmptyClusters;
    vffs_dev_info->bps = DevInf.BytesPerSector;
    vffs_dev_info->spc = DevInf.SectorsPerCluster;
    vffs_dev_info->cpg = DevInf.ClustersPerGroup;
    vffs_dev_info->ecg = DevInf.EmptyClusterGroups;
    vffs_dev_info->fmt = DevInf.FormatType;

done:
    msg->parameter[0] = rVal;

    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcRename
 *
 *  @Description::  process rename command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcRename(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal;
    AMBA_IPC_VFFS_RENAME_s *arg = (AMBA_IPC_VFFS_RENAME_s *)msg->parameter;
    char Buf1[512], Buf2[512];
    char *SrcName, *DstName;
    AMBA_FS_FILE_INFO_s Stat;

    SrcName = (char *)arg->filename;
    DstName = (char *)&arg->filename[arg->src_len];

    Ascii2Unicode(SrcName, Buf1);
    Ascii2Unicode(DstName, Buf2);

    /* Check whether the file attribute is read-only */
    rVal = VFSOperation.Stat(Buf1, &Stat);
    if(rVal) {
        rVal = AmbaFS_GetError();
        goto done;
    }

    // Following syntax of Linux "rename" system call,
    // one must delete dst if it exists
    rVal = VFSOperation.Stat(Buf2, &Stat);
    if(!rVal) {
        if(Stat.Attr & ATTR_DIR) {
            rVal = VFSOperation.Rmdir(Buf2);
        }
        else {
            rVal = VFSOperation.remove(Buf2);
        }

        if(rVal) {
            rVal = AmbaFS_GetError();
            goto done;
        }
    }

    rVal = AmbaFS_Rename(Buf1, Buf2);
    if(rVal) {
        rVal = AmbaFS_GetError();
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "failed and error code is %d", rVal, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

done:
    msg->parameter[0] = rVal;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcMkdir
 *
 *  @Description::  process mkdir command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcMkdir(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal;
    char Buf[512];

    Ascii2Unicode((char *)msg->parameter, Buf);
    rVal = VFSOperation.Mkdir(Buf);
    if(rVal) {
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s failed", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

    msg->parameter[0] = rVal;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ProcRmdir
 *
 *  @Description::  process rmdir command
 *
 *  @Return     ::
 *      int : Rlease msg(0)/keep using msg(1)
 *
\*----------------------------------------------------------------------------*/
static int ProcRmdir(AMBA_IPC_VFFS_MSG_s *msg)
{
    int rVal;
    char Buf[512];

    Ascii2Unicode((char *)msg->parameter, Buf);
    rVal = VFSOperation.Rmdir(Buf);
    if(rVal) {
        rVal = AmbaFS_GetError();
        if(DEBUG_MSG) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s path %s failed error code ", __func__, (char *)msg->parameter, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "%d", rVal, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }

    msg->parameter[0] = rVal;
    AmbaIPC_Send(Channel, msg, sizeof(AMBA_IPC_VFFS_MSG_s) + 8);

    return 0;
}

typedef int (*PROC_FUNC)(AMBA_IPC_VFFS_MSG_s *);
static PROC_FUNC proc_list[] = {
    ProcOpen,
    ProcRead,
    ProcWrite,
    ProcClose,
    ProcStat,
    ProcSeek,
    ProcRemove,
    ProcFSFirst,
    ProcFSNext,
    ProcChmod,
    ProcGetDev,
    ProcRename,
    ProcMkdir,
    ProcRmdir,
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
    AMBA_IPC_VFFS_MSG_s *msg;
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
    }
    return NULL;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RpmsgCB
 *
 *  @Description::  relay incomding VFFS msg to target task
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int RpmsgCB(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    AMBA_IPC_VFFS_MSG_s *msg;
    AMBA_IPC_VFFS_MSG_s *dst;

    UNUSED(IpcHandle);
    AmbaKAL_MsgQueueReceive(&rpmsg_queue, &dst, AMBA_KAL_WAIT_FOREVER);
    msg = pMsgCtrl->pMsgData;
    msg->len = pMsgCtrl->Length;
    memcpy(dst, msg, msg->len);

    AmbaKAL_MsgQueueSend(&ops_queue, &dst, AMBA_KAL_WAIT_FOREVER);

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
 void AmbaIPC_FuseVfsInit(void)
 {
    int status  = 0;
    AMBA_LINK_TASK_CTRL_s *pTask;

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_FUSE];

    /* To check whether vfs operation is set. */
    if (VFSOperation.fclose == NULL) {
        InitVFSOperation();
    }

    if (AmbaKAL_MsgQueueCreate(&rpmsg_queue, "rpmsg_queue", sizeof(int), rpmsg_queue_base, sizeof(rpmsg_queue_base))) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create FUSE RPMSG queue", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    } else {
        INT32 addr;
        for (status= 0; status < VFS_QUEUE_SIZE; status++) {
            addr = (INT32)vfs_msg[status];
            AmbaKAL_MsgQueueSend(&rpmsg_queue, &addr, AMBA_KAL_WAIT_FOREVER);
        }
    }

    if (AmbaKAL_MsgQueueCreate(&ops_queue, "ops_queue", sizeof(int), ops_queue_base, sizeof(ops_queue_base))) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create FUSE queue", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    if (AmbaKAL_TaskCreate(&ops_task, pTask->TaskName, pTask->Priority,
            ProcMsg, (void *) &ops_queue,
            (void *) &(FuseVfsStack[0]), (UINT32)sizeof(FuseVfsStack), 0U)) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create FUSE task", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }
    status = AmbaKAL_TaskSetSmpAffinity(&ops_task,(pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
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

#ifdef CONFIG_OPENAMP
    Channel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "aipc_fuse_vffs", RpmsgCB);
#else
    Channel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "aipc_fuse_vffs", RpmsgCB);
#endif
    if (Channel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }
    AmbaIPC_RegisterChannel(Channel, NULL);

 }
