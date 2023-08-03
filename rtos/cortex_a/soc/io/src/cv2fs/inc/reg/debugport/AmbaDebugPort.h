/**
 *  @file AmbaDebugPort.h
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
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

#define AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR                (0xed000000U)

#define AMBA_DBG_PORT_VORC_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x010000U)  /* Vision Orc Vp0, Fex/Stereo */
#define AMBA_DBG_PORT_CORC_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x160000U)  /* Code, IDSP Orc */
#define AMBA_DBG_PORT_EORC_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x020000U)  /* ME Orc */
#define AMBA_DBG_PORT_DORC_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x030000U)  /* MDEnc, MDDec ,TSFM Orc */
#define AMBA_DBG_PORT_SMEM0_BASE_ADDR                       (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x040000U)  /* smem */
#define AMBA_DBG_PORT_DRAM_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x050000U)  /* dram */
#define AMBA_DBG_PORT_SMEM2_BASE_ADDR                       (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x060000U)
#define AMBA_DBG_PORT_SMEM3_BASE_ADDR                       (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x070000U)
#define AMBA_DBG_PORT_RCT_BASE_ADDR                         (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x080000U)
#define AMBA_DBG_PORT_OL2C_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x0a0000U)  /* Vision ORC L2 Cache */
#define AMBA_DBG_PORT_MCTF_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x0b0000U)
#define AMBA_DBG_PORT_TEMPSENSOR_BASE_ADDR                  (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x0d0080U)  /* Temperature Sensor 0xed0d0080 */
#define AMBA_DBG_PORT_VMEM0_BASE_ADDR                       (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x100000U)
#define AMBA_DBG_PORT_VMEM1_BASE_ADDR                       (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x110000U)
#define AMBA_DBG_PORT_VMEM2_BASE_ADDR                       (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x120000U)
#define AMBA_DBG_PORT_VMEM3_BASE_ADDR                       (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x130000U)
#define AMBA_DBG_PORT_IDSP_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x1c0000U)
#define AMBA_DBG_PORT_LF_BASE_ADDR                          (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x1d0000U)  /* Loop Filter */
#define AMBA_DBG_PORT_MDXF_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x1f0000U)
#define AMBA_DBG_PORT_MIPI_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x200000U)
#define AMBA_DBG_PORT_TSFM_BASE_ADDR                        (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x230000U)  /* AVC transform */
#define AMBA_DBG_PORT_VP_BASE_ADDR                          (AMBA_CORTEX_A53_DEBUG_PORT_BASE_ADDR + 0x800000U)  /* Vector Processor */

#define AMBA_DBG_PORT_IDSP_CONTROLLER_BASE_ADDR             (AMBA_DBG_PORT_IDSP_BASE_ADDR + 0x8000U)
#define AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR          (AMBA_DBG_PORT_IDSP_BASE_ADDR)
#define AMBA_DBG_PORT_IDSP_VIN_MAIN_CONFIG_BASE_ADDR        (AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR)
#define AMBA_DBG_PORT_IDSP_VIN_GLOBAL_CONFIG_BASE_ADDR      (AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR + 0x800U)
#define AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_CONFIG_BASE_ADDR (AMBA_DBG_PORT_IDSP_FILTER_CONFIG_BASE_ADDR + 0x400U)

#endif /* AMBA_DEBUG_PORT_H */
