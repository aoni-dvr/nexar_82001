/*
 * kernel/private/drivers/ambdram_statis/ambdram_statis.c
 *
 * History:
 *	2020/06/08 - [Bingliang Hu]
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <soc/ambarella/iav_helper.h>
#else
#include <plat/chip.h>
#endif
#include "ambdram_statis.h"
#include "ambdram_client.h"

struct dram_statis G_dram_statis;

static char *ddr_type_name[] = {
	[DDR_TYPE_LPDDR4]	= "LPDDR4",
	[DDR_TYPE_DDR4]		= "DDR4",
	[DDR_TYPE_LPDDR5]	= "LPDDR5",
	[DDR_TYPE_UNKNOWN]	= "DDR_UNKNOWN",
};

#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
//#define DRV_DBG
static int trigger_mode = 0;
static int dramc_stats_enable = 0;
static u32 G_statis_buf[ARRAY_SIZE(G_dram_client)*3] = {0}; // record (request, burst, mask) for each client
#endif

static void ambdram_start_perf_statis(struct dram_statis *statis)
{
#if (ARCH_PLAT_VER >= ARCH_PLAT_VER_6)
	u32 reg_value = 0, i;

	for (i = statis->ddrc_first; i < statis->ddrc_end; i++) {
		reg_value = readl_relaxed(statis->dram_reg + MISC2_STAT_COLLECT_CTRL + i * DRAM_CONFIG_REG_STEP);
		reg_value |= 1;
		writel_relaxed(reg_value, statis->dram_reg + MISC2_STAT_COLLECT_CTRL + i * DRAM_CONFIG_REG_STEP);
	}

	memset(statis->perf, 0, sizeof(statis->perf));
#endif
}

static void ambdram_stop_perf_statis(struct dram_statis *statis)
{
#if (ARCH_PLAT_VER >= ARCH_PLAT_VER_6)
	u32 reg_value = 0, i, j;

	for (i = statis->ddrc_first; i < statis->ddrc_end; i++) {
		reg_value = readl_relaxed(statis->dram_reg + MISC2_STAT_COLLECT_CTRL + i * DRAM_CONFIG_REG_STEP);
		reg_value &= ~1;
		writel_relaxed(reg_value, statis->dram_reg + MISC2_STAT_COLLECT_CTRL + i * DRAM_CONFIG_REG_STEP);

		for (j = 0; j < DRAM_MAX_PERF; j++) {
			void __iomem *reg = statis->dram_reg + MISC2_PERF_INFO_BEGIN;
			statis->perf[j] += readl_relaxed(reg + i * DRAM_CONFIG_REG_STEP + j * 4);
		}
	}
#endif
}

static enum hrtimer_restart ambdram_hrtimer_handler(struct hrtimer *timer)
{
	struct dram_statis *statis = container_of(timer, struct dram_statis, timer);
	u32 hrtimer_expire = 0, i, j;
	void __iomem *dram_reg = NULL;

	hrtimer_expire = statis->interval_ms ? statis->interval_ms : statis->duration_ms;
	statis->elapsed_ms += hrtimer_expire;

	/* stop statistics */
	writel_relaxed(STAT_CONTROL_OFF, statis->dram_reg + STAT_CONTROL_OFFSET);

	/* collect interval statistics */
	if (statis->interval_ms > 0) {
		u32 burst_group[DRAM_CLIENT_GROUP_NUM] = {0}, burst;
		u32 mask_group[DRAM_CLIENT_GROUP_NUM] = {0}, mask;

		for (i = statis->dram_ctrl_first; i < statis->dram_ctrl_end; i++) {
			dram_reg = statis->dram_reg + STAT_CLIENT_BURSTS_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;

			for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
				burst = readl_relaxed(dram_reg + j * 4);
				burst_group[G_dram_client[j].group] += burst;
			}

			dram_reg = statis->dram_reg + STAT_CLIENT_MASK_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;

			for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
				mask = readl_relaxed(dram_reg + j * 4);
				if ((statis->ddrc_num == 2) && (ARCH_PLAT_VER == ARCH_PLAT_VER_6))
					mask >>= 1;
				mask_group[G_dram_client[j].group] += mask;
			}
		}

		for (j = 0; j < ARRAY_SIZE(burst_group); j++) {
			if (statis->buf_ptr >= statis->buf_size / sizeof(u32))
				break;

			statis->buf_burst[statis->buf_ptr] = burst_group[j];
			statis->buf_mask[statis->buf_ptr] = mask_group[j];
			statis->buf_ptr++;
		}

		if (statis->elapsed_ms < statis->duration_ms) {
			/* start statistics again */
			writel_relaxed(STAT_CONTROL_ON, statis->dram_reg + STAT_CONTROL_OFFSET);
			hrtimer_forward_now(timer, ms_to_ktime(hrtimer_expire));

			return HRTIMER_RESTART;
		}
	}

	/* stop and collect perf info */
	ambdram_stop_perf_statis(statis);

    for (i = 0; i < ARRAY_SIZE(G_dram_client); i++) {
        statis->request[i] = 0;
        statis->burst[i] = 0;
        statis->mask[i] = 0;
    }

	/* now collect final statistics */
	for (i = statis->dram_ctrl_first; i < statis->dram_ctrl_end; i++) {
		for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
			dram_reg = statis->dram_reg + STAT_CLIENT_REQ_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->request[j] += readl_relaxed(dram_reg + j * 4);

			dram_reg = statis->dram_reg + STAT_CLIENT_BURSTS_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->burst[j] += readl_relaxed(dram_reg + j * 4);

			dram_reg = statis->dram_reg + STAT_CLIENT_MASK_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->mask[j] += readl_relaxed(dram_reg + j * 4);
			if ((statis->ddrc_num == 2) && (ARCH_PLAT_VER == ARCH_PLAT_VER_6))
				statis->mask[j] >>= 1;
		}
	}

    for (i = 0; i < ARRAY_SIZE(statis->bank); i++) {
        statis->bank[i] = 0;
    }
	for (i = statis->dram_ctrl_first; i < statis->dram_ctrl_end; i++) {
		for (j = 0; j < ARRAY_SIZE(statis->bank); j++) {
			dram_reg = statis->dram_reg + STAT_GLOBAL_BANK_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->bank[j] += readl_relaxed(dram_reg + j * 4);
		}
	}

    statis->rw = 0;
	for (i = statis->dram_ctrl_first; i < statis->dram_ctrl_end; i++) {
		dram_reg = statis->dram_reg + STAT_GLOBAL_RW_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
		if (i % 2)
			dram_reg += 0x4000;
		statis->rw += readl_relaxed(dram_reg);
	}

	wake_up_interruptible(&statis->wq);

	return HRTIMER_NORESTART;
}

