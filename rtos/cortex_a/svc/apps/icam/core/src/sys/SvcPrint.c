/**
*  @file SvcPrint.c
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
*  @details svc print functions
*
*/

#include "AmbaKAL.h"
#include "AmbaPrint.h"

#include "SvcLog.h"
#include "SvcPrint.h"

#define SVC_LOG_PRN             "PRN"
#define PRINT_BUF_SIZE          (0x100000U)
#define PRNLOG_BUF_SIZE         (0x100000U)
#define PRINT_STACK_SIZE        (16384U)

/**
* initialization of print module
* @param [in] Priority task priority
* @param [in] CpuBits task cpu bits
* @return 0-OK, 1-NG
*/
UINT32 SvcPrint_Init(UINT32 Priority, UINT32 CpuBits)
{
    extern void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut);

    static UINT8  PrintBuffer[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8  PrnLogBuffer[PRNLOG_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8  PrintTaskStack[PRINT_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    UINT32               Rval;
    AMBA_PRINT_CONFIG_s  Config = {0};

    Config.TaskPriority    = Priority;
    Config.SmpCoreSet      = CpuBits;
    Config.PrintBufferSize = PRINT_BUF_SIZE;
    Config.pPrintBuffer    = PrintBuffer;
    Config.LogBufferSize   = PRNLOG_BUF_SIZE;
    Config.pLogBuffer      = PrnLogBuffer;
    Config.TaskStackSize   = PRINT_STACK_SIZE;
    Config.pTaskStack      = PrintTaskStack;
    Config.PutCharFunc     = AmbaUserConsole_Write;

    Rval = AmbaPrint_Init(&Config);
    if (PRINT_ERR_SUCCESS != Rval) {
        SvcLog_NG(SVC_LOG_PRN, "## Fail to print init", 0U, 0U);
    }

    return Rval;
}
