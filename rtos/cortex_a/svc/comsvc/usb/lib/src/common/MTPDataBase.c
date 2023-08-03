/**
 *  @file MTPDataBase.c
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
 *  @details USB MTP device class functions for APP/MW. It's for object cache to speed up file enumeration.
 */

#ifndef COMSVC_MTP_DBASE_H      /* Dir 4.10 */
#define COMSVC_MTP_DBASE_H

#define MTP_DB_MAX_FILE_NUMBER    MTP_MAX_HANDLES
#define MTP_DB_ATTRIBUTE_DIR      0x80U
#define MTP_DB_ATTRIBUTE_IMAGE    0x40U
//#define MTP_DB_ATTRIBUTE_VIDEO 0x20
#define MTP_DB_ATTRIBUTE_USED     0x01U

#define MTP_DB_MAX_BYTES_PER_LINE (1024U)

static char MTP_DB_FILE_NAME[] = "c:\\amba_mtp.txt";

static MTP_CLASS_INFO_s MtpClassInfo GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static MTP_PREPARSE_INFO_s PreParseBuffer[MTP_DB_MAX_FILE_NUMBER] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 UInt32ToStr(char *Buffer, UINT32 BufSize, UINT32 Value, UINT32 Radix)
{
    UINT32 StrLength = 0U;
    UINT32 ut = Value, uTmp;
    char *pWorkChar1, *pWorkChar2 = Buffer, Tmp;
    UINT32 idx1 = 0U, idx2 = 0U, swap_len;
    UINT32 Base = Radix;

    if (Base > 16U) {
        Base = 16U;
    }

    if (Base == 0U) {
        Base = 10U;
    }

    /* variable length of string */
    if (ut == 0U) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength        = 1U;
    } else {
        pWorkChar1 = pWorkChar2;
        idx1       = idx2;

        while (ut > 0U) {
            uTmp = ut % Base;
            if (uTmp <= 9U) {
                pWorkChar1[idx1] = '0' + uTmp;
            } else {
                pWorkChar1[idx1] = 'A' + (uTmp - 10U);
            }
            idx1++;
            if (idx1 >= (BufSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength        = idx1;

        // change the order of the digits
        swap_len = idx1 / 2U;  /* n = length of the string / 2 */
        while (0U < swap_len) {
            if (0U < idx1) {
                idx1--;
                Tmp              = pWorkChar1[idx1];
                pWorkChar1[idx1] = pWorkChar2[idx2];
                pWorkChar2[idx2] = Tmp;
                idx2++;
                swap_len--;
            }
        }
    }

    /* make sure at least a \0 is added.*/
    if (BufSize > 0U) {
        /* make sure at least a \0 is added.*/
        Buffer[BufSize - 1U] = '\0';
    }
    return StrLength;
}

static void StringCat(char *Dest, UINT32 DestSize, const char *Source)
{
    UINT32 DestLen   = (UINT32)AmbaUtility_StringLength(Dest);
    UINT32 SourceLen = (UINT32)AmbaUtility_StringLength(Source);

    if ((DestLen + SourceLen + 1U) > DestSize) {
        // over buffer size, do nothing
    } else {
        UINT32 Idx;
        for (Idx = 0; Idx < SourceLen; Idx++) {
            Dest[DestLen + Idx] = Source[Idx];
        }
        Dest[DestLen + SourceLen] = '\0';
    }
}

static void StringAppendUInt32(char *Buffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    char IntString[UTIL_MAX_INT_STR_LEN];

    if (Buffer != NULL) {
        if (0U < UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, Radix)) {
            StringCat(Buffer, BufferSize, IntString);
        }
    }
}

static SIZE_t WStrLen(const WCHAR *str)
{
    SIZE_t  len = 0;

    if (str != NULL) {
        while (str[len] != 0x0000U) {
            len++;
        }
    }

    return len;
}

static MTP_PREPARSE_INFO_s *GetPreParseBuffer(void)
{
    return PreParseBuffer;
}

static UINT64 GetObjectSizeFromBuf(ULONG Handle)
{
    const MTP_PREPARSE_INFO_s *PreParseBufPtr = GetPreParseBuffer();

    return PreParseBufPtr[Handle].ObjectLength;
}

static void SendObjectSizeToBuf(ULONG Handle, UINT64 ObjectSize)
{
    MTP_PREPARSE_INFO_s *PreParseBufPtr = GetPreParseBuffer();

    PreParseBufPtr[Handle].ObjectLength = ObjectSize;
}

static void ResetPreParseBuf(ULONG Handle)
{
    MTP_PREPARSE_INFO_s *PreParseBufPtr = GetPreParseBuffer();
    MTP_PREPARSE_INFO_s *ptr            = &PreParseBufPtr[Handle];

    AmbaSvcWrap_MisraMemset(ptr, 0, sizeof(MTP_PREPARSE_INFO_s));
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

static MTP_IMPL_SCAN_DATA_s scan_data GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 DbGetTotalHandleNumber(void)
{
    return scan_data.handle_count;
}

static UINT8 DbGetAttributeFromHandle(ULONG Handle)
{
    UINT8 uRet;

    if (Handle > scan_data.max_files) {
        AmbaPrint_PrintUInt5("DbGetAttributeFromHandle(): Handle > max_files", 0U, 0U, 0U, 0U, 0U);
        uRet = 0;
    } else {
        uRet = scan_data.index_attribute[Handle];
    }
    return uRet;
}

static UINT32 DbGetFileFormat(ULONG Handle)
{
    UINT8 attr = DbGetAttributeFromHandle(Handle);
    UINT32 uRet;

    if ((attr & MTP_DB_ATTRIBUTE_DIR) != 0U) {
        uRet = MTP_OFC_ASSOCIATION;
    } else if ((attr & MTP_DB_ATTRIBUTE_IMAGE) != 0U) {
        uRet = MTP_OFC_EXIF_JPEG;
    } else {
        uRet = MTP_OFC_UNDEFINED;
    }
    return uRet;
}

static UINT32 DbGetFilePathFromHandle(ULONG Handle, char *path)
{
    AMBA_FS_FILE *mtp_db_file = NULL;
    UINT32 size, Rval;
    ULONG pos;
    char string_buffer[MTP_DB_MAX_BYTES_PER_LINE];
    UINT32 uRet = 0U;
    UINT32 uRval_fs;
    UINT32 FullNameIdx = 3U;

    Rval = AmbaFS_FileOpen(MTP_DB_FILE_NAME, "r", &mtp_db_file);

    if ((Rval != OK) || (mtp_db_file == NULL) || (path == NULL)) {
        uRet = USB_ERR_FAIL;
    } else {
        path[0] = '\0';
        pos = MTP_DB_MAX_BYTES_PER_LINE * Handle;
        if (AmbaFS_FileSeek(mtp_db_file, (INT64)pos, (INT32)APP_FS_SEEK_START) != 0U) {
            uRval_fs = AmbaFS_FileClose(mtp_db_file);
            if (uRval_fs != 0U) {
                // need to process it?
            }
            uRet = USB_ERR_FAIL;
        } else {
            Rval = AmbaFS_FileRead(string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, mtp_db_file, &size);
            if ((Rval != OK) || (size == 0U)) {
                AmbaPrint_PrintUInt5("DbGetFilePathFromHandle(): can't read file", 0U, 0U, 0U, 0U, 0U);
                uRval_fs = AmbaFS_FileClose(mtp_db_file);
                if (uRval_fs != 0U) {
                    // need to process it?
                }
                uRet = USB_ERR_FAIL;
            }
        }

        if (uRet == USB_ERR_SUCCESS) {
            char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH];
            UINT32 ArgCount;

            Rval = AmbaUtility_StringToken(&string_buffer[0], '\t', Args, &ArgCount);

            if (Rval == OK) {
                if (ArgCount > FullNameIdx) {
                    // check string length first
                    UINT32 len;

                    len = (UINT32)AmbaUtility_StringLength(&Args[FullNameIdx][0]);
                    if (len >= MTP_MAX_FILENAME_LENGTH) {
                        AmbaPrint_PrintStr5("DbGetFilePathFromHandle(): Filename '%s' is too long, truncated."
                                            , &Args[FullNameIdx][0], NULL, NULL, NULL, NULL);
                        AmbaUtility_StringCopy(path, MTP_MAX_FILENAME_LENGTH - 1U, &Args[FullNameIdx][0]);
                        path[MTP_MAX_FILENAME_LENGTH - 1U] = '\0';
                    } else {
                        AmbaUtility_StringCopy(path, UTIL_MAX_STR_TOKEN_LENGTH, &Args[FullNameIdx][0]);
                        //break;
                    }
                }
            }

            uRval_fs = AmbaFS_FileClose(mtp_db_file);
            if (uRval_fs != 0U) {
                // need to process it?
            }
        }
    }

    return uRet;
}

static UINT64 DbGetFileSize(ULONG Handle)
{
    UINT8 attr = DbGetAttributeFromHandle(Handle);
    APP_USB_FILE_INFO pStat;
    UINT64 uRet = 0U;

    if ((attr & MTP_DB_ATTRIBUTE_DIR) != 0U) {
        // no action. Return 0 directly.
    } else {
        UINT32 status = 0;
        UINT64 size   = GetObjectSizeFromBuf(Handle);
        if ((size == 0U) || (size == 0xFFFFFFFFU)) {
            char FullFileName[MTP_MAX_FILENAME_LENGTH];

            status = DbGetFilePathFromHandle(Handle, FullFileName);
            if (status == USB_ERR_SUCCESS) {
                UINT32 uRval = AppUsb_GetFileInfo(FullFileName, &pStat);
                // Check if the file info is obtained correctly.
                // It's a projection while object data send operation, since the file is not created.
                // The file size info is derived from host through send object info operation, it should be already in buf.
                if (uRval == 0U) {
                    SendObjectSizeToBuf(Handle, pStat.Size);
                } else {
                    status = USB_ERR_FAIL;
                }
            }
        }

        if (status == USB_ERR_SUCCESS) {
            //dbg("%s(): Handle %d, Len = %lld", __func__, Handle, GetObjectSizeFromBuf(Handle));
            uRet = GetObjectSizeFromBuf(Handle);
        }
    }
    return uRet;
}

static USBD_MTP_OBJECT_s objInfo GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static void ParseFileName(const char *Path, char *FileName)
{
    UINT32 StringLength = (UINT32)AmbaUtility_StringLength(Path);
    UINT32 Idx;

    Idx          = StringLength;

    if (StringLength == 0U) {
        *FileName = '\0';
    } else {
        while (Path[Idx - 1U] != '\\') {
            Idx--;
            if (Idx == 0U) {
                break;
            }
        }
        if (Idx == 0U) {
            AmbaPrint_PrintUInt5("ParseFileName(): NG", 0U, 0U, 0U, 0U, 0U);
            *FileName = '\0';
        } else {
            UINT32 Len = StringLength - Idx;
            AmbaSvcWrap_MisraMemcpy(FileName, &Path[Idx], (SIZE_t)Len);
            FileName[Len] = '\0';
        }
    }
    return;
}

static void Asc2PascalString(const char *Asc, UINT8 *Pascal)
{
    const char *pSrc = Asc;
    UINT8      *pDst = Pascal;
    UINT32     IdxS = 0U, IdxD = 0U, StringLength = (UINT32)AmbaUtility_StringLength(Asc);

    if (StringLength == 0U) {
        pDst[IdxD] = 0;
    } else {
        pDst[IdxD] = (UINT8)(StringLength + 1U);
        IdxD++;

        while (StringLength != 0U) {
            pDst[IdxD] = (UINT8)pSrc[IdxS];
            IdxD++;
            IdxS++;
            pDst[IdxD] = 0;
            IdxD++;
            StringLength--;
        }
        pDst[IdxD] = 0;
        IdxD++;
        pDst[IdxD] = 0;
    }

    return;
}

static UINT32 DbGetParentHandle(ULONG Handle)
{
    UINT32 uRet;

    if (Handle > (ULONG)scan_data.max_files) {
        AmbaPrint_PrintUInt5("DbGetParentHandle(): handle > max_files.", 0U, 0U, 0U, 0U, 0U);
        uRet = 0U;
    } else {
        //dbg("%d's parent is %d", Handle, scan_data.index_parent[Handle]);
        uRet = scan_data.index_parent[Handle];
    }
    return uRet;
}

static UINT32 DbGetObjectInfo(ULONG Handle, USBD_MTP_OBJECT_s **object)
{
    //static MTP_PARSE_OBJECT_INFO_s parsedInfo;
    char FullFileName[MTP_MAX_FILENAME_LENGTH], FileName[MTP_MAX_FILENAME_LENGTH];
    UINT8 attribute = DbGetAttributeFromHandle(Handle);

    //AMBA_FS_STAT             pStat;

    AmbaSvcWrap_MisraMemset(&objInfo, 0, sizeof(USBD_MTP_OBJECT_s));
    //AmbaSvcWrap_MisraMemset(&parsedInfo, 0, sizeof(MTP_PARSE_OBJECT_INFO_s));

    if (DbGetFilePathFromHandle(Handle, FullFileName) == USB_ERR_SUCCESS) {
        ParseFileName(FullFileName, FileName);
        Asc2PascalString(FileName, objInfo.Filename);
    }

    //If file size > 4GB, set it as 0xFFFFFFFF
    //Host will get the actual length through object property
    if (DbGetFileSize(Handle) >= 0xFFFFFFFF00000000ULL) {
        objInfo.CompressedSize = 0xFFFFFFFFU;
    } else {
        objInfo.CompressedSize = (UINT32)DbGetFileSize(Handle);
    }
    objInfo.StorageId      = MtpClassInfo.DeviceInfo.StorageId;
    objInfo.HandleId       = (UINT32)Handle;
    objInfo.ParentObject   = DbGetParentHandle(Handle);

    /*   A12 SSP do not support DCF, remove parser code.  */
    if ((attribute & MTP_DB_ATTRIBUTE_DIR) != 0U) {
        objInfo.CompressedSize  = 0;
        objInfo.Format          = MTP_OFC_ASSOCIATION;
        objInfo.ImageBitDepth   = 0;
        objInfo.ImagePixHeight  = 0;
        objInfo.ImagePixWidth   = 0;
        objInfo.ThumbFormat     = MTP_OFC_UNDEFINED;
        objInfo.AssociationType = MTP_ASSOCIATION_TYPE_GENERIC_FOLDER;
    } else {
        objInfo.Format         = MTP_OFC_UNDEFINED;
        objInfo.ImageBitDepth  = 0;
        objInfo.ImagePixHeight = 0;
        objInfo.ImagePixWidth  = 0;
        objInfo.ThumbFormat    = MTP_OFC_UNDEFINED;
    }

    *object = &objInfo;

    return (OK);
}

static void DbSetDrive(UINT8 drive)
{
    MTP_DB_FILE_NAME[0] = (char)drive;
}

static UINT8 DbGetAttributeFromFileName(char *filename)
{
    AmbaMisra_TouchUnused(filename);
    /*   SSP do not support DCF, remove parser code.  */
    return 0;
}

static void DbAddHandle(APP_USB_FS_DTA *fsdta, const char *path)
{
    char *ptr;
    UINT8 attribute = 0;
    UINT32 idx      = scan_data.handle_id;
    UINT32 bytes_written, Rval;
    PF_CHAR target;

    target = fsdta->LongName[0];
    if (target != 0) {
        ptr = (char *)fsdta->LongName;
    } else {
        ptr = (char *)fsdta->FileName;
    }

    //sprintf(scan_data.string_buffer, "%d\t%d\t%s\t%s\\%s\t",
    //        (INT32)scan_data.handle_id,
    //        (INT32)scan_data.handle_parent,
    //        ptr,
    //        path,
    //        ptr);
    AmbaSvcWrap_MisraMemset(scan_data.string_buffer, 0, MTP_DB_MAX_BYTES_PER_LINE);
    StringAppendUInt32(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, scan_data.handle_id, 10);
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
    StringAppendUInt32(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, scan_data.handle_parent, 10);
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, ptr);
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, path);
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\\");
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, ptr);
    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");

    if (((UINT8)fsdta->Attribute & APP_FS_ATTR_DIR) != 0U) {
        attribute |= MTP_DB_ATTRIBUTE_DIR;
    }

    attribute |= DbGetAttributeFromFileName(ptr);
    attribute |= MTP_DB_ATTRIBUTE_USED;

    // cache the file position and parent handle
    if ((idx < MTP_DB_MAX_FILE_NUMBER)) {
        scan_data.index_parent[idx]    = scan_data.handle_parent;
        scan_data.index_attribute[idx] = attribute;
    } else {
        AmbaPrint_PrintUInt5("DbAddHandle(): can't cache index for object handle %d", idx, 0U, 0U, 0U, 0U);
    }

    scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 1U] = '\n';
    scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 2U] = '\r';
    Rval = AmbaFS_FileWrite(scan_data.string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, scan_data.db_file, &bytes_written);
    if ((Rval != OK) || (bytes_written != MTP_DB_MAX_BYTES_PER_LINE)) {
        AmbaPrint_PrintUInt5("DbAddHandle(): write DB data fail.", 0U, 0U, 0U, 0U, 0U);
    }
    //dbg("[MTPImpl] Add %s", scan_data.string_buffer);
}