static inline u64 get_group_bw(struct dram_statis *statis, u64 index)
{
	u64 bandwidth = 0;

	bandwidth = (u64)statis->buf_burst[index] * statis->burst_size * 1000ULL;
	bandwidth += (u64)statis->buf_mask[index] * statis->burst_size * 1000ULL / 2;
	do_div(bandwidth, statis->interval_ms);
	return bandwidth;
}

static void ambdram_start_statistics(struct dram_statis *statis)
{
	/* reset statis */
	writel_relaxed(STAT_CONTROL_RESET, statis->dram_reg + STAT_CONTROL_OFFSET);
	/* wait for reset statis complete */
	msleep(1);
	/* start statis */
	writel_relaxed(STAT_CONTROL_ON, statis->dram_reg + STAT_CONTROL_OFFSET);

	/* start perf */
	ambdram_start_perf_statis(statis);
}

static void ambdram_show_statis_info(struct seq_file *m, struct dram_statis *statis)
{
	u32 i = 0;

	if (statis->verbose == 0)
		return;

	seq_puts(m, "\n[Statistics Info]\n");
	seq_puts(m, "  ID\t              ClientName\t   Request\t     Burst\t MaskBurst\n");

	for (i = 0; i < ARRAY_SIZE(G_dram_client); i++) {
		seq_printf(m, "  %2d\t%24s\t%10u\t%10u\t%10u\n", i,
			G_dram_client[i].name,
			statis->request[i],
			statis->burst[i],
			/* the number of maskburst is counted N times for N-hosts */
			statis->mask[i] / statis->ddrc_num);
	}

	seq_puts(m, "\n  BANK\t");
	for (i = 0; i < ARRAY_SIZE(statis->bank); i++)
		seq_printf(m, "Bank%d\t", i);
	seq_puts(m, "\n  Open\t");
	for (i = 0; i < ARRAY_SIZE(statis->bank); i++)
		seq_printf(m, "%d\t", statis->bank[i]);

	seq_printf(m, "\n\n  RW Toggle: %d\n\n", statis->rw);
}

