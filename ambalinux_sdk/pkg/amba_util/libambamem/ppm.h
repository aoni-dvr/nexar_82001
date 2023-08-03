/*
 * include/linux/ppm.h
 *
 * Copyright 2014 Ambarella Inc.
 *
 */

#ifndef _PPM_H
#define _PPM_H

#include <linux/ioctl.h>

struct PPM_MEM_INFO_s {
	unsigned long virt;
	unsigned long phys;
	unsigned long size;
};

#define __PPM_MEM		0x99

#define PPM_GET_MEMIO_INFO      _IOR(__PPM_MEM, 1, struct PPM_MEM_INFO_s)
#define PPM_PREPARE_CVRTOS      _IOR(__PPM_MEM, 2, int)
#define PPM_GET_INFO_PPM        _IOR(__PPM_MEM, 3, struct PPM_MEM_INFO_s)
#define PPM_GET_INFO_PPM2       _IOR(__PPM_MEM, 4, struct PPM_MEM_INFO_s)
#define PPM_GET_INFO_CVRTOS     _IOR(__PPM_MEM, 5, struct PPM_MEM_INFO_s)
#define PPM_CACHE_CLEAN         _IOW(__PPM_MEM, 6, struct PPM_MEM_INFO_s)
#define PPM_CACHE_INVALID       _IOW(__PPM_MEM, 7, struct PPM_MEM_INFO_s)

#endif	/* _PPM_H */
