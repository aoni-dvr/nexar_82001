/**
 *  @file AmbaBST_UserConfig_A64.asm
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
 *  @details Reserved for user-customized configuration
 *
 */

#include "AmbaCortexA53.h"

/*  Reload Value = (TIMER_COUNT_VALUE << TIMER_COUNT_SHIFT ) - 1, Reload Value is 32 bit value */
/*  Status Value = (TIMER_COUNT_VALUE << TIMER_COUNT_SHIFT ), Status Value is 32 bit value */
#define TIMER_STATUS_REG                0x00
#define TIMER_RELOAD_REG                0x04
#define TIMER_FIRSTMATCH_REG            0x08
#define TIMER_SECONDMATCH_REG           0x0c
#define TIMER_CTRL_REG                  0x30
#define TIMER_COUNT_VALUE               0xFFF     /* 16 bit value */
#define TIMER_COUNT_SHIFT               0x14      /* Value 0 ~ 31 */

        .globl __AmbaBstUserConfig
        .globl __AmbaTimerInit
        .globl __AmbaTimerGet

__AmbaBstUserConfig:
        /* DO NOT USE 'BL' INSTRUCTION BECAUSE THERE IS NO STACK BEFORE DRAM INIT!! */

        /* The functions for the timer 0 init and get */
__AmbaTimerInit:
        LDR     W0, =(AMBA_CORTEX_A53_TIMER0_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR)   /* W0 = AMBA_CORTEX_A53_TIMER0_BASE_ADDR  */
        MOVZ    W1, #0
        STR     W1, [X0, #TIMER_CTRL_REG]           /* Inital the timer control */
        MOVN    W1, #0x0
        STR     W1, [X0, #TIMER_STATUS_REG]         /* Inital 1st Counter Registers : status register */
        MOVZ    W1, #0
        STR     W1, [X0, #TIMER_RELOAD_REG]         /* Inital 1st Counter Registers : reload value register */
        STR     W1, [X0, #TIMER_FIRSTMATCH_REG]     /* Inital 1st Counter Registers : first match register */
        STR     W1, [X0, #TIMER_SECONDMATCH_REG]    /* Inital 1st Counter Registers : second match register */
        MOVZ    W1, #TIMER_COUNT_VALUE
        LSL     W1, W1, #TIMER_COUNT_SHIFT
        SUB     W1, W1, #1
        STR     W1, [X0, #TIMER_RELOAD_REG]         /* Set reload value register */
        MOVZ    W1, #TIMER_COUNT_VALUE
        LSL     W1, W1, #TIMER_COUNT_SHIFT
        STR     W1, [X0]                            /* Set status value register */
        LDR     W1, [X0, #TIMER_CTRL_REG]
        ORR     W1, W1, #0x1
        STR     W1, [X0, #TIMER_CTRL_REG]           /* Enable the timer */
        RET

__AmbaTimerGet:
        LDR     W0, =(AMBA_CORTEX_A53_TIMER0_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR)   /* W0 = AMBA_CORTEX_A53_TIMER0_BASE_ADDR  */
        LDR     W1, [X0, #TIMER_STATUS_REG]         /* W1 = The status value for timer0  */
        LDR     W2, [X0, #TIMER_RELOAD_REG]         /* W2 = The reload value for timer0 */
        RET