static void ambdram_print_bandwidth(struct seq_file *m, struct dram_statis *statis,
	const char *group_name, u64 group_bw)
{
	u64 bw = 0, bw_rem = 0, percent = 0, percent_rem = 0;

	bw = group_bw;
	bw_rem = do_div(bw, 1ULL << 20) >> 10;

	percent = group_bw * 100ULL * 1000ULL / statis->theory_bw;
	percent_rem = do_div(percent, 1000ULL);

	seq_printf(m, "  %-11s\t\t%10llu.%03llu\t\t%10llu.%03llu\n", group_name, bw, bw_rem, percent, percent_rem);
}

static void ambdram_show_basic_info(struct seq_file *m, struct dram_statis *statis)
{
	seq_puts(m, "\n[Usage]\n");
	seq_puts(m, "  echo duration=M interval=N verbose=0/1 > /proc/ambarella/dram_statistics\n");
	seq_printf(m, "    M: statistical time in ms, now M=%d\n", statis->duration_ms);
	seq_printf(m, "    N: statistics in every N ms, 0ms means no interval, now N=%d\n", statis->interval_ms);
#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
	seq_puts(m, "  echo trigger=0/1 > /proc/ambarella/dram_statistics\n");
	seq_printf(m, "    trigger: use trigger mode and stop(0)/start(1), now trigger=%d\n", dramc_stats_enable);
#endif

	seq_puts(m, "\n[DRAM Info]\n");
	seq_printf(m, "  %s with %d-bits @ %lluMHz connected to %d DDRCs, Burst Size: %u, theory BW: %llu MB/s\n",
		ddr_type_name[statis->ddr_type], statis->bus_width, statis->ddr_clock / 1000 / 1000,
		statis->ddrc_num, statis->burst_size, statis->theory_bw >> 20);

	seq_puts(m, "\n");
}

static void ambdram_show_bw_info(struct seq_file *m, struct dram_statis *statis)
{
	u64 i = 0, dram_bw = 0, total_bw = 0, group_bw[DRAM_CLIENT_GROUP_NUM] = {0};

	seq_puts(m, "[Average BW]\n");
	seq_puts(m, "  Module\t\tBandwidth(In MB/s)\t\tPercent\n");
	seq_puts(m, "  -----------\n");

	for (i = 0; i < ARRAY_SIZE(G_dram_client); i++) {
		dram_bw = (u64)statis->burst[i] * statis->burst_size * 1000ULL;
		dram_bw += (u64)statis->mask[i] * statis->burst_size * 1000ULL / 2;
		do_div(dram_bw, statis->duration_ms);

		group_bw[G_dram_client[i].group] += dram_bw;
		total_bw += dram_bw;
	}

	for (i = 0; i < DRAM_CLIENT_GROUP_NUM; i++)
		ambdram_print_bandwidth(m, statis, G_dram_group_name[i], group_bw[i]);

	ambdram_print_bandwidth(m, statis, "All", total_bw);

	seq_puts(m, "\n");
}

