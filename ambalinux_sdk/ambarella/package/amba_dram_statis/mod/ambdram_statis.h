/*
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

#ifndef _AMBARELLA_DRAM_H
#define _AMBARELLA_DRAM_H

#if defined(ambarella_soc_cv5)
#define DRAM_CONFIG_REG_PHY		0x1000000000UL
#define DRAM_CONFIG_REG_SIZE		0x0000C000
#define DRAM_CONFIG_REG_STEP		0x00001000
#define DRAM_TYPE_OFFSET		0x5004
#define MISC2_STAT_COLLECT_CTRL		0x53C0
#define MISC2_PERF_INFO_BEGIN		0x53a8
#define STAT_CLIENT_REQ_OFFSET		0x1500
#define STAT_CLIENT_BURSTS_OFFSET	0x1580
#define STAT_CLIENT_MASK_OFFSET		0x1600
#define STAT_GLOBAL_BANK_OFFSET		0x1680
#define STAT_GLOBAL_RW_OFFSET		0x1800
#define STAT_CONTROL_OFFSET		0x1808
#define DDR_CLOCK_NAME			"gclk_ddr0"
#elif defined(ambarella_soc_cv3)
#define DRAM_CONFIG_REG_PHY		0xFF08000000UL
#define DRAM_CONFIG_REG_SIZE		0x00080000
#define DRAM_CONFIG_REG_STEP		0x0000C000
#define DRAM_TYPE_OFFSET		0x14004
#define MISC2_STAT_COLLECT_CTRL		0x143C0
#define MISC2_PERF_INFO_BEGIN		0x143A8
#define STAT_CLIENT_REQ_OFFSET		0x0E100
#define STAT_CLIENT_BURSTS_OFFSET	0x0E300
#define STAT_CLIENT_MASK_OFFSET		0x0E500
#define STAT_GLOBAL_BANK_OFFSET		0x0E700
#define STAT_GLOBAL_RW_OFFSET		0x0E800
#define STAT_CONTROL_OFFSET		0x0680C	/* host ctrl broadcast */
#define DDR_CLOCK_NAME			"gclk_ddr0"
#else
#define DRAM_CONFIG_REG_PHY		0xdffe0000
#define DRAM_CONFIG_REG_SIZE		0x00002000
#define DRAM_CONFIG_REG_STEP		0x00000000
#define DRAM_TYPE_OFFSET		0x0804
#define STAT_CLIENT_REQ_OFFSET		0x1000
#define STAT_CLIENT_BURSTS_OFFSET	0x1080
#define STAT_CLIENT_MASK_OFFSET		0x1100
#define STAT_GLOBAL_BANK_OFFSET		0x1180
#define STAT_GLOBAL_RW_OFFSET		0x11A0
#define STAT_CONTROL_OFFSET		0x11C0
#define DDR_CLOCK_NAME			"gclk_ddr"
#endif
#define DRAM_CONTROLLER_NUM_OFFSET	0x0000

#define STAT_CONTROL_OFF		0x0
#define STAT_CONTROL_ON			0x1
#define STAT_CONTROL_RESET		0x2

#define DRAM_MAX_CLIENT			(128)
#define DRAM_MAX_BANK			(8)
#define DRAM_MAX_VMALLOC_SIZE		(128U * 1024 * 1024)

#define DRAM_MAX_PERF			(6)

#define PER_INFO_ALLOC_SIZE		(1 * 1024)

enum {
	DDR_TYPE_LPDDR4	= 0,
	DDR_TYPE_DDR4	= 1,
	DDR_TYPE_LPDDR5	= 2,
	DDR_TYPE_UNKNOWN,
};

struct dram_statis {
	void __iomem *dram_reg;
	struct hrtimer timer;
	wait_queue_head_t wq;

	u32 elapsed_ms;
	u32 duration_ms;
	u32 interval_ms;
	u32 *buf_burst;
	u32 *buf_mask;
	u32 buf_size;
	u32 buf_ptr;
	u32 verbose;

	/* dram info */
	u64 theory_bw;
	u64 ddr_clock;
	u32 bus_width : 16;
	u32 ddr_type : 8;
	u32 burst_size : 8;
	u32 ddrc_num : 8;
	u32 ddrc_first : 8;
	u32 ddrc_end : 8;
	u32 dram_ctrl_num : 8;
	u32 dram_ctrl_first : 8;
	u32 dram_ctrl_end : 8;

	/* statis data */
	u32 request[DRAM_MAX_CLIENT];
	u32 burst[DRAM_MAX_CLIENT];
	u32 mask[DRAM_MAX_CLIENT];
	u32 bank[DRAM_MAX_BANK];
	u32 perf[DRAM_MAX_PERF];
	u32 rw;
};

#endif //_AMBARELLA_DRAM_H

