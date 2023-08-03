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
#include "AmbaCSL_DDRC.h"
typedef UINT16      u16;
typedef UINT32      u32;
typedef UINT64      u64;
typedef UINT32      u32_t;
typedef UINT64      u64_t;
#include "uartp.h"
#define DRAMC_DRAM_BASE                 AMBA_CORTEX_A76_DRAM_CTRL_BASE_ADDR
#define DRAMC_DDRC_BASE                 AMBA_CORTEX_A76_DDR_CTRL0_BASE_ADDR
#define DRAM_BROADCAST_BASE             AMBA_CORTEX_A76_DDR_CTRL_ALL_BASE_ADDR
#define DRAM_REG(x)                     (DRAMC_DRAM_BASE + (x))
#define DDRC_REG(x, y)                  (DRAMC_DDRC_BASE + (x) * 0x1000 + (y))
#define DDRB_REG(y)                     (DRAM_BROADCAST_BASE + (y))
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
#define lp5_mode_reg18_param            LPDDR5_ModeReg18ParamFsp0_Host0

#define DDRC_MAX_NUM			2

extern u32 training_param;
extern u32 dram_mode_param;
#if 0
extern u32 dram_host_first_param;
extern u32 dram_host_num_param;
#endif
extern u32 dram_config1_param;
extern u32 dram_config2_param;
extern u32 lp5_mode_reg18_param;


#define CONFIG_AMBST_DRAM_TRAINING_SHOW_RESULT
//#define CONFIG_AMBST_DRAM_TRAINING_SHOW_VREF_RANGE
//#define CONFIG_AMBST_DRAM_TRAINING_SHOW_DELAY_RANGE
#define CONFIG_AMBST_DRAM_TRAINING_SHOW_DCM

/* B0: [0..7]   dq, [32] dm, [36] all */
/* B1: [8..15]  dq, [33] dm, [37] all */
/* B2: [16..23] dq, [34] dm, [38] all */
/* B3: [24..31] dq, [35] dm, [39] all */
#define IDXA(bid)	((bid) + 36)		/* index for ALL */
#define IDXM(bid)	((bid) + 32)		/* index for DM */
#define IDXD(bid, i)	(((bid) << 3) + (i))	/* index for DQ each bit */

static u32 ddrc_byte_mask;
static u16 ddrc_1st;
static u16 ddrc_end;
static u16 best_vref_max[DDRC_MAX_NUM][40];
static u16 best_vref_min[DDRC_MAX_NUM][40];
static u16 best_vref_range[DDRC_MAX_NUM][40];
static u16 best_dly_start[DDRC_MAX_NUM][40];
static u16 best_dly_end[DDRC_MAX_NUM][40];
static u16 best_dly_max[DDRC_MAX_NUM][40];
static u16 max_vref_die0[DDRC_MAX_NUM][4];
static u16 min_vref_die0[DDRC_MAX_NUM][4];

extern void rct_timer_delay(u32 delay_cnt);

static void ddrc_uinst(u32 uinst1, u32 uinst2, u32 uinst4)
{
	writel(DDRB_REG(DRAM_UINST1), uinst1);
	writel(DDRB_REG(DRAM_UINST2), uinst2);
	writel(DDRB_REG(DRAM_UINST4), uinst4);

	writel(DDRB_REG(DRAM_UINST5), 0x1);
	while (readl(DDRC_REG(ddrc_1st, DRAM_UINST5)) & 0x1);
}

static void ddrc_mrw(u32 addr, u32 data, u32 did)
{
	/* did: 0 - Die0, 1 - Die1, 2 - Both Die */
	writel(DDRB_REG(DRAM_MODE_REG), BIT(31) | BIT(24) |
		(0x3 << 27) | ((did + 1) << 25) | (addr << 16) | data);
	while (readl(DDRC_REG(ddrc_1st, DRAM_MODE_REG)) & BIT(31));
}

static void ddrc_mrw_single(u32 ddrc, u32 addr, u32 data, u32 did, u32 cid)
{
	/* cid/did: 0 - Die0/ChA, 1 - Die1/ChB, 2 - Both Die/Channel */
	writel(DDRC_REG(ddrc, DRAM_MODE_REG), BIT(31) | BIT(24) |
		((cid + 1) << 27) | ((did + 1) << 25) | (addr << 16) | data);
	while (readl(DDRC_REG(ddrc, DRAM_MODE_REG)) & BIT(31));
}

static u32 ddrc_mrr_single(u32 ddrc, u32 addr, u32 did, u32 cid)
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

static void show_training_result(void)
{
#if defined(CONFIG_AMBST_DRAM_TRAINING_SHOW_RESULT)
	u32 bytes = (dram_config1_param & BIT(6)) ? 2 : 4, bid;
	u32 ranks = (dram_config1_param & BIT(26)) ? 2 : 1, did;
	u32 ddrc, i, temp, vref[4], dly[6];

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
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
				bst_uart_puthex(ddrc_mrr_single(ddrc, bid & 0x1 ? 15 : 14, did, bid >> 1), 2);

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
#endif
}