static UINT32 DbInsertHandle(char *path, const USBD_MTP_OBJECT_s *object, const ULONG parent_handle)
{
    UINT32 idx            = 0U;
    UINT8 db_attr         = 0;
    UINT8 found           = 0;
    UINT32 parent_handlex = (UINT32)parent_handle;

    // find an empty handle
    for (idx = 1U; idx < scan_data.handle_count_max; idx++) {
        if ((scan_data.index_attribute[idx] & MTP_DB_ATTRIBUTE_USED) == 0U) {
            found = 1;
            break;
        }
    }

    if (found == 0U) {
        AmbaPrint_PrintUInt5("DbInsertHandle(): can't find an empty handle.", 0U, 0U, 0U, 0U, 0U);
        idx = 0;
    } else {
        db_attr |= DbGetAttributeFromFileName(path);
        db_attr |= MTP_DB_ATTRIBUTE_USED;
        if (object->Format == MTP_OFC_ASSOCIATION) {
            db_attr |= MTP_DB_ATTRIBUTE_DIR;
        }

        if (parent_handlex == 0xFFFFFFFFU) {
            parent_handlex = 0U;
        }

        // cache the file position and parent handle
        if (idx < MTP_DB_MAX_FILE_NUMBER) {
            // open cache file
            AMBA_FS_FILE *fd = NULL;
            UINT32 pos       = MTP_DB_MAX_BYTES_PER_LINE * idx, Rval;

            Rval = AmbaFS_FileOpen(MTP_DB_FILE_NAME, "r+", &fd);
            if ((Rval == OK) && (fd != NULL)) {
                if (AmbaFS_FileSeek(fd, (INT64)pos, (INT32)APP_FS_SEEK_START) != 0U) {
                    AmbaPrint_PrintUInt5("DbInsertHandle(): can't seek to %llu for object handle %d", pos, idx, 0U, 0U, 0U);
                } else {
                    // Since only 1 MTP session is supported
                    // we don't need to consider re-entry case.
                    // Just use static variables
                    static char name[256];
                    char *string_buffer = scan_data.string_buffer;
                    UINT32 bytes_written;

                    ParseFileName(path, name);

                    AmbaSvcWrap_MisraMemset(string_buffer, 0, MTP_DB_MAX_BYTES_PER_LINE);
                    string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 2U] = '\r';
                    string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 1U] = '\n';

                    //sprintf(string_buffer, "%d\t%d\t%s\t%s\t",
                    //        (INT32)idx,
                    //        (INT32)parent_handlex,
                    //        name,
                    //        path);
                    StringAppendUInt32(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, idx, 10);
                    StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                    StringAppendUInt32(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, parent_handlex, 10);
                    StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                    StringCat(string_buffer, MTP_DB_MAX_BYTES_PER_LINE, name);
                    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");
                    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, path);
                    StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "\t");

                    Rval = AmbaFS_FileWrite(string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, fd, &bytes_written);
                    if ((Rval != OK) || (bytes_written != MTP_DB_MAX_BYTES_PER_LINE)) {
                        AmbaPrint_PrintUInt5("DbInsertHandle(): can't write %d for object handle %d", MTP_DB_MAX_BYTES_PER_LINE, idx, 0U, 0U, 0U);
                    }
                }
                if (AmbaFS_FileClose(fd) != 0U) {
                    // ignore this error
                }
            } else {
                AmbaPrint_PrintUInt5("DbInsertHandle(): can't open DB file for object handle %d", idx, 0U, 0U, 0U, 0U);
            }
            scan_data.index_parent[idx]    = parent_handlex;
            scan_data.index_attribute[idx] = db_attr;
            scan_data.handle_count++;
        } else {
            AmbaPrint_PrintUInt5("DbInsertHandle(): can't cache index for object handle %d", idx, 0U, 0U, 0U, 0U);
        }

        AmbaPrint_PrintStr5("[MTPImpl] Add %s ", path, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("     for index %d", idx, 0U, 0U, 0U, 0U);

        if (scan_data.handle_count > MTP_DB_MAX_FILE_NUMBER) {
            AmbaPrint_PrintUInt5("DbInsertHandle(): Reach maximum file number!", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return idx;
}

static UINT32 DbIsObjectHandleValid(ULONG Handle)
{
    UINT32 uRet;

    if (Handle >= (ULONG)scan_data.handle_count_max) {
        AmbaPrint_PrintUInt5("DbIsObjectHandleValid(): Handle > handle_count_max", 0U, 0U, 0U, 0U, 0U);
        uRet = 0U;
    } else if ((scan_data.index_attribute[Handle] & MTP_DB_ATTRIBUTE_USED) != 0U) {
        uRet = 1U;
    } else {
        AmbaPrint_PrintUInt5("DbIsObjectHandleValid(): Handle is not used", 0U, 0U, 0U, 0U, 0U);
        uRet = 0U;
    }
    return uRet;
}

static UINT32 DbGetFileNameFromHandle(ULONG handle, char *file_name)
{
    UINT32 uRet;

    char path[MTP_DB_MAX_BYTES_PER_LINE];

    if (file_name == NULL) {
        uRet = USB_ERR_PARAMETER_NULL;
    } else {
        AmbaSvcWrap_MisraMemset(path, 0, MTP_DB_MAX_BYTES_PER_LINE);

        uRet = DbGetFilePathFromHandle(handle, path);

        if (uRet == USB_ERR_SUCCESS) {
            ParseFileName(path, file_name);
        }
    }

    return uRet;
}

static UINT32 DbGetNumberOfObjects(ULONG FormatCode, ULONG Association)
{
    UINT32 AssociationX = (UINT32)Association;
    UINT32 uRet;

    if ((FormatCode == 0U) && (AssociationX == 0U)) {
        uRet = scan_data.handle_count;
    } else {
        if (FormatCode == 0U) {
            UINT32 i     = 0U;
            UINT32 count = 0U;
            if (AssociationX == 0xFFFFFFFFU) {
                AssociationX = 0x0U;
            }
            for (i = 1U; i <= scan_data.handle_count_max; i++) {
                if ((scan_data.index_parent[i] == AssociationX) && ((scan_data.index_attribute[i] & MTP_DB_ATTRIBUTE_USED) != 0U)) {
                    count++;
                }
            }
            uRet = count;
        } else {
            AmbaPrint_PrintUInt5("DbGetNumberOfObjects(): Not impl.", 0U, 0U, 0U, 0U, 0U);
            uRet = 0U;
        }
    }
    return uRet;
}

static UINT32 MTP_Utility_ShortPut(UINT8 *address, ULONG value)
{
    UINT8  *pAddr = address;

    pAddr[0] = (UINT8) (value & 0xFFU);
    pAddr[1] = (UINT8) ((value >> 8U) & 0xFFU);

    return 2U;
}

static UINT32 MTP_Utility_LongPut(UINT8 *address, ULONG value)
{
    UINT8  *pAddr = address;

    pAddr[0] = (UINT8) (value & 0x0FFU);
    pAddr[1] = (UINT8) ((value >> 8U) & 0x0FFU);
    pAddr[2] = (UINT8) ((value >> 16U) & 0x0FFU);
    pAddr[3] = (UINT8) ((value >> 24U) & 0x0FFU);

    return 4U;
}

static UINT32 MTP_Utility_Add_String(UINT8 *buf, const WCHAR *str_uni)
{
    UINT32 uni_str_len = 0U;
    UINT32 pos         = 0U;

    uni_str_len = (UINT32)WStrLen(str_uni);

    if (uni_str_len == 0U) {
        buf[pos] = 0x0;
        pos++;
    } else {
        buf[pos] = (UINT8)((uni_str_len + 1U) & 0x0FFU);
        pos++;
        AmbaSvcWrap_MisraMemcpy(&buf[pos], str_uni, (SIZE_t)uni_str_len * 2U);
        pos += uni_str_len * 2U;
        // put NULL terminated char.
        buf[pos] = 0;
        pos++;
        buf[pos] = 0;
        pos++;
    }

    return pos;
}

static UINT32 DbFillObjectHandles(ULONG FormatCode, ULONG Association, ULONG *Handles)
{
    UINT32 i            = 0U;
    UINT32 count        = 0U;
    UINT32 AssociationX = (UINT32)Association;
    UINT32 uRet         = 0U;

    if ((FormatCode == 0U) && (AssociationX == 0U)) {
        for (i = 1U; i <= scan_data.handle_count_max; i++) {
            if ((scan_data.index_attribute[i] & MTP_DB_ATTRIBUTE_USED) != 0U) {
                if (0U < MTP_Utility_LongPut((UINT8 *)&Handles[count + 1U], i)) {
                    count++;
                }
            }
        }

        if ((count) != scan_data.handle_count) {
            AmbaPrint_PrintUInt5("DbFillObjectHandles(): DB doesn't match", 0U, 0U, 0U, 0U, 0U);
        }

        if (0U < MTP_Utility_LongPut((UINT8 *) Handles, scan_data.handle_count)) {
            uRet = scan_data.handle_count;
        }
    } else {
        if (FormatCode == 0U) {
            if (AssociationX == 0xFFFFFFFFU) {
                AssociationX = 0x0U;
            }

            for (i = 1U; i <= scan_data.handle_count_max; i++) {
                if ((scan_data.index_parent[i] == AssociationX) && ((scan_data.index_attribute[i] & MTP_DB_ATTRIBUTE_USED) != 0U)) {
                    if (0U < MTP_Utility_LongPut((UINT8 *)&Handles[count + 1U], i)) {
                        count++;
                    }
                }
            }

            if (0U < MTP_Utility_LongPut((UINT8 *) Handles, count)) {
                uRet = count;
            }
        } else {
            AmbaPrint_PrintUInt5("DbFillObjectHandles(): Not impl.", 0U, 0U, 0U, 0U, 0U);
            uRet = 0U;
        }
    }
    return uRet;
}

static void DbClean(void)
{
    AmbaSvcWrap_MisraMemset(&scan_data, 0, sizeof(scan_data));
}

static UINT32 DbIsInIgnoreList(const APP_USB_FS_DTA *fsdta, const char *path)
{
    UINT32 uRet = 0U;
    UINT32 len = (UINT32)AmbaUtility_StringLength(path);

    // just hide MTP_DB_FILE_NAME
    if (len == 2U) {
        const char *ptr;
        PF_CHAR target;

        target = fsdta->LongName[0];
        if (target != 0) {
            ptr = (const char *)fsdta->LongName;
        } else {
            ptr = (const char *)fsdta->FileName;
        }
        if (AmbaUtility_StringCompare(ptr, &MTP_DB_FILE_NAME[3], AmbaUtility_StringLength(ptr)) == 0) {
            uRet = 1U;
        }
    }

    return uRet;
}

static void DbScanDirectoryL3(const char *path, UINT8 count_only)
{
    UINT32 uRet_fs;
    APP_FS_DTA_t fsdta; // Need to use stack for passing misra-c, it requires 1K memory.
    char pattern[MTP_MAX_FILENAME_LENGTH];
    char next_path[MTP_MAX_FILENAME_LENGTH];
    const char *ptr;
    UINT32 len;
    UINT32 mtp_parent_id = scan_data.handle_parent;
    UINT32 uRet          = 0U;
    UINT32 is_break;

    // why assign again? Move to here or "unreachable code" issue happens.
    scan_data.handle_parent = mtp_parent_id;

    scan_data.cur_level++;

    len = (UINT32)AmbaUtility_StringLength(path);
    // check file name length
    len = len + 3U;
    if (len >= MTP_MAX_FILENAME_LENGTH) {
        AmbaPrint_PrintStr5("DbScanDirectoryL3():[1] %s\\* ...", path, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("DbScanDirectoryL3():[1]over %d", MTP_MAX_FILENAME_LENGTH, 0U, 0U, 0U, 0U);
        uRet = USB_ERR_FAIL;
    }

    if (uRet == 0U) {
        //sprintf(pattern, "%s\\*", path);
        pattern[0] = '\0';
        StringCat(pattern, MTP_MAX_FILENAME_LENGTH, path);
        StringCat(pattern, MTP_MAX_FILENAME_LENGTH, "\\*");

        // scan files first
        uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_ARCH, &fsdta);
        while (uRet_fs == 0U) {
            if (((UINT8)fsdta.Attribute & AMBA_FS_ATTR_VOLUME) != 0U) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
                continue;
            }

            if ((DbIsInIgnoreList(&fsdta, path) == 1U) && (scan_data.cur_level == 1U)) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
                continue;
            }

            if (scan_data.handle_count < scan_data.max_files) {
                if (count_only == 0U) {
                    DbAddHandle(&fsdta, path);
                }
                scan_data.handle_id++;
                scan_data.handle_count++;
            } else {
                AmbaPrint_PrintUInt5("DbScanDirectoryL3(): reach max files %d.", scan_data.max_files, 0U, 0U, 0U, 0U);
                break;
            }
            uRet_fs = AppUsb_FileFindNext(&fsdta);
        }

        is_break = 0U;

        // scan directory then
        uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_DIR, &fsdta);
        while (uRet_fs == 0U) {
            if (is_break == 1U) {
                break;
            }

            if (((UINT8)fsdta.Attribute & (APP_FS_ATTR_VOLUME | APP_FS_ATTR_HIDDEN | APP_FS_ATTR_SYSTEM)) != 0U) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
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
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if ((ptr[0] == '.') && (ptr[1] == '.') && (ptr[2] == '\0')) {
                    // skip '..' directory
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if (scan_data.handle_count >= scan_data.max_files) {
                    AmbaPrint_PrintUInt5("DbScanDirectoryL3(): reach max files %d.", scan_data.max_files, 0U, 0U, 0U, 0U);
                    is_break = 1U;
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                // check file name length
                {
                    UINT32 len_p1;
                    UINT32 len_p2;
                    len_p1 = (UINT32)AmbaUtility_StringLength(path);
                    len_p2 = (UINT32)AmbaUtility_StringLength(ptr);
                    len = len_p1 + len_p2;
                }
                if (len >= MTP_MAX_FILENAME_LENGTH) {
                    AmbaPrint_PrintStr5("DbScanDirectoryL3():[2] %s\\%s ...", path, ptr, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("DbScanDirectoryL3():[2] over %d", MTP_MAX_FILENAME_LENGTH, 0U, 0U, 0U, 0U);
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if (count_only == 0U) {
                    DbAddHandle(&fsdta, path);
                }

                scan_data.handle_id++;
                scan_data.handle_count++;

                // scan this directory
                //sprintf(next_path, "%s\\%s", path, ptr);
                next_path[0] = '\0';
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, path);
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, "\\");
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, ptr);

                //dbg("%s(): next_path = %s", __func__, next_path);
                if (scan_data.cur_level <= scan_data.max_level) {
                    AmbaPrint_PrintUInt5("DbScanDirectoryL3(): no L4 scan is supported.", 0U, 0U, 0U, 0U, 0U);
                    is_break = 1U;
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                } else {
                    AmbaPrint_PrintUInt5("DbScanDirectoryL3(): scan depth reach max %d.", scan_data.max_level, 0U, 0U, 0U, 0U);
                    is_break = 1U;
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }
            }
            uRet_fs = AppUsb_FileFindNext(&fsdta);
        }
        scan_data.cur_level--;
    }

    return;
}


