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
#include <string.h>
#include <proto.h>
#include "hw/ambarella_spi.h"

static int amba_spi_options(int argc, char *argv[], int *channel, int *slave)
{
    int c;
    int prev_optind;
    int done = 0;

    *slave = 0;    /* Default is SPI master device */

    while (!done) {
        prev_optind = optind;
        c = getopt(argc, argv, "C:S");

        switch (c) {
        case 'S':
            *slave = AMBA_SPI_SLAVE_DEVICE;
            break;
        case 'C':
            *channel = strtol(optarg, &optarg, 0);
            break;
        case '?':
            if (optopt == '-') {
                ++optind;
                break;
            } else {
                return -1;
            }
        case -1:
            if (prev_optind < optind) /* -- */
                return -1;

            if (argv[optind] == NULL) {
                done = 1;
                break;
            }
            if (*argv[optind] != '-') {
                ++optind;
                break;
            }
            return -1;
        case ':':
        default:
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int id;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs[AMBA_NUM_SPI_MASTER];
    resmgr_io_funcs_t io_funcs_slave;
    dispatch_t *dpp;
    resmgr_attr_t rattr;
    dispatch_context_t *ctp;
    iofunc_attr_t ioattr[AMBA_NUM_SPI_MASTER];
    iofunc_attr_t ioattr_slave;
    int SpiChanId = 0, SpiSlave = 0;

    /* Initialize the dispatch interface */
    dpp = dispatch_create();
    if (!dpp) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "SPI error: Failed to create dispatch interface\n");
        goto fail;
    }

    /* Initialize the resource manager attributes */
    memset(&rattr, 0, sizeof(rattr));

    amba_spi_options(argc, argv, &SpiChanId, &SpiSlave);

    if (SpiSlave == AMBA_SPI_SLAVE_DEVICE) {
        /* Attach SPI slave resource manager */
        iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                         _RESMGR_IO_NFUNCS, &io_funcs_slave);
        io_funcs_slave.devctl = spi_io_devctl_slave;
        iofunc_attr_init(&ioattr_slave, S_IFCHR | 0666, NULL, NULL);
        id = resmgr_attach(dpp, &rattr, "/dev/spislave", _FTYPE_ANY, 0,
                           &connect_funcs, &io_funcs_slave, &ioattr_slave);
    } else {
        /* Attach SPI master channel_x resource manager */
        switch (SpiChanId) {
        case 0:
            iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                             _RESMGR_IO_NFUNCS, &io_funcs[0]);
            io_funcs[0].devctl = spi_io_devctl0;
            iofunc_attr_init(&ioattr[0], S_IFCHR | 0666, NULL, NULL);
            id = resmgr_attach(dpp, &rattr, "/dev/spi0", _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs[0], &ioattr[0]);
            break;
        case 1:
            iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                             _RESMGR_IO_NFUNCS, &io_funcs[1]);
            io_funcs[1].devctl = spi_io_devctl1;
            iofunc_attr_init(&ioattr[1], S_IFCHR | 0666, NULL, NULL);
            id = resmgr_attach(dpp, &rattr, "/dev/spi1", _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs[1], &ioattr[1]);
            break;
        case 2:
            iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                             _RESMGR_IO_NFUNCS, &io_funcs[2]);
            io_funcs[2].devctl = spi_io_devctl2;
            iofunc_attr_init(&ioattr[2], S_IFCHR | 0666, NULL, NULL);
            id = resmgr_attach(dpp, &rattr, "/dev/spi2", _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs[2], &ioattr[2]);
            break;
        case 3:
            iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                             _RESMGR_IO_NFUNCS, &io_funcs[3]);
            io_funcs[3].devctl = spi_io_devctl3;
            iofunc_attr_init(&ioattr[3], S_IFCHR | 0666, NULL, NULL);
            id = resmgr_attach(dpp, &rattr, "/dev/spi3", _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs[3], &ioattr[3]);
            break;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        case 4:
            iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                             _RESMGR_IO_NFUNCS, &io_funcs[4]);
            io_funcs[4].devctl = spi_io_devctl4;
            iofunc_attr_init(&ioattr[4], S_IFCHR | 0666, NULL, NULL);
            id = resmgr_attach(dpp, &rattr, "/dev/spi4", _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs[4], &ioattr[4]);
            break;
        case 5:
            iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                             _RESMGR_IO_NFUNCS, &io_funcs[5]);
            io_funcs[5].devctl = spi_io_devctl5;
            iofunc_attr_init(&ioattr[5], S_IFCHR | 0666, NULL, NULL);
            id = resmgr_attach(dpp, &rattr, "/dev/spi5", _FTYPE_ANY, 0,
                               &connect_funcs, &io_funcs[5], &ioattr[5]);
            break;