#if defined(CONFIG_AMBST_DRAM_TRAINING_SHOW_VREF_RANGE)
static void show_training_vref_range(u32 did, u32 delay, u32 id)
{
	u32 ddrc, i, bid, idx, idx_min, range_min, idx_max, range_max;

	if (delay < 32)
		return;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		bst_uart_putchar('\n');
		for (bid = 0; bid < 4; bid++) {
			idx_max = 0;
			idx_min = 0;
			range_max = 0;
			range_min = 128;

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

				bst_uart_putstr((i == 8) ? "<" : "[");
				bst_uart_puthex(best_vref_max[ddrc][idx], 2);
				bst_uart_putchar(',');
				bst_uart_puthex(best_vref_min[ddrc][idx], 2);
				bst_uart_putstr((i == 8) ? "> " : "] ");

				if (range_max < best_vref_range[ddrc][idx]) {
					range_max = best_vref_range[ddrc][idx];
					idx_max = idx;
				}

				if (range_min > best_vref_range[ddrc][idx]) {
					range_min = best_vref_range[ddrc][idx];
					idx_min = idx;
				}
			}

			bst_uart_putchar('{');
			bst_uart_puthex(range_max, 2);
			bst_uart_putstr(": ");
			bst_uart_puthex(best_vref_max[ddrc][idx_max], 2);
			bst_uart_putchar('-');
			bst_uart_puthex(best_vref_min[ddrc][idx_max], 2);
			bst_uart_putstr(", ");
			bst_uart_puthex(range_min, 2);
			bst_uart_putstr(": ");
			bst_uart_puthex(best_vref_max[ddrc][idx_min], 2);
			bst_uart_putchar('-');
			bst_uart_puthex(best_vref_min[ddrc][idx_min], 2);
			bst_uart_putchar('}');
		}
	}
}
#else
    #define show_training_vref_range(did, delay, id)
#endif

#if defined(CONFIG_AMBST_DRAM_TRAINING_SHOW_DELAY_RANGE)
static void show_training_delay_range(u32 did, u32 id)
{
	u32 ddrc, i, bid, idx, dly_c, dly_c_reg;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
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

				bst_uart_putstr((i == 8) ? " <" : " [");
				bst_uart_puthex(best_dly_start[ddrc][idx], 2);
				bst_uart_putchar(',');
				bst_uart_puthex(best_dly_end[ddrc][idx], 2);
				bst_uart_putstr((i == 8) ? ">" : "]");
				bst_uart_putstr("]");
			}

			bst_uart_putstr(" {");
			bst_uart_puthex((best_dly_start[ddrc][IDXA(bid)] + best_dly_end[ddrc][IDXA(bid)]) >> 1, 2);
			bst_uart_putstr("}");
		}
	}
}
#else
    #define show_training_delay_range(did, id)
#endif

#if defined(CONFIG_AMBST_DRAM_TRAINING_SHOW_DCM)
static void show_training_dcm(int ddrc, int did, int bid,
		int *dca_start, int *dca_end, int dca)
{

    static int executed = 0;
    if (executed == 0) {
        rct_timer_delay(0x380000);  // Wait extra 150ms for UART Init done to avoid message disappearred
        executed = 1;
    }
	bst_uart_putstr((bid == 0) ? "\nC" : "C");
	bst_uart_puthex(ddrc, 1);
	bst_uart_putchar('D');
	bst_uart_puthex(did, 1);
	bst_uart_putchar('B');
	bst_uart_puthex(bid, 1);
	bst_uart_putstr(": [");

	if (dca_start[bid] < 0) {
		bst_uart_putstr("-");
		bst_uart_puthex(-dca_start[bid], 1);
	} else {
		bst_uart_putstr(" ");
		bst_uart_puthex(dca_start[bid], 1);
	}
	bst_uart_putstr(",");
	if (dca_end[bid] < 0) {
		bst_uart_putstr("-");
		bst_uart_puthex(-dca_end[bid], 1);
	} else {
		bst_uart_putstr(" ");
		bst_uart_puthex(dca_end[bid], 1);
	}
	bst_uart_putstr("] 0x");
	bst_uart_puthex(dca, 2);
	bst_uart_putstr((bid < 3) ? "  |  " : "");
}
#else
    #define show_training_dcm( ddrc, did, bid, dca_start, dca_end, dca)
#endif

static void set_read_dq_delay(u32 did, u32 value)
{
	u32 ddrc, bid, dly = (value << 24) | (value << 16) | (value << 8) | value;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY1(bid, did)), 0x1f << 25, value << 25);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY4(bid, did)), dly);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY5(bid, did)), dly);
		}
	}
}

static void set_write_dq_delay(u32 did, u32 value)
{
	u32 ddrc, bid, dly = (value << 24) | (value << 16) | (value << 8) | value;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY1(bid, did)), 0x1f << 20, value << 20);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY2(bid, did)), dly);
			writel(DDRC_REG(ddrc, DDRIO_BYTE_DLY3(bid, did)), dly);
		}
	}
}

