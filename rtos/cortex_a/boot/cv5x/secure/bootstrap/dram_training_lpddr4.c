/**
 * History:
 *    2021/10/29 - [Cao Rongrong]
 *
 * Copyright (c) 2020 Ambarella International LP
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
#include <basedef.h>
#include <ambhw/drctl.h>
#include <ambhw/rct.h>
#endif
#include "AmbaBST.h"
#include "AmbaTypes.h"
typedef UINT32      u32;
typedef UINT64      u64;
#include "uartp.h"
#define DRAMC_DRAM_BASE                 AMBA_CORTEX_A76_DRAM_CTRL_BASE_ADDR
#define DRAMC_DDRC_BASE                 AMBA_CORTEX_A76_DDR_CTRL0_BASE_ADDR
#define DRAM_REG(x)                     (DRAMC_DRAM_BASE + (x))
#define DDRC_REG(x, y)                  (DRAMC_DDRC_BASE + (x) * 0x1000 + (y))
#define writel(addr,value)              (*(volatile UINT32*)addr = value)
#define readl(addr)                     (*(volatile UINT32*)addr)
#define setbitsl(p, mask)               writel((p),(readl(p) | (mask)))
#define clrbitsl(p, mask)               writel((p),(readl(p) & ~(mask)))
#define updbitsl(p, mask, v)            writel((p), (readl(p) & ~(mask)) | (v))
#define BIT(x)                          (1 << (x))
#define min(x, y)                       ((x) < (y) ? (x) : (y))
#define max(x, y)                       ((x) > (y) ? (x) : (y))
#define CLIP(a, max, min)               ((a) > (max)) ? (max) : (((a) < (min)) ? (min) : (a))
#define asm                             __asm__
#define volatile                        __volatile__
#define dram_mode_param                 DRAMC_ModeParam
#define dram_config1_param              DRAM_Config1Param
#define dram_config2_param              DRAM_Config2Param


extern u32 training_param;
extern u32 dram_mode_param;
extern u32 dram_config1_param;
extern u32 dram_config2_param;
extern u32 lp5_mode_reg18_param;


#define CONFIG_AMBST_DRAM_TRAINING_SHOW_RESULT
//#define CONFIG_AMBST_DRAM_TRAINING_SHOW_VREF_RANGE
//#define CONFIG_AMBST_DRAM_TRAINING_SHOW_DELAY_RANGE

/* B0: [0..7]   dq, [32] dm, [36] all */
/* B1: [8..15]  dq, [33] dm, [37] all */
/* B2: [16..23] dq, [34] dm, [38] all */
/* B3: [24..31] dq, [35] dm, [39] all */
#define IDXA(bid)	((bid) + 36)		/* index for ALL */
#define IDXM(bid)	((bid) + 32)		/* index for DM */
#define IDXD(bid, i)	(((bid) << 3) + (i))	/* index for DQ each bit */

static u32 best_vref_max[40];
static u32 best_vref_min[40];
static u32 best_dly_start[40];
static u32 best_dly_end[40];
static u32 max_vref_die0[4];
static u32 min_vref_die0[4];

extern void rct_timer_delay(u32 delay_cnt);

static void ddrc_uinst(u32 ddrc, u32 uinst1, u32 uinst2, u32 uinst4)
{
	writel(DDRC_REG(ddrc, DRAM_UINST1), uinst1);
	writel(DDRC_REG(ddrc, DRAM_UINST2), uinst2);
	writel(DDRC_REG(ddrc, DRAM_UINST4), uinst4);

	writel(DDRC_REG(ddrc, DRAM_UINST5), 0x1);
	while (readl(DDRC_REG(ddrc, DRAM_UINST5)) & 0x1);
}

static void ddrc_mrw(u32 ddrc, u32 addr, u32 data, u32 did, u32 cid)
{
	/* cid/did: 0 - Die0/ChA, 1 - Die1/ChB, 2 - Both Die/Channel */
	writel(DDRC_REG(ddrc, DRAM_MODE_REG), BIT(31) | BIT(24) |
		((cid + 1) << 27) | ((did + 1) << 25) | (addr << 16) | data);
	while (readl(DDRC_REG(ddrc, DRAM_MODE_REG)) & BIT(31));
}

