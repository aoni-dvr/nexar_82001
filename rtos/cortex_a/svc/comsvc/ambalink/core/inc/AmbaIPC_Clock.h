/**
 * @file AmbaIPC_Clock.h
 *
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
 *
 * @details Definitions for Clock rpdev driver.
 *
 */

#ifndef _AMBA_IPC_CLOCK_H_
#define _AMBA_IPC_CLOCK_H_

/*-----------------------------------------------------------------------------------------------*\
 * Clock index.
\*-----------------------------------------------------------------------------------------------*/
#define AMBALINK_CLK_IDSP   0U
#define AMBALINK_CLK_CORE   1U
#define AMBALINK_CLK_CORTEX 2U
#define AMBALINK_CLK_SDIO   3U
#define AMBALINK_CLK_SD48   4U
#define AMBALINK_CLK_UART   5U
#define AMBALINK_CLK_ARM    6U
#define AMBALINK_CLK_AHB    7U
#define AMBALINK_CLK_APB    8U
#define AMBALINK_CLK_AXI    9U

/*-----------------------------------------------------------------------------------------------*\
 * Rpdev clock data structure.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AmbaRpdev_Clock_s_ {
    UINT32 ClockIndex;  /**< ClockIndex */
    UINT32 Rate;        /**< Rate */
} AmbaRpdev_Clock_s;

/*-----------------------------------------------------------------------------------------------*\
 * Rpdev clock command enum.
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_RPDEV_CLK_CMD_e_ {
    CLK_SET = 0,
    CLK_GET,
    CLK_RPMSG_ACK_THREADX,
    CLK_RPMSG_ACK_LINUX,
    CLK_CHANGED_PRE_NOTIFY,
    CLK_CHANGED_POST_NOTIFY,
} AMBA_RPDEV_CLK_CMD_e;

/*-----------------------------------------------------------------------------------------------*\
 * Rpdev clock message data structure.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_RPDEV_CLK_MSG_s_ {
    UINT32    Cmd;    /**< Cmd */
    UINT64    Param;  /**< Param */
} AMBA_RPDEV_CLK_MSG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaIPC_Clock.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaIPC_ClockInit(void);

#endif    /* _AMBA_IPC_CLOCK_H_ */