static void ambdram_show_interval_info(struct seq_file *m, struct dram_statis *statis)
{
	u64 group_bw_max[DRAM_CLIENT_GROUP_NUM], group_bw_tmp[DRAM_CLIENT_GROUP_NUM];
	u64 total_bw_max = 0, group_bw = 0, total_bw = 0, buf_ptr = 0;
	char group_name[64] = {0};
	u32 i = 0, j = 0, min_burst = 0;

	if (statis->interval_ms == 0)
		return;

	/* fixup the burst number for each client group */
	for (i = statis->buf_ptr; i > DRAM_CLIENT_GROUP_NUM; i -= DRAM_CLIENT_GROUP_NUM) {
		for (j = i - 1; j >= i - DRAM_CLIENT_GROUP_NUM; j--) {
			statis->buf_burst[j] -= statis->buf_burst[j-DRAM_CLIENT_GROUP_NUM];
			statis->buf_mask[j] -= statis->buf_mask[j-DRAM_CLIENT_GROUP_NUM];
		}
	}

	/* search for the minimal BW for each client group */
	seq_puts(m, "[Minimal BW among each interval]\n");
	seq_puts(m, "  Module\t\tBandwidth(In MB/s)\t\tPercent\n");
	seq_puts(m, "  -----------\n");

	for (i = 0; i < DRAM_CLIENT_GROUP_NUM; i++) {
		min_burst = -1U;
		for (j = i; j < statis->buf_ptr; j += DRAM_CLIENT_GROUP_NUM) {
			if (min_burst > statis->buf_burst[j]) {
				min_burst = statis->buf_burst[j];
				buf_ptr = j;
			}
		}

		buf_ptr = rounddown(buf_ptr, DRAM_CLIENT_GROUP_NUM);

		total_bw = 0;

		for (j = 0; j < DRAM_CLIENT_GROUP_NUM; j++) {
			group_bw = get_group_bw(statis, buf_ptr+j);
			total_bw += group_bw;

			snprintf(group_name, sizeof(group_name), "%s%s%s",
				(i == j) ? "[" : "",
				G_dram_group_name[j],
				(i == j) ? "]" : "");

			ambdram_print_bandwidth(m, statis, group_name, group_bw);
		}

		ambdram_print_bandwidth(m, statis, "All", total_bw);

		seq_puts(m, "  -----------\n");
	}

	seq_puts(m, "\n");

	/* search for the maximal BW for total bandwidth */
	seq_puts(m, "[Maximal Total BW among each interval]\n");
	seq_puts(m, "  Module\t\tBandwidth(In MB/s)\t\tPercent\n");
	seq_puts(m, "  -----------\n");

	for (i = 0; i < statis->buf_ptr; i += DRAM_CLIENT_GROUP_NUM) {
		total_bw = 0;

		for (j = 0; j < DRAM_CLIENT_GROUP_NUM; j++) {
			group_bw = get_group_bw(statis, i+j);
			total_bw += group_bw;
			group_bw_tmp[j] = group_bw;
		}

		if (total_bw_max < total_bw) {
			total_bw_max = total_bw;
			for (j = 0; j < DRAM_CLIENT_GROUP_NUM; j++)
				group_bw_max[j] = group_bw_tmp[j];
		}
	}

	for (i = 0; i < DRAM_CLIENT_GROUP_NUM; i++)
		ambdram_print_bandwidth(m, statis, G_dram_group_name[i], group_bw_max[i]);

	ambdram_print_bandwidth(m, statis, "All", total_bw_max);

	seq_puts(m, "\n");

	if (statis->verbose == 0)
		return;

	seq_puts(m, "[Interval BW]\n");
	seq_puts(m, "  Module\t\tBandwidth(In MB/s)\t\tPercent\n");
	seq_puts(m, "  -----------\n");

	for (i = 0; i < statis->buf_ptr; i += DRAM_CLIENT_GROUP_NUM) {
		total_bw = 0;

		for (j = 0; j < DRAM_CLIENT_GROUP_NUM; j++) {
			group_bw = get_group_bw(statis, i+j);
			total_bw += group_bw;

			ambdram_print_bandwidth(m, statis, G_dram_group_name[j], group_bw);
		}

		ambdram_print_bandwidth(m, statis, "All", total_bw);

		seq_puts(m, "  -----------\n");
	}

	seq_puts(m, "\n");
}