#if defined(CONFIG_AMBST_DRAM_TRAINING_SHOW_RESULT)
static u32 ddrc_mrr(u32 ddrc, u32 addr, u32 did, u32 cid)
{
	u32 data;

	/* cid/did: 0 - Die0/ChA, 1 - Die1/ChB, 2 - Both Die/Channel */
	writel(DDRC_REG(ddrc, DRAM_MODE_REG),
		BIT(31) | ((cid + 1) << 27) | ((did + 1) << 25) | (addr << 16));
	while (readl(DDRC_REG(ddrc, DRAM_MODE_REG)) & BIT(31));

	data = readl(DDRC_REG(ddrc, DRAM_MODE_REG));
	data >>= (cid == 1) ? 8 : 0;
	data &= (cid == 2) ? 0xffff : 0xff;

	return data;
}

static void show_training_result(u32 ddrc)
{
	u32 bytes = (dram_config1_param & BIT(6)) ? 2 : 4, bid;
	u32 ranks = (dram_config1_param & BIT(26)) ? 2 : 1, did;
	u32 i, temp, vref[4], dly[6];

	bst_uart_putstr("\n");

	temp = readl(DDRC_REG(ddrc, DDRIO_VREF_0));
	vref[0] = temp & 0x3f;
	vref[1] = (temp >> 16) & 0x3f;
	temp = readl(DDRC_REG(ddrc, DDRIO_VREF_1));
	vref[2] = temp & 0x3f;
	vref[3] = (temp >> 16) & 0x3f;

	/* wrdly coarse, wrdly fine,  dram DQ vref, rddly coarse, rddly fine, rddly fine */
	for(did = 0; did < ranks; did++) {
		for (bid = 0; bid < bytes; bid++) {
			for(i = 0; i < 6; i++)
				dly[i] = readl(DDRC_REG(ddrc, DDRIO_BYTE_DLY0(bid, did) + i * 4));

			bst_uart_putchar('C');
			bst_uart_puthex(ddrc, 1);
			bst_uart_putchar('D');
			bst_uart_puthex(did, 1);
			bst_uart_putchar('B');
			bst_uart_puthex(bid, 1);

			bst_uart_putstr(": | R - ");
			/* dq rddly coarse */
			bst_uart_puthex((dly[1] >> 15) & 0x1f, 2);
			/* dq rddly fine */
			bst_uart_putstr(" [");
			for (i = 0; i < 4; i++) {
				bst_uart_puthex(dly[4] >> (i << 3), 2);
				bst_uart_putchar(',');
			}
			for (i = 0; i < 4; i++) {
				bst_uart_puthex(dly[5] >> (i << 3), 2);
				bst_uart_putchar(',');
			}
			/* dm rddly fine */
			bst_uart_puthex((dly[1] >> 25) & 0x1f, 2);
			bst_uart_putstr("] ");
			/* ddrc vref */
			bst_uart_puthex(vref[bid], 2);

			bst_uart_putstr(" | W - ");
			/* dq wrdly coarse */
			bst_uart_puthex((dly[1] >> 10) & 0x1f, 2);
			/* dq wrdly fine */
			bst_uart_putstr(" [");
			for (i = 0; i < 4; i++) {
				bst_uart_puthex(dly[2] >> (i << 3), 2);
				bst_uart_putchar(',');
			}
			for (i = 0; i < 4; i++) {
				bst_uart_puthex(dly[3] >> (i << 3), 2);
				bst_uart_putchar(',');
			}
			/* dm wrdly fine */
			bst_uart_puthex((dly[1] >> 20) & 0x1f, 2);
			bst_uart_putstr("] ");
			/* dram DQ vref */
			bst_uart_puthex(ddrc_mrr(ddrc, 14, did, bid >> 1), 2);//differ LP5

			bst_uart_putstr(" | P - ");
			/* dqs wrdly coarse */
			bst_uart_puthex((dly[0] >> 15) & 0x1f, 2);
			/* dqs wrdly fine */
			bst_uart_putstr(" [");
			bst_uart_puthex((dly[0] >> 10) & 0x1f, 2);
			bst_uart_putstr("]");

			bst_uart_putstr("\n");
		}
	}
}
#else
    #define show_training_result(ddrc)
