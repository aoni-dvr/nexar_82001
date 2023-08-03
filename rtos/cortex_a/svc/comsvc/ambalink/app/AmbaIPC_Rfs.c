/**
 * @file AmbaIPC_Rfs.c
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
 * @details Allows RTOS to access Linux ramfs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaMMU.h"
#include "libwrapc.h"
#include "AmbaCSL_TMR.h"
#include "AmbaIPC_Rfs.h"

#define AMBA_IPC_RFS_CMD_OPEN       0
#define AMBA_IPC_RFS_CMD_CLOSE      1
#define AMBA_IPC_RFS_CMD_READ       2
#define AMBA_IPC_RFS_CMD_WRITE      3
#define AMBA_IPC_RFS_CMD_TELL       4
#define AMBA_IPC_RFS_CMD_SEEK       5
#define AMBA_IPC_RFS_CMD_REMOVE     6
#define AMBA_IPC_RFS_CMD_MKDIR      7
#define AMBA_IPC_RFS_CMD_RMDIR      8
#define AMBA_IPC_RFS_CMD_MOVE       9
#define AMBA_IPC_RFS_CMD_CHMOD      10
#define AMBA_IPC_RFS_CMD_CHDMOD     11
#define AMBA_IPC_RFS_CMD_MOUNT      12
#define AMBA_IPC_RFS_CMD_UMOUNT     13
#define AMBA_IPC_RFS_CMD_SYNC       14
#define AMBA_IPC_RFS_CMD_FSYNC      15
#define AMBA_IPC_RFS_CMD_STAT       16
#define AMBA_IPC_RFS_CMD_GETDEV     17
#define AMBA_IPC_RFS_CMD_FEOF       18
#define AMBA_IPC_RFS_CMD_OPENDIR    19
#define AMBA_IPC_RFS_CMD_READDIR    20
#define AMBA_IPC_RFS_CMD_CLOSEDIR   21
#define AMBA_IPC_RFS_CMD_CHDIR      22

#define AMBA_IPC_RFS_MODE_RD     1
#define AMBA_IPC_RFS_MODE_WR     2

#define AMBA_IPC_RFS_REPLY_OK    0
#define AMBA_IPC_RFS_REPLY_NODEV 1

#define AMBA_IPC_RFS_Q_SIZE      8

#define AMBA_IPC_MSGBUF_SIZE    (512)

//#define BUFFERED_RFS_IO

typedef struct {
    UINT8       msg_type;       /**< msg_type */
    UINT8       xprt;           /**< xprt */
    UINT16      msg_len;        /**< msg_len */
    void*       reply;          /**< reply */
    UINT64      parameter[0];   /**< parameter */
} AMBA_IPC_RFS_MSG_s;

typedef struct {
    int     mode;       /**< mode */
    char    name[0];    /**< name */
} AMBA_IPC_RFS_DENT_s;

typedef struct {
    UINT64  fp;         /**< fp */
    int     origin;     /**< origin */
    INT64   offset;     /**< offset */
} AMBA_IPC_RFS_SEEK_s;

typedef struct {
    UINT64  fp;         /**< fp */
} AMBA_IPC_RFS_CLOSE_s, AMBA_IPC_RFS_TELL_s, AMBA_IPC_RFS_FSYNC_s, AMBA_IPC_RFS_FEOF_s;

typedef struct {
    UINT64 fp;          /**< fp */
    int  size;          /**< size */
    UINT64 data;        /**< data */
} AMBA_IPC_RFS_IO_s;

typedef struct {
    int     old_name_size;  /**< old name size */
    int     new_name_size;  /**< new name size */
    char    name[0];        /**< name */
} AMBA_IPC_RFS_MOVE_s;

typedef struct {
    int dev_name_size;      /**< dev_name_size */
    int dir_name_size;      /**< dir_name_size */
    int fs_name_size;       /**< fs_name_size */
    char name[0];           /**< name */
} AMBA_IPC_RFS_MOUNT_s;

typedef struct {
    int                 status;     /**< status */
    UINT64              stat;       /**< stat */
    char                name[0];    /**< name */
} AMBA_IPC_RFS_STAT_MSG_s;

typedef struct {
    int                 status;     /**< status */
    UINT64              devinf;     /**< devinf */
    char                name[0];    /**< name */
} AMBA_IPC_RFS_STATFS_MSG_s;