static void set_write_dqs_delay(u32 did, u32 value)
{
	u32 ddrc, bid;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY0(bid, did)), 0x1f << 10, value << 10);
		}
	}
}

static void set_read_dq_vref(u32 value)
{
	u32 ddrc;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		updbitsl(DDRC_REG(ddrc, DDRIO_VREF_0), 0x003f003f, (value << 16) | value);
		updbitsl(DDRC_REG(ddrc, DDRIO_VREF_1), 0x003f003f, (value << 16) | value);
	}
}

static void set_write_dq_vref(u32 did, u32 value)
{
	ddrc_mrw(14, value, did);
	ddrc_mrw(15, value, did);
}

static void clear_vars(void)
{
	u32 i, j;

	for (i = ddrc_1st; i < ddrc_end; i++) {
		for (j = 0; j < 40; j++) {
			best_vref_max[i][j] = 0;
			best_vref_min[i][j] = 0;
			best_vref_range[i][j] = 0;
			best_dly_max[i][i] = 0;
		}
	}
}

static void update_best(u32 ddrc, u32 bit, u32 vref_start, u32 vref_end, u32 delay)
{
	if (vref_start - vref_end >= best_vref_range[ddrc][bit]) {
		if (vref_start - vref_end > best_vref_range[ddrc][bit])
			best_dly_start[ddrc][bit] = best_dly_max[ddrc][bit] ? best_dly_max[ddrc][bit] : delay;
		best_dly_end[ddrc][bit] = delay;

		if (vref_start - vref_end > (u32)best_vref_max[ddrc][bit] - (u32)best_vref_min[ddrc][bit]) {
			best_vref_range[ddrc][bit] = best_vref_max[ddrc][bit] - best_vref_min[ddrc][bit];
			best_vref_max[ddrc][bit] = vref_start;
			best_vref_min[ddrc][bit] = vref_end;
			best_dly_max[ddrc][bit] = delay;
		} else {
			best_vref_range[ddrc][bit] = vref_start - vref_end;
		}
	}
}

#if defined(CONFIG_AMBST_DRAM_TRAINING_SIMPLE_PATTERN)

#define DQ_CALIB_PATTERN	0x3c5a

static u32 data_pattern[9] = {
	0xaaaa5555, 0xaaaa5555, 0x5555aaaa, 0x5555aaaa,
	0x55555555, 0xaaaaaaaa, 0xaaaaaaaa, 0x55555555,
	0x0ff033cc,
};

static void check_data_pattern(u32 did, u32 opcode, u64 *fail)
{
	u32 wr_data, rd_data, rsltcha[DDRC_MAX_NUM], rsltchb[DDRC_MAX_NUM];
	u32 loop, ddrc, i;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		rsltcha[ddrc] = 0;
		rsltchb[ddrc] = 0;
		fail[ddrc] = 0;
	}

	for (loop = 0; loop < 2; loop++) {
		ddrc_mrw(31, (loop % 2) ? 0xaa : 0x55, did);
		ddrc_mrw(32, (loop % 2) ? 0xaa : 0x55, did);

		for (i = 0; i < 9; i++) {
			wr_data = ((loop % 2) && (i < 8)) ? ~data_pattern[i] : data_pattern[i];
			writel(DDRB_REG(DRAM_MPC_WDATA + (i << 2)), wr_data);
		}

		ddrc_uinst(opcode, 0x24, (0x3 << 8) | (1 << did));
		/* Reset the fifo pointers */
		ddrc_uinst(0x12, 0x0, (0x3 << 8) | (1 << did));

		for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
			/* Check DQ */
			for (i = 0; i < 0x20; i += 4) {
				wr_data = readl(DDRC_REG(ddrc, DRAM_MPC_WDATA + i));

				rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RDATA + 0x00 + i));
				rsltcha[ddrc] |= rd_data ^ wr_data;

				rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RDATA + 0x20 + i));
				rsltchb[ddrc] |= rd_data ^ wr_data;
			}

			fail[ddrc] |= ((rsltcha[ddrc] >> 16) | rsltcha[ddrc]) & 0xffff;
			fail[ddrc] |= (((rsltchb[ddrc] >> 16) | rsltchb[ddrc]) & 0xffff) << 16;

			/* Check DM */
			wr_data = readl(DDRC_REG(ddrc, DRAM_MPC_WMASK));

			rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RMASK + 0x00));
			if ((rd_data ^ wr_data) & 0x55555555)
				fail[ddrc] |= 1ULL << 32;
			if ((rd_data ^ wr_data) & 0xaaaaaaaa)
				fail[ddrc] |= 1ULL << 33;

			rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RMASK + 0x04));
			if ((rd_data ^ wr_data) & 0x55555555)
				fail[ddrc] |= 1ULL << 34;
			if ((rd_data ^ wr_data) & 0xaaaaaaaa)
				fail[ddrc] |= 1ULL << 35;

			fail[ddrc] |= (fail[ddrc] & 0x01000000ff) ? 1ULL << 36 : 0;
			fail[ddrc] |= (fail[ddrc] & 0x020000ff00) ? 1ULL << 37 : 0;
			fail[ddrc] |= (fail[ddrc] & 0x0400ff0000) ? 1ULL << 38 : 0;
			fail[ddrc] |= (fail[ddrc] & 0x08ff000000) ? 1ULL << 39 : 0;
		}
	}
}

