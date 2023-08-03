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
#include <AmbaRTSL_CAN.h>


#define THREAD_POOL_PARAM_T dispatch_context_t


static resmgr_connect_funcs_t    connect_funcs;
static resmgr_io_funcs_t         io_funcs;
static iofunc_attr_t             attr;


int main(int argc, char *argv[])
{

    /* declare variables we'll be using */
    thread_pool_attr_t   pool_attr;
    resmgr_attr_t        resmgr_attr;
    dispatch_t           *dpp;
    thread_pool_t        *tpp;
    dispatch_context_t   *ctp;
    int                  id;

    int mask = 0x00000001;
    ThreadCtl(_NTO_TCTL_RUNMASK, (void *)mask);
    ThreadCtl( _NTO_TCTL_IO, 0 );

    /* initialize dispatch interface */
    if((dpp = dispatch_create()) == NULL) {
        fprintf(stderr,
                "%s: Unable to allocate dispatch handle.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* initialize resource manager attributes */
    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /* initialize functions for handling messages */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.devctl = can_io_devctl;
    /* initialize attribute structure used by the device */
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);

    /* attach our device name */
    id = resmgr_attach(
             dpp,            /* dispatch handle        */
             &resmgr_attr,   /* resource manager attrs */
             "/dev/can",  /* device name            */
             _FTYPE_ANY,     /* open type              */
             0,              /* flags                  */
             &connect_funcs, /* connect routines       */
             &io_funcs,      /* I/O routines           */
             &attr);         /* handle                 */
    if(id == -1) {
        fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* initialize thread pool attributes */
    memset(&pool_attr, 0, sizeof pool_attr);
    pool_attr.handle = dpp;
    pool_attr.context_alloc = dispatch_context_alloc;
    pool_attr.block_func = dispatch_block;
    pool_attr.unblock_func = dispatch_unblock;
    pool_attr.handler_func = dispatch_handler;
    pool_attr.context_free = dispatch_context_free;
    pool_attr.lo_water = 2;
    pool_attr.hi_water = 4;
    pool_attr.increment = 1;
    pool_attr.maximum = 50;

    /* allocate a thread pool handle */
    if((tpp = thread_pool_create(&pool_attr,
                                 POOL_FLAG_EXIT_SELF)) == NULL) {
        fprintf(stderr, "%s: Unable to initialize thread pool.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* start the threads, will not return */
    thread_pool_start(tpp);

}


int can_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    int err = EOK;

    can_msg_t       *can_msg;
    can_fd_msg_t    *can_fd_msg;
    can_info_t      *info;
    can_enable_t    *enable;
    can_disable_t   *disable;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    /* unlockling attributes */
    status = iofunc_attr_unlock(&(attr));
    if(status != EOK) {
        return status;
    }

    nbytes = 0;
    switch (msg->i.dcmd) {

    case DCMD_CAN_ENABLE:
        enable = _DEVCTL_DATA(msg->i);
        if (0x0 != can_dev_init(enable->CanCh)) {
            fprintf(stderr, "can_dev init fail %s(%d)\n", __func__, __LINE__);
        }
        enable->Ret = CAN_Enable(enable->CanCh, &enable->Config);
        nbytes = sizeof(can_enable_t);
        break;

    case DCMD_CAN_WRITE:
        can_msg = _DEVCTL_DATA(msg->i);
        can_msg->Ret = CAN_Write(can_msg->CanCh, &can_msg->Message, can_msg->Timeout);
        nbytes = sizeof(can_msg_t);
        break;

    case DCMD_CAN_FD_WRITE:
        can_fd_msg = _DEVCTL_DATA(msg->i);
        can_fd_msg->Ret = CAN_WriteFd(can_fd_msg->CanCh, &can_fd_msg->FdMessage, can_fd_msg->Timeout);
        nbytes = sizeof(can_fd_msg_t);
        break;

    case DCMD_CAN_READ:
        can_msg = _DEVCTL_DATA(msg->i);
        can_msg->Ret = CAN_Read(can_msg->CanCh, &can_msg->Message, can_msg->Timeout);
        nbytes = sizeof(can_msg_t);
        break;

    case DCMD_CAN_FD_READ:
        can_fd_msg = _DEVCTL_DATA(msg->i);
        can_fd_msg->Ret = CAN_ReadFd(can_fd_msg->CanCh, &can_fd_msg->FdMessage, can_fd_msg->Timeout);
        nbytes = sizeof(can_fd_msg_t);
        break;

    case DCMD_CAN_GET_INFO:
        info = _DEVCTL_DATA(msg->i);
        info->Ret = CAN_GetInfo(info->CanCh, &info->BitInfo, &info->NumFilter, info->Filter);
        nbytes = sizeof(can_info_t);
        break;

    case DCMD_CAN_DISABLE:
        disable = _DEVCTL_DATA(msg->i);
        disable->Ret = CAN_Disable(disable->CanCh);
        if (0x0 != can_dev_dinit(disable->CanCh)) {
            fprintf(stderr, "can_dev de-init fail %s(%d)\n", __func__, __LINE__);
        }
        nbytes = sizeof(can_disable_t);
        break;
    }

    if (nbytes == 0) {
        return (err);
    } else {
        msg->o.ret_val = 0;
        msg->o.nbytes = nbytes;

        status = iofunc_attr_lock(&attr);
        if(status != EOK) {
            return status;
        } else {
            return (_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
        }
    }
}