#endif

#if defined(CONFIG_AMBST_DRAM_TRAINING_SHOW_VREF_RANGE)
static void show_training_vref_range(u32 ddrc, u32 did, u32 delay, u32 id)
{
	u32 i, bid, idx;// idx_min, range_min, idx_max;// range_max;

	if (delay < 32)
		return;

	bst_uart_putchar('\n');
	for (bid = 0; bid < 4; bid++) {
		bst_uart_putstr("\nC");
		bst_uart_puthex(ddrc, 1);
		bst_uart_putchar('D');
		bst_uart_puthex(did, 1);
		bst_uart_putchar('B');
		bst_uart_puthex(bid, 1);
		bst_uart_putchar(id == 2 ? 'P' : id ? 'W' : 'R');
		bst_uart_putstr(": ");

		for (i = 0; i < 9; i++) {
			idx = (i == 8) ? IDXM(bid) : IDXD(bid, i);

			bst_uart_putstr("[");
			bst_uart_puthex(best_vref_max[idx], 2);
			bst_uart_putchar(',');
			bst_uart_puthex(best_vref_min[idx], 2);
			bst_uart_putstr("] ");
		}
	}
}
#else
    #define show_training_vref_range(ddrc, did, delay, id)
#endif

#if defined(CONFIG_AMBST_DRAM_TRAINING_SHOW_DELAY_RANGE)
static void show_training_delay_range(u32 ddrc, u32 did, u32 id)
{
	u32 i, bid, idx, dly_c, dly_c_reg;

	bst_uart_putchar('\n');
	for (bid = 0; bid < 4; bid++) {
		bst_uart_putstr("\nC");
		bst_uart_puthex(ddrc, 1);
		bst_uart_putchar('D');
		bst_uart_puthex(did, 1);
		bst_uart_putchar('B');
		bst_uart_puthex(bid, 1);
		bst_uart_putchar(id == 2 ? 'P' : id ? 'W' : 'R');
		bst_uart_putstr(": ");

		dly_c_reg = (id == 2) ? DDRIO_BYTE_DLY0(bid, did) : DDRIO_BYTE_DLY1(bid, did);
		dly_c = (readl(DDRC_REG(ddrc, dly_c_reg)) >> ((id == 1) ? 10 : 15)) & 0x1f;
		bst_uart_puthex(dly_c, 2);

		for (i = 0; i < 9; i++) {
			idx = (i == 8) ? IDXM(bid) : IDXD(bid, i);
			bst_uart_putstr(" [");
			bst_uart_puthex(best_dly_start[idx], 2);
			bst_uart_putchar(',');
			bst_uart_puthex(best_dly_end[idx], 2);
			bst_uart_putstr("]");
		}

		bst_uart_putstr(" {");
		bst_uart_puthex(best_dly_start[IDXA(bid)], 2);
		bst_uart_putchar(',');
		bst_uart_puthex(best_dly_end[IDXA(bid)], 2);
		bst_uart_putchar(',');
		bst_uart_puthex((best_dly_start[IDXA(bid)] + best_dly_end[IDXA(bid)]) >> 1, 2);
		bst_uart_putstr("}");
	}
}
#else
    #define show_training_delay_range(ddrc, did, id)
#endif

static void set_read_dq_delay(u32 ddrc, u32 did, u32 value)
{
	u32 bid, dly = (value << 24) | (value << 16) | (value << 8) | value;

	for (bid = 0; bid < 4; bid++) {
		updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY1(bid, did)), 0x1f << 25, value << 25);
		writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY4(bid, did)), dly);
		writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY5(bid, did)), dly);
	}
}

static void set_write_dq_delay(u32 ddrc, u32 did, u32 value)
{
	u32 bid, dly = (value << 24) | (value << 16) | (value << 8) | value;

	for (bid = 0; bid < 4; bid++) {
		updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY1(bid, did)), 0x1f << 20, value << 20);
		writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY2(bid, did)), dly);
		writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY3(bid, did)), dly);
	}
}

