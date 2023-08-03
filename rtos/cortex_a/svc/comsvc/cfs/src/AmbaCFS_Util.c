/**
 * @file AmbaCFS_Util.c
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
#include "AmbaCFS_Util.h"
#include <AmbaPrint.h>

static UINT8 AmbaCFS_UtilIsFullPath(const char *FileName);

/**
 *  Get the drive that the path belongs to
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in]  Path               The path
 *  @param [in]  Drive              The returned drive name
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_UtilGetDrive(const char *Path, char *Drive)
{
    UINT32 Ret = CFS_ERR_API;
    if (AmbaCFS_UtilIsFullPath(Path) != 0U) {
        /* Set return value */
        *Drive = Path[0];
        Ret = CFS_OK;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not a full path %s", __func__, Path, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Copy file name
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [out] Dst                Destination
 *  @param [in] Src                 Source
 */
void AmbaCFS_UtilCopyFileName(char *Dst, const char *Src)
{
    AmbaUtility_StringCopy(Dst, AMBA_CFS_MAX_FILENAME_LENGTH - 1U, Src);
    Dst[AMBA_CFS_MAX_FILENAME_LENGTH - 1U] = '\0';
}

/**
 *  Check if the drive number is correct
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] DriveNum            Drive number
 *  @return 1: Is drive number, 0: Not drive number
 */
static UINT8 AmbaCFS_UtilIsDriveNum(const UINT32 DriveNum)
{
    return (DriveNum < AMBA_CFS_MAX_DRIVE_AMOUNT) ? 1U : 0U;
}

/**
 *  Check if fileName is a full path name
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  This function is used to check parameters.
 *  @param [in] FileName            File name
 *  @return 1: Is full path, 0: Not full path
 */
static UINT8 AmbaCFS_UtilIsFullPath(const char *FileName)
{
    UINT8 Rval = 0U;
    if (FileName[1] == ':') {
        Rval = 1U;
    }
    return Rval;
}

/**
 *  Check if fileName is a valid path name
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  This function is used to check parameters.
 *  @param [in] FileName            File name
 *  @return 1: Is valid path, 0: Not valid path
 */
