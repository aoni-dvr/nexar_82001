/**
 *  @file AppUSB_MTPDataBase.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details USB MTP device class functions for APP/MW. It's for object cache to speed up file enumeration.
 */

#include "AppUSB.h"
#include "AmbaUSB_ErrCode.h"
#ifndef AMBA_FS_H
#include "AmbaFS.h"
#endif

#include "AmbaUtility.h"


#define MTP_DB_MAX_FILE_NUMBER    MTP_MAX_HANDLES
#define MTP_DB_ATTRIBUTE_DIR      0x80U
#define MTP_DB_ATTRIBUTE_IMAGE    0x40U
//#define MTP_DB_ATTRIBUTE_VIDEO 0x20
#define MTP_DB_ATTRIBUTE_USED     0x01U

#define MTP_DB_MAX_BYTES_PER_LINE (1024U)

static char MTP_DB_FILE_NAME[] = "c:\\amba_mtp.txt";

static APP_MTPD_CACHED_OBJECT_INFO_s cached_object_array[MTP_DB_MAX_FILE_NUMBER] __attribute__((section(".bss.noinit")));

static APP_MTPD_CACHED_OBJECT_INFO_s *cached_object_array_get(void)
{
    return cached_object_array;
}

//--------------------------------------------------------------------
// Simple DataBase implementation
//--------------------------------------------------------------------
//
// layout of db file
// each line has 1024 bytes
// 1st line is the tile
// 2nd line is the contain of object handle 1
// ...
// Nnd line is the contain of object handle N-1
//

typedef struct {
    char          string_buffer[MTP_DB_MAX_BYTES_PER_LINE];
    AMBA_FS_FILE *db_file;
    UINT32        handle_count;
    UINT32        handle_parent;
    UINT32        handle_count_max;
    UINT32        handle_id;
    UINT32        max_files;
    UINT32        index_parent[MTP_DB_MAX_FILE_NUMBER];
    UINT8         index_attribute[MTP_DB_MAX_FILE_NUMBER];
    UINT8         max_level;
    UINT8         cur_level;
} MTP_IMPL_SCAN_DATA_s;

static MTP_IMPL_SCAN_DATA_s scan_data __attribute__((section(".bss.noinit")));
static USBD_MTP_OBJECT_s    object_information __attribute__((section(".bss.noinit")));

static UINT8 db_attr_from_filename_get(char *FileName)
{
    (void)FileName;
    /*   SSP do not support DCF, remove parser code.  */
    return 0;
}

static void db_handle_add(APP_USB_FS_DTA *FSData, const char *Path)
{
    char *ptr;
    UINT8 attribute = 0;
    UINT32 idx      = scan_data.handle_id;
    UINT32 bytes_written;
    PF_CHAR target;

    target = FSData->LongName[0];
    if (target != 0) {
        ptr = (char *)FSData->LongName;
    } else {
        ptr = (char *)FSData->FileName;
    }

    //sprintf(scan_data.string_buffer, "%d\t%d\t%s\t%s\\%s\t",
    //        (INT32)scan_data.handle_id,
    //        (INT32)scan_data.handle_parent,
    //        ptr,
    //        path,
    //        ptr);
    AppUsb_MemoryZeroSet(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE);
    AppUsb_StringAppendUInt32(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, scan_data.handle_id, 10);
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
    AppUsb_StringAppendUInt32(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, scan_data.handle_parent, 10);
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, ptr);
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, Path);
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\\");
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, ptr);
    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");

    if (((UINT8)FSData->Attribute & APP_FS_ATTR_DIR) != 0U) {
        attribute |= MTP_DB_ATTRIBUTE_DIR;
    }

    attribute |= db_attr_from_filename_get(ptr);
    attribute |= MTP_DB_ATTRIBUTE_USED;

    // cache the file position and parent handle
    if ((idx < MTP_DB_MAX_FILE_NUMBER)) {
        scan_data.index_parent[idx]    = scan_data.handle_parent;
        scan_data.index_attribute[idx] = attribute;
    } else {
        AppUsb_PrintUInt5("db_handle_add(): can't cache index for object handle %d", idx, 0, 0, 0, 0);
    }

    scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 1U] = '\n';
    scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 2U] = '\r';
    (void)AmbaFS_FileWrite(scan_data.string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, scan_data.db_file, &bytes_written);
    if (bytes_written != MTP_DB_MAX_BYTES_PER_LINE) {
        AppUsb_PrintUInt5("db_handle_add(): write DB data fail.", 0, 0, 0, 0, 0);
    }
    //dbg("[MTPImpl] Add %s", scan_data.string_buffer);
}

static UINT32 db_is_in_ignore_list(const APP_USB_FS_DTA *FSData, const char *Path)
{
    UINT32 uret = 0;
    UINT32 len = AmbaUtility_StringLength(Path);

    // just hide MTP_DB_FILE_NAME
    if (len == 2U) {
        const char *ptr;
        PF_CHAR target;

        target = FSData->LongName[0];
        if (target != 0) {
            ptr = (const char *)FSData->LongName;
        } else {
            ptr = (const char *)FSData->FileName;
        }
        if (AmbaUtility_StringCompare(ptr, &MTP_DB_FILE_NAME[3], AmbaUtility_StringLength(ptr)) == 0) {
            uret = 1;
        }
    }

    return uret;
}

