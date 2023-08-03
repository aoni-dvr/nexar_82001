/**
 * @file AmbaIPC_FuseVfs.h
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
 * @details Fuse Vffs Header
 *
 */

#ifndef _AMBA_IPC_VFFS_H_
#define _AMBA_IPC_VFFS_H_

typedef struct {
    unsigned char       cmd;        /**< cmd */
    unsigned char       flag;       /**< flag */
    unsigned short      len;        /**< len */
    UINT64              reserved;   /**< reserved */
    UINT64              parameter[0];   /**< parameter */
} AMBA_IPC_VFFS_MSG_s;

typedef struct {
    int     mode;       /**< mode */
    char    file[0];    /**< file */
} AMBA_IPC_VFFS_OPEN_s;

typedef struct {
    UINT64          buf;    /**< buf */
    int             total;  /**< total */
    AMBA_FS_FILE*   fp;     /**< fp */
} AMBA_IPC_VFS_IO_s;

typedef struct {
    unsigned char   attr;       /**< attr */
    UINT64          res;        /**< res */
    char            path[0];    /**< path */
} AMBA_IPC_VFFS_FSFIRST_s;

typedef struct {
    int rval;                   /**< rval */
    int fs_type;                /**< fs_type */
    unsigned long long fstfz;   /**< file size in bytes */
    unsigned short fstact;      /**< file last access time */
    unsigned short fstad;       /**< last file access date */
    unsigned char fstautc;      /**< last file access date and time UTC offset */
    unsigned short fstut;       /**< last file update time */
    unsigned short fstuc;       /**< last file update time[10ms] */
    unsigned short fstud;       /**< last file update date */
    unsigned char  fstuutc;     /**< last file update date and time UTC offset */
    unsigned short fstct;       /**< file create time */
    unsigned short fstcd;       /**< file create date */
    unsigned short fstcc;       /**< file create component time (ms) */
    unsigned char  fstcutc;     /**< file create date and time UTC offset */
    unsigned short fstat;       /**< file attribute */
} AMBA_IPC_VFFS_STAT_s;

typedef struct {
    INT32       fp;     /**< fp */
    int         origin; /**< origin */
    long long   offset; /**< offset */
} AMBA_IPC_VFFS_SEEK_s;

#define VFFS_SHORT_NAME_LEN  26
#define VFFS_LONG_NAME_LEN   512

typedef struct {
    int rval;   /**< rval */
    void *dta;  /**< dta */
    unsigned short Time;    /**< Time */
    unsigned short Date;    /**< Date */
    unsigned long long FileSize;    /**< File size */
    char Attribute;                 /**< Attribute */
    char FileName[VFFS_SHORT_NAME_LEN]; /**< File name */
    char LongName[VFFS_LONG_NAME_LEN];  /**< Long name */
} AMBA_IPC_VFFS_FSFIND_s;

typedef struct {
    int fs_type;             /**< fs_type */
    unsigned int cls;        /**< total number of clusters */
    unsigned int ecl;        /**< number of unused clusters */
    unsigned int bps;        /**< bytes per sector */
    unsigned int spc;        /**< sectors per cluster, for udf, spc=1 */
    unsigned int cpg;        /**< clusters per cluster group */
    unsigned int ecg;        /**< number of empty cluster groups */
    int fmt;                 /**< format type */
} AMBA_IPC_VFFS_GETDEV_s;

typedef struct {
    int src_len;        /**< the length of filename of src file */
    int dst_len;        /**< the length of filename of src file */
    char filename[0];   /**< src_name & dst_name */
} AMBA_IPC_VFFS_RENAME_s;

#endif  /* _AMBA_IPC_FUSE_VFFS_H_ */