static void DbScanDirectoryL2(const char *path, UINT8 count_only)
{
    UINT32 uRet_fs;
    APP_USB_FS_DTA fsdta; // Need to use stack for passing misra-c, it requires 1K memory.
    char pattern[MTP_MAX_FILENAME_LENGTH];
    char next_path[MTP_MAX_FILENAME_LENGTH];
    const char *ptr;
    UINT32 len;
    UINT32 mtp_parent_id = scan_data.handle_parent;
    UINT32 uRet          = 0U;
    UINT32 is_break;

    scan_data.cur_level++;

    // check file name length
    len = (UINT32)AmbaUtility_StringLength(path);
    len = len + 3U;
    if (len >= MTP_MAX_FILENAME_LENGTH) {
        AmbaPrint_PrintStr5("DbScanDirectoryL2():[1] %s\\* ...", path, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("DbScanDirectoryL2():[1] over %d", MTP_MAX_FILENAME_LENGTH, 0U, 0U, 0U, 0U);
        uRet = USB_ERR_FAIL;
    }

    if (uRet == 0U) {
        //sprintf(pattern, "%s\\*", path);
        pattern[0] = '\0';
        StringCat(pattern, MTP_MAX_FILENAME_LENGTH, path);
        StringCat(pattern, MTP_MAX_FILENAME_LENGTH, "\\*");

        // scan files first
        uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_ARCH, &fsdta);
        while (uRet_fs == 0U) {
            if (((UINT8)fsdta.Attribute & APP_FS_ATTR_VOLUME) != 0U) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
                continue;
            }

            if ((DbIsInIgnoreList(&fsdta, path) == 1U) && (scan_data.cur_level == 1U)) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
                continue;
            }

            if (scan_data.handle_count < scan_data.max_files) {
                if (count_only == 0U) {
                    DbAddHandle(&fsdta, path);
                }
                scan_data.handle_id++;
                scan_data.handle_count++;
            } else {
                AmbaPrint_PrintUInt5("DbScanDirectoryL2(): reach max files %d.", scan_data.max_files, 0U, 0U, 0U, 0U);
                break;
            }
            uRet_fs = AppUsb_FileFindNext(&fsdta);
        }

        is_break = 0U;

        // scan directory then
        uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_DIR, &fsdta);
        while (uRet_fs == 0U) {
            if (is_break == 1U) {
                break;
            }

            if (((UINT8)fsdta.Attribute & (APP_FS_ATTR_VOLUME | APP_FS_ATTR_HIDDEN | APP_FS_ATTR_SYSTEM)) != 0U) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
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
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if ((ptr[0] == '.') && (ptr[1] == '.') && (ptr[2] == '\0')) {
                    // skip '..' directory
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if (scan_data.handle_count >= scan_data.max_files) {
                    AmbaPrint_PrintUInt5("DbScanDirectoryL2(): reach max files %d.", scan_data.max_files, 0U, 0U, 0U, 0U);
                    is_break = 1U;
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                // check file name length
                {
                    UINT32 len_p1;
                    UINT32 len_p2;
                    len_p1 = (UINT32)AmbaUtility_StringLength(path);
                    len_p2 = (UINT32)AmbaUtility_StringLength(ptr);
                    len = len_p1 + len_p2;
                }
                if (len >= MTP_MAX_FILENAME_LENGTH) {
                    AmbaPrint_PrintStr5("DbScanDirectoryL2():[2] %s\\%s ...", path, ptr, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("DbScanDirectoryL2():[2] over %d", MTP_MAX_FILENAME_LENGTH, 0U, 0U, 0U, 0U);
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if (count_only == 0U) {
                    DbAddHandle(&fsdta, path);
                }

                scan_data.handle_id++;
                scan_data.handle_count++;

                // scan this directory
                //sprintf(next_path, "%s\\%s", path, ptr);
                next_path[0] = '\0';
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, path);
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, "\\");
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, ptr);

                //dbg("%s(): next_path = %s", __func__, next_path);
                if (scan_data.cur_level <= scan_data.max_level) {
                    scan_data.handle_parent = (scan_data.handle_id - 1U);
                    DbScanDirectoryL3(next_path, count_only);
                } else {
                    AmbaPrint_PrintUInt5("DbScanDirectoryL2(): scan depth reach max %d.", scan_data.max_level, 0U, 0U, 0U, 0U);
                    is_break = 1U;
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }
                scan_data.handle_parent = mtp_parent_id;
            }
            uRet_fs = AppUsb_FileFindNext(&fsdta);
        }
        scan_data.cur_level--;
    }

    return;
}


