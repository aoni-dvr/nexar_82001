/*
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

#ifndef __AMB_DRCTRL_H__
#define __AMB_DRCTRL_H__

#define DRAMC_DDRC_BASE				(DDRC_BASE + 0x5000)
#define DRAMC_DRAM_BASE				(DDRC_BASE + 0x0000)
#define DRAMC_HOST_BASE				0x20ed180000

#define PLL_DDR_CTRL_OFFSET			0x000
#define PLL_DDR_FRAC_OFFSET			0x004
#define PLL_DDR_CTRL2_OFFSET			0x008
#define PLL_DDR_CTRL3_OFFSET			0x00c

#define DDRC0					0
#define DDRC1					1

/* Dram registers offset*/
#define REG_DRAM_MODE				0x000
#define DRAM_BURST_SIZE_FIXED			64

#define DRAM_CONTRL				0x00000000
#define DRAM_CONFIG1				0x00000004
#define DRAM_CONFIG2				0x00000008
#define DRAM_TIMING1				0x0000000c
#define DRAM_TIMING2				0x00000010
#define DRAM_TIMING3				0x00000014
#define DRAM_TIMING4				0x00000018
#define DRAM_TIMING5				0x0000001c
#define DRAM_DUAL_DIE_TIMING			0x00000020
#define DRAM_REFRESH_TIMING			0x00000024
#define DRAM_LP5_TIMING				0x00000028
#define DRAM_INIT_CTL				0x0000002c
#define DRAM_MODE_REG				0x00000030
#define DRAM_SELF_REFRESH			0x00000034
#define DRAM_RSVD_SPACE				0x00000038
#define DRAM_BYTE_MAP				0x0000003c
#define DRAM_MPC_WDATA				0x00000040
#define DRAM_MPC_WMASK				0x00000060
#define DRAM_MPC_RDATA				0x00000064
#define DRAM_MPC_RMASK				0x000000a4
#define DRAM_UINST1				0x000000ac
#define DRAM_UINST2				0x000000b0
#define DRAM_UINST3				0x000000b4
#define DRAM_UINST4				0x000000b8
#define DRAM_UINST5				0x000000bc
#define DRAM_UINST6				0x000000c0
#define DRAM_SCRATCHPAD				0x000000c4
#define DRAM_WDQS_TIMING			0x000000c8
#define DRAM_CLEAR_MPC_DATA			0x000000cc
#define DRAM_IO_CONTROL				0x000000d0
#define DRAM_DDRC_MISC_1			0x000000d4
#define DRAM_DTTE_CONFIG			0x00000100
#define DRAM_DTTE_DELAY_MAP			0x00000134
#define DRAM_WRITE_VREF_0			0x00000164
#define DRAM_WRITE_VREF_1			0x00000168
#define DRAM_DTTE_TIMING			0x0000016c
#define DDRIO_DLL_CTRL_SBC(b)			(0x00000200 + (b) * 4)
#define DDRIO_DLL_CTRL_SEL(s, d)		(0x00000210 + (s) * 8 + (d) * 4)
#define DDRIO_BYTE_DLY0(b, d)			(0x00000228 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY1(b, d)			(0x0000022c + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY2(b, d)			(0x00000230 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY3(b, d)			(0x00000234 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY4(b, d)			(0x00000238 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY5(b, d)			(0x0000023c + (b) * 24 + (d) * 96)
#define DDRIO_CK_DELAY				0x000002e8
#define DDRIO_CA_DELAY_COARSE			0x000002ec
#define DDRIO_CA_DELAY_FINE_0(c, d)		(0x000002f0 + (c) * 16 + (d) * 8)
#define DDRIO_CA_DELAY_FINE_1(c, d)		(0x000002f4 + (c) * 16 + (d) * 8)
#define DDRIO_CS_DELAY				0x00000310
#define DDRIO_CKE_DELAY_COARSE			0x00000314
#define DDRIO_CKE_DELAY_FINE			0x00000318
#define DDRIO_DQS_TESTIRCV			0x0000035c
#define DDRIO_PAD_CTRL				0x00000360
#define DDRIO_DQS_PUPD				0x00000364
#define DDRIO_ZCTRL				0x0000036c
#define DDRIO_CA_PADCTRL			0x00000370
#define DDRIO_DQ_PADCTRL			0x00000374
#define DDRIO_VREF_0				0x00000378
#define DDRIO_VREF_1				0x0000037c
#define DDRIO_IBIAS_CTRL			0x00000380
#define DDRIO_ZCTRL_STATUS			0x00000384
#define DDRIO_DLL_STATUS_0			0x00000388
#define DDRIO_DLL_STATUS_1			0x0000038c
#define DRAM_DDRC_STATUS			0x000003a4
#define DRAM_DDRC_MISC_2			0x000003c0
#define DRAM_DDRC_MR46				0x000003d4

/* DRAM_CONFIG */
#define DRAM_CONFIG_DRAM_SIZE_MASK		0x0000003C
#define DRAM_CONFIG_DRAM_SIZE_SHIFT		2

/* DRAM_CTL register bit */
#define DRAM_CONTROL_AR_CNT_RELOAD		0x00000004
#define DRAM_CONTROL_AUTO_REF_EN		0x00000002
#define DRAM_CONTROL_ENABLE			0x00000001

/* DRAM_IO_CONTROL register bit */
#define	DRAM_IO_CONTROL_CKE(d, c)		(1 << ((d) * 2 + (c)))
#define DRAM_IO_CONTROL_CKE_ALL			0x0000000f
#define	DRAM_IO_CONTROL_DISABLE_RESET		0x00000010
#define	DRAM_IO_CONTROL_DISABLE_CMD		0x00000020

/* DRAM_INIT_CTL register bit */
#define DRAM_INIT_CTL_RTT_DIE			0x00000400
#define DRAM_INIT_CTL_PAD_CLB_LONG_EN		0x00000200
#define DRAM_INIT_CTL_PAD_CLB_SHORT_EN		0x00000100
#define DRAM_INIT_CTL_DLL_RST_EN		0x00000008
#define DRAM_INIT_CTL_GET_RTT_EN		0x00000004

/* DDRIO_ZCTRL register bit */
#define DDRIO_ZCTRL_PAD_CLB_LONG		0x00000080
#define DDRIO_ZCTRL_PAD_CLB_SHORT		0x00000100
#define DDRIO_ZCTRL_PAD_RESET			0x00000200

/* DDRIO_ZCTRL_STATUS register bit */
#define DDRIO_ZCTRL_STATUS_ACK			0x00002000

/* Dram mode register bit */
#define	DRAM_MSB_BUSY				0x80000000

/* ==========================================================================*/
#endif /* __AMB_DRCTRL_H__ */

