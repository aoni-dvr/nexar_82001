/*
 * $QNXLicenseC:
 * Copyright 2020, QNX Software Systems.
 * Copyright © 2020, Ambarella International LP
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

static clk_dev_t *dev;
AMBA_RCT_REG_s * pAmbaRCT_Reg = (AMBA_RCT_REG_s *)0xed080000;
AMBA_DBG_PORT_ORC_REG_s * pAmbaORC_Reg[4] = {   (AMBA_DBG_PORT_ORC_REG_s *)AMBA_DBG_PORT_CORC_BASE_ADDR,
                                                (AMBA_DBG_PORT_ORC_REG_s *)AMBA_DBG_PORT_EORC_BASE_ADDR,
                                                (AMBA_DBG_PORT_ORC_REG_s *)AMBA_DBG_PORT_DORC_BASE_ADDR,
                                                (AMBA_DBG_PORT_ORC_REG_s *)AMBA_DBG_PORT_VORC_BASE_ADDR
                                            };
AMBA_SCRATCHPAD_NS_REG_s * pAmbaScratchpadNS_Reg = AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR;

static char *ClkIdx2String(int id)
{
    static char *ClockName[AMBA_NUM_PLL] = {
        "AMBA_PLL_AUDIO",
        "AMBA_PLL_CORE",
        "AMBA_PLL_CORTEX0",
        "AMBA_PLL_CORTEX1",
        "AMBA_PLL_DDR",
        "AMBA_PLL_ENET",
        "AMBA_PLL_VIDEO_A",
        "AMBA_PLL_VIDEO_B",
        "AMBA_PLL_IDSP",
        "AMBA_PLL_NAND",
        "AMBA_PLL_SD",
        "AMBA_PLL_SENSOR0",
        "AMBA_PLL_SENSOR2",
        "AMBA_PLL_VISION",
        "AMBA_PLL_FEX",
    };

    return ClockName[id];
}

int main(int argc, char *argv[])
{
    int id;
    int option;
    int verbose = 0;
    unsigned int Freq = 0;
    pll_id_t CLockId = 0;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs;
    dispatch_t *dpp;
    resmgr_attr_t rattr;
    dispatch_context_t *ctp;
    iofunc_attr_t ioattr;
    struct _clockperiod new, old;

    /* correct OS system tick */
    new.nsec=1000*1000;
    ClockPeriod(CLOCK_REALTIME, &new, NULL, 0);

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
    io_funcs.devctl = clk_io_devctl;
    iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

    /* Attach the device name */
    id = resmgr_attach(dpp, &rattr, "/dev/clock", _FTYPE_ANY, 0,
                       &connect_funcs, &io_funcs, &ioattr);
    if (id == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "Clock error: Failed to attach pathname\n");
        goto fail;
    }

    /* Allocate a context structure */
    ctp = dispatch_context_alloc(dpp);

    dev = malloc(sizeof(clk_dev_t));
    if (!dev) {
        goto fail;
    }

    if (0x0 != clk_dev_init(dev)) {
        fprintf(stderr, "clk_dev init fail %s(%d)\n", __func__, __LINE__);
    }

    while ( (option = getopt(argc, argv, "f:i:v")) != -1) {
        switch (option) {
        case 'i':
            CLockId = strtoul(optarg, &optarg, 0);
            if (CLockId == 0)
                slogerr("Cannot set passed frequency to this clock");
            break;
        case 'f':
            Freq = strtoul(optarg, &optarg, 0);
            if (Freq == 0)
                slogerr("Cannot set passed frequency to this clock");
            break;
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

    while (1) {
        if ((ctp = dispatch_block(ctp)) == NULL) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "Clock error: Block error\n");
            goto fail;
        }
        dispatch_handler(ctp);
    }

fail:
    free(dev);

    return EXIT_SUCCESS;
}

int clk_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    clk_freq_t *clk;
    ctrl_id_t ctrl_id;
    unsigned int freq;
    clk_info_t* info;
    pll_freq_t* pll_freq;
    clk_config_t* clk_config;
    clk_boot_info_t *pBootMode;
    clk_orc_timer_t *pOrcTimer;
    clk_reset_info_t *pResetStatus;
    int err = EOK;

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    nbytes = 0;
    switch(msg->i.dcmd) {
    case DCMD_CLOCK_SET_FREQ:
        clk = _DEVCTL_DATA(msg->i);
        ctrl_id = clk->id;
        freq = clk->freq;
        if (Amba_Clk_Set_Freq(ctrl_id, freq) != 0)
            err = EINVAL;
        break;

    case DCMD_CLOCK_GET_FREQ:
        clk = _DEVCTL_DATA(msg->i);
        clk->freq = Amba_Clk_Get_Freq(clk->id);
        if (clk->freq == 0)
            clk->freq = -1;
        nbytes = sizeof(clk_freq_t);
        break;

    case DCMD_CLOCK_INFO:
        info = _DEVCTL_DATA(msg->i);
        Amba_Get_Info(info->id, info);
        nbytes = sizeof(clk_info_t);
        break;

    case DCMD_CLOCK_GET_PLL_FREQ:
        pll_freq = _DEVCTL_DATA(msg->i);
        pll_freq->freq = Amba_Clk_Get_Pll_Freq(pll_freq->id);
        nbytes = sizeof(*pll_freq);
        break;

    case DCMD_CLOCK_SET_PLL_FREQ:
        clk = _DEVCTL_DATA(msg->i);
        ctrl_id = clk->id;
        freq = clk->freq;
        if (Amba_Clk_Set_Pll_Freq(ctrl_id, freq) != 0)
            err = EINVAL;
        break;

    case DCMD_CLOCK_CLK_CONFIG:
        clk_config = _DEVCTL_DATA(msg->i);
        if (Amba_Clk_SetConfig(clk_config->id, clk_config->config) != 0)
            err = EINVAL;
        break;

    case DCMD_CLOCK_GETBOOTMODE:
        pBootMode = _DEVCTL_DATA(msg->i);
        Amba_Get_BootMode(&pBootMode->BootMode);
        nbytes = sizeof(clk_boot_info_t);
        break;

    case DCMD_CLOCK_SETFEATURE:
        clk_config = _DEVCTL_DATA(msg->i);
        Amba_Clk_EnableFeature(clk_config->feature);
        break;

    case DCMD_CLOCK_DISFEATURE:
        clk_config = _DEVCTL_DATA(msg->i);
        Amba_Clk_DisableFeature(clk_config->feature);
        break;

    case DCMD_CLOCK_CHECKFEATURE:
        clk_config = _DEVCTL_DATA(msg->i);
        Amba_Clk_CheckFeature(clk_config->feature, &status);
        clk_config->enable = status;
        nbytes = sizeof(clk_info_t);
        break;

    case DCMD_CLOCK_GETORCTIMER:
        pOrcTimer = _DEVCTL_DATA(msg->i);
        pOrcTimer->TimeTick = AmbaRTSL_GetOrcTimer();
        nbytes = sizeof(clk_orc_timer_t);
        break;

    case DCMD_SYS_RESET:
        Amba_System_Reboot();
        break;

    case DCMD_CLOCK_GET_RESET_ST:
        pResetStatus = _DEVCTL_DATA(msg->i);
        pResetStatus->ResetStatus = AmbaRTSL_PwcCheckSwReset();
        nbytes = sizeof(clk_reset_info_t);
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