static void DbScanDirectoryL1(const char *path, UINT8 count_only)
{
    UINT32 uRet_fs;
    APP_USB_FS_DTA fsdta; // Need to use stack for passing misra-c, it requires 1K memory.
    char pattern[MTP_MAX_FILENAME_LENGTH];
    char next_path[MTP_MAX_FILENAME_LENGTH];
    const char *ptr;
    UINT32 len;
    UINT32 mtp_parent_id = scan_data.handle_parent;
    UINT32 uRet          = 0U;
    UINT32 is_break;

    scan_data.cur_level++;

    // check file name length
    len = (UINT32)AmbaUtility_StringLength(path);
    len = len + 3U;
    if (len >= MTP_MAX_FILENAME_LENGTH) {
        AmbaPrint_PrintStr5("DbScanDirectoryL1():[1] %s\\* ...", path, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("DbScanDirectoryL1():[1] over %d", MTP_MAX_FILENAME_LENGTH, 0U, 0U, 0U, 0U);
        uRet = USB_ERR_FAIL;
    }

    if (uRet == 0U) {
        //sprintf(pattern, "%s\\*", path);
        pattern[0] = '\0';
        StringCat(pattern, MTP_MAX_FILENAME_LENGTH, path);
        StringCat(pattern, MTP_MAX_FILENAME_LENGTH, "\\*");

        // scan files first
        uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_ARCH, &fsdta);
        while (uRet_fs == 0U) {
            if (((UINT8)fsdta.Attribute & APP_FS_ATTR_VOLUME) != 0U) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
                continue;
            }

            if ((DbIsInIgnoreList(&fsdta, path) == 1U) && (scan_data.cur_level == 1U)) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
                continue;
            }

            if (scan_data.handle_count < scan_data.max_files) {
                if (count_only == 0U) {
                    DbAddHandle(&fsdta, path);
                }
                scan_data.handle_id++;
                scan_data.handle_count++;
            } else {
                AmbaPrint_PrintUInt5("DbScanDirectoryL1(): reach max files %d.", scan_data.max_files, 0U, 0U, 0U, 0U);
                break;
            }
            uRet_fs = AppUsb_FileFindNext(&fsdta);
        }

        is_break = 0U;

        // scan directory then
        uRet_fs = AppUsb_FileFindFirst(pattern, APP_FS_ATTR_DIR, &fsdta);
        while (uRet_fs == 0U) {
            if (is_break == 1U) {
                break;
            }

            if (((UINT8)fsdta.Attribute & (APP_FS_ATTR_VOLUME | APP_FS_ATTR_HIDDEN | APP_FS_ATTR_SYSTEM)) != 0U) {
                uRet_fs = AppUsb_FileFindNext(&fsdta);
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
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if ((ptr[0] == '.') && (ptr[1] == '.') && (ptr[2] == '\0')) {
                    // skip '..' directory
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if (scan_data.handle_count >= scan_data.max_files) {
                    AmbaPrint_PrintUInt5("DbScanDirectoryL1(): reach max files %d.", scan_data.max_files, 0U, 0U, 0U, 0U);
                    is_break = 1U;
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                // check file name length
                {
                    UINT32 len_p1;
                    UINT32 len_p2;
                    len_p1 = (UINT32)AmbaUtility_StringLength(path);
                    len_p2 = (UINT32)AmbaUtility_StringLength(ptr);
                    len = len_p1 + len_p2;
                }
                if (len >= MTP_MAX_FILENAME_LENGTH) {
                    AmbaPrint_PrintStr5("DbScanDirectoryL1():[2] %s\\%s ...", path, ptr, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("DbScanDirectoryL1():[2] over %d", MTP_MAX_FILENAME_LENGTH, 0U, 0U, 0U, 0U);
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }

                if (count_only == 0U) {
                    DbAddHandle(&fsdta, path);
                }

                scan_data.handle_id++;
                scan_data.handle_count++;

                // scan this directory
                //sprintf(next_path, "%s\\%s", path, ptr);
                next_path[0] = '\0';
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, path);
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, "\\");
                StringCat(next_path, MTP_MAX_FILENAME_LENGTH, ptr);

                //dbg("%s(): next_path = %s", __func__, next_path);
                if (scan_data.cur_level <= scan_data.max_level) {
                    scan_data.handle_parent = (scan_data.handle_id - 1U);
                    DbScanDirectoryL2(next_path, count_only);
                } else {
                    AmbaPrint_PrintUInt5("DbScanDirectoryL1(): scan depth reach max %d.", scan_data.max_level, 0U, 0U, 0U, 0U);
                    is_break = 1U;
                    uRet_fs = AppUsb_FileFindNext(&fsdta);
                    continue;
                }
                // needs to check this. why the value rewrite again in specific case?
                scan_data.handle_parent = mtp_parent_id;
            }
            uRet_fs = AppUsb_FileFindNext(&fsdta);
        }
        scan_data.cur_level--;
    }

    return;
}

