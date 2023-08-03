/**
 * system/include/hwio.h
 *
 * History:
 *    2004/11/29 - [Charles Chiou] created file
 *
 * This file provides I/O operation wrappers for OS'es that do not already
 * provide it (such as PrKERNELv4). In fact, even if it did, we'd like to
 * ideally use the following ones to be more uniform/portable.
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __HWIO_H__
#define __HWIO_H__
typedef unsigned long int uintptr_t;
typedef long int          intptr_t;
typedef unsigned long long bus_address_t;
#ifndef __ASM__
#define __raw_writeb(v, a)	(*(volatile unsigned char  *)((uintptr_t)a) = (v))
#define __raw_writew(v, a)	(*(volatile unsigned short *)((uintptr_t)a) = (v))
#define __raw_writel(v, a)	(*(volatile unsigned int   *)((uintptr_t)a) = (v))
#define __raw_writeq(v, a)	(*(volatile u64_t          *)((uintptr_t)a) = (v))

#define __raw_readb(a)		(*(volatile unsigned char  *)((uintptr_t)a))
#define __raw_readw(a)		(*(volatile unsigned short *)((uintptr_t)a))
#define __raw_readl(a)		(*(volatile unsigned int   *)((uintptr_t)a))
#define __raw_readq(a)		(*(volatile u64_t          *)((uintptr_t)a))

#define writeb(p, v)	__raw_writeb(v, p)
#define writew(p, v)	__raw_writew(v, p)
#define writel(p, v)	__raw_writel(v, p)
#define writeq(p, v)	__raw_writeq(v, p)

#define readb(p)	__raw_readb(p)
#define readw(p)	__raw_readw(p)
#define readl(p)	__raw_readl(p)
#define readq(p)	__raw_readq(p)

/* read back a pointer */
#define readp(p)	((uintptr_t)__raw_readl(p))

#define setbitsl(p, mask)  writel((p),(readl(p) | (mask)))
#define clrbitsl(p, mask)  writel((p),(readl(p) & ~(mask)))

#endif /* !__ASM__ */

#endif
