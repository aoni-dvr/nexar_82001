/**
 *  @file AmbaIOUTDiag_CmdCRC.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details CRC checksum diagnostic Command
 */

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include "AmbaShell.h"
#include "AmbaWrap.h"
#include <AmbaUtility.h>
#include <AmbaIOUtility.h>
#include <AmbaIOUTDiag.h>

extern UINT32 AmbaUtility_Crc32Hw(const UINT8 *pBuffer, UINT32 Size);
extern UINT32 AmbaUtility_Crc32AddHw(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc);

#define CRC32_DATA_1 0x19B25912
#define CRC32_DATA_2 0xB8EF9079
#define CRC32_DATA_3 0xB39BE20D

static const UINT8 data1[] = {0xA7, 0xA0, 0xA3, 0x79, 0x2C, 0x95, 0x05, 0x0F, 0x70, 0x50,
                              0x6A, 0xFC, 0xA3, 0x5E, 0xC8, 0x6F, 0xDC, 0xD9, 0xAF, 0x30,
                              0x66, 0x42, 0xDF, 0xDA, 0xEF, 0xA0, 0x53, 0x98, 0x45, 0x2E,
                              0x9D, 0xB7, 0xA1, 0x78, 0xDB, 0xB1, 0x63, 0xE1, 0xB8
                             };

static const UINT8 data2[] = {0xDF, 0x69, 0xC0, 0x2C, 0xBC, 0xC9, 0xF8, 0xFC, 0xAF, 0xAB,
                              0x1E, 0xC9, 0x44, 0xB3, 0x54, 0x47, 0xB5, 0x43, 0xF9, 0x9A,
                              0xFD, 0xEF, 0x2D, 0xBA, 0xE4, 0x1E, 0x0E, 0x25, 0x00, 0x0B,
                              0x27, 0xC9, 0xB0, 0x3A, 0x3A, 0x29, 0x0F, 0xC7, 0x9C, 0xD8,
                              0x2B, 0xE2, 0x38, 0x83, 0x5B, 0xFF, 0xF4, 0x66, 0xF2, 0x43,
                              0x8E, 0xFD, 0xB8, 0xDF, 0x7F, 0x72, 0xD9, 0xAB, 0x4F, 0x00,
                              0xD7, 0x30, 0xB3, 0x77, 0x7F, 0x2A, 0xA1, 0x93, 0xAF, 0xA7,
                              0x68, 0xB8, 0x08, 0x33, 0xB5, 0x40, 0x2E, 0x4F, 0x9F, 0x2E,
                              0x2E, 0xCB, 0x93, 0x9C, 0xD2, 0xED, 0x98, 0x71, 0xFF, 0x09,
                              0x8F, 0x81, 0xFE, 0xF6, 0xDD, 0x74, 0xD6, 0xF8, 0x69, 0xAE,
                              0x59, 0x1C, 0x68, 0xE5, 0xEB, 0x6D, 0x81, 0xCC, 0xDD, 0xD6,
                              0x1C, 0xE3, 0xA5, 0x24, 0x9F, 0x20, 0xA7, 0xBF, 0xE9, 0x87,
                              0x62, 0xB8, 0xC6, 0xEB, 0x19, 0xF1, 0xDA, 0x62, 0x4C, 0xF4,
                              0xF8, 0xFD, 0x91, 0xA7, 0x9B, 0xE3, 0x46, 0x46, 0x76, 0x32,
                              0xF1, 0x45, 0x9C, 0x33, 0xC1, 0x73, 0xB6, 0xD2, 0x0E, 0xC4,
                              0x31, 0xA1, 0x32, 0x6B, 0x8A, 0xAE, 0xE2, 0x73
                             };

