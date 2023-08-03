/*
 * iav_util.h
 *
 * History:
 *	2008/1/25 - [Oliver Li] created file
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


#ifndef __IAV_UTILS_H__
#define __IAV_UTILS_H__

// #include <config.h>
#if defined(CONFIG_ARCH_S5L) || defined(CONFIG_DRIVER_VERSION_V5) || defined(CONFIG_DRIVER_VERSION_V6)
#include <amba_arch_mem_lnx.h>
#endif
#include "iav_config.h"
#include "msg_print.h"
#if defined(CONFIG_ARCH_S5L) || defined(CONFIG_DRIVER_VERSION_V5) || defined(CONFIG_DRIVER_VERSION_V6)
#include "ambcma_api.h"
#endif
#if defined(CONFIG_DRIVER_VERSION_V6)
#include <bsp.h>
#include <plat_memory.h>
#endif
#define KB			(1024)
#define MB			(1024 * 1024)
#define KB_ALIGN(addr)		ALIGN(addr, KB)
#define MB_ALIGN(addr)		ALIGN(addr, MB)

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
#include <soc/ambarella/iav_helper.h>
#include <linux/spi/spi.h>
#include <soc/ambarella/spi.h>
#else
#include <plat/iav_helper.h>
#include <plat/spi.h>
#include <plat/fb.h>
#endif
#define clean_d_cache		ambcache_clean_range
#define invalidate_d_cache	ambcache_inv_range

#if defined(CONFIG_DRIVER_VERSION_V6)
/*
 * Take amba_addr_t as a glue layer between dsp_addr and phys_addr.
 * 1.When iav enc modules try to download addresses of configurations, IK etc to dsp,
 *   amba_addr_t become the glue layer between iav enc modules and dsp cmd download layer
 * 2.Used by buffer_cap queue buffer which aims at stitching producer(irq message[dsp_addr])
 *   and consumer(buffer query[phys_addr]).
 *
 *   +-----------------------+           +-----------------------------+
 *   |    iav enc modules    |           |       dequeue buffer        |
 *   |    (phys_addr)        |           |    (comsumer phys_addr)     |
 *   +-----------------------+           +-----------------------------+
 *              |                                      ^
 *              v                                      |
 *   +-----------------------+           +-----------------------------+
 *   |   address_glue_layer  |           |     address_glue_layer      |
 *   |    (amba_addr_t)      |           | (queue buffer amba_addr_t)  |
 *   +-----------------------+           +-----------------------------+
 *              |                                      ^
 *              v                                      |
 *   +-----------------------+           +-----------------------------+
 *   |dsp cmd download layer |           |    dsp irq msg procedure    |
 *   |    (dsp_addr)         |           |          (dsp_addr)         |
 *   +-----------------------+           +-----------------------------+
 *           Figure 1                               Figure 2
 */
typedef union {
	phys_addr_t phys_addr;
	u32 dsp_addr;
} amba_addr_t;

#define PHYS_TO_DSP(addr)	(u32)(addr)
/* When the total DRAM size >= 8GB, DSP memory 0 ~ 4GB is remapped to Physical space 4GB ~ 8GB.
 * Else, the DSP memory addr is equal to Physical memory addr.
 */
#if (IDSP_RAM_START == 4 * SIZE_GB)

/*
 *                                       +----------------------------------------------------------------+
 *                                       |                                                                |
 *                                       |        +--------------------------------------------------+    |
 *                                       |        |                                                  |    |
 *                                       |        +---------------------->IDSP_RAM_START=4GB         |    |
 *                                       |        |                 +---->4GB+IDSP_PRIVATE_SIZE_MB   |    |
 *                                       V        V                 |                                |    |
 *      +--------------------------------+--------+-----------------+---------------------------+    |    |
 *      |                                |        |                 |                           |    |    |
 *      +--------------------------------+--------+-----------------+---------------------------+    |    |
 *                                                ^                 ^                                |    |
 *                                                |                 |                                |    |
 *  +---------------------------------------------+                 |                                |    |
 *  |                        +--------------------------------------+                                |    |
 *  |                        |                                                                       |    |
 *  |      Mapped by ATT     |                                                                       |    |
 *  |                        |                                                                       |    |
 *  +----------------------- +------------+--------+                                                 |    |
 *  |                        |            |        |    DSP Memory layout(0~-4GB)                    |    |
 *  +----------------------- +------------+--------+                                                 |    |
 *  |<-IDSP_PRIVATE_SIZE_MB->|            |        |                                                 |    |
 *  |                        |            |        |                                                 |    |
 *  |                        |            |        |                                                 |    |
 *  |                        |            |        |                                                 |    |
 *  |                        |            |        +-------->IDSP_SHARED_END(4GB)--------------------+    |
 *  |                        |            |              Mapped by ATT                                    |
 *  |                        |            +----------------->IDSP_SHARED_START(4GB-IDSP_SHARE_SIZE_MB)----+
 *  |                        |
 *  |                        +------------------------------> IDSP_PRIVATE_END(0MB + IDSP_PRIVATE_SIZE_MB)
 *  |
 *  +------------------------------------------------------->IDSP_PRIVATE_START(0MB)
 *
 *
 * Due to the 32bit bus width, DSP and GDMA of archv6 can only access 0~4GB memory size. Here we use ATT to mmap
 * physical memory range[IDSP_RAM_START(4GB), IDSP_RAM_START(4GB) + IDSP_PRIVATE_SIZE_MB] to IDSP private memory
 * [IDSP_PRIVATE_START(0) ~ IDSP_PRIVATE_END(0+IDSP_PRIVATE_SIZE_MB)].On the contrary, We use ATT to mmap physical memory
 * range[IDSP_SHARED_START, IDSP_SHARED_END] directly to the same private dsp memory area[IDSP_SHARED_START, IDSP_SHARED_END]
 * So when DSP report a private IDSP_PRIVATE AREA address which is always smaller than IDSP_SHARED_START, converting DSP
 * private address to physical memory become a must step.
 */
