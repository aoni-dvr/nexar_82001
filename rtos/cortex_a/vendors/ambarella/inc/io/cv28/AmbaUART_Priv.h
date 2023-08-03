/**
 *  @file AmbaUART_Priv.h
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

#ifndef AMBA_UART_PRIV_H
#define AMBA_UART_PRIV_H

#define AMBA_UART_APB_CHANNEL0          0U
#define AMBA_UART_AHB_CHANNEL0          1U
#define AMBA_UART_AHB_CHANNEL1          2U
#define AMBA_UART_AHB_CHANNEL2          3U
#define AMBA_UART_AHB_CHANNEL3          4U
#define AMBA_NUM_UART_CHANNEL           5U

#if defined(CONFIG_A53_CONSOLE_AHB_0)
#define AMBA_UART_A53_CONSOLE       AMBA_UART_AHB_CHANNEL0
#elif   defined(CONFIG_A53_CONSOLE_AHB_1)
#define AMBA_UART_A53_CONSOLE       AMBA_UART_AHB_CHANNEL1
#else   /* DEFAULT */
#define AMBA_UART_A53_CONSOLE       AMBA_UART_APB_CHANNEL0
#endif

#define AMBA_UART_R52_CONSOLE       AMBA_NUM_UART_CHANNEL

#endif /* AMBA_UART_PRIV_H */
