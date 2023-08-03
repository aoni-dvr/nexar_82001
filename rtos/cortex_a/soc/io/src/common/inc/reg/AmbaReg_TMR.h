/*
 * Copyright (c) 2020 Ambarella International LP
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
 */

#ifndef AMBA_REG_TMR_H
#define AMBA_REG_TMR_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * Timer: Control Register: [1], [5], [9] : Three clock sources (extclk1, extclk2 and extclk3) can be selected for 1st, 2nd and 3rd timer
 */
#define AMBA_TMR_CLK_INT        0U /* Internal clock */
#define AMBA_TMR_CLK_EXT        1U /* External clock */

/*
 * Timer: Control Register: [3], [7], [11], [15], [19], [23], [27], [31]
 */
#define AMBA_TMR_CLK_APB        0U /* APB clock */
#define AMBA_TMR_CLK_INTCLK     1U /* Internal clock (INTCLK) */

/*
 * Timer: Control Register: [2], [6], [10], [14], [18], [22], [26], [30]
 */
#define AMBA_TMR_INT_DISABLE    0U
#define AMBA_TMR_INT_ENABLE     1U

/*
 * Timer: Control Register
 */
typedef struct {
    UINT32  Enable0:    1;      /* [0] Timer 1 Control: 0 = Disable, 1 = Enable */
    UINT32  ClkSel0:    1;      /* [1] Timer 1 Clock Select: 0 = Internal clock to check Intclksel0 , 1 = External clock(EXTCLK) */
    UINT32  IntEnable0: 1;      /* [2] Timer 1 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel0: 1;      /* [3] Timer 1 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
    UINT32  Enable1:    1;      /* [4] Timer 2 Control: 0 = Disable, 1 = Enable */
    UINT32  ClkSel1:    1;      /* [5] Timer 2 Clock Select: 0 = Internal clock to check Intclksel0 , 1 = External clock(EXTCLK) */
    UINT32  IntEnable1: 1;      /* [6] Timer 2 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel1: 1;      /* [7] Timer 2 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
    UINT32  Enable2:    1;      /* [8] Timer 3 Control: 0 = Disable, 1 = Enable */
    UINT32  ClkSel2:    1;      /* [9] Timer 3 Clock Select: 0 = Internal clock to check Intclksel0 , 1 = External clock(EXTCLK) */
    UINT32  IntEnable2: 1;      /* [10] Timer 3 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel2: 1;      /* [11] Timer 3 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
    UINT32  Enable3:    1;      /* [12] Timer 4 Control: 0 = Disable, 1 = Enable */
    UINT32  Reserved3:  1;      /* [13] Reserved */
    UINT32  IntEnable3: 1;      /* [14] Timer 4 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel3: 1;      /* [15] Timer 4 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
    UINT32  Enable4:    1;      /* [16] Timer 5 Control: 0 = Disable, 1 = Enable */
    UINT32  Reserved4:  1;      /* [17] Reserved */
    UINT32  IntEnable4: 1;      /* [18] Timer 5 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel4: 1;      /* [19] Timer 5 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
    UINT32  Enable5:    1;      /* [20] Timer 6 Control: 0 = Disable, 1 = Enable */
    UINT32  Reserved5:  1;      /* [21] Reserved */
    UINT32  IntEnable5: 1;      /* [22] Timer 6 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel5: 1;      /* [23] Timer 6 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
    UINT32  Enable6:    1;      /* [24] Timer 7 Control: 0 = Disable, 1 = Enable */
    UINT32  Reserved6:  1;      /* [25] Reserved */
    UINT32  IntEnable6: 1;      /* [26] Timer 7 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel6: 1;      /* [27] Timer 7 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
    UINT32  Enable7:    1;      /* [28] Timer 8 Control: 0 = Disable, 1 = Enable */
    UINT32  Reserved7:  1;      /* [29] Reserved */
    UINT32  IntEnable7: 1;      /* [30] Timer 8 Overflow Int: 0 = No interruput, 1 = Generate interruput */
    UINT32  IntClkSel7: 1;      /* [31] Timer 8 Internal Clock Select: 0 = APB clock(PCLK), 1 = Internal clock(INTCLK) */
} AMBA_TMR_CTRL_REG_s;

/*
 * Timer: All Registers
 */
typedef struct {
    volatile UINT32                 Status;             /* 0x000: Status Register */
    volatile UINT32                 Reload;             /* 0x004: Reload Value Register */
    volatile UINT32                 FirstMatch;         /* 0x008: First Match Register */
    volatile UINT32                 SecondMatch;        /* 0x00C: Second Match Register */
} AMBA_TMR_COUNTER_REG_s;

typedef struct {
    AMBA_TMR_COUNTER_REG_s          Counter0;           /* 0x000-0x00C: 1st Counter Registers */
    AMBA_TMR_COUNTER_REG_s          Counter1;           /* 0x010-0x01C: 2nd Counter Registers */
    AMBA_TMR_COUNTER_REG_s          Counter2;           /* 0x020-0x02C: 3rd Counter Registers */
    volatile UINT32                 TimerCtrl0;         /* 0x030: Interval Timer Control Register for Timer 0-7 */
    AMBA_TMR_COUNTER_REG_s          Counter3;           /* 0x034-0x040: 4th Counter Registers */
    AMBA_TMR_COUNTER_REG_s          Counter4;           /* 0x044-0x050: 5th Counter Registers */
    AMBA_TMR_COUNTER_REG_s          Counter5;           /* 0x054-0x060: 6th Counter Registers */
    AMBA_TMR_COUNTER_REG_s          Counter6;           /* 0x064-0x070: 7th Counter Registers */
    AMBA_TMR_COUNTER_REG_s          Counter7;           /* 0x074-0x080: 8th Counter Registers */
    volatile UINT32                 TimerCtrl1;         /* 0x084: Interval Timer Control Register for Timer 9-10 */
    AMBA_TMR_COUNTER_REG_s          Counter8;           /* 0x088-0x094: 9th Counter Registers */
    AMBA_TMR_COUNTER_REG_s          Counter9;           /* 0x098-0x0A4: 10th Counter Registers */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    volatile UINT32                 FaultInject;        /* 0x0A8: Timer fault inject, bit[0] to inject fault */
#endif
} AMBA_TMR_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_TMR_REG_s * pAmbaTMR_Regs[2];
#else
extern AMBA_TMR_REG_s *const pAmbaTMR_Regs[2];
#endif

#endif /* AMBA_REG_TMR_H */
