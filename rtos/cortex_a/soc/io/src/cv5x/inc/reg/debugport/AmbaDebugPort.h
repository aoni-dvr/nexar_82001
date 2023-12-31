/**
 *  @file AmbaDebugPort.h
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Debug Registers
 *
 */

#ifndef AMBA_DEBUG_PORT_H
#define AMBA_DEBUG_PORT_H

#define AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR                (0x20ed000000UL)

#define AMBA_DBG_PORT_VORC_BASE_ADDR                        (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x010000UL)
#define AMBA_DBG_PORT_EORC_BASE_ADDR                        (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x020000UL)  /* ME, MCTF Orc */
#define AMBA_DBG_PORT_DORC_BASE_ADDR                        (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x030000UL)
#define AMBA_DBG_PORT_SMEM0_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x040000UL)
#define AMBA_DBG_PORT_SMEM1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x050000UL)
#define AMBA_DBG_PORT_RCT_BASE_ADDR                         (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x080000UL)
#define AMBA_DBG_PORT_L2C_BASE_ADDR                         (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x0a0000UL)
#define AMBA_DBG_PORT_RTC_BASE_ADDR                         (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x0d0000UL)
#define AMBA_DBG_PORT_ROSC_BASE_ADDR                        (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x0e0000UL)

#define AMBA_DBG_PORT_VMEM0_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x100000UL)
#define AMBA_DBG_PORT_VMEM1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x110000UL)
#define AMBA_DBG_PORT_VMEM2_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x120000UL)
#define AMBA_DBG_PORT_VMEM3_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x130000UL)
#define AMBA_DBG_PORT_CORC0_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x140000UL)
#define AMBA_DBG_PORT_CORC1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x150000UL)
#define AMBA_DBG_PORT_CORC2_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x160000UL)
#define AMBA_DBG_PORT_CORC3_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x170000UL)
#define AMBA_DBG_PORT_DDRH0_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x180000UL)
#define AMBA_DBG_PORT_DDRH1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x190000UL)
#define AMBA_DBG_PORT_IDSP_BASE_ADDR                        (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x1c0000UL)
#define AMBA_DBG_PORT_IDSP1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x1d0000UL)
#define AMBA_DBG_PORT_IDSPV_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x1e0000UL)
#define AMBA_DBG_PORT_DBSE_BASE_ADDR                        (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x1f0000UL)

#define AMBA_DBG_PORT_MIPI_BASE_ADDR                        (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x200000UL)
#define AMBA_DBG_PORT_MDXF0_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x210000UL)
#define AMBA_DBG_PORT_MDXF1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x220000UL)

#define AMBA_DBG_PORT_EORC1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x320000UL)
#define AMBA_DBG_PORT_DORC1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x330000UL)
#define AMBA_DBG_PORT_VP0_BASE_ADDR                         (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x800000UL)
#define AMBA_DBG_PORT_PMEM0_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0xA00000UL)
#define AMBA_DBG_PORT_PMEM1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0xB00000UL)
#define AMBA_DBG_PORT_FMEM0_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x1000000UL)
#define AMBA_DBG_PORT_FMEM1_BASE_ADDR                       (AMBA_CORTEX_A76_DEBUG_PORT_BASE_ADDR + 0x2000000UL)

#define AMBA_DBG_PORT_IDSP_CONTROLLER_BASE_ADDR             (AMBA_DBG_PORT_IDSPV_BASE_ADDR + 0x8000UL)
#define AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR          (AMBA_DBG_PORT_IDSPV_BASE_ADDR)
#define AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR        (AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR)
#define AMBA_DBG_PORT_IDSP_VIN_GLOBAL_CONFIG_BASE_ADDR      (AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR + 0x800UL)
#define AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_CONFIG_BASE_ADDR (AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR + 0x400UL)

#endif /* AMBA_DEBUG_PORT_H */