#define DSP_TO_PHYS(addr)	\
	(unsigned long)(addr + ((unsigned long)(((u32)(addr) < IDSP_SHARED_START)) << 32))
#else
/*
 *                  Physical memory
 *  +-----------------+-------+---------------+
 *  |                 |       |               |
 *  +-----------------+-------+---------------+
 *  |                 |       |               |
 *  |                 |       |               |
 *  |                 |       |               |
 *  |                 |       |               +---------------------------------------------------------------->DRAM_END
 *  |                 |       |               |                                                         ^
 *  |                 |       |               +----------------->IDSP_PRIVATE_END                       |
 *  |                 |       |                       ^                                                 |
 *  |                 |       |                       |                                                 |
 *  |                 |       |                 IDSP_PRIVATE_SIZE_MB                                    |
 *  |                 |       |                       |                                                 |
 *  |                 |       |                       V                                                 |
 *  |                 |       +-------------------------------> IDSP_PRIVATE_START                  DRAM_SIZE <= 4GB
 *  |                 |       |                                                                         |
 *  |                 |       +------------------------------->IDSP_SHARED_END                          |
 *  |                 |                              ^                                                  |
 *  |                 |                              |                                                  |
 *  |                 |                         IDSP_SHARED_SIZE_MB                                     |
 *  |                 |                              |                                                  |
 *  |                 |                              v                                                  |
 *  |                 +--------------------------------------->IDSP_SHARED_START                        V
 *  +--------------------------------------------------------------------------------------------------------->DRAM_START
 *
 */
#define DSP_TO_PHYS(addr)	(unsigned long)(addr)
#endif
#else
/* ARM physical address to DSP address */
#define PHYS_TO_DSP(addr)	(unsigned long)(addr)
#define VIRT_TO_DSP(addr)	PHYS_TO_DSP(virt_to_phys(addr))			// kernel virtual address to DSP address
/* DSP address to ARM physical address */
#define DSP_TO_PHYS(addr)	(unsigned long)(addr)
#define DSP_TO_VIRT(addr)	phys_to_virt(DSP_TO_PHYS(addr))			// DSP address to kernel virtual address
#endif

#ifndef DRV_PRINT
#ifdef BUILD_AMBARELLA_PRIVATE_DRV_MSG
#define DRV_PRINT	print_drv
#else
#define DRV_PRINT	printk
#endif
#endif

#define amba_inc_idx_safe(write_index, max_item)		\
({		\
	wmb();		\
	(write_index) = ((((write_index) + 1) < (max_item)) ? ((write_index) + 1) : 0);		\
})

#define amba_dec_idx_safe(write_index, max_item)		\
({		\
	wmb();		\
	(write_index) = ((((write_index) - 1) >= 0) ? ((write_index) - 1) : ((max_item) - 1));		\
})

#define wake_event_after_signal(event, ms_time_out)		\
({		\
	int rval = -1, loop;		\
	iav_debug("Wake up by the signal.\n");		\
	loop = ms_time_out / HALF_SECOND_IN_MS;		\
	do {		\
		if (!loop) {		\
			break;		\
		}		\
		msleep(HALF_SECOND_IN_MS);		\
		if (event) {		\
			rval = 1;		\
			break;		\
		}		\
	} while(--loop);		\
	if (rval < 0) {		\
		iav_error("Failed to switch condition!\n");		\
	} else {		\
		iav_debug("Successfully switch condition!\n");		\
	}		\
	rval;		\
})

#define wake_event_after_signal_interval(event, ms_time_out, interval)		\
({		\
	int rval = -1, loop;		\
	iav_debug("Wake up by the signal.\n");		\
	loop = ms_time_out / interval;		\
	do {		\
		if (!loop) {		\
			break;		\
		}		\
		msleep(interval);		\
		if (event) {		\
			rval = 1;		\
			break;		\
		}		\
	} while(--loop);		\
	if (rval < 0) {		\
		iav_error("Failed to switch condition!\n");		\
	} else {		\
		iav_debug("Successfully switch condition!\n");		\
	}		\
	rval;		\
})