static void ambdram_show_perf_info(struct seq_file *m, struct dram_statis *statis)
{
	if (ARCH_PLAT_VER >= ARCH_PLAT_VER_6) {
		seq_puts(m, "\n [perf info]\n");
		seq_printf(m,  " page miss counter = %u\n", statis->perf[0]);
		seq_printf(m,  " bg conflict counter = %u\n", statis->perf[1]);
		seq_printf(m,  " die switch counter = %u\n", statis->perf[2]);
		seq_printf(m,  " dtte         stall = %u\n", statis->perf[3]);
		seq_printf(m,  " wready       stall = %u\n", statis->perf[4]);
		seq_printf(m,  " cready       stall = %u\n", statis->perf[5]);
		seq_puts(m, "\n");
	}
}

static int ambdram_statis_proc_show(struct seq_file *m, void *v)
{
	struct dram_statis *statis = m->private;
	u64 hrtimer_expire = 0;

	if (statis->interval_ms > 0) {
		statis->buf_ptr = 0;
		statis->buf_size = statis->duration_ms / statis->interval_ms;
		statis->buf_size *= sizeof(u32) * DRAM_CLIENT_GROUP_NUM;
		statis->buf_size = min(statis->buf_size, DRAM_MAX_VMALLOC_SIZE);

		statis->buf_burst = vmalloc(statis->buf_size);
		if (statis->buf_burst == NULL)
			return -ENOMEM;

		statis->buf_mask = vmalloc(statis->buf_size);
		if (statis->buf_mask == NULL) {
			vfree(statis->buf_burst);
			return -ENOMEM;
		}
	}

	ambdram_start_statistics(statis);

	hrtimer_expire = statis->interval_ms ? statis->interval_ms : statis->duration_ms;
	hrtimer_start(&statis->timer, ms_to_ktime(hrtimer_expire), HRTIMER_MODE_REL);

	statis->elapsed_ms = 0;

	wait_event_interruptible(statis->wq, statis->elapsed_ms >= statis->duration_ms);
	hrtimer_cancel(&statis->timer);

	ambdram_show_basic_info(m, statis);
	ambdram_show_bw_info(m, statis);
	ambdram_show_interval_info(m, statis);
	ambdram_show_statis_info(m, statis);
	ambdram_show_perf_info(m, statis);

	if (statis->buf_burst) {
		vfree(statis->buf_burst);
		statis->buf_burst = NULL;
		vfree(statis->buf_mask);
		statis->buf_mask = NULL;
		statis->buf_size = 0;
	}

	BUG_ON(seq_has_overflowed(m));

	return 0;
}

#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
#if defined(DRV_DBG)
static void print_buf(struct dram_statis *statis, u32* buf)
{
	u32 j = 0;
    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        pr_info("Client[%d]: (%u, %u, %u)\n", j, \
            statis->request[j], \
            statis->burst[j], \
            statis->mask[j]);
    }

    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        pr_info("Client[%d] (from buf): (%u, %u, %u)\n", j, \
            buf[j], \
            buf[j + ARRAY_SIZE(G_dram_client)], \
            buf[j + ARRAY_SIZE(G_dram_client)*2]);
    }
}
#endif

static void ambdram_reset_info(struct dram_statis *statis)
{
	u32 i = 0, j = 0;

	for (i = statis->dram_ctrl_first; i < statis->dram_ctrl_end; i++) {
		for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
			statis->request[j] = 0;
			statis->burst[j]  = 0;
			statis->mask[j]  = 0;
		}
		for (j = 0; j < ARRAY_SIZE(statis->bank); j++) {
			statis->bank[j]  = 0;
		}
		statis->rw  = 0;
	}

    i = 0;
    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        G_statis_buf[i++] = statis->request[j];
    }
    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        G_statis_buf[i++] = statis->burst[j];
    }
    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        G_statis_buf[i++] = statis->mask[j];
    }
}

