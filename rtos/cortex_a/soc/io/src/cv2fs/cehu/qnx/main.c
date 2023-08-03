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
#include <AmbaTypes.h>
#include "proto.h"

static int dev_id = -1;

int main(int argc, char *argv[])
{
    static iofunc_attr_t ioattr;
    static char *dev_name = "/dev/cehu";

    int id;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs;
    dispatch_t *dpp;
    resmgr_attr_t rattr;
    dispatch_context_t *ctp;
    int flag_leave = 0;
    int i;

    (void)argc;

    /* Initialize the dispatch interface */
    dpp = dispatch_create();
    if (!dpp) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "CEHU error: Failed to create dispatch interface\n");
    } else {
        /* Initialize the resource manager attributes */
        memset(&rattr, 0, sizeof(rattr));

        /* Initialize the connect functions */
        iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                         _RESMGR_IO_NFUNCS, &io_funcs);
        io_funcs.devctl = cehu_io_devctl;
        io_funcs.write  = cehu_io_write;

        iofunc_attr_init(&ioattr, S_IFCHR | 0600, NULL, NULL);

        /* Attach the device name */
        id = resmgr_attach(dpp, &rattr, dev_name, _FTYPE_ANY, 0,
                           &connect_funcs, &io_funcs, &ioattr);
        if (id == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to attach pathname %s\n", dev_name);
            flag_leave = 1;
        } else {
            dev_id = id;
        }

        if (flag_leave == 0) {

            /* Allocate a context structure */
            ctp = dispatch_context_alloc(dpp);

            if (0x0 != cehu_dev_init()) {
                fprintf(stderr, "cehu_dev init fail %s(%d)\n", __func__, __LINE__);
            }

            /* Run in the background */
            if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE
                               | PROCMGR_DAEMON_NODEVNULL ) == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
                      argv[0]);

            } else {

                printf("cehu driver init done\n");

                while (1) {
                    if ((ctp = dispatch_block(ctp)) == NULL) {
                        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                              "CEHU error: Block error\n");
                        break;
                    } else {
                        dispatch_handler(ctp);
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

int cehu_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    UINT32 uret_func;
    cehu_msg_t       *cehu_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch (msg->i.dcmd) {
        printf("cehu_io_devctl(): command %d\n", msg->i.dcmd);
    case DCMD_CEHU_MASK_ENABLE:
        cehu_msg = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_CEHUMaskEnable(cehu_msg->InstanceID, cehu_msg->ErrorID);
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to enable mask 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;

    case DCMD_CEHU_MASK_DISABLE:
        cehu_msg = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_CEHUMaskDisable(cehu_msg->InstanceID, cehu_msg->ErrorID);
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to disable mask 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_CEHU_MASK_GET:
        cehu_msg = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_CEHUMaskGet(cehu_msg->InstanceID, cehu_msg->ErrorID, &cehu_msg->Data0);
        if (uret_func == 0U) {
            nbytes = sizeof(cehu_msg_t);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to get mask 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_CEHU_ERROR_GET:
        cehu_msg = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_CEHUErrorGet(cehu_msg->InstanceID, cehu_msg->ErrorID, &cehu_msg->Data0);
        if (uret_func == 0U) {
            nbytes = sizeof(cehu_msg_t);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to get error value 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_CEHU_ERROR_CLEAR:
        cehu_msg = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_CEHUErrorClear(cehu_msg->InstanceID, cehu_msg->ErrorID);
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to clear error value 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_CEHU_SAFETY_MODE_GET:
        cehu_msg = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_CEHUSafetyModeGet(cehu_msg->InstanceID, cehu_msg->ErrorID, &cehu_msg->Data0);
        if (uret_func == 0U) {
            nbytes = sizeof(cehu_msg_t);
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to set safety mode 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    case DCMD_CEHU_SAFETY_MODE_SET:
        cehu_msg = _DEVCTL_DATA(msg->i);
        uret_func = AmbaRTSL_CEHUSafetyModeSet(cehu_msg->InstanceID, cehu_msg->ErrorID, cehu_msg->Data0);
        if (uret_func == 0U) {
            nbytes = 0;
        } else {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "CEHU error: Failed to set safety mode 0x%X\n", uret_func);
            err = (int)uret_func;
        }
        break;
    default:
        nbytes = 0;
        err = EINVAL;
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

static void process_command(const char *buffer, size_t size)
{
    UINT32 i;
    UINT32 instance_idx;

    (void)size;

    if (strncmp(buffer, "disable", 7) == 0) {
        printf ("CEHU disabled for reboot\n");
        /* The following CEHU programming sequences must be followed. */
        /* Otherwise reboot doesn't work */
        /* 1. Enable all CEHU masks to supress all errors */
        for (instance_idx = 0; instance_idx < AMBA_NUM_CEHU_INSTANCES; instance_idx++) {
            for (i = 0; i < AMBA_NUM_CEHU_ERRORS; i++) {
                if (AmbaRTSL_CEHUMaskEnable(instance_idx, i) != 0U) {
                    // action TBD
                }
            }
        }

        /* 2. Disable CEHU GIC propagation */
        for (instance_idx = 0; instance_idx < AMBA_NUM_CEHU_INSTANCES; instance_idx++) {
            for (i = 0; i < AMBA_NUM_CEHU_ERRORS; i++) {
                if ((i == 1U) || (i == 28U)) {
                    // VROM safety error (1) and OTP safety error (28) should have default value (GIC+PIN propagation)
                    if (AmbaRTSL_CEHUSafetyModeSet(instance_idx, i, 3) != 0U) {
                        // action TBD
                    }
                } else {
                    // others should have default value (PIN propagation)
                    if (AmbaRTSL_CEHUSafetyModeSet(instance_idx, i, 2) != 0U) {
                        // action TBD
                    }
                }
            }
        }

    } else {
        // do nothing
    }
}

int cehu_io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
    int status;
    char *buf;
    size_t nbytes;
    int nret = 0;

    if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK) {
        nret = status;
    } else if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE) {
        nret = ENOSYS;
    } else {
        /* Extract the length of the client's message. */
        nbytes = _IO_WRITE_GET_NBYTES(msg);

        /* Filter out malicious write requests that attempt to write more
        data than they provide in the message. */
        if(nbytes > (size_t)ctp->info.srcmsglen - (size_t)ctp->offset - sizeof(io_write_t)) {
            nret = EBADMSG;
        } else {
            /* set up the number of bytes (returned by client's write()) */
            _IO_SET_WRITE_NBYTES (ctp, nbytes);
            buf = (char *) malloc(nbytes + 1);
            if (buf == NULL) {
                nret =(ENOMEM);
            } else {
                /*
                * Reread the data from the sender's message buffer.
                * We're not assuming that all of the data fit into the
                * resource manager library's receive buffer.
                */
                resmgr_msgread(ctp, buf, nbytes, sizeof(msg->i));
                buf [nbytes] = '\0'; /* just in case the text is not NULL terminated */
                //printf ("Received %d bytes = '%s'\n", nbytes, buf);
                process_command(buf, nbytes);
                free(buf);
                if (nbytes > 0) {
                    ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;
                }
                nret = (_RESMGR_NPARTS (0));
            }
        }
    }


    return nret;
}