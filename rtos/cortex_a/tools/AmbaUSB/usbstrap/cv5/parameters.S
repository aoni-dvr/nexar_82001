/**
 * History:
 *    2019/01/31 - [Cao Rongrong]
 *    2021/10/01 - [Jorney] update
 *
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
#include <config.h>
#include "parameters.h"

#ifndef DRAM_BYTE_MAP_DDRC1_VAL
#define DRAM_BYTE_MAP_DDRC1_VAL		DRAM_BYTE_MAP_DDRC0_VAL
#endif


.globl __params_multi_start
.globl __params_multi_size

.globl pll_cortex_ctrl_param
.globl pll_dsu_ctrl_param
.globl pll_core_ctrl_param
.globl pll_idsp_ctrl_param
.globl pll_vision_ctrl_param
.globl pll_ddr_ctrl_param
.globl pll_ddr_ctrl2_param
.globl pll_ddr_ctrl3_param
.globl dll_ctrl_sel0_param
.globl dll_ctrl_sel1_param
.globl dll_ctrl_sel2_param
.globl dll_ctrl_sbc_param
.globl ddrio_ck_delay_param
.globl ddrio_ca_cs_delay_param
.globl ddrio_wck_dqs_delay_param
.globl dq_dly_coarse_param
.globl dq_wrdly_fine_param
.globl dq_rddly_fine_param
.globl ddrio_vref_0
.globl ddrio_vref_1
.globl ddrio_pad_ctrl_param
.globl ddrio_dqs_pupd_param
.globl ddrio_ca_padctrl_param
.globl ddrio_dq_padctrl_param
.globl dram_mode_param
.globl dram_config1_param
.globl dram_config2_param
.globl dram_timing1_param
.globl dram_timing2_param
.globl dram_timing3_param
.globl dram_timing4_param
.globl dram_timing5_param
.globl dram_dual_die_timing_param
.globl dram_refresh_timing_param
.globl dram_lp5_timing_param
.globl dram_wdqs_timing_param
.globl dram_dtte_timing_param
.globl dram_byte_map_ddrc0_param
.globl dram_byte_map_ddrc1_param
.globl dram_rsvd_space_param
.globl training_param
.globl dtte_param
.globl lp4_mode_reg14_param
.globl lp4_mode_reg1_param
.globl lp4_mode_reg2_param
.globl lp4_mode_reg3_param
.globl lp4_mode_reg11_param
.globl lp4_mode_reg12_param
.globl lp4_mode_reg22_param
.globl lp5_mode_reg1_param
.globl lp5_mode_reg2_param
.globl lp5_mode_reg3_param
.globl lp5_mode_reg10_param
.globl lp5_mode_reg11_param
.globl lp5_mode_reg12_param
.globl lp5_mode_reg13_param
.globl lp5_mode_reg14_param
.globl lp5_mode_reg15_param
.globl lp5_mode_reg17_param
.globl lp5_mode_reg18_param
.globl lp5_mode_reg19_param
.globl lp5_mode_reg20_param
.globl lp5_mode_reg22_param
.globl lp5_mode_reg28_param
.globl lp5_mode_reg41_param

/*******************************************************************/
.align 3	/* Must be aligned to 8B */

__params_multi_start:

pll_cortex_ctrl_param:		.int PLL_CORTEX_CTRL_REG_VAL
pll_dsu_ctrl_param:		.int PLL_DSU_CTRL_REG_VAL
pll_core_ctrl_param:		.int PLL_CORE_CTRL_REG_VAL
pll_idsp_ctrl_param:		.int PLL_IDSP_CTRL_REG_VAL
pll_vision_ctrl_param:		.int PLL_VISION_CTRL_REG_VAL
pll_ddr_ctrl_param:		.int PLL_DDR_CTRL_REG_DDRH0_VAL
pll_ddr_ctrl2_param:		.int PLL_DDR_CTRL2_REG_DDRH0_VAL
pll_ddr_ctrl3_param:		.int PLL_DDR_CTRL3_REG_DDRH0_VAL