#else

#define DQ_CALIB_PATTERN	0x95a6

static u32 data_pattern[][9] = {
	{0x80017ffe, 0x7ffe0000, 0xffff8001, 0x80010000,
	 0x00007ffe, 0x4002bffd, 0xbffd0000, 0xffff4002,
	 0x9969a95a},

	{0x40020000, 0x0000bffd, 0x2004dffb, 0xdffb0000,
	 0xffff2004, 0x20040000, 0x0000dffb, 0x1008eff7,
	 0x33c303f0},

	{0xeff70000, 0xffff1008, 0x10080000, 0x0000eff7,
	 0x0810f7ef, 0xf7ef0000, 0xffff0810, 0x08100000,
	 0x669656a5},

	{0x0000f7ef, 0x0420fbdf, 0xfbdf0000, 0xffff0420,
	 0x04200000, 0x0000fbdf, 0x0240fdbf, 0xfdbf0000,
	 0x33c303f0},

	{0xffff0240, 0x02400000, 0x0000fdbf, 0x0180fe7f,
	 0xfe7f0000, 0xffff0180, 0x01800000, 0x0000fe7f,
	 0xa659a9a6},

	{0xaa5555aa, 0xaa5555aa, 0xaa5555aa, 0xaa5555aa,
	 0x00ff7f00, 0x00ffff00, 0xcc3333cc, 0xaa55ff00,
	 0x59a65659},

	{0x55aaff00, 0x55aa00ff, 0xaa5500ff, 0xaa5555aa,
	 0xc0030000, 0x0c30300c, 0x000003c0, 0x1c38e3c7,
	 0xf30cfcf3},

	{0x000003c0, 0x0ff0f00f, 0xf81f0000, 0x000007e0,
	 0x03c0fc3f, 0xfe7f0000, 0x00000180, 0x0000ffff,
	 0x0cf3030c},
};

static void check_data_pattern(u32 did, u32 opcode, u64 *fail)
{
	u32 wr_data, rd_data, rsltcha[DDRC_MAX_NUM], rsltchb[DDRC_MAX_NUM];
	u32 ddrc, i, loop, mask, data[9];
	u64 __fail;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		rsltcha[ddrc] = 0;
		rsltchb[ddrc] = 0;
		fail[ddrc] = 0;
	}

	for (loop = 0; loop < 8; loop++) {
		if (opcode == 0xa) {
			mask = (1 << loop) | (0x8000 >> loop);

			ddrc_mrw(31, (mask >> 0) & 0xff, did);
			ddrc_mrw(32, (mask >> 8) & 0xff, did);

			data[8] = 0;
			for (i = 0; i < 8; i++) {
				if (DQ_CALIB_PATTERN & (1 << (2 * i))) {
					data[i] = mask ^ 0xffff;
					data[8] |= 0x3 << (4 * i);
				} else {
					data[i] = mask;
				}

				if (DQ_CALIB_PATTERN & (1 << (2 * i + 1))) {
					data[i] |= (mask ^ 0xffff) << 16;
					data[8] |= 0x3 << (4 * i + 2);
				} else {
					data[i] |= mask << 16;
				}
			}
		} else {
			for (i = 0; i < 9; i++)
				data[i] = data_pattern[loop][i];
		}

		for (i = 0; i < 9; i++)
			writel(DDRB_REG(DRAM_MPC_WDATA + (i << 2)), data[i]);

		ddrc_uinst(opcode, 0x24, (0x3 << 8) | (1 << did));
		/* Reset the fifo pointers */
		ddrc_uinst(0x12, 0x0, (0x3 << 8) | (1 << did));

		__fail = ~0UL;

		for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
			/* Check DQ */
			for (i = 0; i < 0x20; i += 4) {
				wr_data = readl(DDRC_REG(ddrc, DRAM_MPC_WDATA + i));

				rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RDATA + 0x00 + i));
				rsltcha[ddrc] |= rd_data ^ wr_data;

				rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RDATA + 0x20 + i));
				rsltchb[ddrc] |= rd_data ^ wr_data;
			}

			fail[ddrc] |= ((rsltcha[ddrc] >> 16) | rsltcha[ddrc]) & 0xffff;
			fail[ddrc] |= (((rsltchb[ddrc] >> 16) | rsltchb[ddrc]) & 0xffff) << 16;

			/* Check DM */
			wr_data = readl(DDRC_REG(ddrc, DRAM_MPC_WMASK));

			rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RMASK + 0x00));
			if ((rd_data ^ wr_data) & 0x55555555)
				fail[ddrc] |= 1ULL << 32;
			if ((rd_data ^ wr_data) & 0xaaaaaaaa)
				fail[ddrc] |= 1ULL << 33;

			rd_data = readl(DDRC_REG(ddrc, DRAM_MPC_RMASK + 0x04));
			if ((rd_data ^ wr_data) & 0x55555555)
				fail[ddrc] |= 1ULL << 34;
			if ((rd_data ^ wr_data) & 0xaaaaaaaa)
				fail[ddrc] |= 1ULL << 35;

			fail[ddrc] |= (fail[ddrc] & 0x01000000ff) ? 1ULL << 36 : 0;
			fail[ddrc] |= (fail[ddrc] & 0x020000ff00) ? 1ULL << 37 : 0;
			fail[ddrc] |= (fail[ddrc] & 0x0400ff0000) ? 1ULL << 38 : 0;
			fail[ddrc] |= (fail[ddrc] & 0x08ff000000) ? 1ULL << 39 : 0;

			__fail &= fail[ddrc];
		}

		if (__fail == 0xffffffffff)
			break;
	}
}

