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

#ifndef AMBA_REG_WDT_H
#define AMBA_REG_WDT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/* WDOG_RESTART_REG only works with magic 0x4755. */
/* Set this value would transferred the value in  */
/* WDOG_RELOAD_REG into WDOG_STATUS_REG and would */
/* not trigger the underflow event.               */
#define AMBA_WDT_MAGIC_CODE         0x4755

/*
 * WDT: Ctrl Register
 */
typedef struct {
    UINT32  Enable:             1;      /* [0] 0 = Disable Watch Dog Timer */
    UINT32  SysResetEnable:     1;      /* [1] 0 = Disable System Reset on Counter Underflow */
    UINT32  IrqEnable:          1;      /* [2] 0 = Disable Interrupt on Counter Underflow */
    UINT32  Reserved:           29;     /* [31:3] */
} AMBA_WDT_CTRL_REG_s;

/*
 * WDT: Timeout Status Register
 */
typedef struct {
    UINT32  Status:             1;      /* [0] 1 = Timeout has occurred */
    UINT32  Reserved:           31;     /* [31:1] */
} AMBA_WDT_TIMEOUT_STATUS_REG_s;

/*
 * WDT: Clear Register
 */
typedef struct {
    UINT32  Value:              1;      /* [0] Write 1 to Clear Timeout Register */
    UINT32  Reserved:           31;     /* [31:1] */
} AMBA_WDT_CLEAR_REG_s;

/*
 * WDT: Reset Width Register
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
typedef struct {
    UINT32  Width:              32;      /* [7:0] Pulse Width of Reset Signal and Reset Interrupt */
} AMBA_WDT_RST_WD_REG_s;
#else
typedef struct {
    UINT32  Width:              8;      /* [7:0] Pulse Width of Reset Signal and Reset Interrupt */
    UINT32  Reserved:           24;     /* [31:8] */
} AMBA_WDT_RST_WD_REG_s;
#endif
/*
 * WDT: All Registers
 */
typedef struct {
    volatile UINT32                         CurCounter;     /* 0x00(RO): Current Counter value of WDT Register */
    volatile UINT32                         Reload;         /* 0x04(RW): Counter Reload Value Register */
    volatile UINT32                         Restart;        /* 0x08(WO): Counter Restart Register */
    volatile UINT32                         Ctrl;           /* 0x0C(RW): Ctrl Register */
    volatile UINT32                         TimeOutStatus;  /* 0x10(RO): Timeout Status Register */
    volatile UINT32                         Clear;          /* 0x14(WO): Clear Timeout Status Register */
    volatile AMBA_WDT_RST_WD_REG_s          ResetWidth;     /* 0x18(RW): Reset Width Register */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    volatile UINT32                         FaultInject;    /* 0x1c(RW): Check the safety error output can assert successfully */
#endif
} AMBA_WDT_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
extern AMBA_WDT_REG_s * pAmbaWDT_Reg[6];
#else
extern AMBA_WDT_REG_s * pAmbaWDT_Reg;

#endif

#endif /* AMBA_REG_WDT_H */