static const UINT8 data3[] = {0xF9, 0x0A, 0x0F, 0x1D, 0x63, 0x14, 0x57, 0x30, 0x63, 0xD4,
                              0xD3, 0xE8, 0xA6, 0x1B, 0x90, 0xE6, 0xA0, 0x0B, 0x3D, 0x18,
                              0x9F, 0xF6, 0xE8, 0xBE, 0x32, 0xFB, 0xAE, 0x63, 0xAB, 0xC6,
                              0x3B, 0x5E, 0xD0, 0xCB, 0xDE, 0xCE, 0xD6, 0xA9, 0xD8, 0xBB,
                              0x7C, 0xFD, 0x47, 0x28, 0xB8, 0xDE, 0x0E, 0xFE, 0xBA, 0xB8,
                              0x2E, 0x5F, 0xE7, 0x1E, 0x96, 0xC4, 0x62, 0xD5, 0xE5, 0x0D,
                              0xB7, 0x2E, 0x02, 0x5E, 0x51, 0xED, 0xE3, 0x3E, 0x7B, 0x70,
                              0xF4, 0xF6, 0x72, 0xAC, 0xCC, 0x46, 0x6C, 0xCE, 0xE6, 0x75,
                              0x94, 0x97, 0xE1, 0xDE, 0xFF, 0xE5, 0x38, 0xB0, 0xFE, 0x63,
                              0x7E, 0x9D, 0x83, 0xFA, 0x41, 0xD2, 0x6B, 0x30, 0xC1, 0xCD,
                              0x48, 0xB3, 0x8A, 0xFA, 0xD2, 0x70, 0xEC, 0xF7, 0xE5, 0xD4,
                              0xA9, 0x60, 0x87, 0x0C, 0x71, 0xEC, 0xFB, 0xB3, 0x77, 0xD5,
                              0x61, 0xED, 0xCA, 0x11, 0xBC, 0x19, 0xEE, 0x8C, 0x56, 0xC5,
                              0xFC, 0x67, 0x7C, 0x4D, 0x4D, 0x95, 0xC9, 0x78, 0x92, 0xED,
                              0xDF, 0x17, 0x0A, 0xFB, 0xCC, 0xE4, 0xFB, 0xEB, 0xE1, 0x48,
                              0xBF, 0xB9, 0xC0, 0x37, 0x79, 0x07, 0xFE, 0x1A, 0x5A, 0x10,
                              0x1D, 0x16, 0xF5, 0xAD, 0x38, 0xF5, 0x0B, 0x8A, 0x91, 0x75,
                              0xBC, 0x44, 0xF4, 0x3A, 0x27, 0xDB, 0x41, 0xFC, 0xFF, 0x0C,
                              0x92, 0xD2, 0x04, 0x25, 0xE6, 0x91, 0x39, 0xD7, 0xC7, 0xD5,
                              0x1C, 0x88, 0x4B, 0x3D, 0x80, 0xCC, 0xF0, 0x01, 0x7C, 0xDD,
                              0x1D, 0xF6, 0x55, 0xA1, 0x30, 0xB7, 0xAA, 0x80, 0x84, 0xEC,
                              0xFF, 0xB7, 0xCA, 0x8F, 0xCB, 0x98, 0xE2, 0x82, 0x13, 0x24,
                              0xF0, 0xF1, 0xD2, 0x45, 0x4F, 0xE6, 0x4E, 0xD2, 0x5D, 0x7D,
                              0x98, 0xFE, 0xF1, 0x11, 0xF9, 0x36, 0x29, 0x52, 0x50, 0xA9,
                              0x20, 0x47, 0x5F, 0x84, 0x08, 0x82, 0xA3, 0x7D, 0x0E, 0x16,
                              0x4C, 0xE5, 0x94, 0x3E, 0xF4, 0xCF, 0xC0, 0xF9, 0xB8, 0xC9,
                              0xE6, 0xF6, 0x18, 0x98, 0x98, 0x2D, 0xC3, 0x29, 0x42, 0xED,
                              0x5D, 0xF0, 0xFF, 0x44, 0xE1, 0xBC, 0xE9, 0xD2, 0xA6, 0x87,
                              0xFF, 0xD9, 0xEF, 0x36, 0xBE, 0xD9, 0x65, 0x17, 0xEE, 0xD3,
                              0x70, 0x26, 0x6C, 0xEE
                             };

static void IoDiag_CrcPrintUInt32(AMBA_SHELL_PRINT_f PrintFunc,
                                  const char *pFmtString,
                                  UINT32 Count,
                                  UINT32 Arg1,
                                  UINT32 Arg2,
                                  UINT32 Arg3,
                                  UINT32 Arg4,
                                  UINT32 Arg5)
{
    char StrBuf[256];
    UINT32 Arg[5];

    Arg[0] = Arg1;
    Arg[1] = Arg2;
    Arg[2] = Arg3;
    Arg[3] = Arg4;
    Arg[4] = Arg5;

    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, Count, Arg);
    PrintFunc(StrBuf);
}