static void set_read_dq_vref(u32 ddrc, u32 value)
{
	updbitsl(DDRC_REG(ddrc, DDRIO_VREF_0), 0x003f003f, (value << 16) | value);
	updbitsl(DDRC_REG(ddrc, DDRIO_VREF_1), 0x003f003f, (value << 16) | value);
}

static void set_write_dq_vref(u32 ddrc, u32 did, u32 value)
{
	// differ LP5 whole function
	/*
	 * range0: [0-50]
	 * range1: [21-50]
	 */
	if (value > 50)
		value = ((value - 30) | 0x40);

	ddrc_mrw(ddrc, 14, value, did, 2);

	/* Also update ddrc WRITE_VREF register */
	value |= (value << 8);
	value |= (value << 16);
	writel(DDRC_REG(ddrc, did ? DRAM_WRITE_VREF_1 : DRAM_WRITE_VREF_0), value);
}

static void clear_vars(void)
{
	for (u32 i = 0; i < 40; i++) {
		best_vref_max[i] = 0;
		best_vref_min[i] = 0;
	}
}

static void update_best(u32 bit, u32 vref_start, u32 vref_end, u32 delay)
{
	if (vref_start - vref_end >= best_vref_max[bit] - best_vref_min[bit]) {
		if (vref_start - vref_end > best_vref_max[bit] - best_vref_min[bit])
			best_dly_start[bit] = delay;
		best_dly_end[bit] = delay;

		best_vref_max[bit] = vref_start;
		best_vref_min[bit] = vref_end;
	}
}

#define DQ_CALIB_PATTERN	0x3c5a

static u32 data_pattern[9] = {
	0xaaaa5555, 0xaaaa5555, 0x5555aaaa, 0x5555aaaa,
	0x55555555, 0xaaaaaaaa, 0xaaaaaaaa, 0x55555555,
	0x0ff033cc,
};

static u64 check_data_pattern(u32 ddrc, u32 did, u32 opcode)
{
	u32 wr_data, rd_data, rsltcha = 0, rsltchb = 0, loop, i;
	u64 fail = 0;

	for (loop = 0; loop < 2; loop++) {
		ddrc_mrw(ddrc, 15, (loop % 2) ? 0xaa : 0x55, did, 2);//differ LP5
		ddrc_mrw(ddrc, 20, (loop % 2) ? 0xaa : 0x55, did, 2);//differ LP5

		for (i = 0; i < 9; i++) {
			wr_data = ((loop % 2) && (i < 8)) ? ~data_pattern[i] : data_pattern[i];
			writel(DDRC_REG(ddrc, DRAM_MPC_WDATA + (i << 2)), wr_data);
		}

		ddrc_uinst(ddrc, opcode, 0x24, (0x3 << 8) | (1 << did));
		/* Reset the fifo pointers */
		ddrc_uinst(ddrc, 0x12, 0x0, (0x3 << 8) | (1 << did));

		/* Check DQ */
		for (i = 0; i < 0x20; i += 4) {
			wr_data = readl(DDRC_REG(ddrc, DRAM_MPC_WDATA + i));

			rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RDATA + 0x00 + i));
			rsltcha |= rd_data ^ wr_data;

			rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RDATA + 0x20 + i));
			rsltchb |= rd_data ^ wr_data;
		}

		fail |= ((rsltcha >> 16) | rsltcha) & 0xffff;
		fail |= (((rsltchb >> 16) | rsltchb) & 0xffff) << 16;

		/* Check DM */
		wr_data = readl(DDRC_REG(ddrc, DRAM_MPC_WMASK));

		rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RMASK + 0x00));
		if ((rd_data ^ wr_data) & 0x55555555)
			fail |= 1ULL << 32;
		if ((rd_data ^ wr_data) & 0xaaaaaaaa)
			fail |= 1ULL << 33;

		rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RMASK + 0x04));
		if ((rd_data ^ wr_data) & 0x55555555)
			fail |= 1ULL << 34;
		if ((rd_data ^ wr_data) & 0xaaaaaaaa)
			fail |= 1ULL << 35;

		fail |= (fail & 0x01000000ff) ? 1ULL << 36 : 0;
		fail |= (fail & 0x020000ff00) ? 1ULL << 37 : 0;
		fail |= (fail & 0x0400ff0000) ? 1ULL << 38 : 0;
		fail |= (fail & 0x08ff000000) ? 1ULL << 39 : 0;
	}

	return fail;
}

