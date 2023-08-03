/**
 * Copyright (c) 2021 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
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
 */
#ifndef __AMBUST_CV22_H__
#define __AMBUST_CV22_H__

#define DRAM_SPACE_START	0x00000000UL
#define DRAM_SPACE_MAX_SIZE	0xC0000000UL		/* 3 GB */
#define DRAM_SPACE_MAX_GB	3
#define DEVICE_SPACE_START	0xC0000000UL
#define DEVICE_SPACE_MAX_SIZE	0x40000000UL		/* 1 GB */

#define AHB_BASE		0xE0000000UL
#define APB_BASE		0xE4000000UL
#define AHB_S_BASE		0xE8000000UL
#define APB_S_BASE		0xEC000000UL
#define DBGBUS_BASE		0xED000000UL

#define AXI_BASE		0xF2000000UL
#define GIC_BASE		0xF3000000UL

#define UART0_BASE		0xE4000000UL
#define UART1_BASE		0xE0017000UL
#define UART2_BASE		0xE0018000UL
#define UART3_BASE		0xE0019000UL
#define UART4_BASE		0xE001A000UL

#define IOMUX_BASE		0xEC000000UL
#define RCT_BASE		0xED080000UL
#define USBD_BASE		0xE0006000UL

#define CHIP_REV		22000

#endif