static void AppMtpd_DbScanDirectoryL3(const char *Path, UINT8 CountOnly)
{
    UINT32 uRet_fs;
    APP_FS_DTA_t fsdta; // Need to use stack for passing misra-c, it requires 1K memory.
    char pattern[MTP_MAX_FILENAME_LENGTH];
    char next_path[MTP_MAX_FILENAME_LENGTH];
    const char *ptr;
    UINT32 len;
    UINT32 mtp_parent_id = scan_data.handle_parent;
    UINT32 uret          = 0;
    UINT32 is_break;

    // why assign again? Move to here or "unreachable code" issue happens.
    scan_data.handle_parent = mtp_parent_id;

    scan_data.cur_level++;

    len = AmbaUtility_StringLength(Path);
    // check file name length
    len = len + 3U;
    if (len >= MTP_MAX_FILENAME_LENGTH) {
        AppUsb_PrintStr5("AppMtpd_DbScanDirectoryL3():[1] %s\\* ...", Path, NULL, NULL, NULL, NULL);
        AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL3():[1]over %d", MTP_MAX_FILENAME_LENGTH, 0, 0, 0, 0);
        uret = USB_ERR_FAIL;
    }

    if (uret == 0U) {
        //sprintf(pattern, "%s\\*", Path);
        pattern[0] = '\0';
        AppUsb_StringCat(pattern, MTP_MAX_FILENAME_LENGTH, Path);
        AppUsb_StringCat(pattern, MTP_MAX_FILENAME_LENGTH, "\\*");

        // scan files first
        for (uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_ARCH, &fsdta);
             uRet_fs == 0U;
             uRet_fs = AppUsb_FileFindNext(&fsdta)) {
            if (((UINT8)fsdta.Attribute & AMBA_FS_ATTR_VOLUME) != 0U) {
                continue;
            }

            if ((db_is_in_ignore_list(&fsdta, Path) == 1U) && (scan_data.cur_level == 1U)) {
                continue;
            }

            if (scan_data.handle_count < scan_data.max_files) {
                if (CountOnly == 0U) {
                    db_handle_add(&fsdta, Path);
                }
                scan_data.handle_id++;
                scan_data.handle_count++;
            } else {
                AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL3(): reach max files %d.", scan_data.max_files, 0, 0, 0, 0);
                break;
            }
        }

        is_break = 0;

        // scan directory then
        for (uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_DIR, &fsdta);
             uRet_fs == 0U;
             uRet_fs = AppUsb_FileFindNext(&fsdta)) {
            if (is_break == 1U) {
                break;
            }

            if (((UINT8)fsdta.Attribute & (APP_FS_ATTR_VOLUME | APP_FS_ATTR_HIDDEN | APP_FS_ATTR_SYSTEM)) != 0U) {
                continue;
            }

            if (((UINT8)fsdta.Attribute & APP_FS_ATTR_DIR) != 0U) {
                // scan this directory
                PF_CHAR target;
                target = fsdta.LongName[0];
                if (target != 0) {
                    ptr = (char *)fsdta.LongName;
                } else {
                    ptr = (char *)fsdta.FileName;
                }

                if ((ptr[0] == '.') && (ptr[1] == '\0')) {
                    // skip '.' directory
                    continue;
                }

                if ((ptr[0] == '.') && (ptr[1] == '.') && (ptr[2] == '\0')) {
                    // skip '..' directory
                    continue;
                }

                if (scan_data.handle_count >= scan_data.max_files) {
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL3(): reach max files %d.", scan_data.max_files, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }

                // check file name length
                {
                    UINT32 len_p1;
                    UINT32 len_p2;
                    len_p1 = AmbaUtility_StringLength(Path);
                    len_p2 = AmbaUtility_StringLength(ptr);
                    len = len_p1 + len_p2;
                }
                if (len >= MTP_MAX_FILENAME_LENGTH) {
                    AppUsb_PrintStr5("AppMtpd_DbScanDirectoryL3():[2] %s\\%s ...", Path, ptr, NULL, NULL, NULL);
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL3():[2] over %d", MTP_MAX_FILENAME_LENGTH, 0, 0, 0, 0);
                    continue;
                }

                if (CountOnly == 0U) {
                    db_handle_add(&fsdta, Path);
                }

                scan_data.handle_id++;
                scan_data.handle_count++;

                // scan this directory
                //sprintf(next_path, "%s\\%s", Path, ptr);
                next_path[0] = '\0';
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, Path);
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, "\\");
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, ptr);

                //dbg("%s(): next_path = %s", __func__, next_path);
                if (scan_data.cur_level <= scan_data.max_level) {
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL3(): no L4 scan is supported.", 0, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                } else {
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL3(): scan depth reach max %d.", scan_data.max_level, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }
            }
        }

        scan_data.cur_level--;
    }

    return;
}


static void AppMtpd_DbScanDirectoryL2(const char *Path, UINT8 CountOnly)
{
    UINT32 uRet_fs;
    APP_USB_FS_DTA fsdta; // Need to use stack for passing misra-c, it requires 1K memory.
    char pattern[MTP_MAX_FILENAME_LENGTH];
    char next_path[MTP_MAX_FILENAME_LENGTH];
    const char *ptr;
    UINT32 len;
    UINT32 mtp_parent_id = scan_data.handle_parent;
    UINT32 uret          = 0;
    UINT32 is_break;

    scan_data.cur_level++;

    // check file name length
    len = AmbaUtility_StringLength(Path);
    len = len + 3U;
    if (len >= MTP_MAX_FILENAME_LENGTH) {
        AppUsb_PrintStr5("AppMtpd_DbScanDirectoryL2():[1] %s\\* ...", Path, NULL, NULL, NULL, NULL);
        AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL2():[1] over %d", MTP_MAX_FILENAME_LENGTH, 0, 0, 0, 0);
        uret = USB_ERR_FAIL;
    }

    if (uret == 0U) {
        //sprintf(pattern, "%s\\*", Path);
        pattern[0] = '\0';
        AppUsb_StringCat(pattern, MTP_MAX_FILENAME_LENGTH, Path);
        AppUsb_StringCat(pattern, MTP_MAX_FILENAME_LENGTH, "\\*");

        // scan files first
        for (uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_ARCH, &fsdta);
             uRet_fs == 0U;
             uRet_fs = AppUsb_FileFindNext(&fsdta)) {
            if (((UINT8)fsdta.Attribute & APP_FS_ATTR_VOLUME) != 0U) {
                continue;
            }

            if ((db_is_in_ignore_list(&fsdta, Path) == 1U) && (scan_data.cur_level == 1U)) {
                continue;
            }

            if (scan_data.handle_count < scan_data.max_files) {
                if (CountOnly == 0U) {
                    db_handle_add(&fsdta, Path);
                }
                scan_data.handle_id++;
                scan_data.handle_count++;
            } else {
                AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL2(): reach max files %d.", scan_data.max_files, 0, 0, 0, 0);
                break;
            }
        }

        is_break = 0;

        // scan directory then
        for (uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_DIR, &fsdta);
             uRet_fs == 0U;
             uRet_fs = AppUsb_FileFindNext(&fsdta)) {
            if (is_break == 1U) {
                break;
            }

            if (((UINT8)fsdta.Attribute & (APP_FS_ATTR_VOLUME | APP_FS_ATTR_HIDDEN | APP_FS_ATTR_SYSTEM)) != 0U) {
                continue;
            }

            if (((UINT8)fsdta.Attribute & APP_FS_ATTR_DIR) != 0U) {
                // scan this directory
                PF_CHAR target;
                target = fsdta.LongName[0];
                if (target != 0) {
                    ptr = (char *)fsdta.LongName;
                } else {
                    ptr = (char *)fsdta.FileName;
                }

                if ((ptr[0] == '.') && (ptr[1] == '\0')) {
                    // skip '.' directory
                    continue;
                }

                if ((ptr[0] == '.') && (ptr[1] == '.') && (ptr[2] == '\0')) {
                    // skip '..' directory
                    continue;
                }

                if (scan_data.handle_count >= scan_data.max_files) {
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL2(): reach max files %d.", scan_data.max_files, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }

                // check file name length
                {
                    UINT32 len_p1;
                    UINT32 len_p2;
                    len_p1 = AmbaUtility_StringLength(Path);
                    len_p2 = AmbaUtility_StringLength(ptr);
                    len = len_p1 + len_p2;
                }
                if (len >= MTP_MAX_FILENAME_LENGTH) {
                    AppUsb_PrintStr5("AppMtpd_DbScanDirectoryL2():[2] %s\\%s ...", Path, ptr, NULL, NULL, NULL);
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL2():[2] over %d", MTP_MAX_FILENAME_LENGTH, 0, 0, 0, 0);
                    continue;
                }

                if (CountOnly == 0U) {
                    db_handle_add(&fsdta, Path);
                }

                scan_data.handle_id++;
                scan_data.handle_count++;

                // scan this directory
                //sprintf(next_path, "%s\\%s", Path, ptr);
                next_path[0] = '\0';
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, Path);
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, "\\");
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, ptr);

                //dbg("%s(): next_path = %s", __func__, next_path);
                if (scan_data.cur_level <= scan_data.max_level) {
                    scan_data.handle_parent = (scan_data.handle_id - 1U);
                    AppMtpd_DbScanDirectoryL3(next_path, CountOnly);
                } else {
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL2(): scan depth reach max %d.", scan_data.max_level, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }
                scan_data.handle_parent = mtp_parent_id;
            }
        }

        scan_data.cur_level--;
    }

    return;
}