typedef struct {
    UINT64                  dirp;   /**< dirp */
    AMBA_IPC_RFS_DIRENT_s   dirent; /**< dirent */
} AMBA_IPC_RFS_DIR_s;

static AMBA_KAL_EVENT_FLAG_t rpmsg_flags;
static AMBA_KAL_MSG_QUEUE_t  rpmsg_queue;
static int rpmsg_queue_base[AMBA_IPC_RFS_Q_SIZE];
static AMBA_IPC_HANDLE Channel;
static int MsgBuf[AMBA_IPC_MSGBUF_SIZE];

#ifdef BUFFERED_RFS_IO
#define IO_BUFF_SIZE 4096*16
#pragma data_alignment = 0x1000
static char io_buff[IO_BUFF_SIZE];
static AMBA_KAL_MUTEX_t io_lock;
#endif

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RpmsgCB
 *
 *  @Description::  Rpmsg handler
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int RpmsgCB(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    AMBA_IPC_RFS_MSG_s  *in_msg, *out_msg;
    int xprt_idx;

    UNUSED(IpcHandle);
    in_msg =  (AMBA_IPC_RFS_MSG_s *)pMsgCtrl->pMsgData;
    AmbaCache_DataInvalidate((UINT32)in_msg, pMsgCtrl->Length);
    out_msg = (AMBA_IPC_RFS_MSG_s *)in_msg->reply;
    memcpy(out_msg, in_msg, pMsgCtrl->Length);

    xprt_idx = in_msg->xprt;
    AmbaKAL_MsgQueueSend(&rpmsg_queue, &xprt_idx, AMBA_KAL_WAIT_FOREVER);
    AmbaKAL_EventFlagSet(&rpmsg_flags, 1<<xprt_idx);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  ExecRpmsg
 *
 *  @Description::  Send a rfs msg and wait on reply
 *
 *  @Input      ::
 *      msg :  rfs message
 *
\*----------------------------------------------------------------------------*/
static void ExecRpmsg(AMBA_IPC_RFS_MSG_s *msg)
{
    int xprt_idx;
    UINT32 reqFlag, dummy;

    AmbaKAL_MsgQueueReceive(&rpmsg_queue, &xprt_idx, AMBA_KAL_WAIT_FOREVER);
    msg->xprt = xprt_idx;
    msg->reply = msg;

    AmbaCache_DataClean((UINT32)msg,(UINT32)msg->msg_len);
    AmbaIPC_Send(Channel, msg, msg->msg_len);

    reqFlag = 1 << xprt_idx;
    AmbaKAL_EventFlagGet(&rpmsg_flags, reqFlag, 1U, 1U,
                          &dummy, AMBA_KAL_WAIT_FOREVER);
    return;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_dent
 *
 *  @Description::  General operation involoves a file/dir name
 *
 *  @Input      ::
 *      cmd:    RFS command to be executed
 *      name:   the name of the target file/directory
 *      mode:   depends on @cmd
 *
 *  @Return     ::
 *      int : interpolation depends on @cmd
\*----------------------------------------------------------------------------*/
static UINT64 AmbaIPC_dent(int cmd, const char *name, int mode)
{
    AMBA_IPC_RFS_MSG_s  *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_DENT_s *param = (AMBA_IPC_RFS_DENT_s*)msg->parameter;

    strcpy(param->name, name);
    param->mode   = mode;
    msg->msg_type = cmd;
    msg->msg_len  = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_DENT_s) +
        strlen(param->name) + 1;
    ExecRpmsg(msg);
    return msg->parameter[0];
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_fio
 *
 *  @Description::  read from a remote file
 *
 *  @Input      ::
 *      buf :   buf to read to
 *      size :  size of data
 *      fp :    file pointer
 *      cmd :   read or write
 *
 *  @Return     ::
 *      int : OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
static UINT64 AmbaIPC_fio(void *data, int size, UINT64 fp, int cmd)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_IO_s  *param = (AMBA_IPC_RFS_IO_s*)msg->parameter;
    UINT32 PhysAddr;

    msg->msg_type = cmd;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s)  + sizeof(AMBA_IPC_RFS_IO_s);
    param->fp = fp;
    AmbaMMU_VirtToPhys((ULONG)data, (ULONG *)&PhysAddr);
    param->data = (UINT64) (PhysAddr);
    param->size = size;
    ExecRpmsg(msg);
    return msg->parameter[0];
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_fopen
 *
 *  @Description::  Open a remote file
 *
 *  @Input      ::
 *      name:   the name of the target file
 *      mode:   "r" or "w"
 *
 *  @Return     ::
 *      int : file ID(>0) or Error(<=0)
\*----------------------------------------------------------------------------*/
UINT64 AmbaIPC_fopen(const char *name, char *mode_str)
{
    int mode;

    switch (mode_str[0]) {
    case 'r':
        mode = AMBA_IPC_RFS_MODE_RD;
        break;
    case 'w':
        mode = AMBA_IPC_RFS_MODE_WR;
        break;
    default:
        return 0;
    }
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_OPEN, name, mode);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_fclose
 *
 *  @Description::  Close a remote file
 *
 *  @Input      ::
 *      fp :  file pointer returned by AmbaIPC_fopen
 *
 *  @Return     ::
 *      int : OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
int AmbaIPC_fclose(UINT64 fp)
{
    AMBA_IPC_RFS_MSG_s  *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_CLOSE_s *param = (AMBA_IPC_RFS_CLOSE_s*)msg->parameter;

    param->fp = fp;
    msg->msg_type = AMBA_IPC_RFS_CMD_CLOSE;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s)  + sizeof(AMBA_IPC_RFS_CLOSE_s);
    ExecRpmsg(msg);
    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_fread
 *
 *  @Description::  read from a remote file
 *
 *  @Input      ::
 *      buf :   buf to read to
 *      size :  size of data
 *      fp :    file pointer
 *
 *  @Return     ::
 *      int :   bytes read
\*----------------------------------------------------------------------------*/
int AmbaIPC_fread(void *data, int size, UINT64 fp)
{
 #ifdef BUFFERED_RFS_IO
    char *dest = (char*)data;
    int bytes_to_read, bytes_read;

    AmbaKAL_MutexTake(&io_lock, AMBA_KAL_WAIT_FOREVER);

    AmbaCache_DataInvalidate((UINT32)data,(UINT32)size);

    while (size > 0) {
        /* read up to IO_BUFF_SIZE bytes */
        bytes_to_read = (size <= IO_BUFF_SIZE) ? size : IO_BUFF_SIZE;

        bytes_read = AmbaIPC_fio(io_buff, bytes_to_read,
            fp, AMBA_IPC_RFS_CMD_READ);

        /* break out on err */
        if (bytes_read < 0) {
            AmbaKAL_MutexGive(&io_lock);
            return bytes_read;
        }

        /* finish reading */
        if(bytes_read == 0) {
            break;
        }
        memcpy(dest, io_buff, bytes_read);
        dest += bytes_read;
        size -= bytes_read;
    }
    AmbaKAL_MutexGive(&io_lock);
    return dest - (char*)data;
#else
   /*
     * We need to FLUSH cache here and now, otherwise RTOS might flush the
     * @data later when LINUX is reading data from its file system to @data,
     * which makes values in @data corrupted.
     */

    int ret;

    AmbaCache_DataInvalidate((UINT32)data,(UINT32)size);
    ret = AmbaIPC_fio(data, size, fp, AMBA_IPC_RFS_CMD_READ);

    return ret;
#endif
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_fwrite
 *
 *  @Description::  write to a remote file
 *
 *  @Input      ::
 *      buf :   buf to write from
 *      size :  size of data
 *      fp :    file pointer
 *
 *  @Return     ::
 *      int :   bytes written
\*----------------------------------------------------------------------------*/
int AmbaIPC_fwrite(void *data, int size, UINT64 fp)
{
#ifdef BUFFERED_RFS_IO
    char *src = (char*)data;
    int bytes_to_write, bytes_written;

    AmbaKAL_MutexTake(&io_lock, AMBA_KAL_WAIT_FOREVER);
    AmbaCache_DataClean((UINT32)data,(UINT32)size);
    while (size > 0) {
        /* write up to IO_BUFF_SIZE bytes */
        bytes_to_write = (size <= IO_BUFF_SIZE) ? size : IO_BUFF_SIZE;
        memcpy(io_buff, src, bytes_to_write);
        bytes_written = AmbaIPC_fio(io_buff, bytes_to_write,
            fp, AMBA_IPC_RFS_CMD_WRITE);

        /* break out on err */
        if (bytes_written < 0) {
            AmbaKAL_MutexGive(&io_lock);
            return bytes_written;
        }
        src  += bytes_written;
        size -= bytes_written;
    }
    AmbaKAL_MutexGive(&io_lock);
    return src - (char*)data;
#else
    AmbaCache_DataClean((UINT32)data,(UINT32)size);
    return AmbaIPC_fio(data, size, fp, AMBA_IPC_RFS_CMD_WRITE);
#endif
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_fseek
 *
 *  @Description::  seek to a postion
 *
 *  @Input      ::
 *      fp:     file pointer
 *      offset: number of bytes to offset from @orgin
 *      orgin:  reference position for @offset
 *
 *  @Return     ::
 *      int :   OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
int AmbaIPC_fseek(UINT64 fp,  INT64 offset, int orgin)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_SEEK_s  *param = (AMBA_IPC_RFS_SEEK_s*)msg->parameter;

    msg->msg_type = AMBA_IPC_RFS_CMD_SEEK;
    msg->msg_len  = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_SEEK_s);
    param->fp     = fp;
    param->origin = orgin;
    param->offset = offset;
    ExecRpmsg(msg);
    return (INT32)msg->parameter[0];
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_ftell
 *
 *  @Description::  seek to a postion
 *
 *  @Input      ::
 *      fp:     file pointer
 *
 *  @Return     ::
 *      int :   OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
INT64 AmbaIPC_ftell(UINT64 fp)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_TELL_s  *param = (AMBA_IPC_RFS_TELL_s*)msg->parameter;

    msg->msg_type = AMBA_IPC_RFS_CMD_TELL;
    msg->msg_len  = sizeof(AMBA_IPC_RFS_MSG_s)  + sizeof(AMBA_IPC_RFS_TELL_s);
    param->fp     = fp;
    ExecRpmsg(msg);
    return msg->parameter[0];
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_remove
 *
 *  @Description::  Remove a remote file
 *
 *  @Input      ::
 *      name: the name of the target file
 *
 *  @Return     ::
 *      int : OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
int AmbaIPC_remove(const char *name)
{
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_REMOVE, name, 0);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_move
 *
 *  @Description::  Rename a remote file
 *
 *  @Input      ::
 *      old_name: the original name of the remote file
 *      new_name: the new name of the remote file
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_move(const char *old_name, const char *new_name)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_MOVE_s *param = (AMBA_IPC_RFS_MOVE_s*) msg->parameter;

    msg->msg_type = AMBA_IPC_RFS_CMD_MOVE;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_MOVE_s) +
    strlen(old_name) + 1 + strlen(new_name) + 1;

    param->old_name_size = strlen(old_name) + 1;
    param->new_name_size = strlen(new_name) + 1;
    // put old and new names together and use name_size to separate.
    memcpy(param->name, old_name, param->old_name_size);
    memcpy(&param->name[param->old_name_size], new_name, param->new_name_size);

    ExecRpmsg(msg);
    return msg->parameter[0];
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_chmod
 *
 *  @Description::  Change the access permission of the designated file
 *
 *  @Input      ::
 *      name    : the name of the remote file
 *      mode    : the access permission in octal digits.
 *          (Please refer to the numeric mode in chmod in Linux.)
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_chmod(const char *file_name, int mode)
{
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_CHMOD, file_name, mode);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_chdmod
 *
 *  @Description::  Change the access permission of the designated directory
 *
 *  @Input      ::
 *      name    : the name of the remote directory
 *      mode    : the access permission
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_chdmod(const char *dir_name, int mode)
{
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_CHMOD, dir_name, mode);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_mount
 *
 *  @Description::  Attach the filesystem on device at the directory dir_name.
 *
 *  @Input          ::
 *      dev_name    : the device name
 *      dir_name    : the directory name
 *      type        : filesystem type
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_mount(const char *dev_name, const char *dir_name, const char* type)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_MOUNT_s *param = (AMBA_IPC_RFS_MOUNT_s *) msg->parameter;

    msg->msg_type = AMBA_IPC_RFS_CMD_MOUNT;
    param->dev_name_size = strlen(dev_name) + 1;
    param->dir_name_size = strlen(dir_name) + 1;
    param->fs_name_size = strlen(type) + 1;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_MOUNT_s) +
    param->dev_name_size + param->dir_name_size + param->fs_name_size;

    memcpy(param->name, dev_name, param->dev_name_size);
    memcpy(&param->name[param->dev_name_size], dir_name, param->dir_name_size);
    memcpy(&param->name[param->dev_name_size + param->dir_name_size], type,
        param->fs_name_size);

    ExecRpmsg(msg);
    return msg->parameter[0];
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_umount
 *
 *  @Description::  Unattach the filesystem on device at the directory dir_name.
 *
 *  @Input          ::
 *      dir_name    : the directory name
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_umount(const char *name)
{
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_UMOUNT, name, 0);
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_sync
 *
 *  @Description::
 *
 *  @Input          ::
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_sync()
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;

    msg->msg_type = AMBA_IPC_RFS_CMD_SYNC;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + 8;
    ExecRpmsg(msg);
    return msg->parameter[0];
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_fsync
 *
 *  @Description::
 *
 *  @Input          ::
 *          fp  :: file pointer
 *  @Return     ::
 *      int : OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
