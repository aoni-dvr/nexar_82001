/**
 *  @file AmbaShell_MemIO_A64.c
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
 *  @details Memory read/write shell command.
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include "AmbaWrap.h"
#include <AmbaShell.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

#if 0
#define WriteByte(Addr, d)          (*((volatile UINT8  *) (Addr)) = (d))
#define WriteHalfWord(Addr, d)      (*((volatile UINT16 *) (Addr)) = (d))
#define WriteWord(Addr, d)          (*((volatile UINT32 *) (Addr)) = (d))

#define ReadByte(Addr)              (*((volatile UINT8  *) (Addr)))
#define ReadHalfWord(Addr)          (*((volatile UINT16 *) (Addr)))
#define ReadWord(Addr)              (*((volatile UINT32 *) (Addr)))
#else
static void SHELL_WriteByte(UINT64 Address, UINT8 Value)
{
    volatile UINT8 *ptr;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(Address)) == 0U) {
        *ptr = Value;
    }
}

static void WriteHalfWord(UINT64 Address, UINT16 Value)
{
    volatile UINT16 *ptr;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(Address)) == 0U) {
        *ptr = Value;
    }
}

static void SHELL_WriteWord(UINT64 Address, UINT32 Value)
{
    volatile UINT32 *ptr;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(Address)) == 0U) {
        *ptr = Value;
    }
}

static UINT32 SHELL_ReadByte(UINT64 Address)
{
    const volatile UINT8 *ptr;
    UINT32 uret = 0;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(Address)) == 0U) {
        uret = *ptr;
    }
    return uret;
}
static UINT32 ReadHalfWord(UINT64 Address)
{
    const volatile UINT16 *ptr;
    UINT32 uret = 0;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(Address)) == 0U) {
        uret = *ptr;
    }
    return uret;
}
static UINT32 ReadWord(UINT64 Address)
{
    const volatile UINT32 *ptr;
    UINT32 uret = 0;
    if (AmbaWrap_memcpy(&ptr, &Address, sizeof(Address)) == 0U) {
        uret = *ptr;
    }
    return uret;
}

#endif
static void SHELL_WriteCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <addr> <val>\n");
}

static void SHELL_ReadCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <addr> <count>\n");
}

static UINT32 SHELL_Write(UINT32 ArgCount, char * const *pArgVector, UINT64 *pAddr, UINT32 *pVal, char *pAddrMode, char *pDataMode, UINT32 *pCount)
{
    UINT32 nRet = 0;

    if ((ArgCount < 3U) || (pAddr == NULL) || (pVal == NULL)) {
        nRet = 0xFFFFFFFFU;
    } else {
        (void)AmbaUtility_StringToUInt64(pArgVector[1], pAddr);
        (void)AmbaUtility_StringToUInt32(pArgVector[2], pVal);

        if (pAddrMode != NULL) {
            *pAddrMode = pArgVector[1][AmbaUtility_StringLength(pArgVector[1]) - 1U];
        }

        if (pDataMode != NULL) {
            if (ArgCount >= 4U) {
                *pDataMode = pArgVector[3][0];
            } else {
                *pDataMode = '\0';
            }
        }

        if (pCount != NULL) {
            if (ArgCount >= 5U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[4], pCount);
            } else {
                *pCount = 0;
            }
        }
    }
    return nRet;
}

static UINT32 SHELL_Read(UINT32 ArgCount, char * const *pArgVector, UINT64 *pAddr, UINT32 *pCount)
{
    UINT32 nRet = 0;
    if ((ArgCount < 2U) || (pAddr == NULL) || (pCount == NULL)) {
        nRet = 0xFFFFFFFFU;
    } else if (ArgCount < 3U) {
        *pCount = 1;
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], pCount);
    }

    if (nRet == 0U) {
        (void)AmbaUtility_StringToUInt64(pArgVector[1], pAddr);
    }
    return nRet;
}

/**
 *  AmbaShell_CommandWriteByte - execute command write byte
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandWriteByte(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 Addr = 0, AddrOffset = 1;
    UINT32 Val = 0, DataOffset, Count = 0;
    UINT32 i;
    char AddrMode, DataMode;

    if (SHELL_Write(ArgCount, pArgVector, &Addr, &Val, &AddrMode, &DataMode, &Count) != 0U) {
        SHELL_WriteCmdUsage(pArgVector, PrintFunc);
    } else {
        if (ArgCount == 3U) {
            SHELL_WriteByte(Addr, (UINT8) Val);
        } else {

            if ((DataMode != '+') && (DataMode != '-')) {
                for (i = 2; i < ArgCount; i ++) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[i], &Val);
                    SHELL_WriteByte(Addr, (UINT8) Val);
                    if (AddrMode == '-') {
                        Addr -= AddrOffset;
                    } else {
                        Addr += AddrOffset;
                    }
                }
            } else {
                (void)AmbaUtility_StringToUInt32(&pArgVector[3][1], &DataOffset);

                for (i = 0; i < Count; i ++) {
                    SHELL_WriteByte(Addr, (UINT8) Val);
                    if (AddrMode == '-') {
                        Addr -= AddrOffset;
                    } else {
                        Addr += AddrOffset;
                    }
                    if (DataMode == '+') {
                        Val += DataOffset;
                    } else if (DataMode == '-') {
                        Val -= DataOffset;
                    } else {
                        /* make misra happy */
                    }
                }
            }
        }
    }
    return;
}