static void AppMtpd_DbScanDirectoryL1(const char *Path, UINT8 CountOnly)
{
    UINT32 uRet_fs;
    APP_USB_FS_DTA fsdta; // Need to use stack for passing misra-c, it requires 1K memory.
    char pattern[MTP_MAX_FILENAME_LENGTH];
    char next_path[MTP_MAX_FILENAME_LENGTH];
    const char *ptr;
    UINT32 len;
    UINT32 mtp_parent_id = scan_data.handle_parent;
    UINT32 uret          = 0;
    UINT32 is_break;

    scan_data.cur_level++;

    // check file name length
    len = AmbaUtility_StringLength(Path);
    len = len + 3U;
    if (len >= MTP_MAX_FILENAME_LENGTH) {
        AppUsb_PrintStr5("AppMtpd_DbScanDirectoryL1():[1] %s\\* ...", Path, NULL, NULL, NULL, NULL);
        AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL1():[1] over %d", MTP_MAX_FILENAME_LENGTH, 0, 0, 0, 0);
        uret = USB_ERR_FAIL;
    }

    if (uret == 0U) {
        //sprintf(pattern, "%s\\*", Path);
        pattern[0] = '\0';
        AppUsb_StringCat(pattern, MTP_MAX_FILENAME_LENGTH, Path);
        AppUsb_StringCat(pattern, MTP_MAX_FILENAME_LENGTH, "\\*");

        AppUsb_PrintStr5("AppMtpd_DbScanDirectoryL1(): pattern = %s, Path = %s", pattern, Path, NULL, NULL, NULL);

        // scan files first
        for (uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_ARCH, &fsdta);
             uRet_fs == 0U;
             uRet_fs = AppUsb_FileFindNext(&fsdta)) {
            if (((UINT8)fsdta.Attribute & APP_FS_ATTR_VOLUME) != 0U) {
                continue;
            }

            if ((db_is_in_ignore_list(&fsdta, Path) == 1U) && (scan_data.cur_level == 1U)) {
                continue;
            }
            if (scan_data.handle_count < scan_data.max_files) {
                if (CountOnly == 0U) {
                    db_handle_add(&fsdta, Path);
                }
                scan_data.handle_id++;
                scan_data.handle_count++;
            } else {
                AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL1(): reach max files %d.", scan_data.max_files, 0, 0, 0, 0);
                break;
            }
        }

        is_break = 0;

        // scan directory then
        for (uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_DIR, &fsdta);
             uRet_fs == 0U;
             uRet_fs = AppUsb_FileFindNext(&fsdta)) {
            if (is_break == 1U) {
                break;
            }

            if (((UINT8)fsdta.Attribute & (APP_FS_ATTR_VOLUME | APP_FS_ATTR_HIDDEN | APP_FS_ATTR_SYSTEM)) != 0U) {
                continue;
            }

            if (((UINT8)fsdta.Attribute & APP_FS_ATTR_DIR) != 0U) {
                // scan this directory
                PF_CHAR target;
                target = fsdta.LongName[0];
                if (target != 0) {
                    ptr = (char *)fsdta.LongName;
                } else {
                    ptr = (char *)fsdta.FileName;
                }

                if ((ptr[0] == '.') && (ptr[1] == '\0')) {
                    // skip '.' directory
                    continue;
                }

                if ((ptr[0] == '.') && (ptr[1] == '.') && (ptr[2] == '\0')) {
                    // skip '..' directory
                    continue;
                }

                if (scan_data.handle_count >= scan_data.max_files) {
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL1(): reach max files %d.", scan_data.max_files, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }

                // check file name length
                {
                    UINT32 len_p1;
                    UINT32 len_p2;
                    len_p1 = AmbaUtility_StringLength(Path);
                    len_p2 = AmbaUtility_StringLength(ptr);
                    len = len_p1 + len_p2;
                }
                if (len >= MTP_MAX_FILENAME_LENGTH) {
                    AppUsb_PrintStr5("AppMtpd_DbScanDirectoryL1():[2] %s\\%s ...", Path, ptr, NULL, NULL, NULL);
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL1():[2] over %d", MTP_MAX_FILENAME_LENGTH, 0, 0, 0, 0);
                    continue;
                }

                if (CountOnly == 0U) {
                    db_handle_add(&fsdta, Path);
                }

                scan_data.handle_id++;
                scan_data.handle_count++;

                // scan this directory
                //sprintf(next_path, "%s\\%s", Path, ptr);
                next_path[0] = '\0';
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, Path);
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, "\\");
                AppUsb_StringCat(next_path, MTP_MAX_FILENAME_LENGTH, ptr);

                //dbg("%s(): next_path = %s", __func__, next_path);
                if (scan_data.cur_level <= scan_data.max_level) {
                    scan_data.handle_parent = (scan_data.handle_id - 1U);
                    AppMtpd_DbScanDirectoryL2(next_path, CountOnly);
                } else {
                    AppUsb_PrintUInt5("AppMtpd_DbScanDirectoryL1(): scan depth reach max %d.", scan_data.max_level, 0, 0, 0, 0);
                    is_break = 1;
                    continue;
                }
                // needs to check this. why the value rewrite again in specific case?
                scan_data.handle_parent = mtp_parent_id;
            }
        }

        scan_data.cur_level--;
    }

    return;
}
/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Get the cached size information for the object
*/
UINT64 AppMtpd_DbCachedObjectSizeGet(UINT32 Handle)
{
    const APP_MTPD_CACHED_OBJECT_INFO_s *PreParseBufPtr = cached_object_array_get();

    return PreParseBufPtr[Handle].ObjectLength;
}
/**
 * Set the cached size information for the object
*/
VOID AppMtpd_DbCachedObjectSizeSet(ULONG Handle, UINT64 ObjectSize)
{
    APP_MTPD_CACHED_OBJECT_INFO_s *array = cached_object_array_get();

    array[Handle].ObjectLength = ObjectSize;
}
/**
 * Reset the cached information for the object
*/
VOID AppMtpd_DbCachedObjectInfoReset(UINT32 Handle)
{
    APP_MTPD_CACHED_OBJECT_INFO_s *array = cached_object_array_get();
    APP_MTPD_CACHED_OBJECT_INFO_s *ptr   = &array[Handle];

    AppUsb_MemoryZeroSet(ptr, sizeof(APP_MTPD_CACHED_OBJECT_INFO_s));
}