static void training_vref_inner(u32 ddrc, u32 did, u32 bytes, u32 delay, u32 write)
{
	int vref_start[40], vref_end[40], vref_training, opcode, bid, idx, i;
	u64 fail;

	if (write) {
		vref_training = 79;//differ from LP5
		opcode = 0xb;
	} else {
		vref_training = 63;
		opcode = 0xa;
	}

	for (i = 0; i < 40; i++) {
		vref_start[i] = 0;
		vref_end[i] = 0;
	}

	while (vref_training > 0) {
		if (write)
			set_write_dq_vref(ddrc, did, vref_training);
		else
			set_read_dq_vref(ddrc, vref_training);

		rct_timer_delay(10); /* delay about 400ns */

		fail = check_data_pattern(ddrc, did, opcode);

		for (bid = 0; bid < 4; bid++) {
			if ((bytes & (0x1 << bid)) == 0)
				continue;

			for (i = 0; i < 10; i++) {
				idx = (i == 9) ? IDXA(bid) : (i == 8) ? IDXM(bid) : IDXD(bid, i);

				if (fail & (1ULL << idx)) {
					vref_start[idx] = 0;
					vref_end[idx] = 0;
				} else {
					if (vref_start[idx] == 0)
						vref_start[idx] = vref_training;
					vref_end[idx] = vref_training;

					/* update the best value for dly/dll and vref */
					update_best(idx, vref_start[idx], vref_end[idx], delay);
				}
			}
		}

		vref_training -= 2;
	}

	show_training_vref_range(ddrc, did, delay, write);
}

static int training_delay_update(u32 ddrc, u32 did, u32 id, u32 retry, int offset)
{
	int bid, dly_c, dly_c_new, dly_cp, reg, offs, index;

	show_training_delay_range(ddrc, did, id);

	for (bid = 0; bid < 4; bid++) {
		switch (id) {
		case 2:	/* wdqs */
			reg = DDRIO_BYTE_DLY0(bid, did);
			index = IDXM(bid);
			offs = 15;
			break;
		case 1:	/* wdq */
			reg = DDRIO_BYTE_DLY1(bid, did);
			index = IDXA(bid);
			offs = 10;
			break;
		case 0:	/* rdq */
        default:
			reg = DDRIO_BYTE_DLY1(bid, did);
			index = IDXA(bid);
			offs = 15;
			break;
		}

		dly_c = dly_c_new = (readl(DDRC_REG(ddrc, reg)) >> offs) & 0x1f;
		dly_cp = (best_dly_start[index] + best_dly_end[index]) >> 1;

		if (dly_cp + offset > 23)
			dly_c_new = min(dly_c + 1, 31);
		else if (dly_cp + offset < 8)
			dly_c_new = max(dly_c - 1, 0);

		if (dly_c_new == dly_c) {
			retry &= ~(0x1 << bid);
			continue;
		}

		updbitsl(DDRC_REG(ddrc, reg), 0x1f << offs, dly_c_new << offs);
	}

	return retry;
}

static void training_delay_done(u32 ddrc, u32 did, u32 id, int offset)
{
	int bid, i, dly_f0, dly_f1, dly_dm, dly_best;

	for (bid = 0; bid < 4; bid++) {
		dly_f0 = dly_f1 = 0;

		for (i = 0; i < 8; i++) {
			dly_best = (best_dly_start[IDXD(bid,i)] + best_dly_end[IDXD(bid,i)]) >> 1;
			dly_best = CLIP(dly_best + offset, 31, 0);
			if (i < 4)
				dly_f0 |= dly_best << (i << 3);
			else
				dly_f1 |= dly_best << ((i - 4) << 3);
		}

		dly_best = (best_dly_start[IDXM(bid)] + best_dly_end[IDXM(bid)]) >> 1;
		dly_dm = CLIP(dly_best + offset, 31, 0);

		switch (id) {
		case 1:
			updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY1(bid, did)), 0x1f << 20, dly_dm << 20);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY2(bid, did)), dly_f0);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY3(bid, did)), dly_f1);
			break;
		case 0:
        default:
			updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY1(bid, did)), 0x1f << 25, dly_dm << 25);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY4(bid, did)), dly_f0);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY5(bid, did)), dly_f1);
			break;
		}
	}
}

