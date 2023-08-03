/**
 * @file AmbaIPC_Rfs.h
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
 * @details Definitions & Constants for Amba RFS Support.
 *
 */

#ifndef AMBAIPC_RFS_H_
#define AMBAIPC_RFS_H_

/**
 * @defgroup AmbaIPC_RFS RFS support
 * @{
 */

/**
 * @brief This function is used to open a remote file.
 *
 * @param [in] name The name of the target file.
 * @param [in] mode The file accessing mode r(read) or w(write)
 *
 * @return >0 - File pointer, others - NG
 * @see AmbaIPC_fclose
 */
UINT64 AmbaIPC_fopen(const char *name, char *mode);

/**
 * @brief This function is used to close a remote file.
 *
 * @param [in] fp The file pointer returned by AmbaIPC_open function
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_fopen
 */
int AmbaIPC_fclose(UINT64 fp);

/**
 * @brief This function is used to read a remote file.
 *
 * @param [in] buf The pointer to a buffer to store read data.
 * @param [in] size The size of read data
 * @param [in] fp The file pointer
 *
 * @return >0 - Total number of bytes read, others - NG
 * @see AmbaIPC_fwrite
 */
int AmbaIPC_fread(void *buf, int size, UINT64 fp);

/**
 * @brief This function is used to write a remote file.
 *
 * @param [in] buf The pointer to a buffer to be written.
 * @param [in] size The size of data to be written
 * @param [in] fp The file pointer
 *
 * @return >0 - Total number of bytes written, others - NG
 * @see AmbaIPC_fread
 */
int AmbaIPC_fwrite(void *buf, int size, UINT64 fp);

/**
 * The defintion for the reference posision in AmbaIPC_fseek.
 *
 * @see AmbaIPC_fseek
 */
#define AMBA_IPC_RFS_SEEK_SET 0  /**< Start of file */
#define AMBA_IPC_RFS_SEEK_CUR 1  /**< Current position of the file pointer */
#define AMBA_IPC_RFS_SEEK_END 2  /**< End of file */

/**
 * @brief This function is used to set file pointer to a new position.
 *
 * @param [in] fp The file pointer
 * @param [in] offset The bytes from the referenced position to a new position.
 * @param [in] origin The referenced position to set a new position.
 *
 * @return 0 - OK, <0 - NG
 */
int AmbaIPC_fseek(UINT64 fp,  INT64 offset, int origin);

/**
 * @brief This function is used to get the current position of the file pointer.
 *
 * @param [in] fp The file pointer
 *
 * @return >=0 - The current position, <0 - NG
 */
INT64 AmbaIPC_ftell(UINT64 fp);

/**
 * @brief This function is used to create a remote directory.
 *
 * @param [in] name The name of the target directory.
 *
 * @return 0 - OK, <0 - NG
 */
int   AmbaIPC_mkdir (const char *name);

/**
 * @brief This function is used to remove a remote directory.
 *
 * @param [in] name The name of the target directory.
 *
 * @return 0 - OK, <0 - NG
 */
int AmbaIPC_rmdir(const char *name);

/**
 * @brief This function is used to delete a remote file.
 *
 * @param [in] name The name of the target file.
 *
 * @return 0 - OK, <0 - NG
 */
int AmbaIPC_remove(const char *name);

/**
 * @brief This function is used to chane the name of a remote file.
 *
 * @param [in] old_name The original name of the target file.
 * @param [in] new_name The new name of the target file.
 *
 * @return 0 - OK, <0 - NG
 */
int AmbaIPC_move(const char *old_name, const char *new_name);

/**
 * @brief This function is used to change the access permission of a remote file.
 * Only numeric mode in chmod is provided.
 *
 * @param [in] file_name The name of the target file.
 * @param [in] mode The access permission in octal digits.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_chdmod
 */
int AmbaIPC_chmod(const char *file_name, int mode);

/**
 * @brief This function is used to change the access permission of a remote directory.
 * Only numeric mode in chmod is provided.
 *
 * @param [in] dir_name The name of the target directory.
 * @param [in] mode The access permission in octal digits.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_chmod
 */
int AmbaIPC_chdmod(const char *dir_name, int mode);

/**
 * @brief This function is used to attach the filesystem on device to the target directory.
 *
 * @param [in] dev_name The device which is desired to be attached.
 * @param [in] dir_name The target directory to mount.
 * @param [in] type The type of the file system
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_umount
 */
int AmbaIPC_mount(const char *dev_name, const char *dir_name, const char* type);

/**
 * @brief This function is used to unmount the file system.
 *
 * @param [in] name The remote directory where the file system has been mounted.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_mount
 */
int AmbaIPC_umount(const char *name);

/**
 * @brief This function is used to flush file system buffers.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_fsync
 */
int AmbaIPC_sync(void);

/**
 * @brief This function is used to synchronize all the modified buffer for the file.
 *
 * @param [in] fp The file pointer
 *
 * @return 0 - OK, <0 - NG
 */
int AmbaIPC_fsync(UINT64 fp);

/**
 * To define the data structure storing the file information.
 * It has to sync with the "struct kstat" of Linux kernel.
 *
 * @see AmbaIPC_stat
 */