#endif

static void training_vref_inner(u32 did, u32 bytes, u32 delay, u32 write)
{
	int vref_start[DDRC_MAX_NUM][40], vref_end[DDRC_MAX_NUM][40];
	int vref_training, opcode, ddrc, bid, idx, i;
	u64 fail[DDRC_MAX_NUM];

	if (write) {
		vref_training = 127;
		opcode = 0xb;
	} else {
		vref_training = 63;
		opcode = 0xa;
	}

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (i = 0; i < 40; i++) {
			vref_start[ddrc][i] = 0;
			vref_end[ddrc][i] = 0;
		}
	}

	while (vref_training > 0) {
		if (write)
			set_write_dq_vref(did, vref_training);
		else
			set_read_dq_vref(vref_training);

		rct_timer_delay(10); /* delay about 400ns */

		check_data_pattern(did, opcode, fail);

		for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
			for (bid = 0; bid < 4; bid++) {
				if ((bytes & (0x1 << (ddrc * 4 + bid))) == 0)
					continue;

				for (i = 0; i < 10; i++) {
					idx = (i == 9) ? IDXA(bid) : (i == 8) ? IDXM(bid) : IDXD(bid, i);

					if (fail[ddrc] & (1ULL << idx)) {
						vref_start[ddrc][idx] = 0;
						vref_end[ddrc][idx] = 0;
					} else {
						if (vref_start[ddrc][idx] == 0)
							vref_start[ddrc][idx] = vref_training;
						vref_end[ddrc][idx] = vref_training;

						/* update the best value for dly/dll and vref */
						update_best(ddrc, idx,
							vref_start[ddrc][idx], vref_end[ddrc][idx], delay);
					}
				}
			}
		}

		vref_training -= 2;
	}

	show_training_vref_range(did, delay, write);
}

static int training_delay_update(u32 did, u32 id, u32 retry, int offset)
{
	int ddrc, bid, dly_c, dly_c_new, dly_cp, reg, offs, index;

	show_training_delay_range(did, id);

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
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
			dly_cp = (best_dly_start[ddrc][index] + best_dly_end[ddrc][index]) >> 1;

			if (dly_cp + offset > 23)
				dly_c_new = min(dly_c + 1, 31);
			else if (dly_cp + offset < 8)
				dly_c_new = max(dly_c - 1, 0);

			if (dly_c_new == dly_c) {
				retry &= ~(0x1 << (ddrc * 4 + bid));
				continue;
			}

			updbitsl(DDRC_REG(ddrc, reg), 0x1f << offs, dly_c_new << offs);
		}
	}

	return retry;
}

static void training_delay_done(u32 did, u32 id, int offset)
{
	int ddrc, bid, i, dly_f0, dly_f1, dly_dm, dly_best;

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			dly_f0 = dly_f1 = 0;

			for (i = 0; i < 8; i++) {
				dly_best = (best_dly_start[ddrc][IDXD(bid,i)] + best_dly_end[ddrc][IDXD(bid,i)]) >> 1;
				dly_best = CLIP(dly_best + offset, 31, 0);
				if (i < 4)
					dly_f0 |= dly_best << (i << 3);
				else
					dly_f1 |= dly_best << ((i - 4) << 3);
			}

			dly_best = (best_dly_start[ddrc][IDXM(bid)] + best_dly_end[ddrc][IDXM(bid)]) >> 1;
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
}

static void training_read_delay(u32 did)
{
	int dly_training, offset, retry = ddrc_byte_mask;

	/* extract and extend offset to signed value */
	asm volatile("sbfx %0, %1, #12, #4\n\t" : "=&r"(offset) : "r"(training_param));

	while (retry) {
		clear_vars();

		dly_training = 32;
		while (dly_training-- > 0) {
			set_read_dq_delay(did, dly_training);
			training_vref_inner(did, retry, dly_training, 0);
		}

		retry = training_delay_update(did, 0, retry, offset);
	}

	training_delay_done(did, 0, offset);
}

