/*
 * $QNXLicenseC:
 * Copyright 2009, 2018, QNX Software Systems.
 * Copyright 2013, Adeneo Embedded.
 * Copyright 2020, Ambarella International LP
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

#include "Amba_timer.h"
#include <sys/siginfo.h>

unsigned int TimerAvail = 0x3ffffU;

int main(int argc, char *argv[])
{
    int id;
    int option;
    int verbose = 0;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs;
    dispatch_t *dpp;
    resmgr_attr_t rattr;
    dispatch_context_t *ctp;
    iofunc_attr_t ioattr;

    /* Initialize the dispatch interface */
    dpp = dispatch_create();
    if (!dpp) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "Clock error: Failed to create dispatch interface\n");
        goto fail;
    }

    /* Initialize the resource manager attributes */
    memset(&rattr, 0, sizeof(rattr));

    /* Attach the device name */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.devctl = timer_io_devctl;
    iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

    id = resmgr_attach(dpp, &rattr, "/dev/amba_tmr", _FTYPE_ANY, 0,
                       &connect_funcs, &io_funcs, &ioattr);
    if (id == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "Clock error: Failed to attach pathname\n");
        goto fail;
    }

    /* Allocate a context structure */
    ctp = dispatch_context_alloc(dpp);

    if (0x0 != Amba_Timer_Init()) {
        printf( "timer_dev init fail %s(%d)\n", __func__, __LINE__);
    }

    while ( (option = getopt(argc, argv, "v")) != -1) {
        switch (option) {
        case 'v':
            verbose++;
            break;
        default:
            fprintf(stderr, "Unsupported option '-%c'\n", option);
            goto fail;
        }
    }

    /* Run in the background */
    if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
              argv[0]);
        goto fail;
    }

    printf("internal timer driver init done\n");

    while (1) {
        if ((ctp = dispatch_block(ctp)) == NULL) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "Clock error: Block error\n");
            goto fail;
        }
        dispatch_handler(ctp);
    }

fail:

    return EXIT_SUCCESS;
}

static unsigned int TimerFindAvail(void)
{
    unsigned int i;
    unsigned int Tmp = TimerAvail;

    for (i = 0U; i < AMBA_NUM_TIMER; i++) {
        if ((Tmp & 0x1U) != 0x0U) {
            break;
        }
        Tmp = Tmp >> 1U;
    }

    return i;
}

static int TimerLock(unsigned int TimerID)
{
    unsigned int Mask = (1U << TimerID);
    int Ret = TimerID;

    if (0x0U != (TimerAvail & Mask)) {
        TimerAvail = TimerAvail & (~Mask);
    } else {
        Ret = -1;
    }

    return Ret;
}

static void TimerRelease(unsigned int TimerID)
{
    unsigned int Mask = (1U << TimerID);

    TimerAvail = TimerAvail | Mask;
}

int timer_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    amba_timer_t *pTimer;
    timer_info_t *pTimerInfo;
    unsigned int NumPeriodicTick;
    AMBA_TMR_INFO_s TimerInfo = {0U};
    int err = EOK;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_TIMER_CONFIG:
        pTimer = _DEVCTL_DATA(msg->i);
        (void)AmbaRTSL_TmrConfig(pTimer->TimerId, pTimer->TimerFreq, pTimer->NumPeriodicTick);
        break;

    case DCMD_TIMER_START:
        pTimer = _DEVCTL_DATA(msg->i);
        (void)AmbaRTSL_TmrStart(pTimer->TimerId, pTimer->NumRemainTick);
        break;

    case DCMD_TIMER_STOP:
        pTimer = _DEVCTL_DATA(msg->i);
        (void)AmbaRTSL_TmrStop(pTimer->TimerId);
        break;

    case DCMD_TIMER_SHOW:
        pTimer = _DEVCTL_DATA(msg->i);
        (void)AmbaRTSL_TmrShowTickCount(pTimer->TimerId, &NumPeriodicTick);
        pTimer->NumRemainTick = NumPeriodicTick;
        nbytes = sizeof(amba_timer_t);
        break;

    case DCMD_TIMER_GETINFO:
        pTimerInfo = _DEVCTL_DATA(msg->i);
        (void)AmbaRTSL_TmrGetInfo(pTimerInfo->TimerId, &TimerInfo);
        pTimerInfo->SysFreq = TimerInfo.SysFreq;
        pTimerInfo->TimerFreq = TimerInfo.TimerFreq;
        pTimerInfo->PeriodicInterval = TimerInfo.PeriodicInterval;
        pTimerInfo->ExpireCount = TimerInfo.ExpireCount;
        pTimerInfo->State = TimerInfo.State;
        nbytes = sizeof(timer_info_t);
        break;

    case DCMD_TIMER_GETAVAIL:
        pTimer = _DEVCTL_DATA(msg->i);
        pTimer->TimerId = TimerFindAvail();
        nbytes = sizeof(amba_timer_t);
        break;

    case DCMD_TIMER_LOCK:
        pTimer = _DEVCTL_DATA(msg->i);
        pTimer->TimerId = TimerLock(pTimer->TimerId);
        nbytes = sizeof(amba_timer_t);
        break;

    case DCMD_TIMER_RELEASE:
        pTimer = _DEVCTL_DATA(msg->i);
        TimerRelease(pTimer->TimerId);
        break;
    }

    if (nbytes == 0) {
        return (err);
    } else {
        msg->o.ret_val = 0;
        msg->o.nbytes = nbytes;
        return (_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
    }
}