UINT8 AmbaCFS_UtilIsValidPath(const char *FileName)
{
    UINT8 Rval = 1U;
    if (AmbaCFS_UtilIsFullPath(FileName) != 0U) {
        UINT32 StrLen;
        StrLen = (UINT32)AmbaUtility_StringLength(FileName);
        if ((StrLen > 0U) && (StrLen <= AMBA_CFS_MAX_FILENAME_LENGTH)) {
            for (UINT32 i = 0U; i < StrLen; ++i) {
                if ((FileName[i] == '/') || /* Only '\' is allowed */
                    (FileName[i] == '?') ||
                    (FileName[i] == '*') ||
                    (FileName[i] == '"') ||
                    (FileName[i] == '<') ||
                    (FileName[i] == '>') ||
                    (FileName[i] == '|') ||
                    ((FileName[i] == ':') && (i != 1U)) ) { /* Only FileName[1] can be ':' */
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UtilIsValidPath: [ERROR] FileName[%u] '%c' is not allowed", i, (UINT8) FileName[i], 0, 0, 0);
                    Rval = 0U;
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UtilIsValidPath: [ERROR] File name length %u is not valid", StrLen, 0, 0, 0, 0);
            Rval = 0U;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName \"%s\" is not a full path.", __func__, FileName, NULL, NULL, NULL);
        Rval = 0U;
    }
    return Rval;
}

/**
 *  Check if fileName is a valid regular expression
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  This function is used to check parameters.
 *  @param [in] FileName            File name
 *  @return 1: Is valid, 0: Not valid
 */
UINT8 AmbaCFS_UtilIsValidRegExp(const char *FileName)
{
    UINT8 Rval = 1U;
    UINT32 StrLen;
    StrLen = (UINT32)AmbaUtility_StringLength(FileName);
    if ((StrLen > 0U) && (StrLen <= AMBA_CFS_MAX_FILENAME_LENGTH)) {
        for (UINT32 i = 0U; i < StrLen; ++i) {
            /* '?' and '*' are allowed */
            if ((FileName[i] == '/') || /* Only '\' is allowed */
                (FileName[i] == '"') ||
                (FileName[i] == '<') ||
                (FileName[i] == '>') ||
                (FileName[i] == '|') ||
                ((FileName[i] == ':') && (i != 1U)) ) { /* Only FileName[1] can be ':' */
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UtilIsValidRegExp: [ERROR] FileName[%u] '%c' is not allowed", i, (UINT8) FileName[i], 0, 0, 0);
                Rval = 0U;
                break;
            }
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UtilIsValidRegExp: [ERROR] File name length %u is not valid", StrLen, 0, 0, 0, 0);
        Rval = 0U;
    }
    return Rval;
}

/**
 *  Get drive number
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  This function is used to check parameters, so return CFS_ERR_ARG when failed.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] DriveNum           Drive number
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_UtilGetDriveNum(char Drive, UINT32 *DriveNum)
{
    UINT32 Ret = CFS_ERR_ARG;
    char Dri = AmbaCFS_UtilToUpper((char) Drive);
    UINT8 DriU8 = (UINT8)Dri;
    UINT8 CharA = (UINT8)'A';
    UINT8 CharZ = (UINT8)'Z';
    if ((CharA <= DriU8) && (DriU8 <= CharZ)) {
        UINT8 RetDriveNum = DriU8 - CharA;
        if (AmbaCFS_UtilIsDriveNum((UINT32)RetDriveNum) != 0U) {
            /* Set return value */
            *DriveNum = (UINT32)RetDriveNum;
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UtilGetDriveNum: [ERROR] Drive %c is not supported", DriU8, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UtilGetDriveNum: [ERROR] Drive %c not supported", DriU8, 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Check whether the value is aligned
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Val                 Value to be aligned
 *  @param [in] AlignBase           Align base. Should be power of 2
 *  @return 0 - Not aligned, 1 - Is aligned
 */
UINT8 AmbaCFS_UtilIsAlignedU32(UINT32 Val, UINT32 AlignBase)
{
    UINT8 Ret = 0U;
    UINT32 ValAlign = GetAlignedValU32(Val, AlignBase);
    if (Val == ValAlign) {
        Ret = 1U;
    }
    return Ret;
}

/**
 *  Check whether the address is aligned
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Addr                Address to be aligned
 *  @param [in] AlignBase           Align base. Should be power of 2
 *  @return 0 - Not aligned, 1 - Is aligned
 */
UINT8 AmbaCFS_UtilIsAlignedAddr(const void *Addr, UINT32 AlignBase)
{
    UINT8 Ret = 0U;
    ULONG AddrUL;
    ULONG AddrAlign;
    AmbaMisra_TypeCast(&AddrUL, &Addr);
    AddrAlign = AmbaCFS_GetAlignedValUL(AddrUL, AlignBase);
    if (AddrUL == AddrAlign) {
        Ret = 1U;
    }
    return Ret;
}

/**
 *  Change the alphabet character form lower case to upper case
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  If it's not alphabet then the character won't change
 *  @param [in] C                   Character
 *  @return Character of upper case
 */
char AmbaCFS_UtilToUpper(char C)
{
    UINT8 CharU8 = (UINT8) C;
    UINT8 Ret = CharU8;
    if (((UINT8)'a' <= CharU8) && (CharU8 <= (UINT8)'z')) {
        Ret = CharU8 + (UINT8)'A' - (UINT8)'a';
    }
    return (char) Ret;
}

/**
 *  Compare two strings
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] String1             String to be compared
 *  @param [in] String2             String to be compared
 *  @return 0 - Strings are equal, Others - Strings are different
 */
INT32 AmbaCFS_UtilStringCompare(const char *String1, const char *String2)
{
    INT32 Rval = 0;
    if ((String1 == NULL) || (String2 == NULL)) {
        /* Return different */
        Rval = -1;
    } else {
        UINT32 StrLen1 = (UINT32)AmbaUtility_StringLength(String1);
        UINT32 StrLen2 = (UINT32)AmbaUtility_StringLength(String2);

        if (StrLen1 > StrLen2) {
            Rval = 1;
        } else if (StrLen1 < StrLen2) {
            Rval = -1;
        } else {
            Rval = AmbaUtility_StringCompare(String1, String2, StrLen2);
        }
    }
    return Rval;
}

