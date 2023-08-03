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
#if SH_IPCAM_SDK
#include <ambhw/scratchpad.h>
#include <ambhw/spinor.h>
#include <boot_cookie.h>
#else
#include "AmbaBST.h"
/* Copy from ambhw/spinor.h */
#define SPINOR_LENGTH_OFFSET		0x00
#define SPINOR_CTRL_OFFSET		0x04
#define SPINOR_CFG_OFFSET		0x08
#define SPINOR_CMD_OFFSET		0x0c
#define SPINOR_ADDRHI_OFFSET		0x10
#define SPINOR_ADDRLO_OFFSET		0x14
#define SPINOR_DMACTRL_OFFSET		0x18
#define SPINOR_TXFIFOTHLV_OFFSET	0x1c
#define SPINOR_RXFIFOTHLV_OFFSET	0x20
#define SPINOR_TXFIFOLV_OFFSET		0x24
#define SPINOR_RXFIFOLV_OFFSET		0x28
#define SPINOR_FIFOSTA_OFFSET		0x2c
#define SPINOR_INTRMASK_OFFSET		0x30
#define SPINOR_INTR_OFFSET		0x34
#define SPINOR_RAWINTR_OFFSET		0x38
#define SPINOR_CLRINTR_OFFSET		0x3c
#define SPINOR_TXFIFORST_OFFSET		0x40
#define SPINOR_RXFIFORST_OFFSET		0x44
#define SPINOR_START_OFFSET		0x50
#define SPINOR_TXDATA_OFFSET		0x100
#define SPINOR_RXDATA_OFFSET		0x200
/* SPINOR_LENGTH_REG */
#define SPINOR_LENGTH_CMD(x)            (((x) & 0x3) << 30)
#define SPINOR_LENGTH_ADDR(x)           (((x) & 0x7) << 27)
#define SPINOR_LENGTH_DUMMY(x)          (((x) & 0x1f) << 22)                                                     
/* SPINOR_CTRL_REG */
#define SPINOR_CTRL_ADDRDTR		0x40000000
#define SPINOR_CTRL_DATADTR		0x10000000
#define SPINOR_CTRL_CMD1LANE		0x00000000
#define SPINOR_CTRL_CMD8LANE		0x0000c000
#define SPINOR_CTRL_ADDR1LANE		0x00000000
#define SPINOR_CTRL_ADDR8LANE		0x00003000
#define SPINOR_CTRL_DATA2LANE		0x00000400
#define SPINOR_CTRL_DATA8LANE		0x00000c00
#define SPINOR_CTRL_RXLANE_TXRX		0x00000200
#define SPINOR_CTRL_RDEN		0x00000001
/* SPINOR_INTRMASK_REG, SPINOR_INTR_REG, SPINOR_RAWINTR_REG, SPINOR_CLRINTR_REG */
#define SPINOR_INTR_TXUNDERFLOW		0x00000040
#define SPINOR_INTR_DATALENREACH	0x00000020
#define SPINOR_INTR_RXALMOSTFULL	0x00000010
#define SPINOR_INTR_RXOVERFLOW		0x00000008
#define SPINOR_INTR_RXUNDERFLOW		0x00000004
#define SPINOR_INTR_TXOVERFLOW		0x00000002
#define SPINOR_INTR_TXALMOSTEMPTY	0x00000001
#define SPINOR_INTR_ALL			SPINOR_INTR_TXUNDERFLOW | \
					SPINOR_INTR_DATALENREACH | \
					SPINOR_INTR_RXALMOSTFULL | \
					SPINOR_INTR_RXOVERFLOW | \
					SPINOR_INTR_RXUNDERFLOW | \
					SPINOR_INTR_TXOVERFLOW | \
					SPINOR_INTR_TXALMOSTEMPTY
.text
.section .text.first_stage
spinor_misc_param:		.int 0x00000000
#endif