int AmbaIPC_fsync(UINT64 fp)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_FSYNC_s *param = (AMBA_IPC_RFS_FSYNC_s *) msg->parameter;

    msg->msg_type = AMBA_IPC_RFS_CMD_FSYNC;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_FSYNC_s);
    param->fp = fp;

    ExecRpmsg(msg);
    return msg->parameter[0];
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_stat
 *
 *  @Description::  Get the information about a file or directory
 *
 *  @Input      ::
 *      name    : pointer to the path of the remote directory or a file.
 *      stat    : pointer to the information of the remote dir or a file.
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_stat(const char *name, AMBA_IPC_RFS_STAT_s *stat)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_STAT_MSG_s *param = (AMBA_IPC_RFS_STAT_MSG_s *)msg->parameter;
    UINT32 PhysAddr;

    AmbaMMU_VirtToPhys((ULONG)stat, (ULONG *)&PhysAddr);
    param->stat = (UINT64)(PhysAddr);
    strcpy(param->name, name);
    msg->msg_type = AMBA_IPC_RFS_CMD_STAT;
    msg->msg_len  = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_STAT_MSG_s) +
    strlen(param->name) + 1;

    AmbaCache_DataInvalidate((UINT32)stat, sizeof(AMBA_IPC_RFS_STAT_s));
    ExecRpmsg(msg);

    return msg->parameter[0];

}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_getdev
 *
 *  @Description::  Get device information.
 *
 *  @Input      ::
 *      path    : The path where the device is located.
 *      statfs  : The data structure is used to store the device information.
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_getdev(const char *path, AMBA_IPC_RFS_DEVINF_s *devinf)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_STATFS_MSG_s *param = (AMBA_IPC_RFS_STATFS_MSG_s *)msg->parameter;
    UINT32 PhysAddr;

    AmbaMMU_VirtToPhys((ULONG)devinf, (ULONG *)&PhysAddr);
    param->devinf = (UINT64) (PhysAddr);
    strcpy(param->name, path);
    msg->msg_type = AMBA_IPC_RFS_CMD_GETDEV;
    msg->msg_len  = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_STATFS_MSG_s) +
    strlen(param->name) + 1;

    AmbaCache_DataInvalidate((UINT32)devinf, sizeof(AMBA_IPC_RFS_DEVINF_s));
    ExecRpmsg(msg);

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "free space for %s is ", path, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "%u", devinf->ecl * devinf->spc * devinf->bps, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    return param->status;

}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_feof
 *
 *  @Description::  Check whether the file pointer points to the end-of-File
 *
 *  @Input      ::
 *      fp    : file pointer
 *
 *  @Return     ::
 *      int : OK(0) or NG(<0)