static void ambdram_get_info(struct dram_statis *statis)
{
	u32 i = 0, j = 0;
	void __iomem *dram_reg = NULL;

	for (i = statis->dram_ctrl_first; i < statis->dram_ctrl_end; i++) {
		for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
			dram_reg = statis->dram_reg + STAT_CLIENT_REQ_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->request[j] += readl_relaxed(dram_reg + j * 4);

			dram_reg = statis->dram_reg + STAT_CLIENT_BURSTS_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->burst[j] += readl_relaxed(dram_reg + j * 4);

			dram_reg = statis->dram_reg + STAT_CLIENT_MASK_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->mask[j] += readl_relaxed(dram_reg + j * 4);
			if ((statis->ddrc_num == 2) && (ARCH_PLAT_VER == ARCH_PLAT_VER_6))
				statis->mask[j] >>= 1;
		}

		for (j = 0; j < ARRAY_SIZE(statis->bank); j++) {
			dram_reg = statis->dram_reg + STAT_GLOBAL_BANK_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
			if (i % 2)
				dram_reg += 0x4000;
			statis->bank[j] += readl_relaxed(dram_reg + j * 4);
		}

		dram_reg = statis->dram_reg + STAT_GLOBAL_RW_OFFSET + (i / 2) * DRAM_CONFIG_REG_STEP;
		if (i % 2)
			dram_reg += 0x4000;
		statis->rw += readl_relaxed(dram_reg);
	}

    i = 0;
    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        G_statis_buf[i++] = statis->request[j];
    }
    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        G_statis_buf[i++] = statis->burst[j];
    }
    for (j = 0; j < ARRAY_SIZE(G_dram_client); j++) {
        G_statis_buf[i++] = statis->mask[j];
    }

#if defined(DRV_DBG)
    print_buf(statis, G_statis_buf);
#endif
}

static int ambdram_statis_proc_prof(struct seq_file *m, void *v)
{
	struct dram_statis *statis = m->private;

	ambdram_show_basic_info(m, statis);

    if(dramc_stats_enable) {
	    /* stop statistics */
	    writel_relaxed(STAT_CONTROL_OFF, statis->dram_reg + STAT_CONTROL_OFFSET);
	    ambdram_stop_perf_statis(statis);
        ambdram_get_info(statis);
        dramc_stats_enable = 0;
#if defined(DRV_DBG)
        pr_info("DRAMC stats profiling stop.\n");
        pr_info("DRAMC stats is now disabled.\n");
    } else {
        pr_info("DRAMC stats is now disabled.\n");
#endif
    }

    trigger_mode = 0;

	return 0;
}
#endif

static int ambdram_statis_proc_open(struct inode *inode, struct file *file)
{
	struct dram_statis *statis = &G_dram_statis;
	size_t size = 0, interval_num = 0;

#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
    if(trigger_mode)
    	return single_open(file, ambdram_statis_proc_prof, PDE_DATA(inode));
#endif

	/* basic + minial + interval + statistics */
	interval_num = (statis->interval_ms > 0) ? (statis->duration_ms / statis->interval_ms + 1) : 0;
	size = PER_INFO_ALLOC_SIZE * (1 + DRAM_CLIENT_GROUP_NUM + (interval_num + 1) * statis->verbose);

	return single_open_size(file, ambdram_statis_proc_show, PDE_DATA(inode), size);
}