#endif
        default:
            fprintf(stderr, "Invalid SpiChanId:%d\n", SpiChanId);
            goto fail;
            break;
        }
    }

    if (id == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "SPI error: Failed to attach pathname\n");
        goto fail;
    }

    /* Allocate a context structure */
    ctp = dispatch_context_alloc(dpp);

    if (0x0 != spi_dev_init(SpiChanId, SpiSlave)) {
        fprintf(stderr, "spi_dev init fail %s(%d)\n", __func__, __LINE__);
    }

    /* Run in the background */
    if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE
                       | PROCMGR_DAEMON_NODEVNULL ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
              argv[0]);
        goto fail;
    }

    while (1) {
        if ((ctp = dispatch_block(ctp)) == NULL) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "SPI error: Block error\n");
            goto fail;
        }
        dispatch_handler(ctp);
    }

fail:

    return EXIT_SUCCESS;
}

int spi_io_devctl0(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    mst_xfer_d8_msg_t *mst_xfer_d8_msg;
    mst_xfer_d16_msg_t  *mst_xfer_d16_msg;
    mst_info_get_msg_t  *mst_info_get_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_SPI_MST_XFER_D8:
        mst_xfer_d8_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD8(AMBA_SPI_MASTER0, mst_xfer_d8_msg->In.SlaveMask, &(mst_xfer_d8_msg->In.Config), mst_xfer_d8_msg->In.BufSize,
                                 mst_xfer_d8_msg->In.TxBuf, mst_xfer_d8_msg->Out.RxBuf, &(mst_xfer_d8_msg->Out.ActualSize), mst_xfer_d8_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d8_msg->Out);
        break;
    case DCMD_SPI_MST_XFER_D16:
        mst_xfer_d16_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD16(AMBA_SPI_MASTER0, mst_xfer_d16_msg->In.SlaveMask, &(mst_xfer_d16_msg->In.Config), mst_xfer_d16_msg->In.BufSize,
                                  mst_xfer_d16_msg->In.TxBuf, mst_xfer_d16_msg->Out.RxBuf, &(mst_xfer_d16_msg->Out.ActualSize), mst_xfer_d16_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d16_msg->Out);
        break;
    case DCMD_SPI_MST_INFO_GET:
        mst_info_get_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterGetInfo(AMBA_SPI_MASTER0, &(mst_info_get_msg->NumSlaves), &(mst_info_get_msg->Status)) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_info_get_msg);
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

int spi_io_devctl1(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    mst_xfer_d8_msg_t *mst_xfer_d8_msg;
    mst_xfer_d16_msg_t  *mst_xfer_d16_msg;
    mst_info_get_msg_t  *mst_info_get_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_SPI_MST_XFER_D8:
        mst_xfer_d8_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD8(AMBA_SPI_MASTER1, mst_xfer_d8_msg->In.SlaveMask, &(mst_xfer_d8_msg->In.Config), mst_xfer_d8_msg->In.BufSize,
                                 mst_xfer_d8_msg->In.TxBuf, mst_xfer_d8_msg->Out.RxBuf, &(mst_xfer_d8_msg->Out.ActualSize), mst_xfer_d8_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d8_msg->Out);
        break;
    case DCMD_SPI_MST_XFER_D16:
        mst_xfer_d16_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD16(AMBA_SPI_MASTER1, mst_xfer_d16_msg->In.SlaveMask, &(mst_xfer_d16_msg->In.Config), mst_xfer_d16_msg->In.BufSize,
                                  mst_xfer_d16_msg->In.TxBuf, mst_xfer_d16_msg->Out.RxBuf, &(mst_xfer_d16_msg->Out.ActualSize), mst_xfer_d16_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d16_msg->Out);
        break;
    case DCMD_SPI_MST_INFO_GET:
        mst_info_get_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterGetInfo(AMBA_SPI_MASTER1, &(mst_info_get_msg->NumSlaves), &(mst_info_get_msg->Status)) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_info_get_msg);
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