\*----------------------------------------------------------------------------*/
int AmbaIPC_feof(UINT64 fp)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_FEOF_s *param = (AMBA_IPC_RFS_FEOF_s *)msg->parameter;

    param->fp = fp;
    msg->msg_type = AMBA_IPC_RFS_CMD_FEOF;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_FEOF_s);

    ExecRpmsg(msg);
    return msg->parameter[0];
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_mkdir
 *
 *  @Description::  make a directory
 *
 *  @Input      ::
 *      name: the name of the target directory
 *
 *  @Return     ::
 *      int : OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
int AmbaIPC_mkdir(const char *name)
{
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_MKDIR, name, 0);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_rmdir
 *
 *  @Description::  remove a directory
 *
 *  @Input      ::
 *      name: the name of the target directory
 *
 *  @Return     ::
 *      int : OK(0) or NG(-1)
\*----------------------------------------------------------------------------*/
int AmbaIPC_rmdir(const char *name)
{
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_RMDIR, name, 0);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_opendir
 *
 *  @Description::  open a directory
 *
 *  @Input      ::
 *      name: the name of the target directory
 *
 *  @Return     ::
 *      void* : file pointer
 *              -1 (NG)
\*----------------------------------------------------------------------------*/
UINT64 AmbaIPC_opendir(const char *name)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_DENT_s *param = (AMBA_IPC_RFS_DENT_s *)msg->parameter;

    strcpy(param->name, name);

    msg->msg_type = AMBA_IPC_RFS_CMD_OPENDIR;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_DENT_s) +
    strlen(name) + 1;

    ExecRpmsg(msg);
    return msg->parameter[0];

}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_readdir
 *
 *  @Description::  read from a remote directory
 *
 *  @Input      ::
 *      dirp :  pointer to the structure storing the info for remote directory
 *
 *  @Return     ::
 *      AMBA_IPC_RFS_DIRENT_s :  a directory structure
