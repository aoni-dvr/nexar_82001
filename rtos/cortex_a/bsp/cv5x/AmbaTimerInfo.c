/**
 *  @file AmbaTimerInfo.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Implementation of reference code timer info api
 *
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaCSL_RCT.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_TMR.h"
#include "AmbaCSL_TMR.h"
#include "AmbaCortexA76.h"
#include "AmbaUtility.h"
#include <AmbaRTSL_MMU.h>

#include "AmbaMisraFix.h"
#include "AmbaTimerInfo.h"
#ifdef CONFIG_ICAM_CLK_LINE
#include ".svc_autogen"
#endif

static AMBA_TIMER_INFO_s *TimerInfoPhy;

/* --------------------------------------------------------------- */
/* Debug print function                                            */
/* --------------------------------------------------------------- */
#define DEBUG_TIME (0U)
#if DEBUG_TIME /* Keep for debug */
#include "AmbaIOUtility.h"
#define PRINT_BUF_SIZE  (1024U)
static void PrintUInt(const char *Fmt, UINT32* Arg)
{
    static UINT8 BldPrintBuf[PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    char *pString;
    const UINT8 *pBuf = BldPrintBuf;

    AmbaMisra_TypeCast32(&pString, &pBuf);
    UartTxSize = IO_UtilityStringPrintUInt32(pString, PRINT_BUF_SIZE, Fmt, (UINT32)1U, Arg);
    while (UartTxSize > 0U) {
        if (AmbaRTSL_UartWrite(AMBA_UART_APB_CHANNEL0, UartTxSize, &BldPrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
            break;
        } else {
            UartTxSize -= ActualTxSize;
            Index += ActualTxSize;
        }
    }
}
#endif

static void TimeInfoInit(void)
{
    UINT64 PhyAddr  = (AMBA_DRAM_RESERVED_PHYS_ADDR + AMBA_RAM_APPLICATION_SPECIFIC_OFFSET);

    AmbaMisra_TypeCast64(&TimerInfoPhy, &PhyAddr);
}

/* --------------------------------------------------------------- */
/* Public function                                                 */
/* --------------------------------------------------------------- */
void AmbaTime_BldGetBssInfo(void)
{
    extern UINT32 __bss_start;
    extern UINT32 __bss_end;
    const  UINT32* p__bss_start = &__bss_start;
    const  UINT32* p__bss_end   = &__bss_end;
    ULONG  bss_start_addr       = 0U;
    ULONG  bss_end_addr         = 0U;
    UINT32 bss_size;

    AmbaMisra_TypeCast32(&bss_start_addr, &p__bss_start);
    AmbaMisra_TypeCast32(&bss_end_addr,   &p__bss_end);
    bss_size = bss_end_addr - bss_start_addr;

    TimeInfoInit();

#ifdef CONFIG_CPU_CORTEX_R52
    TimerInfoPhy->R52_BssSize = bss_size;
#else
    TimerInfoPhy->A53_BssSize = bss_size;
#endif
}

void AmbaTimeInfo_Init(void)
{
    UINT32 Rval;

    TimeInfoInit();
    Rval = AmbaWrap_memset(TimerInfoPhy, 0, sizeof(AMBA_TIMER_INFO_s));
    if (0U == Rval) {
        /* make misra-c happy */
    }
}

void AmbaTime_A53Bld(UINT32 id)
{
    if ((id < STAMP_NUM) && (TimerInfoPhy != NULL)) {
        AmbaCSL_RctTimer1Freeze();
        TimerInfoPhy->A53_BLD_Time[id] = AmbaCSL_RctTimer1GetCounter();
        AmbaCSL_RctTimer1Enable();
    }
}