static void training_read_vref(u32 did)
{
	u32 ddrc, bid, best_vref, offset, vref_reg, vref_offs, retry = ddrc_byte_mask;

	clear_vars();

	training_vref_inner(did, retry, 0xbb, 0);

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			if (did == 0) {
				max_vref_die0[ddrc][bid] = best_vref_max[ddrc][IDXA(bid)];
				min_vref_die0[ddrc][bid] = best_vref_min[ddrc][IDXA(bid)];
			} else {
				if (best_vref_max[ddrc][IDXA(bid)] > max_vref_die0[ddrc][bid])
					best_vref_max[ddrc][IDXA(bid)] = max_vref_die0[ddrc][bid];
				if (best_vref_min[ddrc][IDXA(bid)] < min_vref_die0[ddrc][bid])
					best_vref_min[ddrc][IDXA(bid)] = min_vref_die0[ddrc][bid];
			}

			best_vref = (best_vref_max[ddrc][IDXA(bid)] + best_vref_min[ddrc][IDXA(bid)]) >> 1;

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
}

static void training_read_rtt(u32 did)
{
	u32 dram_config2_val = (dram_config2_param & ~(0x3 << 11)) | (0x1 << 11);

	writel(DDRB_REG(DRAM_CONFIG2), dram_config2_param);
	ddrc_uinst(0x12, 0x0, (0x3 << 8) | (1 << did)); /* Reset fifo pointers */
	writel(DDRB_REG(DRAM_CONFIG2), dram_config2_val);

	/* update RTT (Round-Trip-Time) */
	writel(DDRB_REG(DRAM_INIT_CTL), DRAM_INIT_CTL_GET_RTT_EN | (did << 10));
	while(readl(DDRC_REG(ddrc_1st, DRAM_INIT_CTL)) & DRAM_INIT_CTL_GET_RTT_EN);

	/* Reset the fifo pointers */
	ddrc_uinst(0x12, 0x0, (0x3 << 8) | (1 << did));
}

static void training_write_delay(u32 did)
{
	int dly_training, offset, retry = ddrc_byte_mask;

	/* extract and extend offset to signed value */
	asm volatile("sbfx %0, %1, #8, #4\n\t" : "=&r"(offset) : "r"(training_param));

	while (retry) {
		clear_vars();

		dly_training = 32;
		while (dly_training-- > 0) {
			set_write_dq_delay(did, dly_training);
			training_vref_inner(did, retry, dly_training, 1);
		}

		retry = training_delay_update(did, 1, retry, offset);
	}

	training_delay_done(did, 1, offset);
}

static void training_write_dqs(u32 did)
{
	int ddrc, bid, dly_best, dly_training, offset, retry = ddrc_byte_mask;

	/* extract and extend offset to signed value, same as write delay offset */
	asm volatile("sbfx %0, %1, #8, #4\n\t" : "=&r"(offset) : "r"(training_param));

	writel(DDRB_REG(DRAM_DDRC_MISC_2),     BIT(2));
	ddrc_mrw(46, 0x4, did);

	while (retry) {
		clear_vars();

		dly_training = 32;
		while (dly_training-- > 0) {
			set_write_dqs_delay(did, dly_training);
			training_vref_inner(did, retry, dly_training, 2);
		}

		retry = training_delay_update(did, 2, retry, offset);
	}

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			dly_best = (best_dly_start[ddrc][IDXM(bid)] + best_dly_end[ddrc][IDXM(bid)]) >> 1;
			dly_best = CLIP(dly_best + offset, 31, 0);
			updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY0(bid, did)), 0x1f << 10, dly_best << 10);
		}
	}

	writel(DDRB_REG(DRAM_DDRC_MISC_2),     0x0);
	ddrc_mrw(46, 0x0, did);
}

static void training_write_vref(u32 did)
{
	u32 ddrc, bid, best_vref, offset, retry = ddrc_byte_mask;

	clear_vars();

	training_vref_inner(did, retry, 0xcc, 1);

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			best_vref = (best_vref_max[ddrc][IDXA(bid)] + best_vref_min[ddrc][IDXA(bid)]) >> 1;

			/* adjusted by offset */
			__asm__ __volatile__("sbfx %0, %2, #16, #8\n\t"
					     "add %1, %1, %0\n\t"
					     : "=&r"(offset), "+r"(best_vref)
					     : "r"(training_param));

			ddrc_mrw_single(ddrc, (bid & 0x1) ? 15 : 14, best_vref, did, bid >> 1);
		}
	}
}

static void training_read_preamble(u32 did)
{
	u32 dram_config2_val = (dram_config2_param & ~(0x3 << 11)) | (0x1 << 11);

	/* setting DDR to Enhance RDQS mode */
	ddrc_mrw(46, 0x1, did);
	/* set DQS gate into always on mode */
	writel(DDRB_REG(DRAM_CONFIG2), dram_config2_val);
}