static ssize_t ambdram_statis_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *ppos)
{
	struct dram_statis *statis = PDE_DATA(file_inode(file));
	int verbose = -1U, duration_ms = -1U, interval_ms = -1U;
	char buf[128] = {0}, *str = NULL;

	if (copy_from_user(buf, buffer, min(count, sizeof(buf))))
		return -EFAULT;

#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
	str = strstr(buf, "trigger=");
	if (str) {
		sscanf(str, "trigger=%d", &dramc_stats_enable);
        if(dramc_stats_enable == 1) {
            ambdram_reset_info(statis);
	        ambdram_start_statistics(statis);
#if defined(DRV_DBG)
            pr_info("DRAMC stats profiling start.\n");
#endif
        } else {
	        /* stop statistics */
	        writel_relaxed(STAT_CONTROL_OFF, statis->dram_reg + STAT_CONTROL_OFFSET);
	        ambdram_stop_perf_statis(statis);
            ambdram_get_info(statis);
#if defined(DRV_DBG)
            pr_info("DRAMC stats profiling stop.\n");
#endif
        }
        trigger_mode = 1;
    } else {
        trigger_mode = 0;
    }
#endif

	str = strstr(buf, "verbose=");
	if (str)
		sscanf(str, "verbose=%d", &verbose);

	str = strstr(buf, "duration=");
	if (str)
		sscanf(str, "duration=%d", &duration_ms);

	str = strstr(buf, "interval=");
	if (str)
		sscanf(str, "interval=%d", &interval_ms);

	/* Sanity check */
	if (duration_ms && interval_ms) {
		if (duration_ms < interval_ms)
			return -EINVAL;
	} else if (duration_ms) {
		if (duration_ms < statis->interval_ms)
			return -EINVAL;
	} else if (interval_ms) {
		if (statis->duration_ms < interval_ms)
			return -EINVAL;
	}

	if (duration_ms != -1)
		statis->duration_ms = duration_ms;

	if (interval_ms != -1)
		statis->interval_ms = interval_ms;

	if (verbose != -1)
		statis->verbose = !!verbose;

	if (statis->interval_ms)
		statis->duration_ms = roundup(statis->duration_ms, statis->interval_ms);

	return count;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
static const struct proc_ops proc_dram_statis_fops = {
	.proc_open = ambdram_statis_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_write = ambdram_statis_proc_write,
	.proc_release = single_release,
};
#else
static const struct file_operations proc_dram_statis_fops = {
	.owner = THIS_MODULE,
	.open = ambdram_statis_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.write = ambdram_statis_proc_write,
	.release = single_release,
};
#endif

static u32 calculate_ddrc_first(u32 reg_value)
{
	u32 ddrc_first = 0;

	if (ARCH_PLAT_VER == ARCH_PLAT_VER_6)
		ddrc_first = ((reg_value & 0x2) == 0x2) ? 0 : 1;
	else if (ARCH_PLAT_VER == ARCH_PLAT_VER_7)
		ddrc_first = reg_value & 0xf;
	else
		ddrc_first = 0;

	return ddrc_first;
}

static u32 calculate_ddrc_num(u32 reg_value)
{
	u32 ddrc_num = 0;

	if (ARCH_PLAT_VER == ARCH_PLAT_VER_6)
		ddrc_num = ((reg_value & 0x6) == 0x6) ? 2 : 1;
	else if (ARCH_PLAT_VER == ARCH_PLAT_VER_7)
		ddrc_num = (((reg_value >> 4) & 0xf) + 1) / 2;
	else
		ddrc_num = (reg_value & 0x1) ? 2 : 1;

	return ddrc_num;
}

static u32 calculate_dram_ctrl_num(u32 reg_value)
{
	u32 dram_ctrl_num = 0;

	if (ARCH_PLAT_VER <= ARCH_PLAT_VER_6)
		dram_ctrl_num = 1;
	else
		dram_ctrl_num = ((reg_value >> 4) & 0xf) + 1;

	return dram_ctrl_num;
}

static u32 calculate_ddrc_burst_size(u32 reg_value)
{
	u32 burst_size;

	if (ARCH_PLAT_VER == ARCH_PLAT_VER_5)
		burst_size = 16 * (1U << ((reg_value >> 3) & 0x3));
	else
		burst_size = 64;

	return burst_size;
}

static u32 calculate_ddrc_buswidth(u32 reg_value)
{
	u32 bus_width = 0;

	if (ARCH_PLAT_VER >= ARCH_PLAT_VER_6)
		bus_width = 32;		/* always 32-bit */
	else
		bus_width = (reg_value & 0x100) ? 16 : 32;

	return bus_width;
}

static u32 ddrc_dram_type(u32 reg_value)
{
	u32 ddr_type = DDR_TYPE_UNKNOWN;

	if (ARCH_PLAT_VER >= ARCH_PLAT_VER_6) {
		switch (reg_value & 3) {
		case 0:
			ddr_type = DDR_TYPE_LPDDR4;
			break;
		case 1:
			ddr_type = DDR_TYPE_LPDDR5;
			break;
		default:
			pr_err("%s: invalid dram type %d!\n", __func__, reg_value);
			break;
		}
	} else {
		/* bit 27~29 for dram type */
		reg_value = (reg_value >> 27) & 0x07;
		switch (reg_value) {
		case 5:
			ddr_type = DDR_TYPE_DDR4;
			break;
		case 6:
			ddr_type = DDR_TYPE_LPDDR4;
			break;
		default:
			pr_err("%s: invalid dram type %d!\n", __func__, reg_value);
			break;
		}
	}

	return ddr_type;
}

static int __init ambdram_init(void)
{
	struct dram_statis *statis = &G_dram_statis;
	u32 reg_value = 0;

	BUG_ON(ARRAY_SIZE(G_dram_client) > DRAM_MAX_CLIENT);

	statis->ddr_clock = clk_get_rate(clk_get_sys(NULL, DDR_CLOCK_NAME));
	if (statis->ddr_clock == 0) {
		pr_err("%s: get ddr frequency failed!\n", __func__);
		return -EINVAL;
	}

	statis->dram_reg = ioremap(DRAM_CONFIG_REG_PHY, DRAM_CONFIG_REG_SIZE);
	if (!statis->dram_reg) {
		pr_err("%s: iomap failed!\n", __func__);
		goto err_exit;
	}

	init_waitqueue_head(&statis->wq);

	reg_value = readl_relaxed(statis->dram_reg + DRAM_CONTROLLER_NUM_OFFSET);

	statis->ddrc_first = calculate_ddrc_first(reg_value);
	statis->ddrc_num = calculate_ddrc_num(reg_value);
	statis->ddrc_end = statis->ddrc_first + statis->ddrc_num;
	statis->dram_ctrl_first = statis->ddrc_first;
	statis->dram_ctrl_num = calculate_dram_ctrl_num(reg_value);
	statis->dram_ctrl_end = statis->dram_ctrl_first + statis->dram_ctrl_num;
	statis->burst_size = calculate_ddrc_burst_size(reg_value);

	reg_value = readl_relaxed(statis->dram_reg + DRAM_TYPE_OFFSET);
	statis->bus_width = calculate_ddrc_buswidth(reg_value);
	statis->bus_width *= statis->ddrc_num;
	statis->ddr_type = ddrc_dram_type(reg_value);

	statis->theory_bw = statis->ddr_clock * 2 * (statis->bus_width >> 3);

	hrtimer_init(&statis->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	statis->timer.function = ambdram_hrtimer_handler;
	statis->duration_ms = 1000;	/* defalut wait 1 seconds */
	statis->interval_ms = 0;	/* defalut no interval checking */
	statis->verbose = 0;

	proc_create_data("dram_statistics", S_IRUGO, get_ambarella_proc_dir(),
		&proc_dram_statis_fops, statis);

	return 0;

err_exit:
	if (statis->dram_reg != NULL)
		iounmap(statis->dram_reg);

	return -ENOMEM;
}

static void __exit ambdram_exit(void)
{
	remove_proc_subtree("dram_statistics", get_ambarella_proc_dir());
	iounmap(G_dram_statis.dram_reg);
}

module_init(ambdram_init);
module_exit(ambdram_exit);

MODULE_DESCRIPTION("Ambarella DRAM Driver");
MODULE_AUTHOR("Bingliang Hu <blhu@ambarella.com>");
MODULE_LICENSE("GPL v2");

#if defined(AMBA_DRAM_STAT_NEW_FORMAT)
module_param_array(G_statis_buf, uint, NULL, 0644);
#endif

