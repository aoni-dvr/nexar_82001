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

#ifndef AMBA_REG_IRIF_H
#define AMBA_REG_IRIF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * IRIF: Control Register
 */
typedef struct {
    UINT32  Reserved0:          2;      /* [1:0] Reserved */
    UINT32  IntEnable:          1;      /* [2] 1 = Enabled, 0 = Disabled */
    UINT32  FifoOverflow:       1;      /* [3] 1 = Overflow occurred, 0 = No overflow */
    UINT32  IntCond:            6;      /* [9:4] Data count threshold to trigger interrupt  */
    UINT32  Reserved1:          2;      /* [11:10] Reserved */
    UINT32  IntStatus:          1;      /* [12] Interrupt status : 1 = interrupt asserted, 0 = No interrupt asserted (write 1 to clear) */
    UINT32  Enable:             1;      /* [13] 1 = Enable IR Interface, 0 = Disable IR Interface */
    UINT32  Reset:              1;      /* [14] 1 = Reset the entire IR Interface */
    UINT32  Reserved2:          17;     /* [31:15] Reserved */
} AMBA_IR_CTRL_REG_s;

/*
 * IRIF: Status Register
 */
typedef struct {
    UINT32  Count:              6;      /* [5:0] FIFO fullness */
    UINT32  Reserved:           26;     /* [31:6] Reserved */
} AMBA_IR_STATUS_REG_s;

/*
 * IRIF: Modulated Signal Data Register
 */
typedef struct {
    UINT32  Data:               16;     /* [15:0] Number of clock cycles since last edge */
    UINT32  Reserved:           16;     /* [31:16] Reserved */
} AMBA_IR_DATA_REG_s;

/*
 * IRIF: All Registers
 */
typedef struct {
    volatile AMBA_IR_CTRL_REG_s     Ctrl;       /* 0x00(RW): IR Remote Control Register */
    volatile AMBA_IR_STATUS_REG_s   Status;     /* 0x04(RO): IR Remote Status Register */
    volatile AMBA_IR_DATA_REG_s     Data;       /* 0x08(RO): IR Remote Data Register */
} AMBA_IR_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_IR_REG_s *const pAmbaIR_Reg;

#endif /* _AMBA_REG_IRIF_H_ */