/**
 * Get the total number of the objects
*/
UINT32 AppMtpd_DbTotalHandleNumberGet(void)
{
    return scan_data.handle_count;
}
/**
 * Get the format of the object file
 * @return The format of the object file.
*/
UINT32 AppMtpd_DbFileFormatGet(UINT32 Handle)
{
    UINT8 attr = AppMtpd_DbAttributeFromHandleGet(Handle);
    UINT32 uret;

    if ((attr & MTP_DB_ATTRIBUTE_DIR) != 0U) {
        uret = MTP_OFC_ASSOCIATION;
    } else if ((attr & MTP_DB_ATTRIBUTE_IMAGE) != 0U) {
        uret = MTP_OFC_EXIF_JPEG;
    } else {
        uret = MTP_OFC_UNDEFINED;
    }
    return uret;
}

/**
 * Get object file size
 * @return The size of the object file.
*/
UINT64 AppMtpd_DbFileSizeGet(UINT32 Handle)
{
    UINT8 attr = AppMtpd_DbAttributeFromHandleGet(Handle);
    APP_USB_FILE_INFO pStat;
    UINT64 uret = 0;

    if ((attr & MTP_DB_ATTRIBUTE_DIR) != 0U) {
        // no action. Return 0 directly.
    } else if (Handle == MTP_EVENT_OBJECT_HANDLE_TEST) {
        // a test object handle for MTP event
        UINT32 uRval = AppUsb_FSFileInfoGet(MTP_DB_FILE_NAME, &pStat);
        if (uRval == 0U) {
            uret = pStat.Size;
        } else {
            uret = 0;
        }
    } else {
        UINT32 status = 0;
        UINT64 size   = AppMtpd_DbCachedObjectSizeGet(Handle);
        if ((size == 0U) || (size == 0xFFFFFFFFU)) {
            char FullFileName[MTP_MAX_FILENAME_LENGTH];
            status = AppMtpd_DbFilePathFromHandleGet(Handle, FullFileName);
            if (status == USB_ERR_SUCCESS) {
                UINT32 uRval = AppUsb_FSFileInfoGet(FullFileName, &pStat);
                // Check if the file info is obtained correctly.
                // It's a projection while object data send operation, since the file is not created.
                // The file size info is derived from host through send object info operation, it should be already in buf.
                if (uRval == 0U) {
                    AppMtpd_DbCachedObjectSizeSet(Handle, pStat.Size);
                } else {
                    status = USB_ERR_FAIL;
                }
            }
        }

        if (status == USB_ERR_SUCCESS) {
            //dbg("%s(): Handle %d, Len = %lld", __func__, Handle, AppMtpd_DbCachedObjectSizeGet(Handle));
            uret = AppMtpd_DbCachedObjectSizeGet(Handle);
        }
    }
    return uret;
}
/**
 * Get object information
 * @retval 0 Success
*/
UINT32 AppMtpd_DbObjectInfoGet(UINT32 Handle, USBD_MTP_OBJECT_s **Object)
{
    char FullFileName[MTP_MAX_FILENAME_LENGTH], FileName[MTP_MAX_FILENAME_LENGTH];
    UINT8 attribute = AppMtpd_DbAttributeFromHandleGet(Handle);

    AppUsb_MemoryZeroSet(&object_information, sizeof(USBD_MTP_OBJECT_s));

    (VOID)AppMtpd_DbFilePathFromHandleGet(Handle, FullFileName);
    AppMtpd_FileNameExtract(FullFileName, FileName);

    AppMtpd_Asc2PascalString(FileName, object_information.Filename);

    //If file size > 4GB, set it as 0xFFFFFFFF
    //Host will get the actual length through object property
    if (AppMtpd_DbFileSizeGet(Handle) >= 0xFFFFFFFF00000000ULL) {
        object_information.CompressedSize = 0xFFFFFFFFU;
    } else {
        object_information.CompressedSize = (UINT32)AppMtpd_DbFileSizeGet(Handle);
    }
    object_information.StorageId      = g_mtp_class_info.DeviceInfo.StorageId;
    object_information.HandleId       = Handle;
    object_information.ParentObject   = AppMtpd_DbParentHandleGet(Handle);

    /* Do not support DCF */
    if ((attribute & MTP_DB_ATTRIBUTE_DIR) != 0U) {
        object_information.CompressedSize  = 0;
        object_information.Format          = MTP_OFC_ASSOCIATION;
        object_information.ImageBitDepth   = 0;
        object_information.ImagePixHeight  = 0;
        object_information.ImagePixWidth   = 0;
        object_information.ThumbFormat     = MTP_OFC_UNDEFINED;
        object_information.AssociationType = MTP_ASSOCIATION_TYPE_GENERIC_FOLDER;
    } else {
        object_information.Format         = MTP_OFC_UNDEFINED;
        object_information.ImageBitDepth  = 0;
        object_information.ImagePixHeight = 0;
        object_information.ImagePixWidth  = 0;
        object_information.ThumbFormat    = MTP_OFC_UNDEFINED;
    }

    *Object = &object_information;

    return (OK);
}
/**
 * Set the drive of the root folder.
*/
void AppMtpd_DbDriveSet(UINT8 Drive)
{
    MTP_DB_FILE_NAME[0] = (char)Drive;
}

