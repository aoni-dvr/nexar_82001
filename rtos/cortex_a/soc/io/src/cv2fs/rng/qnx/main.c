/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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
#include <stdio.h>
#include "AmbaTypes.h"
#include "proto.h"
#include <AmbaCSL_Scratchpad.h>

AMBA_RCT_REG_s * pAmbaRCT_Reg;
AMBA_SCRATCHPAD_S_REG_s * pAmbaScratchpadS_Reg;

int main(int argc, char *argv[])
{
    int id;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs;
    dispatch_t *dpp;
    resmgr_attr_t rattr;
    dispatch_context_t *ctp;
    iofunc_attr_t ioattr;

    (void)argc;

    /* Initialize the dispatch interface */
    dpp = dispatch_create();
    if (!dpp) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "RNG error: Failed to create dispatch interface\n");
        goto fail;
    }

    /* Initialize the resource manager attributes */
    memset(&rattr, 0, sizeof(rattr));

    /* Initialize the connect functions */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.devctl = rng_io_devctl;
    iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

    /* Attach the device name */
    id = resmgr_attach(dpp, &rattr, "/dev/rng", _FTYPE_ANY, 0,
                       &connect_funcs, &io_funcs, &ioattr);
    if (id == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "RNG error: Failed to attach pathname\n");
        goto fail;
    }

    /* Allocate a context structure */
    ctp = dispatch_context_alloc(dpp);

    if (0x0 != rng_dev_init()) {
        fprintf(stderr, "rng_dev init fail %s(%d)\n", __func__, __LINE__);
    }

    /* Run in the background */
    if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE
                       | PROCMGR_DAEMON_NODEVNULL ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
              argv[0]);
        goto fail;
    }

    printf("rng driver init done\n");

    while (1) {
        if ((ctp = dispatch_block(ctp)) == NULL) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "RNG error: Block error\n");
            goto fail;
        }
        dispatch_handler(ctp);
    }

fail:

    return EXIT_SUCCESS;
}

int rng_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;

    rng_msg_t       *rng_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch (msg->i.dcmd) {

    case DCMD_RNG_ENABLE:
        rng_msg = _DEVCTL_DATA(msg->i);
        AmbaRTSL_RngInit();
        break;

    case DCMD_RNG_OUTPUT:
        rng_msg = _DEVCTL_DATA(msg->i);
        AmbaRTSL_RngDataGet(&rng_msg->data0, &rng_msg->data1, &rng_msg->data2, &rng_msg->data3, &rng_msg->data4);
        nbytes = sizeof(rng_msg_t);
        break;

    case DCMD_RNG_DISABLE:
        rng_msg = _DEVCTL_DATA(msg->i);
        AmbaRTSL_RngDeinit();
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