dram_mode_param:		.int DRAM_MODE_REG_VAL
dram_config1_param:		.int DRAM_CONFIG1_DDRC0_VAL
dram_config2_param:		.int DRAM_CONFIG2_DDRC0_VAL
dram_timing1_param:		.int DRAM_TIMING1_DDRC0_VAL
dram_timing2_param:		.int DRAM_TIMING2_DDRC0_VAL
dram_timing3_param:		.int DRAM_TIMING3_DDRC0_VAL
dram_timing4_param:		.int DRAM_TIMING4_DDRC0_VAL
dram_timing5_param:		.int DRAM_TIMING5_DDRC0_VAL
dram_dual_die_timing_param:	.int DUAL_DIE_TIMING_DDRC0_VAL
dram_refresh_timing_param:	.int REFRESH_TIMING_DDRC0_VAL
dram_lp5_timing_param:		.int LP5_TIMING_DDRC0_VAL
dram_wdqs_timing_param:		.int DRAM_WDQS_TIMING_DDRC0_VAL
dram_dtte_timing_param:		.int DTTE_TIMING_DDRC0_VAL
dram_byte_map_ddrc0_param:	.int DRAM_BYTE_MAP_DDRC0_VAL
dram_byte_map_ddrc1_param:	.int DRAM_BYTE_MAP_DDRC1_VAL
dram_rsvd_space_param:		.int DRAM_RSVD_SPACE_DDRC0_VAL

ddrio_pad_ctrl_param:		.int DDRIO_PAD_CTRL_DDRC0_VAL
ddrio_dqs_pupd_param:		.int DDRIO_DQS_PUPD_DDRC0_VAL
ddrio_ca_padctrl_param:		.int DDRIO_CA_PADCTRL_DDRC0_VAL
ddrio_dq_padctrl_param:		.int DDRIO_DQ_PADCTRL_DDRC0_VAL
ddrio_ck_delay_param:		.int CK_DELAY_DDRC0_VAL
ddrio_ca_cs_delay_param:	.int CA_DELAY_COARSE_DDRC0_VAL
ddrio_wck_dqs_delay_param:	.int BYTE0_DLY0_DIE0_DDRC0_VAL

dll_ctrl_sbc_param:		.int DLL_CTRL_SBC_0_DDRC0_VAL
dtte_param:			.int PARAM_DTTE_TRAINING_VAL
/* do NOT change the order, including LPDDR4 MR14, as BLD may use it */
training_param:			.int PARAM_DRAM_TRAINING_VAL
dll_ctrl_sel0_param:		.int DLL_CTRL_SEL0_DIE0_DDRC0_VAL
dll_ctrl_sel1_param:		.int DLL_CTRL_SEL1_DIE0_DDRC0_VAL
dll_ctrl_sel2_param:		.int DLL_CTRL_SEL2_DIE0_DDRC0_VAL
ddrio_vref_0:			.int DDRIO_VREF_0_DDRC0_VAL
ddrio_vref_1:			.int DDRIO_VREF_1_DDRC0_VAL
dq_dly_coarse_param:		.int BYTE0_DLY1_DIE0_DDRC0_VAL
dq_wrdly_fine_param:		.int BYTE0_DLY2_DIE0_DDRC0_VAL
dq_rddly_fine_param:		.int BYTE0_DLY4_DIE0_DDRC0_VAL

/*******************************************************************/

#ifdef DRAM_TYPE_LPDDR4
lp4_mode_reg14_param:		.int LPDDR4_MR14_VAL
lp4_mode_reg1_param:		.int LPDDR4_MR1_VAL
lp4_mode_reg2_param:		.int LPDDR4_MR2_VAL
lp4_mode_reg3_param:		.int LPDDR4_MR3_VAL
lp4_mode_reg11_param:		.int LPDDR4_MR11_VAL
lp4_mode_reg12_param:		.int LPDDR4_MR12_VAL
lp4_mode_reg22_param:		.int LPDDR4_MR22_VAL
#endif

#ifdef DRAM_TYPE_LPDDR5
lp5_mode_reg1_param:		.int LPDDR5_MR1_VAL
lp5_mode_reg2_param:		.int LPDDR5_MR2_VAL
lp5_mode_reg3_param:		.int LPDDR5_MR3_VAL
lp5_mode_reg10_param:		.int LPDDR5_MR10_VAL
lp5_mode_reg11_param:		.int LPDDR5_MR11_VAL
lp5_mode_reg12_param:		.int LPDDR5_MR12_VAL
lp5_mode_reg13_param:		.int LPDDR5_MR13_VAL
lp5_mode_reg14_param:		.int LPDDR5_MR14_VAL
lp5_mode_reg15_param:		.int LPDDR5_MR15_VAL
lp5_mode_reg17_param:		.int LPDDR5_MR17_VAL
lp5_mode_reg18_param:		.int LPDDR5_MR18_VAL
lp5_mode_reg19_param:		.int LPDDR5_MR19_VAL
lp5_mode_reg20_param:		.int LPDDR5_MR20_VAL
lp5_mode_reg22_param:		.int LPDDR5_MR22_VAL
lp5_mode_reg28_param:		.int LPDDR5_MR28_VAL
lp5_mode_reg41_param:		.int LPDDR5_MR41_VAL
#endif