/**
 * Get the attribute of the object
 * @return the attribute of the object
*/
UINT8 AppMtpd_DbAttributeFromHandleGet(UINT32 Handle)
{
    UINT8 uret;

    if ((Handle > scan_data.max_files) && (Handle != MTP_EVENT_OBJECT_HANDLE_TEST)) {
        AppUsb_PrintUInt5("AppMtpd_DbAttributeFromHandleGet(): handle %d > %d.", Handle, scan_data.max_files, 0, 0, 0);
        uret = 0;
    } else if (Handle == MTP_EVENT_OBJECT_HANDLE_TEST) {
        // a test object handle for MTP event
        uret = MTP_DB_ATTRIBUTE_USED;
    } else {
        uret = scan_data.index_attribute[Handle];
    }
    return uret;
}
/**
 * Check if the object is of image format.
 * @retval 0 Non-image format
 * @retval 1 Image format
*/
UINT8 AppMtpd_DbIsImageHandle(UINT32 Handle)
{
    UINT8 uret;
    UINT8 attr = AppMtpd_DbAttributeFromHandleGet(Handle);

    if ((attr & MTP_DB_ATTRIBUTE_IMAGE) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }
    return uret;
}

/**
 * Insert one object into database
 * @return The object handle (index) of the object. 0 means database is full.
*/
UINT32 AppMtpd_DbHandleInsert(char *Path, const USBD_MTP_OBJECT_s *Object, const UINT32 ParentHandle)
{
    UINT32 idx               ;
    UINT8 db_attr         = 0;
    UINT8 found           = 0;
    UINT32 parent_handlex = ParentHandle;

    // find an empty handle
    for (idx = 1; idx < scan_data.handle_count_max; idx++) {
        if ((scan_data.index_attribute[idx] & MTP_DB_ATTRIBUTE_USED) == 0U) {
            found = 1;
            break;
        }
    }

    if (found == 0U) {
        AppUsb_PrintUInt5("AppMtpd_DbHandleInsert(): can't find an empty handle.", 0, 0, 0, 0, 0);
        idx = 0;
    } else {
        db_attr |= db_attr_from_filename_get(Path);
        db_attr |= MTP_DB_ATTRIBUTE_USED;
        if (Object->Format == MTP_OFC_ASSOCIATION) {
            db_attr |= MTP_DB_ATTRIBUTE_DIR;
        }

        if (parent_handlex == 0xFFFFFFFFU) {
            parent_handlex = 0;
        }

        // cache the file position and parent handle
        if (idx < MTP_DB_MAX_FILE_NUMBER) {
            // open cache file
            AMBA_FS_FILE *fd = NULL;
            UINT32 pos       = MTP_DB_MAX_BYTES_PER_LINE * idx;
            (void)AmbaFS_FileOpen(MTP_DB_FILE_NAME, "r+", &fd);
            if (fd != NULL) {
                if (AmbaFS_FileSeek(fd, (INT64)pos, (INT32)APP_FS_SEEK_START) != 0U) {
                    AppUsb_PrintUInt5("AppMtpd_DbHandleInsert(): can't seek to %llu for object handle %d", pos, idx, 0, 0, 0);
                } else {
                    // Since only 1 MTP session is supported
                    // we don't need to consider re-entry case.
                    // Just use static variables
                    static char name[256];
                    char *string_buffer = scan_data.string_buffer;
                    UINT32 bytes_written;

                    AppMtpd_FileNameExtract(Path, name);

                    AppUsb_MemoryZeroSet(string_buffer, MTP_DB_MAX_BYTES_PER_LINE);
                    string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 2U] = '\r';
                    string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 1U] = '\n';

                    //sprintf(string_buffer, "%d\t%d\t%s\t%s\t",
                    //        (INT32)idx,
                    //        (INT32)parent_handlex,
                    //        name,
                    //        Path);
                    AppUsb_StringAppendUInt32(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, idx, 10);
                    AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                    AppUsb_StringAppendUInt32(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, parent_handlex, 10);
                    AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                    AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, name);
                    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, Path);
                    AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");

                    (void)AmbaFS_FileWrite(string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, fd, &bytes_written);
                    if (bytes_written != MTP_DB_MAX_BYTES_PER_LINE) {
                        AppUsb_PrintUInt5("AppMtpd_DbHandleInsert(): can't write %d for object handle %d", MTP_DB_MAX_BYTES_PER_LINE, idx, 0, 0, 0);
                    }
                }
                if (AmbaFS_FileClose(fd) != 0U) {
                    // ignore this error
                }
            } else {
                AppUsb_PrintUInt5("AppMtpd_DbHandleInsert(): can't open DB file for object handle %d", idx, 0, 0, 0, 0);
            }
            scan_data.index_parent[idx]    = parent_handlex;
            scan_data.index_attribute[idx] = db_attr;
            scan_data.handle_count++;
        } else {
            AppUsb_PrintUInt5("AppMtpd_DbHandleInsert(): can't cache index for object handle %d", idx, 0, 0, 0, 0);
        }

        AppUsb_PrintStr5("[MTPImpl] Add %s ", Path, NULL, NULL, NULL, NULL);
        AppUsb_PrintUInt5("     for index %d", idx, 0, 0, 0, 0);

        if (scan_data.handle_count > MTP_DB_MAX_FILE_NUMBER) {
            AppUsb_PrintUInt5("AppMtpd_DbHandleInsert(): Reach maximum file number!", 0, 0, 0, 0, 0);
        }
    }

    return idx;
}