#define get_dsp_work_state(iav_state)		\
({		\
	u32 _dsp_work_state = DSP_WORK_STATE_IDLE;	\
	switch (iav_state) {	\
	case IAV_STATE_INIT:	\
		_dsp_work_state = DSP_WORK_STATE_INIT;	\
		break;	\
	case IAV_STATE_IDLE:	\
		_dsp_work_state = DSP_WORK_STATE_IDLE;	\
		break;	\
	case IAV_STATE_PREVIEW: \
		_dsp_work_state = DSP_WORK_STATE_PREVIEW;	\
		break;	\
	case IAV_STATE_ENCODING:	\
		_dsp_work_state = DSP_WORK_STATE_ENCODING;	\
		break;	\
	case IAV_STATE_DECODING:	\
		_dsp_work_state = DSP_WORK_STATE_DECODING;	\
		break;	\
	case IAV_STATE_EXITING_PREVIEW: \
		_dsp_work_state = DSP_WORK_STATE_EXITING_PREVIEW;	\
		break;	\
	default:	\
		iav_warn("Invalid iav state: %d.\n", iav_state);	\
		break;	\
	}	\
	_dsp_work_state;	\
})

#define get_iav_state(dsp_work_state)		\
({		\
	u32 _iav_state = IAV_STATE_IDLE; \
	switch (dsp_work_state) {	\
	case DSP_WORK_STATE_INIT:	\
		_iav_state = IAV_STATE_INIT; \
		break;	\
	case DSP_WORK_STATE_IDLE:	\
		_iav_state = IAV_STATE_IDLE; \
		break;	\
	case DSP_WORK_STATE_PREVIEW:	\
		_iav_state = IAV_STATE_PREVIEW;	\
		break;	\
	case DSP_WORK_STATE_ENCODING:	\
		_iav_state = IAV_STATE_ENCODING; \
		break;	\
	case DSP_WORK_STATE_DECODING:	\
		_iav_state = IAV_STATE_DECODING; \
		break;	\
	case DSP_WORK_STATE_EXITING_PREVIEW:	\
	case DSP_WORK_STATE_EXITING_PREVIEW_3A_STOPPED: \
		_iav_state = IAV_STATE_EXITING_PREVIEW;	\
		break;	\
	default:	\
		iav_warn("Invalid dsp_work_state: %d.\n", dsp_work_state);	\
		break;	\
	}	\
	_iav_state;	\
})

#define INVALID_FRAME_BUF_ID				(0xFFFFFFFF)

#define is_valid_dsp_addr(dsp_addr, base, limit)	\
({	\
	u8 _valid = ((dsp_addr) != 0) && ((dsp_addr) != 0xdeadbeef);	\
	if (_valid && (base) && (limit)) {	\
		_valid = ((dsp_addr) >= (base)) && ((dsp_addr) < (limit));	\
	}	\
	_valid;	\
})

/* NOTE: Since debug levels higher than KERN_DEBUG output msg through UART
   which blocks irq reaction time too long, this will affect DSP cmd transaction
   because DSP requires ARM to react it's interrupt as soon as possible in order
   to prevent DSP cmd loss. Here we change all levels to KERN_DEBUG. */

#if 0 // for debug usage
#define iav_trace()				DRV_PRINT(KERN_INFO ">>>> %s(%d)\n", __func__, __LINE__)
#define iav_debug(str, arg...)	DRV_PRINT(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_printk(str, arg...)	DRV_PRINT(KERN_INFO "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_error(str, arg...)	DRV_PRINT(KERN_ERR "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_warn(str, arg...)	DRV_PRINT(KERN_WARNING "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_info(str...)	DRV_PRINT(KERN_INFO str)
#else
#define iav_trace()				DRV_PRINT(KERN_DEBUG ">>>> %s(%d)\n", __func__, __LINE__)
#define iav_debug(str, arg...)	DRV_PRINT(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_printk(str, arg...)	DRV_PRINT(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg)
#define iav_error(str, arg...)	DRV_PRINT(KERN_DEBUG "#iav_error# %s(%d): "str, __func__, __LINE__, ##arg)
#define iav_warn(str, arg...)	DRV_PRINT(KERN_DEBUG "#iav_warn# %s(%d): "str, __func__, __LINE__, ##arg)
#define iav_info(str...)	DRV_PRINT(KERN_DEBUG str)
#endif

#ifndef CONFIG_AMBARELLA_VIN_DEBUG
#define vin_debug(format, arg...)
#else
#define vin_debug(str, arg...)	iav_debug("VIN: "str, ##arg)
#endif
#define vin_printk(str, arg...)	iav_printk(str, ##arg)
#define vin_error(str, arg...)	iav_error("VIN: "str, ##arg)
#define vin_warn(str, arg...)	iav_warn("VIN: "str, ##arg)
#define vin_info(str, arg...)	iav_info("VIN: "str, ##arg)

#define vout_debug(str, arg...)	iav_debug("VOUT: "str, ##arg)
#define vout_error(str, arg...)	iav_error("VOUT: "str, ##arg)
#define vout_warn(str, arg...)	iav_warn("VOUT: "str, ##arg)
#define vout_info(str, arg...)	iav_info("VOUT: "str, ##arg)

#endif	// UTIL_H

