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

#include "Amba_gpio.h"

int get_gpiofuncs(gpio_functions_t *functable, int tabsize)
{
    GPIO_ADD_FUNC(functable, init, Amba_Gpio_Init, tabsize);
    GPIO_ADD_FUNC(functable, fini, Amba_Gpio_Fini, tabsize);

    GPIO_ADD_FUNC(functable, gpio_set_direction, Amba_Gpio_Set_Direction, tabsize);
    GPIO_ADD_FUNC(functable, gpio_get_direction, Amba_Gpio_Get_Direction, tabsize);
    GPIO_ADD_FUNC(functable, gpio_set_output_enable, Amba_Gpio_Set_Output_Enable, tabsize);
    GPIO_ADD_FUNC(functable, gpio_get_output_enable, Amba_Gpio_Get_Output_Enable, tabsize);
    GPIO_ADD_FUNC(functable, gpio_get_input, Amba_Gpio_Get_Input, tabsize);
    GPIO_ADD_FUNC(functable, gpio_get_irq_type, Amba_Gpio_Get_Irq_Type, tabsize);
    GPIO_ADD_FUNC(functable, gpio_set_irq_type, Amba_Gpio_Set_Irq_Type, tabsize);
    GPIO_ADD_FUNC(functable, gpio_irq_clear, Amba_Gpio_Irq_Clear, tabsize);
    GPIO_ADD_FUNC(functable, gpio_get_irq_enable, Amba_Gpio_Get_Irq_Enable, tabsize);
    GPIO_ADD_FUNC(functable, gpio_irq_enable, Amba_Gpio_Irq_Enable, tabsize);
    GPIO_ADD_FUNC(functable, gpio_irq_disable, Amba_Gpio_Irq_Disable, tabsize);

    GPIO_ADD_FUNC(functable, gpio_get_pin_pull_enable, Amba_Get_Pin_Pull_Enable, tabsize);
    GPIO_ADD_FUNC(functable, gpio_get_pin_pull_level, Amba_Get_Pin_Pull_UpDown, tabsize);
    GPIO_ADD_FUNC(functable, gpio_get_pin_func, Amba_Get_Pin_Func, tabsize);
    GPIO_ADD_FUNC(functable, gpio_set_pin_func, Amba_Set_Pin_Func, tabsize);
    GPIO_ADD_FUNC(functable, gpio_set_pin_pull_level, Amba_Set_Pin_Pull_UpDown, tabsize);

    return 0;
}

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

    /* Initialize the connect functions */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.devctl = gpio_io_devctl;
    iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

    /* Attach the device name */
    id = resmgr_attach(dpp, &rattr, "/dev/gpio", _FTYPE_ANY, 0,
                       &connect_funcs, &io_funcs, &ioattr);
    if (id == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "Clock error: Failed to attach pathname\n");
        goto fail;
    }

    /* Allocate a context structure */
    ctp = dispatch_context_alloc(dpp);

    if (0x0 != Amba_Gpio_Init()) {
        fprintf(stderr, "gpio_dev init fail %s(%d)\n", __func__, __LINE__);
    }

    while ( (option = getopt(argc, argv, "v")) != -1) {
        switch (option) {
        case 'v':
            verbose++;
            break;
        default:
            fprintf(stderr,"Unsupported option '-%c'\n",option);
            goto fail;
        }
    }

    /* Run in the background */
    if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE
                       | PROCMGR_DAEMON_NODEVNULL ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
              argv[0]);
        goto fail;
    }

    printf("gpio driver init done\n");

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

int gpio_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    gpio_pin_t *pin;
    gpio_pin_info_t *pPinInfo;
    unsigned int PinId;
    unsigned int PinValue;
    int err = EOK;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_GPIO_SET_GPI:
        pin = _DEVCTL_DATA(msg->i);
        PinId = pin->PinId;
        (void)Amba_Gpio_Set_Direction(PinId, 0x0);
        break;

    case DCMD_GPIO_SET_GPO:
        pin = _DEVCTL_DATA(msg->i);
        PinId = pin->PinId;
        PinValue = pin->PinValue;
        (void)Amba_Gpio_Set_Output_Enable(PinId, PinValue);
        break;

    case DCMD_GPIO_SET_FUNC:
        pin = _DEVCTL_DATA(msg->i);
        PinId = pin->PinId;
        (void)Amba_Set_Pin_Func(PinId);
        break;

    case DCMD_GPIO_SET_PULL:
        pin = _DEVCTL_DATA(msg->i);
        PinId = pin->PinId;
        PinValue = pin->PinValue;
        (void)Amba_Set_Pin_Pull_UpDown(PinId, PinValue);
        break;

    case DCMD_GPIO_GET_INFO:
        pPinInfo = _DEVCTL_DATA(msg->i);
        (void)AmbaRTSL_GpioGetPinInfo(pPinInfo->PinId, &pPinInfo->PinInfo);
        nbytes = sizeof(gpio_pin_info_t);
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
