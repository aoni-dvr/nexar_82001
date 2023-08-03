/**
 *  @file AmbaIOUTDiag_CmdFio.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details FIO diag Command for CV2FS
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaShell_Utility.h>
#ifndef AMBA_UTILITY_H
#include "AmbaUtility.h"
#endif

#include "AmbaNAND_OP.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaSD_Ctrl.h"

#ifndef AMBA_IOUT_DIAG_H
#include <AmbaIOUTDiag.h>
#endif

static void shell_fio_cmd_usage(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)ArgCount;
    (void)pArgVector;
    PrintFunc("Usage: fio nand read [page] [number] [memory address] : read form nand.\n");
    PrintFunc("Usage: fio nand erase [block]                         : erase nand blcok.\n");
    PrintFunc("Usage: fio nand write [page] [number] [memory address]: write to nand.\n");

    PrintFunc("Usage: fio nor read [page] [number] [memory address]  : read form nor.\n");
    PrintFunc("Usage: fio nor erase [Offset] [ByteCount]             : erase nor blcok.\n");
    PrintFunc("Usage: fio nor write [page] [number] [memory address] : write to nor.\n");

    PrintFunc("Usage: fio sd read [channel] [sector] [sectors] [memory address]  : read form sd.\n");
    PrintFunc("Usage: fio sd write [channel] [sector] [sectors] [memory address] : write form sd.\n");
}

void AmbaIOUTDiag_CmdFio(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 4U) {
        shell_fio_cmd_usage(ArgCount, pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "nand", 4) == 0) {
            if (ArgCount >= 6U) {
                UINT32 PageAddr, NumPage, Addr;
                UINT8 *pMainBuf = NULL;
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &PageAddr);
                (void)AmbaUtility_StringToUInt32(pArgVector[4], &NumPage);
                (void)AmbaUtility_StringToUInt32(pArgVector[5], &Addr);
                AmbaMisra_TypeCast(&pMainBuf, &Addr);
                if (AmbaUtility_StringCompare(pArgVector[2], "read", 4) == 0) {
                    AmbaNandOp_Read(PageAddr, NumPage, pMainBuf, NULL, 5000U);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "write", 4) == 0) {
                    AmbaNandOp_Program(PageAddr, NumPage, pMainBuf, NULL, 5000U);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "erase", 5) == 0) {
                UINT32 BlkAddr;
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &BlkAddr);
                AmbaNandOp_EraseBlock(BlkAddr, 5000U);
            } else {
                shell_fio_cmd_usage(ArgCount, pArgVector, PrintFunc);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "nor", 3) == 0) {
            if (ArgCount >= 6U) {
                UINT32 Offset, ByteCount, Addr;
                UINT8 *pDataBuf = NULL;

                (void)AmbaUtility_StringToUInt32(pArgVector[3], &Offset);
                (void)AmbaUtility_StringToUInt32(pArgVector[4], &ByteCount);
                (void)AmbaUtility_StringToUInt32(pArgVector[5], &Addr);

                AmbaMisra_TypeCast(&pDataBuf, &Addr);
                if (AmbaUtility_StringCompare(pArgVector[2], "read", 4) == 0) {
                    AmbaSpiNOR_Readbyte(Offset, ByteCount, pDataBuf, 5000U);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "write", 4) == 0) {
                    AmbaSpiNOR_Program(Offset, ByteCount, pDataBuf, 5000U);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "erase", 5) == 0) {
                UINT32 Offset, ByteCount;
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &Offset);
                (void)AmbaUtility_StringToUInt32(pArgVector[4], &ByteCount);
                AmbaSpiNOR_Erase(Offset, ByteCount, 5000U);
            } else {
                shell_fio_cmd_usage(ArgCount, pArgVector, PrintFunc);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "sd", 2) == 0) {
            if (ArgCount == 7U) {
                UINT32 SdChanNo, Addr;
                AMBA_NVM_SECTOR_CONFIG_s SecConfig;

                (void)AmbaUtility_StringToUInt32(pArgVector[3], &SdChanNo);
                (void)AmbaUtility_StringToUInt32(pArgVector[4], &SecConfig.StartSector);
                (void)AmbaUtility_StringToUInt32(pArgVector[5], &SecConfig.NumSector);
                (void)AmbaUtility_StringToUInt32(pArgVector[6], &Addr);

                AmbaMisra_TypeCast(&SecConfig.pDataBuf, &Addr);
                if (AmbaUtility_StringCompare(pArgVector[2], "read", 4) == 0) {
                    AmbaSD_ReadSector(SdChanNo, &SecConfig);
                } else if (AmbaUtility_StringCompare(pArgVector[2], "write", 4) == 0) {
                    AmbaSD_WriteSector(SdChanNo, &SecConfig);
                }
            } else {
                shell_fio_cmd_usage(ArgCount, pArgVector, PrintFunc);
            }
        }
    }
}

