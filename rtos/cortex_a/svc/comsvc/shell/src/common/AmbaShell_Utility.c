/**
 *  @file AmbaShell_Utility.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Utility functions for Shell commands.
 *
 */

#include <AmbaShell_Utility.h>

void AmbaShell_PrintUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char IntString[UTIL_MAX_INT_STR_LEN];
    (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, 10);
    PrintFunc(IntString);
}

/**
 *  AMBSH_CHKCWD - check CWD
 *  @param[in] PrintFunc print function point
 */
UINT32 AMBSH_CHKCWD(AMBA_SHELL_PRINT_f PrintFunc)
{
    char Path[256];
    SIZE_t LenXyz;
    const char *pCwdXyz = AmbaShell_CurrentWorkDirGet();
    UINT32 uRet = 0;

    AmbaUtility_StringCopy(Path, 256, pCwdXyz);
    LenXyz = AmbaUtility_StringLength(Path);
    if (LenXyz > 0U) {
        if ((Path[LenXyz - 1U] == '\\') && (LenXyz != 3U)) {
            Path[LenXyz - 1U] = '\0';
        }
        if (AmbaFS_ChangeDir(Path) != 0U) {
            PrintFunc("cwd '");
            PrintFunc(pCwdXyz);
            PrintFunc("' is offline!\n\r");
            uRet = 0xFFFFFFFFU;
        }
    }
    return uRet;
}

/**
 *  AmbaShell_PrintSupportedCmds - print support command list
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_PrintSupportedCmds(AMBA_SHELL_PRINT_f PrintFunc)
{
    const AMBA_SHELL_COMMAND_s *pCmd;
    UINT32 i = 0;

    PrintFunc("Supported Shell Commands:\n");

    /* Simple Commands */
    for (pCmd = AmbaShell_CommandListGet(); pCmd != NULL; pCmd = pCmd->pNext) {
        PrintFunc("\t");
        PrintFunc(pCmd->pName);
        if (AmbaUtility_StringLength(pCmd->pName) <= 7U) {
            PrintFunc("\t");
        }

        if ((i & 0x3U) == 0x3U) {
            PrintFunc("\n");
        }
        i++;
    }
    PrintFunc("\n");

    return;
}

/* string length definition */
#define UTIL_MAX_HEX_STR_LEN       13

/**
 *  AmbaShell_PrintHexUInt32 - print unsigned integer in HEX
 *  @param[in] Value data to be printed
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_PrintHexUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char HexString[UTIL_MAX_HEX_STR_LEN];
    HexString[0] = '0';
    HexString[1] = 'x';
    HexString[2] = '\0';
    (void)AmbaUtility_UInt32ToStr(&HexString[2], UTIL_MAX_HEX_STR_LEN-2, Value, 16);
    PrintFunc(HexString);
}

/**
 *  AmbaShell_PrintHexUInt64 - print unsigned integer in HEX
 *  @param[in] Value data to be printed
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_PrintHexUInt64(UINT64 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char HexString[UTIL_MAX_HEX_STR_LEN];
    HexString[0] = '0';
    HexString[1] = 'x';
    HexString[2] = '\0';
    (void)AmbaUtility_UInt64ToStr(&HexString[2], UTIL_MAX_HEX_STR_LEN-2, Value, 16);
    PrintFunc(HexString);
}

/**
 *  AmbaShell_PrintUInt64 - print unsigned integer
 *  @param[in] Value data to be printed
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_PrintUInt64(UINT64 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char IntString[22];
    (void)AmbaUtility_UInt64ToStr(IntString, 22, Value, 10);
    PrintFunc(IntString);
}

/**
 *  AmbaShell_PrintChar - print unsigned char
 *  @param[in] Value data to be printed
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_PrintChar(char Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char CharString[2];
    CharString[0] = Value;
    CharString[1] = '\0';
    PrintFunc(CharString);
}

/**
 *  AmbaShell_PrintChar - check repeat sep sign
 *  @param[in] pString string point
 *  @return string index
 */
UINT32 AmbaShell_FindRepeatSep(const char *pString)
{
    UINT32 Index;
    UINT32 Position = 0;

    if (pString != NULL) {
        for (Index = 0; pString[Index] != '\0'; Index++) {
            if (pString[Index] == '\\') {
                Position = Index + 1U;
            }
        }
    }

    return Position;
}

/**
 *  AmbaShell_PrintChar - find wild sign
 *  @param[in] pString string point
 *  @return result
 */
UINT32 AmbaShell_FindWild(const char *pString)
{
    UINT32 l, Found = 0;

    if (pString != NULL) {
        for (l = 0 ; pString[l] != '\0'; l++) {
            if (pString[l] == '*') {
                Found = 1;
                break;
            }
        }
    }
    return Found;
}

/**
 *  AmbaShell_PrintChar - do string copy
 *  @param[in] pDest dest string point
 *  @param[in] DestSize dest string size
 *  @param[in] pSrc soutce string point
 *  @param[in] CopyLength the size to copy
 */
void AmbaShell_StringCopyLength(char *pDest, SIZE_t DestSize, const char *pSrc, SIZE_t CopyLength)
{
    SIZE_t LengthToCopy = CopyLength;
    SIZE_t i;
    SIZE_t SrcLength;
    if ((pDest != NULL) && (pSrc != NULL)) {
        SrcLength = AmbaUtility_StringLength(pSrc);
        if (LengthToCopy > SrcLength) {
            LengthToCopy = SrcLength;
        }
        if (LengthToCopy >= DestSize) {
            LengthToCopy = DestSize - 1U;
        }

        for (i = 0; i < LengthToCopy; i++) {
            pDest[i] = pSrc[i];
        }
        pDest[LengthToCopy] = '\0';
    }
}

/**
 *  AmbaShell_IsVolumePath - check if path is volume
 *  @param[in] pFilePath file name
 *  @return function result
 */
UINT32 AmbaShell_IsVolumePath(const char *pFilePath)
{
    UINT32 uRet;
    if ((AmbaUtility_StringLength(pFilePath) == 2U) && (pFilePath[1] == ':')) {
        uRet = 0;
    } else if ((AmbaUtility_StringLength(pFilePath) == 3U) && ((pFilePath[1] == ':') && (pFilePath[2] == '\\'))) {
        uRet = 0;
    } else {
        uRet = 0xFFFFFFFFU;
    }
    return uRet;
}

#ifdef CONFIG_THREADX
/**
 *  AmbaShell_PFChar2Char - transfer PFchar to char
 *  @param[in] pfc_ptr file name
 *  @return pointer to char
 */
char *AmbaShell_PFChar2Char(const PF_CHAR *pfc_ptr)
{
    char *ptr = NULL;
    AmbaMisra_TypeCast32(&ptr, &pfc_ptr);
    return ptr;
}
#endif