/**
 * Update full path of one object into database
 * @retval 0: Success
 * @retval 0xFFFFFFFF: Fail
*/
UINT32 AppMtpd_DbHandlePathUpdate(UINT32 ObjectHandle, const char *NewPath)
{
    UINT32 idx            = ObjectHandle;
    UINT32 parent_handlex = AppMtpd_DbParentHandleGet(ObjectHandle);
    UINT32 func_uret;
    UINT32 uret = 0;

    // cache the file position and parent handle
    if (idx < MTP_DB_MAX_FILE_NUMBER) {
        // open cache file
        AMBA_FS_FILE *fd = NULL;
        UINT32 pos       = MTP_DB_MAX_BYTES_PER_LINE * idx;
        func_uret = AmbaFS_FileOpen(MTP_DB_FILE_NAME, "r+", &fd);
        if ((fd != NULL) && (func_uret == 0U)) {
            if (AmbaFS_FileSeek(fd, (INT64)pos, (INT32)APP_FS_SEEK_START) != 0U) {
                AppUsb_PrintUInt5("AppMtpd_DbHandlePathUpdate(): can't seek to %llu for object handle %d", pos, idx, 0, 0, 0);
                uret = 0xFFFFFFFFUL;
            } else {
                // Since only 1 MTP session is supported
                // we don't need to consider re-entry case.
                // Just use static variables
                static char name[256];
                char *string_buffer = scan_data.string_buffer;
                UINT32 bytes_written;

                AppMtpd_FileNameExtract(NewPath, name);

                AppUsb_MemoryZeroSet(string_buffer, MTP_DB_MAX_BYTES_PER_LINE);
                string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 2U] = '\r';
                string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 1U] = '\n';

                //sprintf(string_buffer, "%d\t%d\t%s\t%s\t",
                //        (INT32)idx,
                //        (INT32)parent_handlex,
                //        name,
                //        Path);
                AppUsb_StringAppendUInt32(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, idx, 10);
                AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                AppUsb_StringAppendUInt32(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, parent_handlex, 10);
                AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, name);
                AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, NewPath);
                AppUsb_StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");

                (void)AmbaFS_FileWrite(string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, fd, &bytes_written);
                if (bytes_written != MTP_DB_MAX_BYTES_PER_LINE) {
                    AppUsb_PrintUInt5("AppMtpd_DbHandlePathUpdate(): can't write %d for object handle %d", MTP_DB_MAX_BYTES_PER_LINE, idx, 0, 0, 0);
                    uret = 0xFFFFFFFFUL;
                }
            }
            if (AmbaFS_FileClose(fd) != 0U) {
                // ignore this error
            }
        } else {
            AppUsb_PrintUInt5("AppMtpd_DbHandlePathUpdate(): can't open DB file for object handle %d", idx, 0, 0, 0, 0);
            uret = 0xFFFFFFFFUL;
        }
    } else {
        AppUsb_PrintUInt5("AppMtpd_DbHandlePathUpdate(): can't cache index for object handle %d", idx, 0, 0, 0, 0);
        uret = 0xFFFFFFFFUL;
    }

    return uret;
}


/**
 * Check if the object handle is valid
 * @retval 0 Inalid
 * @retval 1 Valid
*/
UINT32 AppMtpd_DbIsObjectHandleValid(UINT32 Handle)
{
    UINT32 uret;

    if ((Handle >= scan_data.handle_count_max) && (Handle != MTP_EVENT_OBJECT_HANDLE_TEST)) {
        AppUsb_PrintUInt5("AppMtpd_DbIsObjectHandleValid(): handle %d > max %d", Handle, scan_data.handle_count_max, 0, 0, 0);
        uret = 0;
    } else if (Handle == MTP_EVENT_OBJECT_HANDLE_TEST) {
        // a test object handle for MTP event
        uret = 1;
    } else if ((scan_data.index_attribute[Handle] & MTP_DB_ATTRIBUTE_USED) != 0U) {
        uret = 1;
    } else {
        AppUsb_PrintUInt5("AppMtpd_DbIsObjectHandleValid(): handle %d is not used", Handle, 0, 0, 0, 0);
        uret = 0;
    }
    return uret;
}
/**
 * Get the file name of the input object.
 * @retval 0 Success
 * @retval USB_ERR_FAIL Fail
*/
UINT32 AppMtpd_DbFileNameFromHandleGet(UINT32 Handle, char *FileName)
{
    UINT32 uret;

    static char path[MTP_DB_MAX_BYTES_PER_LINE];

    if (FileName == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    } else {
        AppUsb_MemoryZeroSet(path, MTP_DB_MAX_BYTES_PER_LINE);

        if (Handle == MTP_EVENT_OBJECT_HANDLE_TEST) {
            // a test object handle for MTP event
            AppUsb_StringCat(path, MTP_DB_MAX_BYTES_PER_LINE, MTP_DB_FILE_NAME);
            uret = USB_ERR_SUCCESS;
        } else {
            uret = AppMtpd_DbFilePathFromHandleGet(Handle, path);
        }

        if (uret == USB_ERR_SUCCESS) {
            AppMtpd_FileNameExtract(path, FileName);
        }
    }

    return uret;
}