static void IoDiag_CrcCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void) pArgVector;

    PrintFunc(" crc check : check the correctness of crc32 output \n");
}

static void IoDiag_CrcCheck(UINT32 Standard, const UINT8 *DataPtr, UINT32 DataSize, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, Crc, Output;

    Output = AmbaUtility_Crc32Sw(DataPtr, DataSize);
    if (Output != Standard) {
        IoDiag_CrcPrintUInt32(PrintFunc, "[NG] SW Crc32: 0x%x != 0x%x \n", 2, Output, Standard, 0, 0, 0);
    } else {
        IoDiag_CrcPrintUInt32(PrintFunc, "[OK] SW Crc32\n", 0, 0, 0, 0, 0, 0);
    }

    Output = AmbaUtility_Crc32Hw(DataPtr, DataSize);
    if (Output != Standard) {
        IoDiag_CrcPrintUInt32(PrintFunc, "[NG] HW Crc32: 0x%x != 0x%x \n", 2, Output, Standard, 0, 0, 0);
    } else {
        IoDiag_CrcPrintUInt32(PrintFunc, "[OK] HW Crc32 \n", 0, 0, 0, 0, 0, 0);
    }

    Output = IO_UtilityCrc32(DataPtr, DataSize);
    if (Output != Standard) {
        IoDiag_CrcPrintUInt32(PrintFunc, "[NG] IO_UtilityCrc32: 0x%x != 0x%x \n", 2, Output, Standard, 0, 0, 0);
    } else {
        IoDiag_CrcPrintUInt32(PrintFunc, "[OK] IO_UtilityCrc32 \n", 0, 0, 0, 0, 0, 0);
    }

    // Segmental calculation through SW implementation
    Crc = ~0U; // Initialize
    for (i = 0; i < DataSize; i++) {
        Crc = AmbaUtility_Crc32AddSw(&DataPtr[i], sizeof(UINT8), Crc);
    }
    Output = AmbaUtility_Crc32Finalize(Crc);
    if (Output != Standard) {
        IoDiag_CrcPrintUInt32(PrintFunc,"[NG] SW Crc32 (add): 0x%x != 0x%x \n", 2, Output, Standard, 0, 0, 0);
    } else {
        IoDiag_CrcPrintUInt32(PrintFunc,"[OK] SW Crc32 (add) \n", 0, 0, 0, 0, 0, 0);
    }

    // Segmental calculation through HW implementation
    Crc = ~0U; // Initialize
    for (i = 0; i < DataSize; i++) {
        Crc = AmbaUtility_Crc32AddHw(&DataPtr[i], sizeof(UINT8), Crc);
    }
    Output = AmbaUtility_Crc32Finalize(Crc);
    if (Output != Standard) {
        IoDiag_CrcPrintUInt32(PrintFunc,"[NG] HW Crc32 (add): 0x%x != 0x%x \n", 2, Output, Standard, 0, 0, 0);
    } else {
        IoDiag_CrcPrintUInt32(PrintFunc,"[OK] HW Crc32 (add) \n", 0, 0, 0, 0, 0, 0);
    }

}


void AmbaIOUTDiag_CmdCRC(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount >= 2) {
        if (AmbaUtility_StringCompare(pArgVector[1], "check", 5U) == 0) {
            IoDiag_CrcPrintUInt32(PrintFunc, "=== Test Case 1 === \n", 0, 0, 0, 0, 0, 0);
            IoDiag_CrcCheck(CRC32_DATA_1, data1, sizeof(data1), PrintFunc);
            IoDiag_CrcPrintUInt32(PrintFunc, "=== Test Case 2 === \n", 0, 0, 0, 0, 0, 0);
            IoDiag_CrcCheck(CRC32_DATA_2, data2, sizeof(data2), PrintFunc);
            IoDiag_CrcPrintUInt32(PrintFunc, "=== Test Case 3 === \n", 0, 0, 0, 0, 0, 0);
            IoDiag_CrcCheck(CRC32_DATA_3, data3, sizeof(data3), PrintFunc);
        } else {
            IoDiag_CrcCmdUsage(pArgVector, PrintFunc);
        }
    } else {
        IoDiag_CrcCmdUsage(pArgVector, PrintFunc);
    }
}