int spi_io_devctl2(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    mst_xfer_d8_msg_t *mst_xfer_d8_msg;
    mst_xfer_d16_msg_t  *mst_xfer_d16_msg;
    mst_info_get_msg_t  *mst_info_get_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_SPI_MST_XFER_D8:
        mst_xfer_d8_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD8(AMBA_SPI_MASTER2, mst_xfer_d8_msg->In.SlaveMask, &(mst_xfer_d8_msg->In.Config), mst_xfer_d8_msg->In.BufSize,
                                 mst_xfer_d8_msg->In.TxBuf, mst_xfer_d8_msg->Out.RxBuf, &(mst_xfer_d8_msg->Out.ActualSize), mst_xfer_d8_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d8_msg->Out);
        break;
    case DCMD_SPI_MST_XFER_D16:
        mst_xfer_d16_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD16(AMBA_SPI_MASTER2, mst_xfer_d16_msg->In.SlaveMask, &(mst_xfer_d16_msg->In.Config), mst_xfer_d16_msg->In.BufSize,
                                  mst_xfer_d16_msg->In.TxBuf, mst_xfer_d16_msg->Out.RxBuf, &(mst_xfer_d16_msg->Out.ActualSize), mst_xfer_d16_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d16_msg->Out);
        break;
    case DCMD_SPI_MST_INFO_GET:
        mst_info_get_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterGetInfo(AMBA_SPI_MASTER2, &(mst_info_get_msg->NumSlaves), &(mst_info_get_msg->Status)) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_info_get_msg);
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

int spi_io_devctl3(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    mst_xfer_d8_msg_t *mst_xfer_d8_msg;
    mst_xfer_d16_msg_t  *mst_xfer_d16_msg;
    mst_info_get_msg_t  *mst_info_get_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_SPI_MST_XFER_D8:
        mst_xfer_d8_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD8(AMBA_SPI_MASTER3, mst_xfer_d8_msg->In.SlaveMask, &(mst_xfer_d8_msg->In.Config), mst_xfer_d8_msg->In.BufSize,
                                 mst_xfer_d8_msg->In.TxBuf, mst_xfer_d8_msg->Out.RxBuf, &(mst_xfer_d8_msg->Out.ActualSize), mst_xfer_d8_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d8_msg->Out);
        break;
    case DCMD_SPI_MST_XFER_D16:
        mst_xfer_d16_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD16(AMBA_SPI_MASTER3, mst_xfer_d16_msg->In.SlaveMask, &(mst_xfer_d16_msg->In.Config), mst_xfer_d16_msg->In.BufSize,
                                  mst_xfer_d16_msg->In.TxBuf, mst_xfer_d16_msg->Out.RxBuf, &(mst_xfer_d16_msg->Out.ActualSize), mst_xfer_d16_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d16_msg->Out);
        break;
    case DCMD_SPI_MST_INFO_GET:
        mst_info_get_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterGetInfo(AMBA_SPI_MASTER3, &(mst_info_get_msg->NumSlaves), &(mst_info_get_msg->Status)) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_info_get_msg);
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

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
int spi_io_devctl4(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    mst_xfer_d8_msg_t *mst_xfer_d8_msg;
    mst_xfer_d16_msg_t  *mst_xfer_d16_msg;
    mst_info_get_msg_t  *mst_info_get_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_SPI_MST_XFER_D8:
        mst_xfer_d8_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD8(AMBA_SPI_MASTER4, mst_xfer_d8_msg->In.SlaveMask, &(mst_xfer_d8_msg->In.Config), mst_xfer_d8_msg->In.BufSize,
                                 mst_xfer_d8_msg->In.TxBuf, mst_xfer_d8_msg->Out.RxBuf, &(mst_xfer_d8_msg->Out.ActualSize), mst_xfer_d8_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d8_msg->Out);
        break;
    case DCMD_SPI_MST_XFER_D16:
        mst_xfer_d16_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD16(AMBA_SPI_MASTER4, mst_xfer_d16_msg->In.SlaveMask, &(mst_xfer_d16_msg->In.Config), mst_xfer_d16_msg->In.BufSize,
                                  mst_xfer_d16_msg->In.TxBuf, mst_xfer_d16_msg->Out.RxBuf, &(mst_xfer_d16_msg->Out.ActualSize), mst_xfer_d16_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d16_msg->Out);
        break;
    case DCMD_SPI_MST_INFO_GET:
        mst_info_get_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterGetInfo(AMBA_SPI_MASTER4, &(mst_info_get_msg->NumSlaves), &(mst_info_get_msg->Status)) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_info_get_msg);
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

