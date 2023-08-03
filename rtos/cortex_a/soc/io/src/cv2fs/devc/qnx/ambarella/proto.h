/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#define SER_STTY 0
#define SER_INIT 1

#define AmbaMisra_TypeCast32(a, b) memcpy((void *)a, (void *)b, 4)

void        create_device(TTYINIT_AMBA *dip, unsigned unit);
void        ser_stty(DEV_UART *dev, int init);
void        ser_ctrl(DEV_UART *dev, unsigned flags);
void        ser_attach_intr(DEV_UART *dev);
void *      query_default_device(TTYINIT_AMBA *dip, void *link);
unsigned    options(int argc, char *argv[]);

