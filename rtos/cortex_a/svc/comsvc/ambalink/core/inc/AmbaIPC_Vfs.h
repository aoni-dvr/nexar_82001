/**
 * @file AmbaIPC_Vfs.h
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
 * @details Definitions for AmbaIPC_Vfs.
 *
 */

#ifndef _AMBA_IPC_VFS_H_
#define _AMBA_IPC_VFS_H_

#define AMBA_IPC_VFS_CMD_LS_INIT           0
#define AMBA_IPC_VFS_CMD_LS_NEXT           1
#define AMBA_IPC_VFS_CMD_LS_EXIT           2
#define AMBA_IPC_VFS_CMD_STAT              3
#define AMBA_IPC_VFS_CMD_OPEN              4
#define AMBA_IPC_VFS_CMD_CLOSE             5
#define AMBA_IPC_VFS_CMD_READ              6
#define AMBA_IPC_VFS_CMD_WRITE             7
#define AMBA_IPC_VFS_CMD_CREATE            8
#define AMBA_IPC_VFS_CMD_DELETE            9
#define AMBA_IPC_VFS_CMD_MKDIR             10
#define AMBA_IPC_VFS_CMD_RMDIR             11
#define AMBA_IPC_VFS_CMD_RENAME            12
#define AMBA_IPC_VFS_CMD_MOUNT             13
#define AMBA_IPC_VFS_CMD_UMOUNT            14

#define AMBA_IPC_VFS_CMD_SCATTER           15
#define AMBA_IPC_VFS_CMD_DOWRITE           16

#define AMBA_IPC_VFS_CMD_VOLSIZE           17
#define AMBA_IPC_VFS_CMD_QUICKSTAT         18
#define AMBA_IPC_VFS_CMD_SET_TIME          19

typedef struct {
    unsigned char       cmd;            /**< cmd */
    unsigned char       flag;           /**< flag */
    unsigned short      len;            /**< len */
    UINT32              padding;        /**< padding */
    UINT64              reserved;       /**< reserved */
    UINT64              parameter[0];   /**< parameter */
} AMBA_IPC_VFS_MSG_s;

typedef struct {
    UINT64  statp;  /**< statp */
#define AMBA_IPC_VFS_STAT_NULL             0
#define AMBA_IPC_VFS_STAT_FILE             1
#define AMBA_IPC_VFS_STAT_DIR              2
    INT64   size;   /**< size */
    UINT64  atime;  /**< access time */
    UINT64  mtime;  /**< modified time */
    UINT64  ctime;  /**< change time */
    int     type;   /**< type */
    char    name[0];    /**< name */
} AMBA_IPC_VFS_STAT_s;

typedef struct {
    UINT64  statp;  /**< statp */
    INT64   size;   /**< size */
    UINT64  atime;  /**< access time */
    UINT64  mtime;  /**< modified time */
    UINT64  ctime;  /**< change time */
    int     type;   /**< type */
#define AMBAFS_QSTAT_MAGIC    0x99998888
    UINT32  magic;  /**< magic number @sa AMBAFS_QSTAT_MAGIC */
} AMBA_IPC_VFS_QUICK_STAT_s;

typedef struct {
    int year;   /**< year */
    int month;  /**< month */
    int day;    /**< day */
    int hour;   /**< hour */
    int min;    /**< minutes */
    int sec;    /**< seconds */
} AMBA_IPC_VFS_TIMESTMP;

typedef struct {
    AMBA_IPC_VFS_TIMESTMP   atime;  /**< Access time */
    AMBA_IPC_VFS_TIMESTMP   mtime;  /**< Modified time */
    AMBA_IPC_VFS_TIMESTMP   ctime;  /**< Change time */
    char                    name[0];    /**< Name */
} AMBA_IPC_VFS_STAT_TIMESTMP_s;

typedef struct {
    UINT64        size;       /**< size */
    char        name[0];    /**< name */
} AMBA_IPC_VFS_STAT_SIZE_s;

typedef struct {
    INT64   offset;     /**< offset */
    UINT64  addr;       /**< address */
    int     len;        /**< length */
    UINT32  padding;    /**< padding */
} AMBA_IPC_VFS_BH_s;

typedef struct {
    UINT64              fp;         /**< fp */
    int                 total;      /**< total */
    UINT32              padding;    /**< padding */
    AMBA_IPC_VFS_BH_s   bh[0];      /**< bh */
} AMBA_IPC_VFS_IO_s;

#endif  /* _AMBA_IPC_VFS_H_ */