#define NOR_OCTA_DDR_READ_CMD		0x8B
#define NOR_READ_CMD				0x03
#define NOR_OCTA_DDR_BOOT			0x00004000
#define BST_HEADER_SIZE             (128)

.globl copy_2nd_bst

.text
.section .text.first_stage

/**
 * Initialize the SPINOR controller and load the BLD/ATF binary into the dram,
 * and then jump to it.
 *
 * Garbles [x0 - x2, x4]
 *
 * x12 - DRAM address for data to load
 * x13 - SPINOR device address for data to load
 * x14 - Length of data need to load
 *
 * x15 - Used in PTB and Recovery sub-routines
 * x16 - Used in PTB and Recovery sub-routines
 * x17 - Used in PTB and Recovery sub-routines
 *
 * x19 - spinor control register setting
 * x20 - Length register setting
 * x21 - Read command, READ or READ4B
 *
 * x22 - DRAM address of next binary to jump
 *
 * x24 - system POC value
 *
 * x25 - base address of SPINOR Controller registers
 * x26 - base address of boot cookie
 *
 */

nor_init:
	mov	x29, x30

	ldr	x25, =SPINOR_BASE

	tst w24, #(NOR_OCTA_DDR_BOOT)

	mov	w0, #(SPINOR_LENGTH_CMD(1) | SPINOR_LENGTH_ADDR(3))
	mov	w1, #(SPINOR_LENGTH_CMD(1) | SPINOR_LENGTH_ADDR(4) | SPINOR_LENGTH_DUMMY(16))
	csel w20, w0, w1, eq

	mov	w0, #(NOR_READ_CMD)				/* Read Command */
	mov	w1, #(NOR_OCTA_DDR_READ_CMD)	/* DDR 0x8b Read Command */
	csel w21, w0, w1, eq

	mov	w0, #(SPINOR_CTRL_CMD1LANE | SPINOR_CTRL_ADDR1LANE | SPINOR_CTRL_DATA2LANE | \
			SPINOR_CTRL_RXLANE_TXRX | SPINOR_CTRL_RDEN)

	ldr	w1, =(SPINOR_CTRL_ADDRDTR | SPINOR_CTRL_DATADTR | SPINOR_CTRL_CMD8LANE | \
			SPINOR_CTRL_ADDR8LANE | SPINOR_CTRL_DATA8LANE | SPINOR_CTRL_RDEN)
	csel w19, w0, w1, eq

	/*
	 * Enter 4B address mode if enabled for address accessed >= 16MB
	 *
	 * Note:
	 *   Normally there are 3 ways to enter 4B address mode:
	 *   1) Config register inside spinor device
	 *   2) Use "Enter 4B mode" command
	 *   3) Use "Enter 4B mode" command with "Write Enable" together
	 *
	 * PS:
	 *   Some spinor device no need to switch to 4B address mode obviously,
	 *   since their 4B mode is determined by the command id automatically.
	 *   You need to check spinor datasheet to know details.
	 */
	ldr	w5, spinor_misc_param
	cbz	w5, load_bld

	mov	w20, #(SPINOR_LENGTH_CMD(1) | SPINOR_LENGTH_ADDR(4))
	mov	w21, #0x13			/* Read4B Command */
set_4byte:
	tbz	w5, #0, 1f
	mov	w0, #0x17			/* Bank register write */
	mov	w1, #0x80
	bl	spinor_send_cmd

1:	tbz	w5, #2, 2f
	mov	w0, #0x06			/* Write Enable Command */
	mov	w1, #0x80000000			/* Flag indicating no address */
	bl	spinor_send_cmd

2:	tbz	w5, #1, load_bld
	mov	w0, #0xb7			/* Enter 4B mode Command */
	mov	w1, #0x80000000			/* Flag indicating no address */
	bl	spinor_send_cmd

3:	tbz	w5, #2, load_bld
	mov	w0, #0x04			/* Write Disable Command */
	mov	w1, #0x80000000			/* Flag indicating no address */
	bl	spinor_send_cmd

load_bld:
	ret	x29

