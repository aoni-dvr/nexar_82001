/*
 *  fs/partitions/ambptb.c
 *
 * Author: Anthony Ginger <hfjiang@ambarella.com>
 *
 * Copyright (C) 2004-2010, Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "check.h"
#include "ambptb.h"
#include <linux/of.h>

#if 0
#define ambptb_prt printk
#else
#define ambptb_prt(format, arg...) do {} while (0)
#endif

#if defined(CONFIG_ARCH_AMBARELLA_AMBALINK) || defined(CONFIG_ARCH_AMBA_CAMERA)
int G_ambptb_ready = 0;
EXPORT_SYMBOL(G_ambptb_ready);
int ambptb_partition(struct parsed_partitions *state)
{
	u32 sect_size;
	char *part_label;
	int i, result = 0;
	struct device_node *ofpart_node;
	struct device_node *pp;
	u64 div_result;

	/* emmc MUST be mmcblk0 */
	if (strncmp(state->pp_buf, " mmcblk0", strlen(" mmcblk0"))) {
		result = -1;
		goto ambptb_partition_exit;
	}

	sect_size = queue_logical_block_size(state->disk->queue);

	ofpart_node = of_get_parent(of_find_node_by_name(NULL, "partition"));
	if(!ofpart_node) {
		pr_err("device node partition is not found!");
		return -1;
	}
	/*we only handle ambarella emmc*/
	if (!of_device_is_compatible(ofpart_node, "ambarella,sdmmc") &&
		!of_device_is_compatible(ofpart_node, "ambarella,sdhci")) {
		return -1;
	}

	i = 0;
	for_each_child_of_node(ofpart_node,  pp) {
		const __be32 *reg;
		int len;
		int a_cells, s_cells;

		reg = of_get_property(pp, "reg", &len);
		if (!reg) {
			continue;
		}

		a_cells = of_n_addr_cells(pp);
		s_cells = of_n_size_cells(pp);
		if (len / 4 != a_cells + s_cells) {
			pr_debug("%s: partition %s error parsing reg property.\n",
				 __func__, pp->full_name);
			goto ambptb_partition_exit;
		}

		div_result = of_read_number(reg, a_cells);
		do_div(div_result, sect_size);
		state->parts[i].from = div_result;

		div_result = of_read_number(reg + a_cells, s_cells);
		do_div(div_result, sect_size);
		state->parts[i].size = div_result;

		part_label = (char *) of_get_property(pp, "label", &len);
		if (!part_label)
			part_label = (char *) of_get_property(pp, "name", &len);
		strlcat(state->pp_buf, part_label, PAGE_SIZE);
		strlcat(state->pp_buf, " ", PAGE_SIZE);

		printk(KERN_NOTICE "0x%012llx-0x%012llx : \"[p%d] %s\"\n",
			(unsigned long long)state->parts[i].from,
			(unsigned long long)(state->parts[i].from + state->parts[i].size),
			i, part_label);

		i++;
	}
	G_ambptb_ready = 1;

	strlcat(state->pp_buf, "\n", PAGE_SIZE);
	result = 1;

ambptb_partition_exit:
	return result;
}
#else
int ambptb_partition(struct parsed_partitions *state)
{
	struct device_node *np = NULL, *pp;
	struct device *dev;
	int sector_size, index = 1, rval = -1;

	dev = disk_to_dev(state->disk);

	while (dev) {
		if (of_device_is_compatible(dev->of_node, "ambarella,sdmmc") ||
			of_device_is_compatible(dev->of_node, "ambarella,sdhci")) {
			np = of_get_child_by_name(dev->of_node, "partitions");
			break;
		}

		dev = dev->parent;
	}

	if (np == NULL)
		return 0;

	sector_size = queue_logical_block_size(state->disk->queue);

	for_each_child_of_node(np,  pp) {
		char str_tmp[1 + BDEVNAME_SIZE + 10 + 1];
		const char *partition_name;
		const __be32 *reg;
		int len, a_cells, s_cells;
		u64 addr, size;

		reg = of_get_property(pp, "reg", &len);
		if (!reg)
			goto exit;

		a_cells = of_n_addr_cells(pp);
		s_cells = of_n_size_cells(pp);
		if (len / 4 != a_cells + s_cells)
			goto exit;

		addr = of_read_number(reg, a_cells);
		do_div(addr, sector_size);
		size = of_read_number(reg + a_cells, s_cells);
		do_div(size, sector_size);

		put_partition(state, index++, addr, size);

		partition_name = of_get_property(pp, "label", &len);
		if (!partition_name)
			goto exit;

		snprintf(str_tmp, sizeof(str_tmp), "(%s)", partition_name);
		strlcat(state->pp_buf, str_tmp, PAGE_SIZE);

		printk(KERN_INFO " %s: 0x%016llx, 0x%016llx\n", str_tmp,
			of_read_number(reg, a_cells),
			of_read_number(reg + a_cells, s_cells));
	}

	strlcat(state->pp_buf, "\n", PAGE_SIZE);
	rval = 1;

exit:
	if (np)
		of_node_put(np);
	return rval;
}
#endif
