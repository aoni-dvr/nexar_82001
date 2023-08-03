/**
 * @file AmbaCFS_Util.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */
#ifndef AMBACFS_UTIL_H
#define AMBACFS_UTIL_H

#include <AmbaCFS.h>
#include <AmbaKAL.h>
#include <AmbaFS.h>
#include <AmbaCache.h>
#include <AmbaDef.h>
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#include <AmbaUtility.h>
#include <AmbaGDMA.h>
#include <AmbaPrint.h>

/**
 * Error code
 */
#define CFS_OK                          OK                      /**< Execution OK */
#define CFS_ERR_ARG                     (CFS_ERR_0000)          /**< Invalid argument */
#define CFS_ERR_FS                      (CFS_ERR_0002)          /**< FS error */
#define CFS_ERR_OBJ_UNAVAILABLE         (CFS_ERR_0003)          /**< A request object is unavailable */
#define CFS_ERR_INVALID_SEARCH          (CFS_ERR_0004)          /**< Invalid search (due to cache remove). Need to search again. */
#define CFS_ERR_API                     (CFS_ERR_0001)          /**< CFS API error */
#define CFS_ERR_OS                      (CFS_ERR_0001)          /**< OS error */
#define CFS_ERR_FATAL                   (CFS_ERR_0001)          /**< Fatal error */

#define AMBA_CFS_TIMEOUT_MUTEX          (30U * 1000U)           /**< Mutex wait timeout value */
#define AMBA_CFS_TIMEOUT_EVTFLAG        (30U * 1000U)           /**< Flag wait timeout value */

#define AmbaCFS_UtilGetAmbaFSError      AmbaFS_GetError         /**< Get AmbaFS error code */

#define AMBA_CFS_PRINT_MODULE_ID        ((UINT16)(CFS_ERR_BASE >> 16U))     /**< Module ID for AmbaPrint */

/**
 *  Return the severity of error code
 *  @param [in]  Ret                Error code
 *  @return Severity
 */
static inline UINT32 CFS_ERR_SEVERITY(UINT32 Ret)
{
    UINT32 Rval;
    /*
     * Severity:
     *     CFS_ERR_FATAL = CFS_ERR_OS = CFS_ERR_API > CFS_ERR_ARG > CFS_ERR_FS > CFS_ERR_INVALID_SEARCH > CFS_ERR_OBJ_UNAVAILABLE > CFS_OK
     */
    switch (Ret) {
        case CFS_OK:
            Rval = 0U;
            break;
        case CFS_ERR_OBJ_UNAVAILABLE:
            Rval = 1U;
            break;
        case CFS_ERR_INVALID_SEARCH:
            Rval = 2U;
            break;
        case CFS_ERR_FS:
            Rval = 3U;
            break;
        case CFS_ERR_ARG:
            Rval = 4U;
            break;
        default: /* CFS_ERR_API, CFS_ERR_OS, CFS_ERR_FATAL */
            Rval = 5U;
            break;
    }
    return Rval;
}

/**
 *  Select the most critical error code between "Ret" and "NewRet" and then assign it to "Ret".
 *  If the severity of both errors are the same, then choose the earliest error (which is "Ret").
 *  Return "NewRet" in case that user wants to access "NewRet".
 *  @param [in,out] Ret             The current error code. And the returned error code.
 *  @param [in]     NewRet          The new error code.
 *  @return NewRet
 */
static inline UINT32 CFS_SET_RETURN(UINT32 *Ret, UINT32 NewRet)
{
    /* Choose the error code with the highest severity */
    if (CFS_ERR_SEVERITY(NewRet) > CFS_ERR_SEVERITY(*Ret)) {
        *Ret = NewRet;
    } else {
        /* Do nothing. Let "Ret" keep the original value. */
    }
    return NewRet;
}

/**
 *  Convert KAL error to CFS error
 *  @param [in]  Ret                KAL error
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 CFS_K2C(UINT32 Ret)
{
    UINT32 Rval;
    switch (Ret) {
        case KAL_ERR_NONE: /* OK */
            Rval = CFS_OK;
            break;
        case KAL_ERR_0007: /* Timeout occurred */
            Rval = CFS_ERR_OS;
            break;
        default:
            Rval = CFS_ERR_FATAL; /* Fatal error */
            break;
    }
    return Rval;
}

/**
 *  Convert FS error to CFS error
 *  @param [in]  Ret                FS error
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 CFS_F2C(UINT32 Ret)
{
    UINT32 Rval;
    if (Ret == AMBA_FS_ERR_NONE) {
        Rval = CFS_OK;
    } else if (Ret == AMBA_FS_ERR_API) {
        UINT32 ErrNum = AmbaFS_GetError();
        /* Sometimes the return value from AmbaFS API is not consistant to AmbaFS_GetError(). */
        /* So use AmbaFS_GetError() instead of Ret. */
        switch (ErrNum) {
            case FS_ERR_0002: /* No such file or directory */
                Rval = CFS_ERR_OBJ_UNAVAILABLE;
                break;
            case FS_ERR_0005: /* I/O Error(Driver Error).           Ex: Remove SD card when writing file. */
            case FS_ERR_0008: /* Not Executable by internal         Ex: Unmount drive when writing file. */
            case FS_ERR_000D: /* Permission denied.                 Ex: Remove a read-only file. */
            case FS_ERR_0011: /* File already exists.               Ex: Create a new file, and then create a new directory with the same name. */
            case FS_ERR_0015: /* Find directory when file request.  Ex: Create a new directory, and then create a new file with the same name. */
            case FS_ERR_001B: /* Over file size limit(4GB-1).       Ex: Write file until file size > 4GB. */
            case FS_ERR_001C: /* Device out of space.               Ex: Write file until SD card is full. */
            case FS_ERR_005A: /* Directory is not empty.            Ex: Remove a directory which is not empty. */
            case FS_ERR_0302: /* EBADFSYS: Corrupted filesystem detected */
            case FS_ERR_0049: /* EDQUOT: Disk quota exceeded */
                /* When an error can be solved by formatting and reinserting SD card, return CFS_ERR_FS. */
                Rval = CFS_ERR_FS;
                break;
            default:
                Rval = CFS_ERR_FATAL; /* Fatal error */
                break;
        }
    } else { /* AMBA_FS_ERR_SYSTEM or AMBA_FS_ERR_ARG */
        Rval = CFS_ERR_FATAL; /* Fatal error */
    }
    return Rval;
}