static UINT32 DbScan(void)
{
    UINT32 uRet = USB_ERR_SUCCESS;
    UINT32 uRet_fs;
    UINT32 bytes_written;
    AMBA_FS_FILE *mtp_db_file = NULL;
    char root_dir[3]          = "c:";

    root_dir[0] = (char)MtpClassInfo.DeviceInfo.RootPath[0];

    DbClean();

    uRet_fs = AmbaFS_FileOpen(MTP_DB_FILE_NAME, "w", &mtp_db_file);

    if ((uRet_fs != OK) || (mtp_db_file == NULL)) {
        AmbaPrint_PrintUInt5("DbScan(): can't create db file.", 0U, 0U, 0U, 0U, 0U);
        uRet = USB_ERR_FAIL;
    }

    if (uRet == USB_ERR_SUCCESS) {
        scan_data.handle_id = 1;
        scan_data.max_level = 2;
        scan_data.max_files = MTP_DB_MAX_FILE_NUMBER;
        scan_data.db_file   = mtp_db_file;

        // get total file number
        DbScanDirectoryL1(root_dir, 1);

        scan_data.handle_count_max = scan_data.handle_count + 4U;

        AmbaSvcWrap_MisraMemset(scan_data.index_parent, 0, sizeof(scan_data.index_parent));
        AmbaSvcWrap_MisraMemset(scan_data.index_attribute, 0, sizeof(scan_data.index_attribute));

        AmbaSvcWrap_MisraMemset(scan_data.string_buffer, 0, MTP_DB_MAX_BYTES_PER_LINE);
        StringCat(scan_data.string_buffer, MTP_DB_MAX_BYTES_PER_LINE, "ID\tPARENT\tNAME\tPATH");
        scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 1U] = '\n';
        scan_data.string_buffer[MTP_DB_MAX_BYTES_PER_LINE - 2U] = '\r';
        uRet_fs = AmbaFS_FileWrite(scan_data.string_buffer, 1U, MTP_DB_MAX_BYTES_PER_LINE, mtp_db_file, &bytes_written);
        if ((uRet_fs != OK) || (bytes_written != MTP_DB_MAX_BYTES_PER_LINE)) {
            AmbaPrint_PrintUInt5("DbScan(): can't write DB file.", 0U, 0U, 0U, 0U, 0U);
            uRet = USB_ERR_FAIL;
        } else {
            scan_data.handle_id     = 1;
            scan_data.handle_parent = 0;
            scan_data.handle_count  = 0;
            scan_data.cur_level     = 0;

            // get information for files
            DbScanDirectoryL1(root_dir, 0);
        }

        uRet_fs = AmbaFS_FileClose(mtp_db_file);
        if (uRet_fs != 0U) {
            AmbaPrint_PrintUInt5("DbScan(): can't close DB file.", 0U, 0U, 0U, 0U, 0U);
            uRet = USB_ERR_FAIL;
        }

        scan_data.db_file = NULL;
    }

    return uRet;
}

