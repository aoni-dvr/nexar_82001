/**
 *  @file AmbaIoUtDiag.h
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
 *  @details Ambarella IOUT Diag Functions
 *
 */

#ifndef AMBA_IOUT_DIAG_H
#define AMBA_IOUT_DIAG_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_IOUT_DIAG_PRIV_H
#include "AmbaIOUTDiag_Priv.h"
#endif

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif

#define IOUT_MEMID_PCIE     (0x00U)
#define IOUT_MEMID_USB      (0x01U)
#define IOUT_MEMID_NUM      (0x02U)

typedef struct {
    UINT32   ID;
    UINT32   AlignSize;
    UINT64   Size;
    UINT8   *Ptr;
} IOUT_MEMORY_INFO_s;

#ifdef AMBA_SHELL_H
void AmbaIOUTDiag_CmdADC(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdCVBS(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdDDR(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdDMA(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdDRAM(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdGDMA(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdGPIO(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdHDMI(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaDiag_CEC(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdI2C(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdIR(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdPWM(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdSPI(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdUART(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdVOUT(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdWDT(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdOS(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandATF(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdUsb(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdFio(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandMemTester(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdDiag(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaDiag_TempSensor(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdCAN(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdRNG(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdConsole(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdOS(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdAdvShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIOUTDiag_CmdCRC(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#ifdef CONFIG_ENABLE_INTERNAL_DIAG_CMDS
void AmbaIOUTDiag_CmdROTP(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
VOID AmbaShell_CommandRNGTest(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#endif
#endif

IOUT_MEMORY_INFO_s *AmbaIOUT_NoncacheMemInfoGet(UINT32 ID);

/* Defined in AmbaBackTrace.c */
void AmbaIOUT_Backtrace(UINT32 ExceptionID, ULONG * pSavedRegs);
void AmbaIOUT_StackOverflow(AMBA_KAL_TASK_t *pTask);
void AmbaIOUT_ConsoleSwitch(UINT32 ConsoleIdx);

#endif /* AMBA_DIAG_H */