/**
 * Get the full file path of the input object.
 * @retval 0 Success
 * @retval USB_ERR_FAIL Fail
*/
UINT32 AppMtpd_DbFilePathFromHandleGet(UINT32 Handle, char *Path)
{
    AMBA_FS_FILE *mtp_db_file = NULL;
    UINT32 size;
    UINT32 pos;
    static char string_buffer[MTP_DB_MAX_BYTES_PER_LINE];
    UINT32 uret = 0;
    UINT32 uRval_fs;
    UINT32 FullNameIdx = 3;

    //AppUsb_PrintStr5("AppMtpd_DbFilePathFromHandleGet(): path = %s", path, 0, 0, 0, 0);

    (void)AmbaFS_FileOpen(MTP_DB_FILE_NAME, "r", &mtp_db_file);
    if ((mtp_db_file == NULL) || (Path == NULL)) {
        uret = USB_ERR_FAIL;
    } else {
        Path[0] = '\0';
        pos = MTP_DB_MAX_BYTES_PER_LINE * Handle;
        if (AmbaFS_FileSeek(mtp_db_file, (INT64)pos, (INT32)APP_FS_SEEK_START) != 0U) {
            uRval_fs = AmbaFS_FileClose(mtp_db_file);
            if (uRval_fs != 0U) {
                // need to process it?
            }
            uret = USB_ERR_FAIL;
        } else {
            (void)AmbaFS_FileRead(string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, mtp_db_file, &size);
            if (size == 0U) {
                uRval_fs = AmbaFS_FileClose(mtp_db_file);
                if (uRval_fs != 0U) {
                    // need to process it?
                }
                uret = USB_ERR_FAIL;
            }
        }
        if (uret == USB_ERR_SUCCESS) {
            static char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH];
            UINT32 ArgCount;

            (VOID)AmbaUtility_StringToken(&string_buffer[0], '\t', Args, &ArgCount);

            if (ArgCount > FullNameIdx) {
                // check string length first
                UINT32 len;
                len = AmbaUtility_StringLength(&Args[FullNameIdx][0]);
                if (len >= MTP_MAX_FILENAME_LENGTH) {
                    AppUsb_PrintStr5("AppMtpd_DbFilePathFromHandleGet(): Filename '%s' is too long, truncated.", &Args[FullNameIdx][0], NULL, NULL, NULL, NULL);
                    AmbaUtility_StringCopy(Path, MTP_MAX_FILENAME_LENGTH - 1U, &Args[FullNameIdx][0]);
                    Path[MTP_MAX_FILENAME_LENGTH - 1U] = '\0';
                } else {
                    AmbaUtility_StringCopy(Path, UTIL_MAX_STR_TOKEN_LENGTH, &Args[FullNameIdx][0]);
                }
            }

            uRval_fs = AmbaFS_FileClose(mtp_db_file);
            if (uRval_fs != 0U) {
                // need to process it?
            }
        }
    }
    return uret;
}
/**
 * Get the parent object handle of the input object.
 * @return the parent object handle
*/
UINT32 AppMtpd_DbParentHandleGet(UINT32 Handle)
{
    UINT32 uret;

    if ((Handle > scan_data.max_files) && (Handle != MTP_EVENT_OBJECT_HANDLE_TEST)) {
        AppUsb_PrintUInt5("AppMtpd_DbParentHandleGet(): handle %d > %d.", Handle, scan_data.max_files, 0, 0, 0);
        uret = 0;
    } else if (Handle == MTP_EVENT_OBJECT_HANDLE_TEST) {
        // a test object handle for MTP event
        uret = 0;
    } else {
        //AppUsb_PrintUInt5("%d's parent is %d", Handle, scan_data.index_parent[Handle], 0, 0, 0);
        uret = scan_data.index_parent[Handle];
    }
    return uret;
}

/**
 * Get the number of objects according to object format code and association.
 * @return the number of objects
*/
UINT32 AppMtpd_DbNumberOfObjectsGet(UINT32 FormatCode, UINT32 Association)
{
    UINT32 association_x = Association;
    UINT32 uret;

    if ((FormatCode == 0U) && (association_x == 0U)) {
        uret = scan_data.handle_count;
    } else {
        if (FormatCode == 0U) {
            UINT32 i;
            UINT32 count = 0;
            if (association_x == 0xFFFFFFFFU) {
                association_x = 0x0;
            }
            for (i = 1; i <= scan_data.handle_count_max; i++) {
                if ((scan_data.index_parent[i] == association_x) && ((scan_data.index_attribute[i] & MTP_DB_ATTRIBUTE_USED) != 0U)) {
                    count++;
                }
            }
            uret = count;
        } else {
            AppUsb_PrintUInt5("AppMtpd_DbNumberOfObjectsGet(): Not impl.", 0, 0, 0, 0, 0);
            uret = 0;
        }
    }
    return uret;
}
/**
 * Fill handles array according to object format code and association.
 * @return the number of objects in the handles array
*/
UINT32 AppMtpd_DbObjectHandlesFill(UINT32 FormatCode, UINT32 Association, UINT32 *Handles)
{
    UINT32 i            = 0;
    UINT32 count        = 0;
    UINT32 association_x = Association;
    UINT32 uret         = 0;

    if ((FormatCode == 0U) && (association_x == 0U)) {
        for (i = 1; i <= scan_data.handle_count_max; i++) {
            if ((scan_data.index_attribute[i] & MTP_DB_ATTRIBUTE_USED) != 0U) {
                (void)AppMtpd_LongPut((UINT8 *)&Handles[count + 1U], i);
                count++;
            }
        }

        if ((count) != scan_data.handle_count) {
            AppUsb_PrintUInt5("AppMtpd_DbObjectHandlesFill(): DB doesn't match, %d != %d", count, scan_data.handle_count, 0, 0, 0);
        }
        (void)AppMtpd_LongPut((UINT8 *) Handles, scan_data.handle_count);

        uret = scan_data.handle_count;
    } else {
        if (FormatCode == 0U) {
            if (association_x == 0xFFFFFFFFU) {
                association_x = 0x0;
            }

            for (i = 1; i <= scan_data.handle_count_max; i++) {
                if ((scan_data.index_parent[i] == association_x) && ((scan_data.index_attribute[i] & MTP_DB_ATTRIBUTE_USED) != 0U)) {
                    (void)AppMtpd_LongPut((UINT8 *)&Handles[count + 1U], i);
                    count++;
                }
            }
            (void)AppMtpd_LongPut((UINT8 *) Handles, count);
            uret = count;
        } else {
            AppUsb_PrintUInt5("AppMtpd_DbObjectHandlesFill(): Not impl.", 0, 0, 0, 0, 0);
            uret = 0;
        }
    }
    return uret;
}

