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

#include "externs.h"

void *ser_term_thread (void *arg)
{
    sigset_t signals;

    (void) arg;
    if (ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Unable to get I/O privity: %s", __FUNCTION__, strerror(errno));
        _exit(EXIT_FAILURE);
    }

    sigemptyset (&signals);
    sigaddset (&signals, SIGTERM);
    sigaddset (&signals, SIGINT);
    sigaddset(&signals, SIGBUS);

    sigwaitinfo (&signals, NULL);

    _exit (0);
}

int main(int argc, char *argv[])
{
    pthread_attr_t attr;
    sigset_t signals;

    pthread_attr_init(&attr);

    sigfillset (&signals);
    pthread_sigmask (SIG_BLOCK, &signals, NULL);

    pthread_create (NULL, &attr, ser_term_thread, NULL);

    ttyctrl.max_devs = 1;
    ttc(TTC_INIT_PROC, &ttyctrl, 24);

    if (options(argc, argv) == 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: No serial ports found", __FUNCTION__);
        exit(0);
    }

    ttc(TTC_INIT_START, &ttyctrl, 0);

    return 0;
}