static void training_read_postamble(u32 did)
{
	/* restore DQS gate to original value */
	writel(DDRB_REG(DRAM_CONFIG2), dram_config2_param);
	/* Unset DDR Enhance RDQS mode */
	ddrc_mrw(46, 0x0, did);
}

static void training_dcm(u32 did)
{
	int ddrc, bid, temp, step, status, old_status[4];
	int dca, dca_start[4], dca_end[4];

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			dca_start[bid] = 7;
			dca_end[bid] = 7;
			old_status[bid] = 0;
		}

		for (step = -7; step < 8; step++) {
			/* set DCA for all channels */
			temp = (step == 0) ? step : (step < 0) ? -step : step | (0x1 << 3);
			ddrc_mrw_single(ddrc, 30, temp | (temp << 4), did, 2);
			/* trigger DCM for all channels */
			ddrc_mrw_single(ddrc, 26, 0x1, did, 2);
			rct_timer_delay(50); /* wait for tDCMM(2us) */
			ddrc_mrw_single(ddrc, 26, 0x3, did, 2);
			rct_timer_delay(50); /* wait for tDCMM(2us) */
			/* stop the DCM */
			ddrc_mrw_single(ddrc, 26, 0x2, did, 2);
			/* read DCM status */
			status = ddrc_mrr_single(ddrc, 26, did, 2);

			for (bid = 0; bid < 4; bid++) {
				temp = status >> ((bid / 2) ? 8 : 0);
				temp = (temp >> ((bid % 2) ? 4 : 2)) & 0x3;

				if (temp != 0 && old_status[bid] == 0)
					dca_start[bid] = step;
				if (temp == 3 && old_status[bid] != 3)
					dca_end[bid] = step;

				old_status[bid] = temp;
			}

			/* branch out if no space to increase duty cycle */
			if ((status & 0x3c3c) == 0x3c3c)
				break;
		}

		for (bid = 0; bid < 4; bid++) {
			dca = max((dca_start[bid] + dca_end[bid]) / 2 - 1, -7);
#if 0
			if (dca_start[bid] == -7)
				dca = max(dca - 1, -7);
			if (dca_end[bid] == 7)
				dca = min(dca + 1, 7);
#endif
			dca = (dca == 0) ? dca : (dca < 0) ? -dca : dca | (0x1 << 3);

			temp = (bid % 2) ? temp | (dca << 4): dca;
			ddrc_mrw_single(ddrc, 30, temp, did, bid >> 1);

			if (bid%2) {    /* Keep MR30 training result in scratchpad for later save */
				writel(DDRCT_RESULT_MR30_ADDR(ddrc, did, bid/2), temp);
			}
			show_training_dcm(ddrc, did, bid, dca_start, dca_end, dca);
		}
	}
	/* Set magic code to indicate the MR30 result is valid */
	writel(DDRCT_RESULT_MR30_VALID, DDRCT_RESULT_VALID_MAGIC_CODE);
}

#if defined(CONFIG_AMBST_DRAM_TRAINING_WCK2CK)
static void training_wck2ck(u32 did)
{
	u32 mr18, ddrc, bid, uinst3_val, status;
	int wck_dly, wck_dly_c, wck_dly_f;

	/* Drive WCK low */
	ddrc_uinst(0x00020e, 0x0, (0x3 << 8) | (1 << did));
	/* MR18[6] = 1 to trigger WCK2CK leveling mode */
	mr18 = lp5_mode_reg18_param & 0xff;
	ddrc_mrw(18, mr18 | BIT(6) | BIT(7), did);

	/* During write leveling, design drives WCK half cycle late if
	 * WT_PRE is even, so to fix this issue we need WT_PRE to be odd.*/
	uinst3_val = readl(DDRC_REG(ddrc_1st, DRAM_UINST3));
	writel(DDRB_REG(DRAM_UINST3), uinst3_val | 0x1);

	for (ddrc = ddrc_1st; ddrc < ddrc_end; ddrc++) {
		for (bid = 0; bid < 4; bid++) {
			wck_dly_c = wck_dly_f = 0;

			while (1) {
				wck_dly = (wck_dly_c << 5) | wck_dly_f;
				updbitsl(DDRC_REG(ddrc, DDRIO_BYTE_DLY0(bid, did)), 0x3ff, wck_dly);

				/* Toggle WCK */
				ddrc_mrw_single(ddrc, 0x08060e, 0x0, (0x3 << 8) | (1 << did), 2);
				/* wait for tWCKTGGL, i.e., 7.5 cycles of WCK */
				rct_timer_delay(3);
				/* Capture DQ */
				ddrc_mrw_single(ddrc, 0x000105, 0x0, (0x3 << 8) | (1 << did), 2);
				status = readl(DDRC_REG(ddrc, DRAM_UINST6));
				if (((status >> (bid << 3)) & 0xff) == 0xff)
					break;

				if (++wck_dly_f > 24) {
					wck_dly_f = 0;
					wck_dly_c++;
				}
			}
		}
	}

	/* Revert the WT_PRE */
	writel(DDRB_REG(DRAM_UINST3), uinst3_val);

	/* Exit test */
	ddrc_mrw(18, mr18, did);

	/* Reset CAS effect so that RTL issues a CAS_FS on next tx */
	ddrc_uinst(0x00080e, 0x0, (0x3 << 8) | (1 << did));
	/* Release WCK */
	ddrc_uinst(0x00000e, 0x0, (0x3 << 8) | (1 << did));
	/* Issue CAS FS */
	ddrc_uinst(0x00100e, 0x0, (0x3 << 8) | (1 << did));
	/* Reset the fifo pointers */
	ddrc_uinst(0x12, 0x0, (0x3 << 8) | (1 << did));
}
#else
    #define training_wck2ck(did)