/**
 *  AmbaShell_CommandWriteWord - execute command write word
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandWriteWord(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 Addr = 0, AddrOffset = 2;
    UINT32 Val = 0, DataOffset, Count = 0;
    UINT32 i;
    char AddrMode, DataMode;

    if (SHELL_Write(ArgCount, pArgVector, &Addr, &Val, &AddrMode, &DataMode, &Count) != 0U) {
        SHELL_WriteCmdUsage(pArgVector, PrintFunc);
    } else {
        if (ArgCount == 3U) {
            WriteHalfWord(Addr, (UINT16) Val);
        } else {

            if ((DataMode != '+') && (DataMode != '-')) {
                for (i = 2; i < (UINT32)ArgCount; i ++) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[i], &Val);
                    WriteHalfWord(Addr, (UINT16) Val);
                    if (AddrMode == '-') {
                        Addr -= AddrOffset;
                    } else {
                        Addr += AddrOffset;
                    }
                }
            } else {
                (void)AmbaUtility_StringToUInt32(&pArgVector[3][1], &DataOffset);

                for (i = 0; i < Count; i ++) {
                    WriteHalfWord(Addr, (UINT16) Val);
                    if (AddrMode == '-') {
                        Addr -= AddrOffset;
                    } else {
                        Addr += AddrOffset;
                    }
                    if (DataMode == '+') {
                        Val += DataOffset;
                    } else if (DataMode == '-') {
                        Val -= DataOffset;
                    } else {
                        /* make misra happy */
                    }
                }
            }
        }
    }
    return;
}

/**
 *  AmbaShell_CommandWriteLong - execute command write long
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandWriteLong(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 Addr = 0, AddrOffset = 4;
    UINT32 Val = 0, DataOffset, Count = 0;
    UINT32 i, DirectionBack = 0;
    char AddrMode, DataMode;

    if (SHELL_Write(ArgCount, pArgVector, &Addr, &Val, &AddrMode, &DataMode, &Count) != 0U) {
        SHELL_WriteCmdUsage(pArgVector, PrintFunc);
    } else {
        if (ArgCount == 3U) {
            SHELL_WriteWord(Addr, Val);
        } else {
            if (AddrMode == '-') {
                DirectionBack = 1;
            }

            if ((DataMode != '+') && (DataMode != '-')) {
                for (i = 2; i < ArgCount; i ++) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[i], &Val);
                    SHELL_WriteWord(Addr, Val);
                    if (DirectionBack == 0U) {
                        Addr += AddrOffset;
                    } else {
                        Addr -= AddrOffset;
                    }
                }
            } else {
                (void)AmbaUtility_StringToUInt32(&pArgVector[3][1], &DataOffset);

                for (i = 0; i < Count; i ++) {
                    SHELL_WriteWord(Addr, Val);
                    if (DirectionBack == 0U) {
                        Addr += AddrOffset;
                    } else {
                        Addr -= AddrOffset;
                    }
                    if (DataMode == '+') {
                        Val += DataOffset;
                    } else if (DataMode == '-') {
                        Val -= DataOffset;
                    } else {
                        /* make misra happy */
                    }
                }
            }
        }
    }

    return;
}