static void training_read_delay(u32 ddrc, u32 did)
{
	int dly_training, retry = 0xf, offset;

	/* extract and extend offset to signed value */
	asm volatile("sbfx %0, %1, #12, #4\n\t" : "=&r"(offset) : "r"(training_param));

	while (retry) {
		clear_vars();

		dly_training = 32;
		while (dly_training-- > 0) {
			set_read_dq_delay(ddrc, did, dly_training);
			training_vref_inner(ddrc, did, retry, dly_training, 0);
		}

		retry = training_delay_update(ddrc, did, 0, retry, offset);
	}

	training_delay_done(ddrc, did, 0, offset);
}

static void training_read_vref(u32 ddrc, u32 did)
{
	u32 best_vref, offset, bid, vref_reg, vref_offs;

	clear_vars();

	training_vref_inner(ddrc, did, 0xf, 0xbb, 0);

	for (bid = 0; bid < 4; bid++) {
		if (did == 0) {
			max_vref_die0[bid] = best_vref_max[IDXA(bid)];
			min_vref_die0[bid] = best_vref_min[IDXA(bid)];
		} else {
			if (best_vref_max[IDXA(bid)] > max_vref_die0[bid])
				best_vref_max[IDXA(bid)] = max_vref_die0[bid];
			if (best_vref_min[IDXA(bid)] < min_vref_die0[bid])
				best_vref_min[IDXA(bid)] = min_vref_die0[bid];
		}

		best_vref = (best_vref_max[IDXA(bid)] + best_vref_min[IDXA(bid)]) >> 1;

		/* adjusted by offset */
		__asm__ __volatile__("sbfx %0, %2, #24, #8\n\t"
				     "add %1, %1, %0\n\t"
				     : "=&r"(offset), "+r"(best_vref)
				     : "r"(training_param));

		vref_reg = (bid / 2) ? DDRIO_VREF_1 : DDRIO_VREF_0;
		vref_offs = (bid % 2) ? 16 : 0;
		updbitsl(DDRC_REG(ddrc, vref_reg), 0x3f << vref_offs, best_vref << vref_offs);
	}
}

static void training_read_rtt(u32 ddrc, u32 did)
{
	writel(DDRC_REG(ddrc, DRAM_CONFIG2), dram_config2_param);
	ddrc_uinst(ddrc, 0x12, 0x0, (0x3 << 8) | (1 << did)); /* Reset fifo pointers */
	updbitsl(DDRC_REG(ddrc, DRAM_CONFIG2), 0x3 << 11, 0x1 << 11);

	/* update RTT (Round-Trip-Time) */
	writel(DDRC_REG(ddrc, DRAM_INIT_CTL), DRAM_INIT_CTL_GET_RTT_EN | (did << 10));
	while(readl(DDRC_REG(ddrc, DRAM_INIT_CTL)) & DRAM_INIT_CTL_GET_RTT_EN);

	/* Reset the fifo pointers */
	ddrc_uinst(ddrc, 0x12, 0x0, (0x3 << 8) | (1 << did));
}

static void training_write_delay(u32 ddrc, u32 did)
{
	int dly_training, retry = 0xf, offset;

	/* extract and extend offset to signed value */
	asm volatile("sbfx %0, %1, #8, #4\n\t" : "=&r"(offset) : "r"(training_param));

	while (retry) {
		clear_vars();

		dly_training = 32;
		while (dly_training-- > 0) {
			set_write_dq_delay(ddrc, did, dly_training);
			training_vref_inner(ddrc, did, retry, dly_training, 1);
		}

		retry = training_delay_update(ddrc, did, 1, retry, offset);
	}

	training_delay_done(ddrc, did, 1, offset);
}

