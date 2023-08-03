/*
 * ambarella/amalgam.h
 *
 * Amalgam Network ARM on QEMU device driver
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#ifndef AMBARELLA_AMALGAM_H
#define AMBARELLA_AMALGAM_H

#if defined(CONFIG_AMALGAM)

extern unsigned int am_domid(void);
extern int am_is_connected(void);
extern int am_set_target(const char *host, uint16_t port);
extern int am_connect(void);
extern int am_disconnect(void);
extern int am_echo(const char *s);
extern int am_memwr(const void *data, size_t len);
extern int am_memrd(void *data, size_t len);
extern int am_sync(uint32_t counter);
extern int am_interrupt(uint32_t client, uint32_t irq);
extern int am_get_irq_status(uint32_t *status);

#endif  /* CONFIG_AMALGAM */

#endif