/**
 *  AmbaShell_CommandReadByte - execute command read byte
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandReadByte(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 Addr = 0;
    UINT32 Count;
    UINT32 i;

    if (SHELL_Read(ArgCount, pArgVector, &Addr, &Count) != 0U) {
        SHELL_ReadCmdUsage(pArgVector, PrintFunc);
    } else {
        for (i = 0; i < Count; i ++) {
            if ((i & 0xfU) == 0x0U) {
                AmbaShell_PrintHexUInt64(Addr, PrintFunc);
                PrintFunc(":  ");
                AmbaShell_PrintHexUInt32(SHELL_ReadByte(Addr), PrintFunc);
            } else {
                AmbaShell_PrintHexUInt32(SHELL_ReadByte(Addr), PrintFunc);
            }
            PrintFunc(" ");

            if ((i & 0xfU) == 0xfU) {
                PrintFunc("\n");
            }
            Addr += 1U;
        }

        if ((i & 0xfU) != 0x0U) {
            PrintFunc("\n");
        }
    }
    return;
}

/**
 *  AmbaShell_CommandReadWord - execute command read word
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandReadWord(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 Addr = 0;
    UINT32 Count;
    UINT32 i;

    if (SHELL_Read(ArgCount, pArgVector, &Addr, &Count) != 0U) {
        SHELL_ReadCmdUsage(pArgVector, PrintFunc);
    } else {
        for (i = 0; i < Count; i ++) {
            if ((i & 0x7U) == 0x0U) {
                AmbaShell_PrintHexUInt64(Addr, PrintFunc);
                PrintFunc(": ");
                AmbaShell_PrintHexUInt32(ReadHalfWord(Addr), PrintFunc);
            } else {
                AmbaShell_PrintHexUInt32(ReadHalfWord(Addr), PrintFunc);
            }
            PrintFunc(" ");

            if ((i & 0x7U) == 0x7U) {
                PrintFunc("\n");
            }
            Addr += 2U;
        }

        if ((i & 0x7U) != 0x0U) {
            PrintFunc("\n");
        }
    }
    return;
}

/**
 *  AmbaShell_CommandReadLong - execute command read long
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandReadLong(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 Addr = 0;
    UINT32 Count;
    UINT32 i;

    if (SHELL_Read(ArgCount, pArgVector, &Addr, &Count) != 0U) {
        SHELL_ReadCmdUsage(pArgVector, PrintFunc);
    } else {
        for (i = 0; i < Count; i++) {
            if ((i & 0x3U) == 0x0U) {
                AmbaShell_PrintHexUInt64(Addr, PrintFunc);
                PrintFunc(": ");
                AmbaShell_PrintHexUInt32(ReadWord(Addr), PrintFunc);
            } else {
                AmbaShell_PrintHexUInt32(ReadWord(Addr), PrintFunc);
            }
            PrintFunc(" ");

            if ((i & 0x3U) == 0x3U) {
                PrintFunc("\n");
            }
            Addr += 4U;
        }

        if ((i & 0x3U) != 0x0U) {
            PrintFunc("\n");
        }
    }
    return;
}

