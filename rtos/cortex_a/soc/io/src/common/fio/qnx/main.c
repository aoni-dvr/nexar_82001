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

#include <stdio.h>
#include <proto.h>

#define FIO_TRANS_DONE_FLAG     (0x1U)

static AMBA_KAL_EVENT_FLAG_t AmbaFioEventFlags;

static void AmbaFio_TransDoneSet(UINT32 Id, UINT32 Value)
{
    (void)Value;
    (void)Id;

    AmbaRTSL_FdmaGetStatus();

    AmbaCSL_FioClearIrqStatus();   /* clear IRQ status */

    /* Set Transfer Done Event Flag */
    (void)AmbaKAL_EventFlagSet(&AmbaFioEventFlags, FIO_TRANS_DONE_FLAG);
}

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
#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
    ULONG Vbase = mmap_device_io(sizeof(AMBA_FIO_REG_s), AMBA_CA53_FLASH_CPU_BASE_ADDR);
#else
    ULONG Vbase = mmap_device_io(sizeof(AMBA_FIO_REG_s), AMBA_CORTEX_A53_FLASH_CPU_BASE_ADDR);
#endif
    extern AMBA_FIO_REG_s *pAmbaFIO_Reg;
    pAmbaFIO_Reg = (AMBA_FIO_REG_s *) Vbase;

    AmbaKAL_EventFlagCreate(&AmbaFioEventFlags, NULL);

    AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER,
        .IrqType     = AMBA_INT_FIQ,  /* FIQ for AmbaLink secure world */
        .CpuTargets  = 1U,
    };

    /* Register the NOR ISR. */
    /* Configure the VIC for level trigger high */
#if defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
    (void)AmbaINT_Disable(AMBA_INT_SPI_ID104_FIO);
    (void)AmbaINT_Config(AMBA_INT_SPI_ID104_FIO, &IntConfig, AmbaFio_TransDoneSet, 0U);
    (void)AmbaINT_Enable(AMBA_INT_SPI_ID104_FIO);
#else
    (void)AmbaINT_Disable(AMBA_INT_SPI_ID100_FIO);
    (void)AmbaINT_Config(AMBA_INT_SPI_ID100_FIO, &IntConfig, AmbaFio_TransDoneSet, 0U);
    (void)AmbaINT_Enable(AMBA_INT_SPI_ID100_FIO);
#endif
    /* Initialize the dispatch interface */
    dpp = dispatch_create();
    if (!dpp) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "SPINOR error: Failed to create dispatch interface\n");
        goto fail;
    }

    /* Initialize the resource manager attributes */
    memset(&rattr, 0, sizeof(rattr));

    /* Attach the device name */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.devctl = fio_io_devctl;
    iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

    id = resmgr_attach(dpp, &rattr, "/dev/fio", _FTYPE_ANY, 0,
                       &connect_funcs, &io_funcs, &ioattr);
    if (id == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "SPINOR error: Failed to attach pathname\n");
        goto fail;
    }

    /* Allocate a context structure */
    ctp = dispatch_context_alloc(dpp);

    /* Run in the background */
    if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
              argv[0]);
        goto fail;
    }

    printf("internal fio driver init done\n");

    while (1) {
        if ((ctp = dispatch_block(ctp)) == NULL) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "FIO error: Block error\n");
            goto fail;
        }
        dispatch_handler(ctp);
    }

fail:
    return EXIT_SUCCESS;
}

int fio_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status;
    amba_fio_config_t *pConfig;
    int err = EOK;
    UINT32 ActFlag;

    (void)pConfig;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    switch (msg->i.dcmd) {
    case DCMD_FIO_WAIT:
        pConfig = _DEVCTL_DATA(msg->i);

        status = AmbaKAL_EventFlagGet(&AmbaFioEventFlags, FIO_TRANS_DONE_FLAG,
                                      KAL_FLAGS_WAIT_ALL, KAL_FLAGS_CLEAR_AUTO, &ActFlag, 5000);
        if (status != 0) {
            fprintf(stderr, "%s %d, %d\n", __func__, __LINE__, status);
        }
        //fprintf(stderr, "%s %d, %d\n", __func__, __LINE__, err);
        break;
    }
#if 0
    if (nbytes == 0) {
        return (err);
    } else {
        msg->o.ret_val = 0;
        msg->o.nbytes = nbytes;
        return (_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
    }
#endif
    return (err);
}

