/*
 * iav_devnum.h
 *
 * History:
 *    2012/10/25 - [Cao Rongrong] Create
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


#ifndef __IAV_DEV_NUM_H__
#define __IAV_DEV_NUM_H__

#define	AMBA_DEV_MAJOR			(248)
#define	AMBA_DEV_MINOR_PUBLIC_START	(128)
#define	AMBA_DEV_MINOR_PUBLIC_END	(240)

#define IAV_DEV_MAJOR			AMBA_DEV_MAJOR
#define IAV_DEV_MINOR			0

#define UCODE_DEV_MAJOR			AMBA_DEV_MAJOR
#define UCODE_DEV_MINOR			1

#define DSPLOG_DEV_MAJOR		AMBA_DEV_MAJOR
#define DSPLOG_DEV_MINOR		(AMBA_DEV_MINOR_PUBLIC_END + 9)

#endif