/**
 * Clean up file information in database.
*/
void AppMtpd_DbClean(void)
{
    AppUsb_MemoryZeroSet(&scan_data, sizeof(scan_data));
}
/**
 * Scan files in the root folder and add them into database.
*/
UINT32 AppMtpd_DbScan(void)
{
    UINT32 uret = USB_ERR_SUCCESS;
    UINT32 uRet_fs;
    UINT32 bytes_written;
    AMBA_FS_FILE *mtp_db_file = NULL;
    char root_dir[3]          = "c:";

    root_dir[0] = (char)g_mtp_class_info.DeviceInfo.RootPath[0];

    AppMtpd_DbClean();

    (void)AmbaFS_FileOpen(MTP_DB_FILE_NAME, "w", &mtp_db_file);

    if (mtp_db_file == NULL) {
        AppUsb_PrintUInt5("AppMtpd_DbScan(): can't create db file.", 0, 0, 0, 0, 0);
        uret = USB_ERR_FAIL;
    }

    if (uret == USB_ERR_SUCCESS) {
        scan_data.handle_id = 1;
        scan_data.max_level = 2;
        scan_data.max_files = MTP_DB_MAX_FILE_NUMBER;
        scan_data.db_file   = mtp_db_file;

        // get total file number
        AppMtpd_DbScanDirectoryL1(root_dir, 1);

        scan_data.handle_count_max = scan_data.handle_count + 4U;

        AppUsb_MemoryZeroSet(scan_data.index_parent, sizeof(scan_data.index_parent));
        AppUsb_MemoryZeroSet(scan_data.index_attribute, sizeof(scan_data.index_attribute));

        AppUsb_MemoryZeroSet(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE);
        AppUsb_StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "ID\tPARENT\tNAME\tPATH");
        scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 1U] = '\n';
        scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 2U] = '\r';
        (void)AmbaFS_FileWrite(scan_data.string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, mtp_db_file, &bytes_written);
        if (bytes_written != MTP_DB_MAX_BYTES_PER_LINE) {
            AppUsb_PrintUInt5("AppMtpd_DbScan(): can't write DB file.", 0, 0, 0, 0, 0);
            uret = USB_ERR_FAIL;
        } else {
            scan_data.handle_id     = 1;
            scan_data.handle_parent = 0;
            scan_data.handle_count  = 0;
            scan_data.cur_level     = 0;

            // get information for files
            AppMtpd_DbScanDirectoryL1(root_dir, 0);
        }

        uRet_fs = AmbaFS_FileClose(mtp_db_file);
        if (uRet_fs != 0U) {
            AppUsb_PrintUInt5("AppMtpd_DbScan(): can't close DB file.", 0, 0, 0, 0, 0);
            uret = USB_ERR_FAIL;
        }

        scan_data.db_file = NULL;
    }

    return uret;
}

/**
 * Get image/video height of the object from database
*/
UINT32 AppMtpd_DbObjectHeightGet(UINT32 Handle)
{
    AppUsb_PrintUInt5("AppMtpd_DbObjectHeightGet(): Not support for %d", Handle, 0, 0, 0, 0);
    return 0;
}

/**
 * Get image/video width of the object from database
*/
UINT32 AppMtpd_DbObjectWidthGet(UINT32 Handle)
{
    AppUsb_PrintUInt5("AppMtpd_DbObjectWidthGet(): Not support for %d", Handle, 0, 0, 0, 0);
    return 0;
}

/**
 * Delete object from database
*/
UINT32 AppMtpd_DbObjectDelete(UINT32 Handle)
{
    UINT32 uret;

    if (scan_data.handle_count > 0U) {
        char path[MTP_MAX_FILENAME_LENGTH];
        UINT32 uRet_fs;

        // remove file first
        UINT8 attr = AppMtpd_DbAttributeFromHandleGet(Handle);
        uret = AppMtpd_DbFilePathFromHandleGet(Handle, path);
        if (uret == USB_ERR_SUCCESS) {
            if ((attr & MTP_DB_ATTRIBUTE_DIR) != 0U) {
                uRet_fs = AppUsb_RemoveDir(path);
                if (uRet_fs != 0U) {
                    AppUsb_PrintStr5("AppMtpd_DbObjectDelete(): can't remove folder %s ...", path, NULL, NULL, NULL, NULL);
                    AppUsb_PrintUInt5("AppMtpd_DbObjectDelete(): handle 0x%X, code %d", Handle, uRet_fs, 0, 0, 0);
                    uret = MTP_RC_ACCESS_DENIED;
                }
            } else {
                uRet_fs = AmbaFS_Remove(path);
                if (uRet_fs != 0U) {
                    AppUsb_PrintStr5("AppMtpd_DbObjectDelete(): can't remove file %s ...", path, NULL, NULL, NULL, NULL);
                    AppUsb_PrintUInt5("AppMtpd_DbObjectDelete(): handle 0x%X, code %d", Handle, uRet_fs, 0, 0, 0);
                    uret = MTP_RC_ACCESS_DENIED;
                }
            }

            if (uret == USB_ERR_SUCCESS) {
                AppUsb_PrintStr5("AppMtpd_DbObjectDelete(): Path %s ...", path, NULL, NULL, NULL, NULL);
                AppUsb_PrintUInt5("AppMtpd_DbObjectDelete(): handle 0x%X is removed. [0x%x]", Handle, uRet_fs, 0, 0, 0);
                uret = MTP_RC_OK;
                scan_data.handle_count--;
                scan_data.index_parent[Handle]    = 0;
                scan_data.index_attribute[Handle] = 0;
                AppMtpd_DbCachedObjectInfoReset(Handle);
            }
        } else {
            uret = MTP_RC_INVALID_OBJECT_HANDLE;
        }
    } else {
        uret = MTP_RC_INVALID_OBJECT_HANDLE;
    }

    return uret;
}

/**
 * Initialize Database information
*/
VOID AppMtpd_DbInitZero(void)
{
    AppUsb_MemoryZeroSet(cached_object_array, sizeof(cached_object_array));
    AppUsb_MemoryZeroSet(&scan_data, sizeof(scan_data));
    AppUsb_MemoryZeroSet(&object_information, sizeof(object_information));
    return;
}
/** @} */