/**
 *
 * Garbles [x0 - x1]
 *
 * x4 - Length of data loaded
 *
 * x12 - DRAM address for data to load
 * x13 - SPINOR device address for data to load
 * x14 - Length of data need to load
 *
 * x20 - Length register setting
 * x21 - Read command, READ or READ4B
 *
 * x25 - SPINOR controller base addr
 */
load_to_sram:
	mov	x29, x30

	/* clear all interrupts */
	mov	w0, #SPINOR_INTR_ALL
	str	w0, [x25, #SPINOR_CLRINTR_OFFSET]

	/* reset tx/rx fifo */
	mov	w0, #1
	str	w0, [x25, #SPINOR_TXFIFORST_OFFSET]
	str	w0, [x25, #SPINOR_RXFIFORST_OFFSET]

	/* set tx/rx fifo threshhold level to blk - 1 */
	mov	w0, #31
	str	w0, [x25, #SPINOR_RXFIFOTHLV_OFFSET]
	str	w0, [x25, #SPINOR_TXFIFOTHLV_OFFSET]

	orr	w0, w20, w14
	str	w0, [x25, #SPINOR_LENGTH_OFFSET]

	str	w19, [x25, #SPINOR_CTRL_OFFSET]

	str	w21, [x25, #SPINOR_CMD_OFFSET]

	str	wzr, [x25, #SPINOR_ADDRHI_OFFSET]
	str	w13, [x25, #SPINOR_ADDRLO_OFFSET]

	/* start transfer */
	mov	w0, #0x1
	str	w0, [x25, #SPINOR_START_OFFSET]

	add	x4, x12, x14			/* end address of BLD */
	add	x13, x13, x14			/* update media addr to load more */
	/* now read data from FIFO one by one */
1:	ldr	w0, [x25, #SPINOR_RXFIFOLV_OFFSET]
	cbz	w0, 1b
	ldrb	w1, [x25, #SPINOR_RXDATA_OFFSET]
	strb	w1, [x12], #1
	cmp	x12, x4
	blo	1b

	ret	x29


/**
 * Garbles [x2 - x3]
 *
 * x0 - command index
 * x1 - address
 *
 * x25 - SD controller base addr
 */
spinor_send_cmd:
	/* mask all interrupts */
	mov	w2, #SPINOR_INTR_ALL
	str	w2, [x25, #SPINOR_CLRINTR_OFFSET]

	mov	w2, #SPINOR_LENGTH_CMD(1)
	mov	w3, #SPINOR_CTRL_CMD1LANE
	tbnz	w1, #31, 1f			/* Branch if no address */
	mov	w2, #(SPINOR_LENGTH_CMD(1) | SPINOR_LENGTH_ADDR(1))
	mov	w3, #(SPINOR_CTRL_CMD1LANE | SPINOR_CTRL_ADDR1LANE)
1:	str	w2, [x25, #SPINOR_LENGTH_OFFSET]
	str	w3, [x25, #SPINOR_CTRL_OFFSET]

	str	w0, [x25, #SPINOR_CMD_OFFSET]

	str	wzr, [x25, #SPINOR_ADDRHI_OFFSET]
	str	w1, [x25, #SPINOR_ADDRLO_OFFSET]

	/* start transfer */
	mov	w0, #0x1
	str	w0, [x25, #SPINOR_START_OFFSET]

2:	ldr	w2, [x25, #SPINOR_RAWINTR_OFFSET]
	tbz	w2, #5, 2b

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
	bl	nor_init
	ldr	x12, =(AHB_SP0_RAM_BASE + 0x2000)
	mov	x13, #(1 << 13) //NOR addr 8k
	add	x13, x13, #BST_HEADER_SIZE	/* BST NOR has a header */
	//ldr	x14, =0x1e00//length
	ldr	x14, =0x2000    //length
	bl	load_to_sram

jump_to_2nd_stage:
	/* jump to bst1 section to run */
	ldr	x0, =__bst_stage2_ram_start
	br	x0