\*----------------------------------------------------------------------------*/
AMBA_IPC_RFS_DIRENT_s* AmbaIPC_readdir(UINT64 dirp)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;
    AMBA_IPC_RFS_DIR_s *param = (AMBA_IPC_RFS_DIR_s *)msg->parameter;
    AMBA_IPC_RFS_DIRENT_s *dirent;
    int name_size = 256; // limit the length of the directory name
    int status;

    msg->msg_type = AMBA_IPC_RFS_CMD_READDIR;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + sizeof(AMBA_IPC_RFS_DIR_s) + name_size;

    param->dirp = dirp;

    ExecRpmsg(msg);
    status = msg->parameter[0];

    if(status > 0){
        dirent = &param->dirent;
        //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s", dirent->name, NULL, NULL, NULL, NULL);
        //AmbaPrint_Flush();
        return dirent;
    }
    else{
        return NULL;
    }


}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_closedir
 *
 *  @Description::  close a remote directory
 *
 *  @Input      ::
 *      dirp :  pointer to the structure storing the info for remote directory
 *
 *  @Return     ::
 *      0 : successful
 *      <0 : failed
\*----------------------------------------------------------------------------*/
int AmbaIPC_closedir(UINT64 dirp)
{
    AMBA_IPC_RFS_MSG_s *msg = (AMBA_IPC_RFS_MSG_s *)MsgBuf;

    msg->msg_type = AMBA_IPC_RFS_CMD_CLOSEDIR;
    msg->msg_len = sizeof(AMBA_IPC_RFS_MSG_s) + 8;
    msg->parameter[0] = dirp;

    ExecRpmsg(msg);

    return msg->parameter[0];
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_chdir
 *
 *  @Description::  change remote working directory
 *
 *  @Input      ::
 *      path : change the current working directory to the directory specified
 *             in path.
 *
 *  @Return     ::
 *      0 : successful
 *      <0 : failed
\*----------------------------------------------------------------------------*/
int AmbaIPC_chdir(const char* path)
{
    return AmbaIPC_dent(AMBA_IPC_RFS_CMD_CHDIR, path, 0);
}
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIOC_RfsInit
 *
 *  @Description::  Module Initialization
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_RfsInit(void)
{
    if (AmbaKAL_EventFlagCreate(&rpmsg_flags,"RpmsgFlags")) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create AmbaIPC RFS event flags", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }

    if (AmbaKAL_MsgQueueCreate(&rpmsg_queue, "rpmsg_queue", sizeof(int),rpmsg_queue_base, sizeof(rpmsg_queue_base))) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create AmbaIPC RFS queue", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    } else {
        int index;
        for (index = 0; index < AMBA_IPC_RFS_Q_SIZE; index++) {
            AmbaKAL_MsgQueueSend(&rpmsg_queue, &index, AMBA_KAL_WAIT_FOREVER);
        }
    }

#ifdef BUFFERED_RFS_IO
    if (AmbaKAL_MutexCreate(&io_lock,"IoLock")) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Failed to create RFS io lock", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }
#endif

#ifdef CONFIG_OPENAMP
    Channel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "aipc_rfs", RpmsgCB);
#else
    Channel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "aipc_rfs", RpmsgCB);
#endif
    if (Channel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }
    AmbaIPC_RegisterChannel(Channel, NULL);
}
