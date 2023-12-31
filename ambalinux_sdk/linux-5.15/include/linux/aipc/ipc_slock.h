/*
 * include/linux/aipc/ipc_slock.h
 *
 * Authors:
 *	Joey Li <jli@ambarella.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * Copyright (C) 2013-2015, Ambarella Inc.
 */

#ifndef __AIPC_SPINLOCK_H__
#define __AIPC_SPINLOCK_H__

/* all OS define the same spinlock id */
#define AMBA_IPC_SPINLOCK_GPIO          0U
#define AMBA_IPC_NUM_SPINLOCK           1U

int aipc_spin_lock_setup(unsigned long addr);
void aipc_spin_lock(int id);
void aipc_spin_unlock(int id);
void aipc_spin_lock_irqsave(int id, unsigned long *flags);
void aipc_spin_unlock_irqrestore(int id, unsigned long flags);

#endif	/* __AIPC_SPINLOCK_H__ */

