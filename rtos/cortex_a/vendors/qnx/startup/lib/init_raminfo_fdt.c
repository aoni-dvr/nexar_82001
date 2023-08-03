/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include "startup.h"
#include <libfdt.h>
#include <libfdt_private.h>

#define REG_CHUNK	8
int

#define fdt_for_each_subnode(node, fdt, parent)		\
	for (node = fdt_first_subnode(fdt, parent);	\
	     node >= 0;					\
	     node = fdt_next_subnode(fdt, node))

init_raminfo_fdt(void) {
	int nrsv,onrsv;
	int node = -1;
	int sub_node = -1;
	uint64_t    mem_addr[REG_CHUNK], mem_size[REG_CHUNK];
	int         i, mem_num;

	if(fdt == NULL) {
		return 0;
	}

	// put fdt reserved memory to the avoid list first
	// otherwise, it could potentially overlap with those allocated during add_rams.
	onrsv = nrsv = fdt_num_mem_rsv(fdt);
	while(onrsv-- > 0) {
		uint64_t		start, size;

		if(fdt_get_mem_rsv(fdt, onrsv, &start, &size) >= 0) {
		    avoid_ram(start, size);
		}
	}

	// Find memory node and add ram
	node = -1;
	node = fdt_node_offset_by_prop_value(fdt, node, "device_type", "memory", sizeof("memory"));
	if(node < 0) {
		crash("Could not find memory node\n");
	}
	mem_num = fdt_get_reg64_cells(fdt, node, 0, REG_CHUNK, mem_addr, mem_size, -1, -1);
	if(mem_num <= 0) {
		crash("Could not find memory reg \n");
	}
	for(i = 0; i < mem_num; i++) {
		add_ram(mem_addr[i], mem_size[i]);
		kprintf("memory: start(0x%x), size(0x%x) \n",mem_addr[i],mem_size[i]);
	}

	// alloc reserved memory
	while(nrsv-- > 0) {
		uint64_t		start, size;

		if(fdt_get_mem_rsv(fdt, nrsv, &start, &size) >= 0) {
			alloc_ram(start, size, 1);
		}
	}

	// Find reserved memory node
	node = -1;
	node = fdt_node_offset_by_prop_value(fdt, node, "device_type", "reserved-memory", sizeof("reserved-memory"));
	if(node >= 0) {
		fdt_for_each_subnode(sub_node, fdt, node) {
			uint64_t	addr, size;
			int	len;
			char *name;
			paddr_t reserved;
			void *prop;

			name = fdt_get_name(fdt, sub_node, &len);
			if(fdt_get_reg64_cells(fdt, sub_node, 0, REG_CHUNK, &addr, &size, -1, -1) <= 0 ) {
				continue;
			}
			if(size > 0) {
				for(i = 0; i < mem_num; i++) {
					if((mem_addr[i] <= addr) && ((mem_addr[i]+mem_size[i]) >= (addr+size))) {
						prop = fdt_getprop(fdt, sub_node, "reusable", &len);
						if (prop) {
							reserved = alloc_ram(addr, size, 0x1000U);
							if(reserved == NULL_PADDR) {
								crash("Could not reserve 0x%x bytes of memory.\n", size);
							}
							as_add_containing(reserved, (reserved + size - 1), AS_ATTR_RAM, name, "ram");
							kprintf("reserved-memory(reusable): name %s start(0x%x), size(0x%x) \n",name,addr,size);
						}
						prop = fdt_getprop(fdt, sub_node, "no-map", &len);
						if (prop) {
							avoid_ram(addr, size);
							kprintf("reserved-memory(no-map): name %s start(0x%x), size(0x%x) \n",name,addr,size);
						}
					}
				}
			}
		}
	}

	return 1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/startup/lib/init_raminfo_fdt.c $ $Rev: 886103 $")
#endif
