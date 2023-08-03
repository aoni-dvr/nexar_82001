/*
 * msg_print.h
 *
 * History:
 *    2012/05/13 - [Rongrong Cao] Create
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

#ifndef __PRINT_PRIVATE_DRV__
#define __PRINT_PRIVATE_DRV__
#include <linux/compiler.h>
#include <linux/types.h>
extern int print_drv(const char *fmt, ...);
extern int do_drv_syslog(int type, char __user *buf, int len, bool from_file);
#endif
