/**
 *  @file AmbaUART_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Common Definitions & Constants for UART APIs
 *
 */

#ifndef AMBA_UART_DEF_H
#define AMBA_UART_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_UART_PRIV_H
#include "AmbaUART_Priv.h"
#endif

#define UART_ERR_0000           (UART_ERR_BASE)             /* Invalid argument */
#define UART_ERR_0001           (UART_ERR_BASE + 0x1U)      /* Unable to do concurrency protection */
#define UART_ERR_0002           (UART_ERR_BASE + 0x2U)      /* Service unavailable */
#define UART_ERR_0003           (UART_ERR_BASE + 0x3U)      /* A timeout occurred */
#define UART_ERR_0004           (UART_ERR_BASE + 0x4U)      /* Recovery is required */
#define UART_ERR_00FF           (UART_ERR_BASE + 0XFFU)     /* Unexpected error */

/* UART error values */
#define UART_ERR_NONE           0U
#define UART_ERR_ARG            UART_ERR_0000
#define UART_ERR_MUTEX          UART_ERR_0001
#define UART_ERR_UNAVAIL        UART_ERR_0002
#define UART_ERR_TIMEOUT        UART_ERR_0003
#define UART_ERR_RECOVERY       UART_ERR_0004
#define UART_ERR_UNEXPECTED     UART_ERR_00FF

#define UART_DATA_5_BIT             (0U)
#define UART_DATA_6_BIT             (1U)
#define UART_DATA_7_BIT             (2U)
#define UART_DATA_8_BIT             (3U)

#define UART_PARITY_NONE            (0U)
#define UART_PARITY_EVEN            (1U)
#define UART_PARITY_ODD             (2U)

#define UART_STOP_BIT_ONE           (0U)
#define UART_STOP_BIT_ONE_AND_HALF  (1U)
#define UART_STOP_BIT_TWO           (2U)

#define UART_FLOW_CTRL_NONE         (0U)
#define UART_FLOW_CTRL_HW           (1U)

/* Legacy definitions */
#define AMBA_UART_DATA_5_BIT            UART_DATA_5_BIT
#define AMBA_UART_DATA_6_BIT            UART_DATA_6_BIT
#define AMBA_UART_DATA_7_BIT            UART_DATA_7_BIT
#define AMBA_UART_DATA_8_BIT            UART_DATA_8_BIT

#define AMBA_UART_PARITY_NONE           UART_PARITY_NONE
#define AMBA_UART_PARITY_EVEN           UART_PARITY_EVEN
#define AMBA_UART_PARITY_ODD            UART_PARITY_ODD

#define AMBA_UART_STOP_1_BIT            UART_STOP_BIT_ONE
#define AMBA_UART_STOP_1D5_BIT          UART_STOP_BIT_ONE_AND_HALF
#define AMBA_UART_STOP_2_BIT            UART_STOP_BIT_TWO

#define AMBA_UART_FLOW_CTRL_NONE        UART_FLOW_CTRL_NONE
#define AMBA_UART_FLOW_CTRL_HARDWARE    UART_FLOW_CTRL_HW

typedef struct {
    UINT32   NumDataBits;
    UINT32   ParityBitMode;
    UINT32   NumStopBits;
} AMBA_UART_CONFIG_s;

typedef void (*AMBA_UART_ISR_f) (UINT32 UserArg);

#endif /* AMBA_UART_DEF_H */
