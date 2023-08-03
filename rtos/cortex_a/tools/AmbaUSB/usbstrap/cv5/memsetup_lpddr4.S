/**
 * History:
 *    2019/01/31 - [Cao Rongrong]
 *
 * Copyright (c) 2016 Ambarella International LP
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
#include <arch_cv5/cv5.h>
#include <arch_cv5/drctl.h>

.text

/****************************************************************************
*  x0 - x3 tmp used
*
*  x19 - DRAMC_DRAM_BASE
*  x20 - DRAMC_DDRC_BASE
*****************************************************************************/

.globl plat_dram_init

plat_dram_init:
	mov	x29, x30

	/* set DDRIO PLL */
	ldr	x5, =DRAMC_HOST_BASE
0:	ldr	w0, pll_ddr_ctrl3_param
	str	w0, [x5, #PLL_DDR_CTRL3_OFFSET]
	ldr	w0, pll_ddr_ctrl2_param
	str	w0, [x5, #PLL_DDR_CTRL2_OFFSET]
	ldr	w0, pll_ddr_ctrl_param
	add	x1, x5, #PLL_DDR_CTRL_OFFSET
	str	w0, [x1]
	orr	w2, w0, #0x1
	str	w2, [x1]
	str	w0, [x1]

	add	x5, x5, #0x10000
	tbnz	x5, #16, 0b
	mov	w0, #48		/* delay 2us */
	bl	udelay

	ldr	x19, =DRAMC_DRAM_BASE
	ldr	x20, =DRAMC_DDRC_BASE

	/* Set DRAM_MODE_REG */
	ldr	w11, dram_mode_param
	str	w11, [x19]
	mov	w10, #(0x1 << 1)

DDRC_CONFIG_LOOP:
	and	w0, w11, w10
	cbz	w0, DDRC_CONFIG_NEXT

	/********* DDRIO Configuration *********/

	/* Program lpddr5 dq vref value */
	ldr	w0, ddrio_vref_0
	str	w0, [x20, #DDRIO_VREF_0]
	ldr	w0, ddrio_vref_1
	str	w0, [x20, #DDRIO_VREF_1]

	ldr	w0, dll_ctrl_sel0_param
	str	w0, [x20, #DDRIO_DLL_CTRL_SEL(0, 0)]
	str	w0, [x20, #DDRIO_DLL_CTRL_SEL(0, 1)]
	ldr	w0, dll_ctrl_sel1_param
	str	w0, [x20, #DDRIO_DLL_CTRL_SEL(1, 0)]
	str	w0, [x20, #DDRIO_DLL_CTRL_SEL(1, 1)]
	ldr	w0, dll_ctrl_sel2_param
	str	w0, [x20, #DDRIO_DLL_CTRL_SEL(2, 0)]
	str	w0, [x20, #DDRIO_DLL_CTRL_SEL(2, 1)]

	ldr	w0, dll_ctrl_sbc_param
	str	w0, [x20, #DDRIO_DLL_CTRL_SBC(0)]
	str	w0, [x20, #DDRIO_DLL_CTRL_SBC(1)]
	str	w0, [x20, #DDRIO_DLL_CTRL_SBC(2)]
	str	w0, [x20, #DDRIO_DLL_CTRL_SBC(3)]

	/* write to CK delay */
	ldr	w0, ddrio_ck_delay_param
	str	w0, [x20, #DDRIO_CK_DELAY]

	/* write to CA Coarse delay. */
	ldr	w0, ddrio_ca_cs_delay_param
	str	w0, [x20, #DDRIO_CA_DELAY_COARSE]
	str	w0, [x20, #DDRIO_CKE_DELAY_COARSE]

	/* write to WCK/DQS WRDLY and RDDLY */
	ldr	w0, ddrio_wck_dqs_delay_param
	str	w0, [x20, #DDRIO_BYTE_DLY0(0, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY0(1, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY0(2, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY0(3, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY0(0, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY0(1, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY0(2, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY0(3, 1)]

	/* write to DQ_WRDLY_COARSE/DQ_RDDLY_COARSE */
	ldr	w0, dq_dly_coarse_param
	str	w0, [x20, #DDRIO_BYTE_DLY1(0, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY1(1, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY1(2, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY1(3, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY1(0, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY1(1, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY1(2, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY1(3, 1)]

	/* write to DQ_WRDLY_FINE */
	ldr	w0, dq_wrdly_fine_param
	str	w0, [x20, #DDRIO_BYTE_DLY2(0, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY2(1, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY2(2, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY2(3, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY2(0, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY2(1, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY2(2, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY2(3, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(0, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(1, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(2, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(3, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(0, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(1, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(2, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY3(3, 1)]

	/* write to DQ_RDDLY_FINE */
	ldr	w0, dq_rddly_fine_param
	str	w0, [x20, #DDRIO_BYTE_DLY4(0, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY4(1, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY4(2, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY4(3, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY4(0, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY4(1, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY4(2, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY4(3, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(0, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(1, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(2, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(3, 0)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(0, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(1, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(2, 1)]
	str	w0, [x20, #DDRIO_BYTE_DLY5(3, 1)]

	/* write to DDRIO_PAD_CTRL */
	ldr	w0, ddrio_pad_ctrl_param
	str	w0, [x20, #DDRIO_PAD_CTRL]

	/* write to DDRIO_DQS_PUPD */
	ldr	w0, ddrio_dqs_pupd_param
	str	w0, [x20, #DDRIO_DQS_PUPD]

	/* write to DDRIO_CA_PADCTRL */
	ldr	w0, ddrio_ca_padctrl_param
	str	w0, [x20, #DDRIO_CA_PADCTRL]

	/* write to DDRIO_DQ_PADCTRL */
	ldr	w0, ddrio_dq_padctrl_param
	str	w0, [x20, #DDRIO_DQ_PADCTRL]


	/********* DDRC Configuration *********/

	/* disable Auto refresh and dram access */
	str	wzr, [x20, #DRAM_CONTRL]

	/* wait 200us for tINIT1 and tINIT2(10ns), add 5us extra time for guarantee */
	mov	w0, #(24 * 200 + 24 * 5)
	bl	udelay

	/* write to DRAM_RSVD_SPACE_REG */
	ldr	w0, dram_rsvd_space_param
	str	w0, [x20, #DRAM_RSVD_SPACE]

	/* set dram byte map */
	ldr	w0, dram_byte_map_ddrc0_param
	tbnz	w10, #1, 1f
	ldr	w0, dram_byte_map_ddrc1_param
1:	str	w0, [x20, #DRAM_BYTE_MAP]

	/* write to DRAM_CONFIG1 */
	ldr	w12, dram_config1_param
	str	w12, [x20, #DRAM_CONFIG1]

	/* write to DRAM_CONFIG2 */
	ldr	w13, dram_config2_param
	str	w13, [x20, #DRAM_CONFIG2]

	/* write to DRAM_TIMING1 reg */
	ldr	w0, dram_timing1_param
	str	w0, [x20, #DRAM_TIMING1]

	/* write to DRAM_TIMING2 reg */
	ldr	w0, dram_timing2_param
	str	w0, [x20, #DRAM_TIMING2]

	/* write to DRAM_TIMING3 reg */
	ldr	w0, dram_timing3_param
	str	w0, [x20, #DRAM_TIMING3]

	/* write to DRAM_TIMING4 reg */
	ldr	w0, dram_timing4_param
	str	w0, [x20, #DRAM_TIMING4]

	/* write to DRAM_TIMING5 reg */
	ldr	w0, dram_timing5_param
	str	w0, [x20, #DRAM_TIMING5]

	/* write to DUAL_DIE_TIMING reg */
	ldr	w0, dram_dual_die_timing_param
	str	w0, [x20, #DRAM_DUAL_DIE_TIMING]

	/* write to REFRESH_TIMING reg */
	ldr	w0, dram_refresh_timing_param
	str	w0, [x20, #DRAM_REFRESH_TIMING]

	/* write to DRAM_WDQS_TIMING reg */
	ldr	w0, dram_wdqs_timing_param
	str	w0, [x20, #DRAM_WDQS_TIMING]

	/* write to DTTE_TIMING reg */
	ldr	w0, dram_dtte_timing_param
	str	w0, [x20, #DRAM_DTTE_TIMING]

	/* Disable DRAM reset, i.e., de-asserted to HIGH */
	mov	w0, #DRAM_IO_CONTROL_DISABLE_RESET
	str	w0, [x20, #DRAM_IO_CONTROL]

	/* wait 2ms for tINIT3, add 10us extra time for guarantee */
	mov	w0, #(24 * 2000 + 24 * 10)
	bl	udelay

	/* issue DLL RST in order to lock on the clock */
	mov	w0, #DRAM_INIT_CTL_DLL_RST_EN
	bl	set_init_ctl

	/* set CKE to HIGH */
	mov	w0, #(DRAM_IO_CONTROL_DISABLE_RESET | DRAM_IO_CONTROL_CKE_ALL)
	str	w0, [x20, #DRAM_IO_CONTROL]

	/* wait 2us for tINIT5, add 2us extra time for guarantee */
	mov	w0, #(24 * 2 + 24 * 2)
	bl	udelay

DRAM_MR_SETUP:
	/* set mode register 13 to select FSP0/FSP1 */
	ldr	w2, =0x9f0d0008
	mov	w9, wzr

DRAM_MR_LOOP:
	add	w0, w2, w9
	bl	set_dram_mode

	/* wait 200ns for tVRCG_Enable */
	mov	w0, #24
	bl	udelay

	ldr	w0, lp4_mode_reg3_param
	bl	set_dram_mode

	ldr	w0, lp4_mode_reg2_param
	bl	set_dram_mode

	ldr	w0, lp4_mode_reg1_param
	bl	set_dram_mode

	ldr	w0, lp4_mode_reg11_param
	bl	set_dram_mode

	ldr	w0, lp4_mode_reg14_param
	bl	set_dram_mode

	ldr	w0, lp4_mode_reg12_param
	bl	set_dram_mode

	ldr	w0, lp4_mode_reg22_param
	bl	set_dram_mode

	add	w9, w9, #0x40
	tbnz	w9, #6, DRAM_MR_LOOP

	/* Perform ZQ calibration */
ZQ_CALIB_START:
	mov	w9, wzr
	mov	w4, #0x00000301

ZQ_CALIB_LOOP:
	/* trigger MPC MPC_ZQCAL_START_LP4  */
	mov	w1, #0x0000000d
	mov	w2, #0x00000f20
	add	w4, w4, w9
	bl	trigger_uinst

	/* wait 1us for tZQCAL */
	mov	w0, #24
	bl	udelay

	/* trigger MPC MPC_ZQCAL_LATCH_LP4  */
	mov	w1, #0x0000000d
	mov	w2, #0x00001120
	bl	trigger_uinst

	/* wait 1us for tZQLAT which need 30ns only */
	mov	w0, #24
	bl	udelay

	/* branch out if single die */
	tbz	w12, #26, ZQ_CALIB_DONE
	add	w9, w9, #1
	tbnz	w9, #0, ZQ_CALIB_LOOP

ZQ_CALIB_DONE:
	/* trigger manual long PAD calibration */
	mov	w0, #(DDRIO_ZCTRL_PAD_RESET | DDRIO_ZCTRL_PAD_CLB_LONG)
	str	w0, [x20, #DDRIO_ZCTRL]
1:	ldr	w0, [x20, #DDRIO_ZCTRL_STATUS]
	and	w0, w0, #DDRIO_ZCTRL_STATUS_ACK
	cbnz	w0, 1b
	/* drop manual long PAD calibration */
	mov	w0, #DDRIO_ZCTRL_PAD_RESET
	str	w0, [x20, #DDRIO_ZCTRL]

	/* Trigger RTT */
GET_RTT_START:
	ldr	w2, =0x9b0d0008
	mov	w3, wzr

GET_RTT_LOOP:
	/* setting DDR to Read preamble mode */
	add	w0, w2, w3, lsl #25
	orr	w0, w0, #0x2
	bl	set_dram_mode

	/* set DQS gate to 1 */
	mov	w0, #0x00000c00
	str	w0, [x20, #DRAM_CONFIG2]

	/* issue GET_RTT_EN command */
	mov	w0, #DRAM_INIT_CTL_GET_RTT_EN
	add	w0, w0, w3, lsl #10
	bl	set_init_ctl

	/* restore DQS gate to original value */
	str	w13, [x20, #DRAM_CONFIG2]

	/* unset DDR Read preamble mode */
	add	w0, w2, w3, lsl #25
	bl	set_dram_mode

	/* branch out if single die */
	tbz	w12, #26, DDRC_CONFIG_NEXT
	add	w3, w3, #1
	tbnz	w3, #0, GET_RTT_LOOP

DDRC_CONFIG_NEXT:
	add	x20, x20, #0x1000
	lsl	w10, w10, #1
	tbz	w10, #3, DDRC_CONFIG_LOOP

DDRC_CONFIG_DONE:
	/* restore x20 to 0x1000005000 */
	sub	x20, x20, #0x2000

	/* Enable AUTO Refresh - Enable DRAM - (for all DDRHOSTs) */
	mov	w0,  #(DRAM_CONTROL_AUTO_REF_EN | DRAM_CONTROL_ENABLE)
	sub	x1, x20, #0x1000	/* broadcase register */
	str	w0, [x1]

	/* Let DDRC building up  AR credit before sending requests (1us) */
	mov	w0, #24
	bl	udelay

	/* DRAM INITIALIZATION done and return */
	ret	x29