/**
 *  Convert GDMA error to CFS error
 *  @param [in]  Ret                GDMA error
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 CFS_G2C(UINT32 Ret)
{
    UINT32 Rval;
    switch (Ret) {
        case GDMA_ERR_NONE: /* OK */
            Rval = CFS_OK;
            break;
        case GDMA_ERR_0002: /* Timeout occurred */
            Rval = CFS_ERR_OS;
            break;
        default:
            Rval = CFS_ERR_FATAL; /* Fatal error */
            break;
    }
    return Rval;
}

/**
 *  Convert cache error to CFS error
 *  @param [in]  Ret                Cache error
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 CFS_CACHE2C(UINT32 Ret)
{
    UINT32 Rval;
    switch (Ret) {
        case CACHE_ERR_NONE: /* OK */
            Rval = CFS_OK;
            break;
        default:
            Rval = CFS_ERR_FATAL; /* Fatal error */
            break;
    }
    return Rval;
}

/**
 *  Convert AmbaWrap error to CFS error
 *  @param [in]  Ret                AmbaWrap error
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 CFS_WRAP2C(UINT32 Ret)
{
    UINT32 Rval;
    switch (Ret) {
        case OK: /* OK */
            Rval = CFS_OK;
            break;
        default:
            Rval = CFS_ERR_FATAL; /* Fatal error */
            break;
    }
    return Rval;
}

/**
 *  Get aligned value
 *  @param [in]  Val                Input value
 *  @param [in]  AlignBase          Align base
 *  @return The aligned value
 */
static inline ULONG AmbaCFS_GetAlignedValUL(ULONG Val, ULONG AlignBase)
{
    return (((Val) + (AlignBase - 1U)) & ~(AlignBase - 1U));
}

/**
 *  Get the drive that the path belongs to
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in]  Path               The path
 *  @param [in]  Drive              The returned drive name
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_UtilGetDrive(const char *Path, char *Drive);

/**
 *  Copy file name
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [out] Dst                Destination
 *  @param [in] Src                 Source
 */
void AmbaCFS_UtilCopyFileName(char *Dst, const char *Src);

/**
 *  Check if fileName is a valid path name
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  This function is used to check parameters.
 *  @param [in] FileName            File name
 *  @return 1: Is valid path, 0: Not valid path
 */
UINT8 AmbaCFS_UtilIsValidPath(const char *FileName);

/**
 *  Check if fileName is a valid regular expression
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  This function is used to check parameters.
 *  @param [in] FileName            File name
 *  @return 1: Is valid, 0: Not valid
 */
UINT8 AmbaCFS_UtilIsValidRegExp(const char *FileName);

/**
 *  Get drive number
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] DriveNum           Drive number
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_UtilGetDriveNum(char Drive, UINT32 *DriveNum);

/**
 *  Check whether the value is aligned
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Val                 Value to be aligned
 *  @param [in] AlignBase           Align base. Should be power of 2
 *  @return 0 - Not aligned, 1 - Is aligned
 */
UINT8 AmbaCFS_UtilIsAlignedU32(UINT32 Val, UINT32 AlignBase);

/**
 *  Check whether the address is aligned
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Addr                Address to be aligned
 *  @param [in] AlignBase           Align base. Should be power of 2
 *  @return 0 - Not aligned, 1 - Is aligned
 */
UINT8 AmbaCFS_UtilIsAlignedAddr(const void *Addr, UINT32 AlignBase);

/**
 *  Change the alphabet character form lower case to upper case
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  If it's not alphabet then the character won't change
 *  @param [in] C                   Character
 *  @return Character of upper case
 */
char AmbaCFS_UtilToUpper(char C);

/**
 *  Compare two strings
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] String1             String to be compared
 *  @param [in] String2             String to be compared
 *  @return 0 - Strings are equal, Others - Strings are different
 */
INT32 AmbaCFS_UtilStringCompare(const char *String1, const char *String2);

/**
 *  This is an internal function
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  Check if a CFS file handler is a valid CFS stream handler
 *  This function is used to check parameters, so return CFS_ERR_ARG when failed.
 *  @param [in] File                The file descriptor
 *  @return 1 - TRUE, 0 - FALSE
 */
UINT8 AmbaCFS_IsValidCfsStream(const AMBA_CFS_FILE_s *File);

/**
 *  Add an event to event queue.
 *  @param [in] EventInfo           Event info.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_AddEvent(const AMBA_CFS_EVENT_INFO_s *EventInfo);

#endif