static void training_write_vref(u32 ddrc, u32 did)
{
	u32 best_vref, offset, bid;

	clear_vars();

	training_vref_inner(ddrc, did, 0xf, 0xcc, 1);

	for (bid = 0; bid < 4; bid++) {
		best_vref = (best_vref_max[IDXA(bid)] + best_vref_min[IDXA(bid)]) >> 1;

		/* adjusted by offset */
		__asm__ __volatile__("sbfx %0, %2, #16, #8\n\t"
				     "add %1, %1, %0\n\t"
				     : "=&r"(offset), "+r"(best_vref)
				     : "r"(training_param));

		ddrc_mrw(ddrc, 14, best_vref, did, bid >> 1);//differ LP5
	}
}

static void training_read_preamble(u32 ddrc, u32 did)
{
	/* setting DDR to Enhance RDQS mode / read preamble mode */
	ddrc_mrw(ddrc, 0xd, 0xa, did, 2);//differ LP5
	/* set DQS gate into always on mode */
	updbitsl(DDRC_REG(ddrc, DRAM_CONFIG2), 0x3 << 11, 0x1 << 11);
}

static void training_read_postamble(u32 ddrc, u32 did)
{
	/* restore DQS gate to original value */
	writel(DDRC_REG(ddrc, DRAM_CONFIG2), dram_config2_param);
	/* Unset DDR Enhance RDQS mode */
	ddrc_mrw(ddrc, 0xd, 0x8, did, 2);//differ LP5
}

void ddrc_training(u32 ddrc)
{
	u32 ranks = (dram_config1_param & BIT(26)) ? 2 : 1, did;

	/* Wait for ddrc to be idle */
	while (!(readl(DDRC_REG(ddrc, DRAM_DDRC_STATUS)) & 0x4));

	/* MR46 values to DDRC for rdqs parity training */
	//writel(DDRC_REG(ddrc, DRAM_DDRC_MR46), 0x4);//just for LP5

	/* Put DRAM into self refresh, both dies/both channels */
	ddrc_uinst(ddrc, 13, 0x18, 0x303);//differ from LP5

	/* Init Read DQ calibration pattern */
	//ddrc_mrw(ddrc, 32, (DQ_CALIB_PATTERN >> 0) & 0xff, 2, 2);//differ from LP5
	//ddrc_mrw(ddrc, 40, (DQ_CALIB_PATTERN >> 8) & 0xff, 2, 2);//differ from LP5

	for (did = 0; did < ranks; did++) {
		training_read_preamble(ddrc, did);
		training_read_delay(ddrc, did);
		training_read_vref(ddrc, did);
		training_read_rtt(ddrc, did);
		training_read_postamble(ddrc, did);

		training_write_delay(ddrc, did);
		training_write_vref(ddrc, did);
	}

	/* Exit self refresh, both dies/both channels */
	ddrc_uinst(ddrc, 13, 0x14, 0x303);//differ from lp5
	/* issue auto refresh to all banks */
	ddrc_uinst(ddrc, 13, 0x28, 0x303);//diff from lp5

	/* Enable Read ECC counter collection, no need for lp4 */
	//writel(DDRC_REG(ddrc, DRAM_DDRC_MISC_2), 0x0);
	//writel(DDRC_REG(ddrc, DRAM_DDRC_MISC_2), 0x2);
}

void dram_training(void)
{
	int ddrc;

	if (!(training_param & 1))
		return;

	/* Disable DRAM and auto refresh */
	writel(DRAM_REG(DRAM_BROADCAST_OFFSET), 0x0);

	for (ddrc = 0; ddrc < 2; ddrc++) {
		if (!(dram_mode_param & (1 << (ddrc + 1))))
			continue;

		ddrc_training(ddrc);

		show_training_result(ddrc);
	}

	/* Enable AUTO Refresh - Enable DRAM - (for all DDRHOSTs) */
	writel(DRAM_REG(DRAM_BROADCAST_OFFSET), DRAM_CONTROL_AUTO_REF_EN | DRAM_CONTROL_ENABLE);

#ifdef CONFIG_RESTORE_TRAINING_RESULT
	/* Set training status in ScratchPad */
	writel(DDRC_REG(0, DDRC_SCRATCHPAD_REG), DRAM_TRAIN_OPMODE_BOTH_RW_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
#endif
}

