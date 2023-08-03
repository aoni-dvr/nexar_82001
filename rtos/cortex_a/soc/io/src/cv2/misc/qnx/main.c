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
#include <AmbaCSL_USB.h>
#include <AmbaCSL_Scratchpad.h>

#define NUM_DEVICES  2
#define DEVICE_IDX_MISC       0
#define DEVICE_IDX_SEC        1

static int dev_id_array [NUM_DEVICES] = {-1, -1};
//static char buffer_info[2048] = {'\0'};

int main(int argc, char *argv[])
{
    static iofunc_attr_t ioattr_array[NUM_DEVICES];
    static char *dev_name_array [NUM_DEVICES] = {
        "/dev/misc",
        "/dev/sec"
    };

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
              "MISC error: Failed to create dispatch interface\n");
    } else {
        /* Initialize the resource manager attributes */
        memset(&rattr, 0, sizeof(rattr));
        rattr.nparts_max = 1;
        rattr.msg_max_size = 2048;

        /* Initialize the connect functions */
        iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                         _RESMGR_IO_NFUNCS, &io_funcs);
        io_funcs.devctl = misc_io_devctl;
        io_funcs.write  = misc_io_write;
        //io_funcs.read   = misc_io_read;

        for (i = 0; i < NUM_DEVICES; i++) {
            if (i == DEVICE_IDX_SEC) {
                /* secure-sensitive: root only */
                iofunc_attr_init(&ioattr_array[i], S_IFCHR | 0600, NULL, NULL);
            } else {
                iofunc_attr_init(&ioattr_array[i], S_IFCHR | 0666, NULL, NULL);
            }
            //ioattr_array[i].nbytes = sizeof(buffer_info);

            /* Attach the device name */
            id = resmgr_attach(dpp, &rattr, dev_name_array[i], _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs, &ioattr_array[i]);
            if (id == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                      "MISC error: Failed to attach pathname %s\n", dev_name_array[i]);
                flag_leave = 1;
            } else {
                dev_id_array[i] = id;
            }
        }

        if (flag_leave == 0) {

            /* Allocate a context structure */
            ctp = dispatch_context_alloc(dpp);

            if (0x0 != misc_dev_init()) {
                fprintf(stderr, "misc_dev init fail %s(%d)\n", __func__, __LINE__);
            }

            /* Run in the background */
            if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE
                               | PROCMGR_DAEMON_NODEVNULL ) == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
                      argv[0]);

            } else {

                printf("misc driver init done\n");

                while (1) {
                    if ((ctp = dispatch_block(ctp)) == NULL) {
                        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                              "MISC error: Block error\n");
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

static void process_command(const char *buffer, size_t size)
{
    (void) size;

    if (strncmp(buffer, "usb_host", 8) == 0) {
        // turn on USB PHY
        AmbaCSL_RctSetUsbPhy0AlwaysOn();
        AmbaCSL_RctSetUsbHostOn();
        // reset USB Host Controller
        AmbaCSL_RctUsbHostSoftReset();
        // Select USB PHY as HOST mode
        AmbaCSL_RctSetUsbPhy0Select(0);

        // set EHCI overcurrent polarity
        AmbaCSL_UsbSetEhciOCPolarity(0);

        printf ("Select USB PHY as Host mode\n");
    } else if (strncmp(buffer, "usb_device", 10) == 0) {
        // turn on USB PHY
        AmbaCSL_RctSetUsbPhy0AlwaysOn();
        // reset USB Device Controller

        // Select USB PHY as DEVICE mode
        AmbaCSL_RctSetUsbPhy0Select(1);
        printf ("Select USB PHY as Device mode\n");
    } else {
        // do nothing
    }
}

int misc_io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
    int status;
    char *buf;
    size_t nbytes;
    int nret = 0;

    if (dev_id_array[DEVICE_IDX_MISC] == ctp->id) {

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
    } else {
        nret = ENOSYS;
    }

    return nret;
}

int misc_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
//    UINT32 uret_func;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    //printf("%s(%d) 0x%x\n", __func__, __LINE__, msg->i.dcmd);
    switch (msg->i.dcmd) {
    case DCMD_SYS_JTAG_ON:
        printf("%s(%d) AmbaCSL_ScratchpadJtagOn();\n", __func__, __LINE__);
        AmbaCSL_ScratchpadJtagOn();
        break;
    default:
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
