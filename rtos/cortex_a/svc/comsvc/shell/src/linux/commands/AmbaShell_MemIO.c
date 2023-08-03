/**
 *  @file AmbaShell_MemIO.c
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
#include <AmbaShell.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

static void WriteByte(ULONG Address, UINT8 Value)
{
    volatile UINT8 *ptr;
    AmbaMisra_TypeCast(&ptr, &Address);
    *ptr = Value;
}

static void WriteHalfWord(ULONG Address, UINT16 Value)
{
    volatile UINT16 *ptr;
    AmbaMisra_TypeCast(&ptr, &Address);
    *ptr = Value;
}

static void WriteWord(ULONG Address, UINT32 Value)
{
    volatile UINT32 *ptr;
    AmbaMisra_TypeCast(&ptr, &Address);
    *ptr = Value;
}

static UINT32 ReadByte(ULONG Address)
{
    const volatile UINT8 *ptr;
    AmbaMisra_TypeCast(&ptr, &Address);
    return *ptr;
}

static UINT32 ReadHalfWord(ULONG Address)
{
    const volatile UINT16 *ptr;
    AmbaMisra_TypeCast(&ptr, &Address);
    return *ptr;
}

static UINT32 ReadWord(ULONG Address)
{
    const volatile UINT32 *ptr;
    AmbaMisra_TypeCast(&ptr, &Address);
    return *ptr;
}

static void SHELL_WriteCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <physical addr> <val>\n");
}

static void SHELL_ReadCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <physical addr> <count>\n");
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
                *pCount = 1;
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
 *  AmbaShell_CommandWriteLong - execute command write long
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandWriteLong(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Val = 0U, DataOffset, i, Count = 0, DirectionBack = 0;
    UINT64 Addr = 0U;
    ULONG AddrOffset = 4;
    char AddrMode, DataMode;
    void* pVirtualAddr;
    ULONG PageOffset;   /* start addr of mapping zone */
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    UINT32 MMapSize;
    int fd;

    if (SHELL_Write(ArgCount, pArgVector, &Addr, &Val, &AddrMode, &DataMode, &Count) != 0U) {
        SHELL_WriteCmdUsage(pArgVector, PrintFunc);
    } else {
        if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
            perror("Unable to open /dev/mem");
            return ;
        }

        /* offset for mmap() must be page aligned */
        PageOffset = Addr & ~(sysconf(_SC_PAGE_SIZE) - 1);
        MMapSize = (Count * 4U) + (Addr - PageOffset);

        if ((pVirtualAddr = mmap(NULL, MMapSize, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, PageOffset)) == MAP_FAILED) {
            perror("mmap failed");
        } else {
            VirtualAddr = (ULONG) pVirtualAddr + (Addr - PageOffset);
            if (ArgCount == 3U) {
                WriteWord(VirtualAddr, Val);
            } else {
                if (AddrMode == '-') {
                    DirectionBack = 1;
                }
                if ((DataMode != '+') && (DataMode != '-')) {
                    for (i = 2; i < ArgCount; i ++) {
                        (void)AmbaUtility_StringToUInt32(pArgVector[i], &Val);
                        WriteWord(VirtualAddr, Val);
                        if (DirectionBack == 0U) {
                            VirtualAddr += AddrOffset;
                        } else {
                            VirtualAddr -= AddrOffset;
                        }
                    }
                } else {
                    (void)AmbaUtility_StringToUInt32(&pArgVector[3][1], &DataOffset);

                    for (i = 0; i < Count; i ++) {
                        WriteWord(VirtualAddr, Val);
                        if (DirectionBack == 0U) {
                            VirtualAddr += AddrOffset;
                        } else {
                            VirtualAddr -= AddrOffset;
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

            if (munmap(pVirtualAddr, MMapSize) < 0) {
                perror("munmap failed");
            }
        }
        close(fd);
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
    UINT32 i, Count = 0U;
    UINT64 Addr = 0U;
    void* pVirtualAddr;
    ULONG PageOffset;   /* start addr of mapping zone */
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    UINT32 MMapSize;
    int fd;

    if (SHELL_Read(ArgCount, pArgVector, &Addr, &Count) != 0U) {
        SHELL_ReadCmdUsage(pArgVector, PrintFunc);
    } else {
        if ((fd = open("/dev/mem", O_RDONLY)) < 0) {
            perror("Unable to open /dev/mem");
            return ;
        }

        /* offset for mmap() must be page aligned */
        PageOffset = Addr & ~(sysconf(_SC_PAGE_SIZE) - 1);
        MMapSize = (Count * 4U) + (Addr - PageOffset);

        if ((pVirtualAddr = mmap(NULL, MMapSize, PROT_READ,
                                 MAP_SHARED, fd, PageOffset)) == MAP_FAILED) {
            perror("mmap failed");
        } else {
            VirtualAddr = (ULONG) pVirtualAddr + (Addr - PageOffset);
            for (i = 0; i < Count; i++) {
                if ((i & 0x3U) == 0x0U) {
                    AmbaShell_PrintHexUInt64(Addr, PrintFunc);
                    PrintFunc(": ");
                    AmbaShell_PrintHexUInt32(ReadWord(VirtualAddr), PrintFunc);
                } else {
                    AmbaShell_PrintHexUInt32(ReadWord(VirtualAddr), PrintFunc);
                }
                PrintFunc(" ");

                if ((i & 0x3U) == 0x3U) {
                    PrintFunc("\n");
                }
                Addr += 4U;
                VirtualAddr += 4U;
            }

            if ((i & 0x3U) != 0x0U) {
                PrintFunc("\n");
            }

            if (munmap(pVirtualAddr, MMapSize) < 0) {
                perror("munmap failed");
            }
        }
        close(fd);
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
    UINT32 i, Count = 0U;
    UINT64 Addr = 0U;
    void* pVirtualAddr;
    ULONG PageOffset;   /* start addr of mapping zone */
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    UINT32 MMapSize;
    int fd;

    if (SHELL_Read(ArgCount, pArgVector, &Addr, &Count) != 0U) {
        SHELL_ReadCmdUsage(pArgVector, PrintFunc);
    } else {
        if ((fd = open("/dev/mem", O_RDONLY)) < 0) {
            perror("Unable to open /dev/mem");
            return ;
        }

        /* offset for mmap() must be page aligned */
        PageOffset = Addr & ~(sysconf(_SC_PAGE_SIZE) - 1);
        MMapSize = (Count * 1U) + (Addr - PageOffset);

        if ((pVirtualAddr = mmap(NULL, MMapSize, PROT_READ,
                                 MAP_SHARED, fd, PageOffset)) == MAP_FAILED) {
            perror("mmap failed");
        } else {
            VirtualAddr = (ULONG) pVirtualAddr + (Addr - PageOffset);
            for (i = 0; i < Count; i++) {
                if ((i & 0xfU) == 0x0U) {
                    AmbaShell_PrintHexUInt64(Addr, PrintFunc);
                    PrintFunc(": ");
                    AmbaShell_PrintHexUInt32(ReadByte(VirtualAddr), PrintFunc);
                } else {
                    AmbaShell_PrintHexUInt32(ReadByte(VirtualAddr), PrintFunc);
                }
                PrintFunc(" ");

                if ((i & 0xfU) == 0xfU) {
                    PrintFunc("\n");
                }
                Addr += 1U;
                VirtualAddr += 1U;
            }

            if ((i & 0xfU) != 0x0U) {
                PrintFunc("\n");
            }

            if (munmap(pVirtualAddr, MMapSize) < 0) {
                perror("munmap failed");
            }
        }
        close(fd);
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
    UINT32 i, Count = 0U;
    UINT64 Addr = 0U;
    void* pVirtualAddr;
    ULONG PageOffset;   /* start addr of mapping zone */
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    UINT32 MMapSize;
    int fd;

    if (SHELL_Read(ArgCount, pArgVector, &Addr, &Count) != 0U) {
        SHELL_ReadCmdUsage(pArgVector, PrintFunc);
    } else {
        if ((fd = open("/dev/mem", O_RDONLY)) < 0) {
            perror("Unable to open /dev/mem");
            return ;
        }

        /* offset for mmap() must be page aligned */
        PageOffset = Addr & ~(sysconf(_SC_PAGE_SIZE) - 1);
        MMapSize = (Count * 2U) + (Addr - PageOffset);

        if ((pVirtualAddr = mmap(NULL, MMapSize, PROT_READ,
                                 MAP_SHARED, fd, PageOffset)) == MAP_FAILED) {
            perror("mmap failed");
        } else {
            VirtualAddr = (ULONG) pVirtualAddr + (Addr - PageOffset);
            for (i = 0; i < Count; i++) {
                if ((i & 0x7U) == 0x0U) {
                    AmbaShell_PrintHexUInt64(Addr, PrintFunc);
                    PrintFunc(": ");
                    AmbaShell_PrintHexUInt32(ReadHalfWord(VirtualAddr), PrintFunc);
                } else {
                    AmbaShell_PrintHexUInt32(ReadHalfWord(VirtualAddr), PrintFunc);
                }
                PrintFunc(" ");

                if ((i & 0x7U) == 0x7U) {
                    PrintFunc("\n");
                }
                Addr += 2U;
                VirtualAddr += 2U;
            }

            if ((i & 0x7U) != 0x0U) {
                PrintFunc("\n");
            }

            if (munmap(pVirtualAddr, MMapSize) < 0) {
                perror("munmap failed");
            }
        }
        close(fd);
    }

    return;
}

/**
 *  AmbaShell_CommandWriteByte - execute command write byte
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandWriteByte(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Val = 0U, DataOffset, i, Count = 0, DirectionBack = 0;
    UINT64 Addr = 0U;
    ULONG AddrOffset = 1;
    char AddrMode, DataMode;
    void* pVirtualAddr;
    ULONG PageOffset;   /* start addr of mapping zone */
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    UINT32 MMapSize;
    int fd;

    if (SHELL_Write(ArgCount, pArgVector, &Addr, &Val, &AddrMode, &DataMode, &Count) != 0U) {
        SHELL_WriteCmdUsage(pArgVector, PrintFunc);
    } else {
        if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
            perror("Unable to open /dev/mem");
            return ;
        }

        /* offset for mmap() must be page aligned */
        PageOffset = Addr & ~(sysconf(_SC_PAGE_SIZE) - 1);
        MMapSize = (Count * 1U) + (Addr - PageOffset);

        if ((pVirtualAddr = mmap(NULL, MMapSize, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, PageOffset)) == MAP_FAILED) {
            perror("mmap failed");
        } else {
            VirtualAddr = (ULONG) pVirtualAddr + (Addr - PageOffset);
            if (ArgCount == 3U) {
                WriteByte(VirtualAddr, (UINT8) Val);
            } else {
                if (AddrMode == '-') {
                    DirectionBack = 1;
                }
                if ((DataMode != '+') && (DataMode != '-')) {
                    for (i = 2; i < ArgCount; i ++) {
                        (void)AmbaUtility_StringToUInt32(pArgVector[i], &Val);
                        WriteByte(VirtualAddr, (UINT8) Val);
                        if (DirectionBack == 0U) {
                            VirtualAddr += AddrOffset;
                        } else {
                            VirtualAddr -= AddrOffset;
                        }
                    }
                } else {
                    (void)AmbaUtility_StringToUInt32(&pArgVector[3][1], &DataOffset);

                    for (i = 0; i < Count; i ++) {
                        WriteByte(VirtualAddr, (UINT8) Val);
                        if (DirectionBack == 0U) {
                            VirtualAddr += AddrOffset;
                        } else {
                            VirtualAddr -= AddrOffset;
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

            if (munmap(pVirtualAddr, MMapSize) < 0) {
                perror("munmap failed");
            }
        }
        close(fd);
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
    UINT32 Val = 0U, DataOffset, i, Count = 0, DirectionBack = 0;
    UINT64 Addr = 0U;
    ULONG AddrOffset = 2;
    char AddrMode, DataMode;
    void* pVirtualAddr;
    ULONG PageOffset;   /* start addr of mapping zone */
    ULONG VirtualAddr;  /* virtual addr for desired Addr */
    UINT32 MMapSize;
    int fd;

    if (SHELL_Write(ArgCount, pArgVector, &Addr, &Val, &AddrMode, &DataMode, &Count) != 0U) {
        SHELL_WriteCmdUsage(pArgVector, PrintFunc);
    } else {
        if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
            perror("Unable to open /dev/mem");
            return ;
        }

        /* offset for mmap() must be page aligned */
        PageOffset = Addr & ~(sysconf(_SC_PAGE_SIZE) - 1);
        MMapSize = (Count * 2U) + (Addr - PageOffset);

        if ((pVirtualAddr = mmap(NULL, MMapSize, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, PageOffset)) == MAP_FAILED) {
            perror("mmap failed");
        } else {
            VirtualAddr = (ULONG) pVirtualAddr + (Addr - PageOffset);
            if (ArgCount == 3U) {
                WriteHalfWord(VirtualAddr, (UINT16) Val);
            } else {
                if (AddrMode == '-') {
                    DirectionBack = 1;
                }
                if ((DataMode != '+') && (DataMode != '-')) {
                    for (i = 2; i < ArgCount; i ++) {
                        (void)AmbaUtility_StringToUInt32(pArgVector[i], &Val);
                        WriteHalfWord(VirtualAddr, (UINT16) Val);
                        if (DirectionBack == 0U) {
                            VirtualAddr += AddrOffset;
                        } else {
                            VirtualAddr -= AddrOffset;
                        }
                    }
                } else {
                    (void)AmbaUtility_StringToUInt32(&pArgVector[3][1], &DataOffset);

                    for (i = 0; i < Count; i ++) {
                        WriteHalfWord(VirtualAddr, (UINT16) Val);
                        if (DirectionBack == 0U) {
                            VirtualAddr += AddrOffset;
                        } else {
                            VirtualAddr -= AddrOffset;
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

            if (munmap(pVirtualAddr, MMapSize) < 0) {
                perror("munmap failed");
            }
        }
        close(fd);
    }

    return;
}