#endif

void dram_training(void)
{
	u32 did, ranks = (dram_config1_param & BIT(26)) ? 2 : 1;

	if (!(training_param & 1))
		return;

#if 0
	ddrc_1st = dram_host_first_param; /* inclusive */
	ddrc_end = dram_host_first_param + dram_host_num_param; /* exclusive */

	ddrc_byte_mask = (1UL << (dram_host_num_param * 4)) - 1;
	ddrc_byte_mask <<= ddrc_1st * 4;
#else
	ddrc_1st = (dram_mode_param & 0x2U) ? 0 : 1; /* inclusive */
	ddrc_end = (dram_mode_param & 0x4U) ? 2 : 1; /* exclusive */

	ddrc_byte_mask = (1UL << ((ddrc_end - ddrc_1st) * 4)) - 1;
	ddrc_byte_mask <<= ddrc_1st * 4;
#endif

	/* Disable DRAM and auto refresh */
	writel(DDRB_REG(DRAM_CONTRL), 0x0);

	/* MR46 values to DDRC for rdqs parity training */
	writel(DDRB_REG(DRAM_DDRC_MR46), 0x4);

	/* Put DRAM into self refresh, both dies/both channels */
	ddrc_uinst(13, 0x68, 0x303);

	/* Init Read DQ calibration pattern */
	ddrc_mrw(33, (DQ_CALIB_PATTERN >> 0) & 0xff, 2);
	ddrc_mrw(34, (DQ_CALIB_PATTERN >> 8) & 0xff, 2);

	for (did = 0; did < ranks; did++) {
		if ((training_param & 0x1f) != 0x3) {
			training_wck2ck(did);
			training_dcm(did);

			training_read_preamble(did);
			training_read_delay(did);
			training_read_vref(did);
			training_read_rtt(did);
			training_read_postamble(did);

			training_write_delay(did);
			if ((DRAM_Config2Param & 0x300U) == 0x300U) {   // bit[9:8] for link ecc enable
				training_write_dqs(did);
			}
			training_write_vref(did);
		} else {	// Only Bit[1:0] set means we are restored and want to train write delay everytime
			u32 backup_vref[DDRC_MAX_NUM][4], ddrc, bid;
			for (ddrc = ddrc_1st; ddrc <ddrc_end; ddrc++) { /* Backup Vref */
				for (bid = 0; bid < 4; bid++) {
					backup_vref[ddrc][bid] = ddrc_mrr_single(ddrc, (bid & 0x1) ? 15 : 14, did, bid >> 1);
				}
			}
			training_write_delay(did);
			for (ddrc = ddrc_1st; ddrc <ddrc_end; ddrc++) { /* Restore Vref */
				for (bid = 0; bid < 4; bid++) {
					ddrc_mrw_single(ddrc, (bid & 0x1) ? 15 : 14, backup_vref[ddrc][bid], did, bid >> 1);
				}
			}
		}
	}

	/* Exit self refresh, both dies/both channels */
	ddrc_uinst(13, 0x28, 0x303);
	/* issue auto refresh to all banks */
	ddrc_uinst(13, 0x4038, 0x303);

	/* Enable Read ECC counter collection */
	writel(DDRB_REG(DRAM_DDRC_MISC_2), 0x2);

	/* Enable AUTO Refresh - Enable DRAM - (for all DDRHOSTs) */
	writel(DDRB_REG(DRAM_CONTRL), DRAM_CONTROL_AUTO_REF_EN | DRAM_CONTROL_ENABLE);

	show_training_result();

#ifdef CONFIG_RESTORE_TRAINING_RESULT
	if ((training_param & 0x1f) != 0x3) {    // Only Bit[1:0] set means we are restored and want to train write delay everytime
		/* Set training status in ScratchPad */
		writel(DDRC_REG(0, DDRC_SCRATCHPAD_REG), DRAM_TRAIN_OPMODE_BOTH_RW_DONE << DRAM_TRAIN_PROC_OPMODE_OFST);
	}
#endif
}

