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
#if 0
#include <ambhw/scratchpad.h>
#include <ambhw/sd.h>
#include <ambhw/sd_dw.h>
#include <sdmmc.h>

#include <ambhw/rct.h>
#include <ambhw/gpio.h>
#else
#include "AmbaBST.h"
#include "AmbaBST_NAND.h"
#include "AmbaBST_eMMC.h"
#endif

#define FIFO_8K_ADDR_START		0x20e000c000
#define FIFO_8K_ADDR_END		0x20e000e000

#define SD0_BASE				AMBA_CORTEX_A76_SD_BASE_ADDR
#define cmd_18					0x123a  /* READ_MULTIPLE_BLOCK  expect 48 bit response (R1) */

.globl copy_2nd_bst

.text
.section .text.first_stage

/**
 * Initialize the SD controller and load the BLD/ATF binary into the dram, and
 * then jump to it.
 *
 * Garbles [x0 - x3]
 *
 * x12 - DRAM address for for data to load
 * x13 - EMMC device address for data to load
 * x14 - Length of data need to load
 *
 * x15 - Used in PTB and Recovery sub-routines
 * x16 - Used in PTB and Recovery sub-routines
 * x17 - Used in PTB and Recovery sub-routines
 *
 * x22 - DRAM address of next binary to jump
 *
 * x23 -  RCT base addr
 *
 * x24 - system POC value
 *
 * x25 - SD controller base addr
 * x26 - base address of boot cookie
 */

emmc_init:
	mov	x29, x30

	bl	cfg_sd_pinmux

	ldr	x25, =SD0_BASE

	/*
	 * RTL issue: when sw_reset_for_all, RTL will wrongly treat it''s in
	 * boot state, and some state machines are waiting for tRSCA timer
	 * expired.
	 */