int spi_io_devctl5(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    mst_xfer_d8_msg_t *mst_xfer_d8_msg;
    mst_xfer_d16_msg_t  *mst_xfer_d16_msg;
    mst_info_get_msg_t  *mst_info_get_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_SPI_MST_XFER_D8:
        mst_xfer_d8_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD8(AMBA_SPI_MASTER5, mst_xfer_d8_msg->In.SlaveMask, &(mst_xfer_d8_msg->In.Config), mst_xfer_d8_msg->In.BufSize,
                                 mst_xfer_d8_msg->In.TxBuf, mst_xfer_d8_msg->Out.RxBuf, &(mst_xfer_d8_msg->Out.ActualSize), mst_xfer_d8_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d8_msg->Out);
        break;
    case DCMD_SPI_MST_XFER_D16:
        mst_xfer_d16_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterTransferD16(AMBA_SPI_MASTER5, mst_xfer_d16_msg->In.SlaveMask, &(mst_xfer_d16_msg->In.Config), mst_xfer_d16_msg->In.BufSize,
                                  mst_xfer_d16_msg->In.TxBuf, mst_xfer_d16_msg->Out.RxBuf, &(mst_xfer_d16_msg->Out.ActualSize), mst_xfer_d16_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_xfer_d16_msg->Out);
        break;
    case DCMD_SPI_MST_INFO_GET:
        mst_info_get_msg = _DEVCTL_DATA(msg->i);

        if (spi_MasterGetInfo(AMBA_SPI_MASTER5, &(mst_info_get_msg->NumSlaves), &(mst_info_get_msg->Status)) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(mst_info_get_msg);
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
#endif

int spi_io_devctl_slave(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;
    slv_xfer_d8_msg_t   *slv_xfer_d8_msg;
    slv_xfer_d16_msg_t  *slv_xfer_d16_msg;
    slv_info_get_msg_t  *slv_info_get_msg;
    slv_cfg_msg_t       *slv_cfg_msg;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_SPI_SLV_XFER_D8:
        slv_xfer_d8_msg = _DEVCTL_DATA(msg->i);

        if (spi_SlaveTransferD8(AMBA_SPI_SLAVE, slv_xfer_d8_msg->In.BufSize, slv_xfer_d8_msg->In.TxBuf,
                                slv_xfer_d8_msg->Out.RxBuf, &(slv_xfer_d8_msg->Out.ActualSize), slv_xfer_d8_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(slv_xfer_d8_msg->Out);
        break;
    case DCMD_SPI_SLV_XFER_D16:
        slv_xfer_d16_msg = _DEVCTL_DATA(msg->i);

        if (spi_SlaveTransferD16(AMBA_SPI_SLAVE, slv_xfer_d16_msg->In.BufSize, slv_xfer_d16_msg->In.TxBuf,
                                 slv_xfer_d16_msg->Out.RxBuf, &(slv_xfer_d16_msg->Out.ActualSize), slv_xfer_d16_msg->In.TimeOut) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(slv_xfer_d16_msg->Out);
        break;
    case DCMD_SPI_SLV_INFO_GET:
        slv_info_get_msg = _DEVCTL_DATA(msg->i);

        if (spi_SlaveGetInfo(AMBA_SPI_SLAVE, &(slv_info_get_msg->Status)) != 0) {
            err = EINVAL;
        }

        nbytes = sizeof(slv_info_get_msg);
        break;
    case DCMD_SPI_SLV_CFG:
        slv_cfg_msg = _DEVCTL_DATA(msg->i);

        if (spi_SlaveConfig(AMBA_SPI_SLAVE, &(slv_cfg_msg->Config)) != 0) {
            err = EINVAL;
        }

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