static UINT32 DbGetObjectHeight(ULONG Handle)
{
    AmbaMisra_TouchUnused(&Handle);

    AmbaPrint_PrintUInt5("DbGetObjectHeight(): Not support", 0U, 0U, 0U, 0U, 0U);
    return 0U;
}

static UINT32 DbGetObjectWidth(ULONG Handle)
{
    AmbaMisra_TouchUnused(&Handle);

    AmbaPrint_PrintUInt5("DbGetObjectWidth(): Not support", 0U, 0U, 0U, 0U, 0U);
    return 0U;
}

static UINT32 DbDeleteObject(ULONG Handle)
{
    UINT32 uRet;

    if (scan_data.handle_count > 0U) {
        char path[MTP_MAX_FILENAME_LENGTH];
        UINT32 uRet_fs;

        // remove file first
        UINT8 attr = DbGetAttributeFromHandle(Handle);
        uRet = DbGetFilePathFromHandle(Handle, path);
        if (uRet == USB_ERR_SUCCESS) {
            if ((attr & MTP_DB_ATTRIBUTE_DIR) != 0U) {
                uRet_fs = AppUsb_RemoveDir(path);
                if (uRet_fs != 0U) {
                    AmbaPrint_PrintStr5("DbDeleteObject(): can't remove folder %s ...", path, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("DbDeleteObject(): code %d", uRet_fs, 0U, 0U, 0U, 0U);
                    uRet = MTP_RC_ACCESS_DENIED;
                }
            } else {
                uRet_fs = AmbaFS_Remove(path);
                if (uRet_fs != 0U) {
                    AmbaPrint_PrintStr5("DbDeleteObject(): can't remove file %s ...", path, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("DbDeleteObject(): code %d", uRet_fs, 0U, 0U, 0U, 0U);
                    uRet = MTP_RC_ACCESS_DENIED;
                }
            }

            if (uRet == USB_ERR_SUCCESS) {
                AmbaPrint_PrintStr5("DbDeleteObject(): Path %s ...", path, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("DbDeleteObject(): Handle is removed. [0x%x]", uRet_fs, 0U, 0U, 0U, 0U);
                uRet = MTP_RC_OK;
                scan_data.handle_count--;
                scan_data.index_parent[Handle]    = 0;
                scan_data.index_attribute[Handle] = 0;
                ResetPreParseBuf(Handle);
            }
        } else {
            uRet = MTP_RC_INVALID_OBJECT_HANDLE;
        }
    } else {
        uRet = MTP_RC_INVALID_OBJECT_HANDLE;
    }

    return uRet;
}

static void DbInitZero(void)
{
    AmbaSvcWrap_MisraMemset(PreParseBuffer, 0, sizeof(PreParseBuffer));
    AmbaSvcWrap_MisraMemset(&scan_data, 0, sizeof(scan_data));
    AmbaSvcWrap_MisraMemset(&objInfo, 0, sizeof(objInfo));
    return;
}

#endif /* COMSVC_MTP_DBASE_H */