mmc_reset_dat_cmd:
	/* software reset dat_line and cmd_line */
	ldr	w0, [x25, #SDHCI_CLOCK_CONTROL]
	orr	w0, w0, #((1 << 25) | (1 << 26))
	str	w0, [x25, #SDHCI_CLOCK_CONTROL]

	/* wait reset finished */
0:	ldr	w0, [x25, #SDHCI_CLOCK_CONTROL]
	tbnz	w0, #25, 0b
	tbnz	w0, #26, 0b

mmc_set_pre_clk:
	/* 828M/2070(0x816)=400K */
	mov	w0,#(0x816)
	bl	mmc_set_clk

mmc_set_power_mode:
	mov	w0, #(SDHCI_POWER_ON | SDHCI_POWER_330)
	strb	w0, [x25, #SDHCI_POWER_CONTROL]

mmc_interrupt_enable:
	/* should enable buffer read ready interrupt */
	ldr     w0, =0x03ff01f7
	str     w0, [x25, #SDHCI_INT_ENABLE]

mmc_go_idle:
	mov	w0, #(cmd_0 << 16)
	mov	w1, #0
	bl	mmc_send_cmd

mmc_send_op_cond:
	mov	w0, #(cmd_1 << 16)
	mov	w1, #0x40ff0000			/* ARGUMENT */
	bl	mmc_send_cmd
	tbz	w0, #31, mmc_send_op_cond	/* Wait for EMMC ready */

mmc_all_send_cid:
	mov	w0, #(cmd_2 << 16)
	mov	w1, #0				/* ARGUMENT */
	bl	mmc_send_cmd

	/* TODO: get CID value */

mmc_send_relative_addr:
	mov	w0, #(cmd_3 << 16)
	mov	w1, #(0x1 << 16)		/* ARGUMENT (rca << 16)*/
	bl	mmc_send_cmd

mmc_select_card:
	mov	w0, #(cmd_7 << 16)
	mov	w1, #(0x1 << 16)		/* ARGUMENT (rca << 16)*/
	bl	mmc_send_cmd

mmc_switch:
	/*
	 * POC14: 8-bit bus width
	 * POC15: 4-bit bus width
	 * Note:
	 *   POC14 and POC15 can NOT be set simultaneously, and they will
	 *   determine the controller bus width.
	 */
	mov	w0, wzr				/* 1-bt bus width */
	mov	w2, wzr
	tbz	w24, #14, 1f
	mov	w0, #2				/* 8-bt bus width */
	mov	w2, 0x20
	/* b	2f */
1:	tbz	w24, #15, 2f
	mov	w0, #1				/* 4-bt bus width */
	mov	w2, 0x02

2:	mov	w1, #((183 << 16) | (0x3 << 24))
	orr	w1, w1, w0, lsl #8

	strb	w2, [x25, #SDHCI_HOST_CONTROL]

	mov	w0, #(mcmd_6 << 16)
	bl	mmc_send_cmd

/* wait for tran state */
switch_status:
	mov	w1,#(1 << 16)
	mov 	w0, #(cmd_13 << 16)
	bl	mmc_send_cmd
	tbz	w0, #8, switch_status
	ubfx	w0, w0, #9, #4
	cmp w0, #4
	b.ne	switch_status

mmc_set_post_clk:
	/* 828M/35(0x23)=23.657M */
	mov	w0, #(0x23)
	bl	mmc_set_clk

mmc_set_blocklen:
	mov	w0, #(cmd_16 << 16)
	mov	w1, #0x200			/* BLOCKLEN = 512 */
	bl	mmc_send_cmd

mmc_switch_partition:
	mov	w0, #(mcmd_6 << 16)
#if defined(CONFIG_EMMC_BOOTPART_USER)
	ldr	w1, =0x02b30300//user
#elif defined(CONFIG_EMMC_BOOTPART_BP1)
	ldr	w1, =0x01B30100//BP1
#elif defined(CONFIG_EMMC_BOOTPART_BP2)
	ldr	w1, =0x01B30200//BP2
#endif
	bl	mmc_send_cmd

	ret	x29
/**
 *
 * Garbles [x0 - x1]
 *
 * x4 -  the remaining transfer length
 *
 * x12 - DRAM address for data to load
 * x13 - eMMC device address for data to load
 * x14 - Length of data need to load
 *
 * x25 - SD controller base addr
 */
load_to_sram:
	mov	x29, x30

	mov	w4, w14				/* save length to the remaining length reg */
	add	w4, w4, #511			/* round length up to 512 */
	and	w4, w4, #~(0x1ff)

	/* poll cmd and data firstly before set other mshc register */
poll_cmd_inhibit:
	ldr	w0, [x25, #SDHCI_PRESENT_STATE]
	tbnz	w0, #0, poll_cmd_inhibit

poll_data_inhibit:
	ldr	w0, [x25, #SDHCI_PRESENT_STATE]
	tbnz	w0, #1, poll_data_inhibit

	/* set block count */
	lsr	w0, w4, #9
	strh	w0, [x25, #SDHCI_BLOCK_COUNT]

	/* set block size */
	ldr	w0, =(0x200)
	strh	w0, [x25, #SDHCI_BLOCK_SIZE]

	/* DMA transfer address */
	//str	w12, [x25, #SDHCI_DMA_ADDRESS]

	/* set transfer mode */
	ldr	w0, =(/*(cmd_18 << 16) | */ SDHCI_TRNS_BLK_CNT_EN | \
			SDHCI_TRNS_AUTO_CMD12 | SDHCI_TRNS_READ | SDHCI_TRNS_MULTI)
	strh	w0, [x25, #SDHCI_TRANSFER_MODE]

	/* set argument */
	lsr	w1, w13, #9			/* data block address */
	str	w1, [x25, #SDHCI_ARGUMENT]

	/* issue command */
	ldr	w0, =cmd_18
	strh	w0, [x25, #SDHCI_COMMAND]

	/* poll transfer and command complete at the same time to avoid clear transfer complete */
poll_cmd_complete_done:
	ldr	w0, [x25, #SDHCI_INT_STATUS]
	tbz	w0, #0, poll_cmd_complete_done
	/* clear cmd complete */
	mov	w0, #(1 << 0)
	str	w0, [x25, #SDHCI_INT_STATUS]

wait_for_buffer_read_ready:
	ldr	w0, [x25, #SDHCI_INT_STATUS]
	tbz	w0, #5, wait_for_buffer_read_ready
/* clear buffer_read_ready */
	mov	w0, #(1 << 5)
	str	w0, [x25, #SDHCI_INT_STATUS]

	mov	w1, #0x200
read_data_from_buf:
	ldr	w0, [x25, #SDHCI_BUFFER]
	str	w0, [x12]
	sub	w1, w1, #4
	sub	w4, w4, #4
	add	x12, x12, #4
	cbnz	w1, read_data_from_buf
	cbnz	w4, wait_for_buffer_read_ready

poll_data_transfer_done:
	ldr	w0, [x25, #SDHCI_INT_STATUS]
	tbz	w0, #1, poll_data_transfer_done

	/* some error happened, please check SD interrupt status register */
3:	tbnz	w0, #15, 3b
	str	w0, [x25, #SDHCI_INT_STATUS]	/* clear interrupt status */

	/* wait until the card become tran state */
read_status:
	mov	w1,#(1 << 16)
	mov 	w0, #(cmd_13 << 16)
	bl	mmc_send_cmd
	tbz	w0, #8, read_status
	ubfx	w0, w0, #9, #4
	cmp	w0, #4
	b.ne	read_status

	ret	x29

/**
 * w0 - command index and transfer mode register value, and return resp0
 * w1 - argument register value
 * w2 - tmp used
 *
 * x25 - SD controller base addr
 */
mmc_send_cmd:
	str	w1, [x25, #SDHCI_ARGUMENT]
	str	w0, [x25, #SDHCI_TRANSFER_MODE]

4:	ldr	w2, [x25, #SDHCI_INT_STATUS]
	tbz	w2, #0, 4b
	str	w2, [x25, #SDHCI_INT_STATUS]
	tbnz	w2, #15, mmc_send_cmd		/* re-send cmd if any error */
	ldr	w0, [x25, #SDHCI_RESPONSE]
	ret

/**
 * w0 - clock divider
 * w1 - tmp used
 *
 * x25 - SD controller base addr
 */
mmc_set_clk:
	/* set sd PLL post divider */
	str	w0, [x23, SCALER_SD0_OFFSET]
	strh	wzr, [x25, #SDHCI_CLOCK_CONTROL]

	ldr	w1, =(0xe << 16 | SDHCI_CLOCK_INT_EN | SDHCI_CLOCK_CARD_EN)
	str	w1, [x25, #SDHCI_CLOCK_CONTROL]
	/* wait clock stable */
5:	ldr	w1, [x25, #SDHCI_CLOCK_CONTROL]
	tbz	w1, #1, 5b
	ret

/* 0x4f~0x5b alt func 1 */
cfg_sd_pinmux:
	ldr	x0, =(0x20E4010000)
	ldr	w2, =0x0fff8000
	/* register set 2 */
	ldr	w1, [x0, #0x18]
	orr	w1, w1, w2
	str	w1, [x0, #0x18]

	ldr	w1, [x0, #0x1c]
	bic	w1, w1, w2
	str	w1, [x0, #0x1c]

	ldr	w1, [x0, #0x20]
	bic	w1, w1, w2
	str	w1, [x0, #0x20]

	mov	w1, #1
	str	w1, [x0, #0xf0]
	str	wzr, [x0, #0xf0]
	ret


copy_2nd_bst:
#if 0
copy_8k:
	/* first 8k can be reused if the emmc addr from 0 */
	ldr	x0, =FIFO_8K_ADDR_START
	ldr	x1, =FIFO_8K_ADDR_END
	ldr	x2, =AHB_SP0_RAM_BASE

6:	ldp	x3, x4, [x0], #16
	stp	x3, x4, [x2], #16
	cmp	x0, x1
	b.lt	6b

#endif
copy_next_8k:
	bl	emmc_init
	ldr	x12, =(AHB_SP0_RAM_BASE + 0x2000)
	mov	x13, #(1 << 13)//emmc addr 8k
	//ldr x14, =0x1e00//length
	ldr	x14, =0x2000//length
	bl	load_to_sram

jump_to_2nd_stage:
	/* jump to bst1 section to run */
	ldr	x0, =__bst_stage2_ram_start
	br	x0