typedef struct {
    UINT32  result_mask;    /**< What fields the user got */
    UINT16  mode;           /**< protection */
    UINT32  nlink;          /**< number of hard links */
    UINT32  blksize;        /**< block size for file system */
    UINT64    attributes;     /**< attributes */
    UINT64    attributes_mask;    /**< Mask of attributes */
    UINT64  ino;            /**< inode number */
    UINT32  dev;            /**< ID of device containing file */
    UINT32  rdev;           /**< device ID */
    UINT32  uid;            /**< user ID of owner */
    UINT32  gid;            /**< group ID of owner */
    INT64   size;           /**< total size in bytes */
    INT64   atime_sec;      /**< last access time */
    INT64   atime_nsec;     /**< last access time */
    INT64   mtime_sec;      /**< last modification time */
    INT64   mtime_nsec;     /**< last modification time */
    INT64   ctime_sec;      /**< last status change time */
    INT64   ctime_nsec;     /**< last status change time */
    INT64   btime_sec;        /**< File creation time */
    INT64   btime_nsec;        /**< File creation time */
    UINT64  blocks;         /**< number of 512 byte blocks allocated */
} AMBA_IPC_RFS_STAT_s;

/**
 * @brief This function is used to get information about a file or directory.
 *
 * @param [in] name The name of the file or directory
 * @param [in] stat The data structure storing the file information.
 *
 * @return 0 - OK, <0 - NG
 * @see AMBA_IPC_RFS_STAT_s
 */
int AmbaIPC_stat(const char *name, AMBA_IPC_RFS_STAT_s *stat);

/**
 * To define the supported formate types.
 *
 * @see AMBA_IPC_RFS_DEVINF_s
 * @see AmbaIPC_getdev
 */
typedef enum _AMBA_IPC_RFS_FMT_TYPE_e_ {
    AMBA_IPC_RFS_FMT_FAT12 = 0,
    AMBA_IPC_RFS_FMT_FAT16 = 1,
    AMBA_IPC_RFS_FMT_FAT32 = 2,
    AMBA_IPC_RFS_FMT_EXFAT = 3
} AMBA_IPC_RFS_FMT_TYPE_e;

/**
 * To define the data structure storing device information.
 *
 * @see AMBA_IPC_RFS_FMT_TYPE_e
 * @see AmbaIPC_getdev
 */
typedef struct _AMBA_IPC_RFS_DEVINF_s_ {
    UINT32 cls;     /**< total number of clusters */
    UINT32 ecl;     /**< number of unused clusters */
    UINT32 bps;     /**< byte count per sector */
    UINT32 spc;     /**< sector count per cluster */
    UINT32 cpg;     /**< cluster count per cluster group */
    UINT32 ecg;     /**< number of unused cluster groups */
    AMBA_IPC_RFS_FMT_TYPE_e fmt;    /**< format type */
} AMBA_IPC_RFS_DEVINF_s;


/**
 * @brief This function is used to get device information.
 *
 * @param [in] path The path where the device is located.
 * @param [in] devinf The data structure storing the device information.
 *
 * @return 0 - OK, <0 - NG
 * @see AMBA_IPC_RFS_DEVINF_s
 * @see AMBA_IPC_RFS_FMT_TYPE_e
 */
int AmbaIPC_getdev(const char *path, AMBA_IPC_RFS_DEVINF_s *devinf);

/**
 * @brief This function is used to check whether the file pointer points to EOF.
 *
 * @param [in] fp File pointer
 *
 * @return 0 - OK, <0 - NG
 */
int AmbaIPC_feof(UINT64 fp);

/**
 * @brief This function is used to open a remote directory.
 *
 * @param [in] name The name of the remote directory.
 *
 * @return >0 - File pointer, NULL- NG
 * @see AmabaIPC_closedir
 */
UINT64 AmbaIPC_opendir(const char *name);

/**
 * To define the data structure storing the information for file
 * read from the remote directory.
 *
 * @see AmbaIPC_readdir
 */
typedef struct {
    UINT64  ino;        /**< inode number */
    UINT64  off;        /**< offset to the next dirent */
    UINT16  reclen;     /**< length of this record */
    UINT8   type;       /**< type of file */
    char    *name;    /**< filename */
} AMBA_IPC_RFS_DIRENT_s;


/**
 * @brief This function is used to read from a remote directory.
 *
 * @param [in] dirp Pointer to the structure storing the information
 * for a remote directory.
 *
 * @return >0 - Direcoty entry pointer, NULL - NG
 * @see AMBA_IPC_RFS_DIRENT_s
 */
AMBA_IPC_RFS_DIRENT_s* AmbaIPC_readdir(UINT64 dirp);

/**
 * @brief This function is used to close a remote directory.
 *
 * @param [in] dirp Pointer to the structure storing the information for
 * a remote directory.
 *
 * @return 0 - OK, <0 - NG
 * @see AmbaIPC_opendir
 */
int AmbaIPC_closedir(UINT64 dirp);

/**
 * @brief This function is used to change the current working directory
 * to the directory specified in the path.
 *
 * @param [in] path The target path
 *
 * @return >0 - OK, <0 - NG
 */
int AmbaIPC_chdir(const char* path);

/**
 * @brief This function is used to initialize the AmbaRfs module.
 *
*/
void AmbaIPC_RfsInit(void);

/** @} */ // end of group AmbaIPC_RFS

#endif  /* _AMBAIPC_RFS_H_ */
