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
#include <ambhw/nand.h>
#include <boot_cookie.h>
#else
#include "AmbaBST.h"
#include "AmbaBST_NAND.h"
#include "AmbaBST_SpiNAND.h"
#endif

#define FIO_NON_DMA_MODE	0x00000100
#define NAND_DBG_CONT_OFFSET	0x18
#define FIFO_8K_ADDR_START		0x20e000c000
#define FIFO_8K_ADDR_END		0x20e000e000

.globl copy_2nd_bst

.text
.section .text.first_stage

/*
 * x14 - Nand device address for data to load
 *
 * x24 - POC value
 *
 * x25 - base address of Nand Controller registers
 */
fifo_to_sram:
	ldr	x25, =FIO_BASE

	/* disable all of nand interrupt */
	str	wzr, [x25, #FIO_INT_ENABLE_OFFSET]

	/* setup main size, spare size and FDMA DSM CTRL */
	mov	w0, #(FIO_CTRL_SKIP_BLANK_ECC | FIO_NON_DMA_MODE)

	tbz	w24, #15, 1f			/* check SYS_CONFIG_NAND_ECC_BCH_EN */
	orr	w0, w0, #FIO_CTRL_ECC_BCH_ENABLE
1:	str	w0, [x25, #FIO_CTRL_OFFSET]

	mov	w0, #8448				/* 8K byte (main+spare) */
	tbz	w24, #14, 2f			/* check SYS_CONFIG_SP_2X */
	mov	w0, #8704				/* resize 8K byte (main+spare*2)  */
2:	mov	w1, #(FDMA_CTRL_WRITE_MEM)
	orr	w0, w0, w1
	str	w0, [x25, #FDMA_MN_CTRL_OFFSET]

	ldr	w0, =(NAND_CTRL_P3 | NAND_CTRL_SIZE_8G | NAND_CTRL_WP)
	str	w0, [x25, #NAND_CTRL_OFFSET]	/* Setup NAND Flash Control Register */
#if 0
	/* send spi reset cmd */
	ldr	w0, =(NAND_CC_WORD_CMD1VAL0(0xff) | NAND_CC_WORD_CMD2VAL0(0x00))
	str	w0, [x25, #NAND_CC_WORD_OFFSET]
	str	wzr, [x25, #SPINAND_CC2_OFFSET]
	str	wzr, [x25, #SPINAND_CC1_OFFSET]

	/* Wait for interrupt for operation done */
3:	ldr	w0, [x25, #FIO_RAW_INT_STATUS_OFFSET]
	tbz	w0, #0, 3b			/* check NAND_INT_DI */

	/* Clear raw interrupt status register */
	str	w0, [x25, #FIO_RAW_INT_STATUS_OFFSET]
#endif

spinand_read_cmd:
	/* Write the address of SPINAND. Note: 4 bits LSB should be always 0 */
	mov	w0, w14
	str	w0, [x25, #NAND_CMD_OFFSET]

	/* Setup Read Error Pattern */
	ldr	w0, =SPINAND_ERR_PATTERN
	str	w0, [x25, #SPINAND_ERR_PATTERN_OFFSET]
	ldr	w0, =SPINAND_DONE_PATTERN
	str	w0, [x25, #SPINAND_DONE_PATTERN_OFFSET]

	/* PAGE READ and READ FROM CACHE x2 command */
	ldr	w0, =(NAND_CC_WORD_CMD1VAL0(0x13) | NAND_CC_WORD_CMD2VAL0(0x3b))
	str	w0, [x25, #NAND_CC_WORD_OFFSET]

	ldr	w0, =(SPINAND_LANE_NUM(2) | SPINAND_CC_RW_READ | \
		       SPINAND_CC_ADDR_SRC(2) | SPINAND_CC_ADDR_CYCLE(2) | \
		       SPINAND_CC_DUMMY_DATA_NUM(1) | SPINAND_CC_DATA_SRC_DMA | \
		       SPINAND_CC2_ENABLE)
	str	w0, [x25, #SPINAND_CC2_OFFSET]

	ldr	w0, =(SPINAND_CC_ADDR_SRC(1) | SPINAND_CC_ADDR_CYCLE(3) | \
		       SPINAND_CC_DATA_SRC_DMA | SPINAND_CC_AUTO_STSCHK)
	str	w0,[x25, #SPINAND_CC1_OFFSET]

	/* Wait for interrupt for operation done */
4:	ldr	w0, [x25, #FIO_RAW_INT_STATUS_OFFSET]
	tbz	w0, #0, 4b			/* check NAND_INT_DI */

	/* Clear raw interrupt status register */
	str	w0, [x25, #FIO_RAW_INT_STATUS_OFFSET]

	//ldr	w13, [x25, #NAND_DBG_CONT_OFFSET]

	/* enable random read mode */
	mov	w0, #0x82
	str	w0, [x25, #FIO_CTRL_OFFSET]
	ret

/*
 * 0x20e0030000-0x20e0033c00 15k used to run bst
 * 0x20e0033c00-0x0x20e0033fff 1k used to read bst from nand
 */
copy_2nd_bst:
#if 0
copy_8k:
	/* first 8k can be reused if the nand addr from 0 */
	//mov	w14, wzr
	//bl	fifo_to_sram
	ldr	x0, =FIFO_8K_ADDR_START
	ldr	x1, =FIFO_8K_ADDR_END
	ldr	x2, =AHB_SP0_RAM_BASE

5:	ldp	x3, x4, [x0], #16
	stp	x3, x4, [x2], #16
	cmp	x0, x1
	b.lt	5b

	/* relocate copy_next_7k address, then jump to copy_next_7k */
	ldr	x0, =copy_next_7k
	br	x0

#endif
copy_next_8k:
	mov	w14, #0x2000//8k
	bl	fifo_to_sram
	ldr	x0, =FIFO_8K_ADDR_START
	//ldr     x1, =(FIFO_8K_ADDR_START + 0x1f80)
	ldr	x1, =FIFO_8K_ADDR_END
	ldr	x2, =(AHB_SP0_RAM_BASE + 0x2000)

6:	ldp	x3, x4, [x0], #16
	stp	x3, x4, [x2], #16
	cmp	x0, x1
	b.lt	6b

jump_to_2nd_stage:
	/* jump to bst1 section to run */
	ldr	x0, =__bst_stage2_ram_start
	br	x0

