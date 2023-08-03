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


/*
#ifdef __USAGE

#endif
*/
#include "externs.h"

/* open() ... */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <hw/ambarella_clk.h>


/*
* Specify parameters for default devices from hwi_info tags.
*/
int
query_hwi_device(TTYINIT_AMBA *dip, unsigned unit)
{
    unsigned hwi_off = hwi_find_device("uart", unit);
    if(hwi_off != HWI_NULL_OFF) {
        hwi_tag *tag_location = hwi_tag_find(hwi_off, HWI_TAG_NAME_location, 0);
        if(tag_location) {
            dip->tty.port = tag_location->location.base;
        }
        hwi_tag *tag_irq = hwi_tag_find(hwi_off, HWI_TAG_NAME_irq, 0);
        if(tag_irq) {
            dip->intr[0] = tag_irq->irq.vector;
        }
        return 1;
    }
    /*
    * No default device, the base address and irq have been specified
    */
    return 0;
}

unsigned options(int argc, char *argv[])
{
    int            opt;
    int            numports = 0;
    void *         link;
    unsigned       unit;
    int            real_clk_opt = 0;
    unsigned       rx_fifo = 32;    // default
    unsigned       tx_fifo = 32;    // default
    TTYINIT_AMBA   devinit = {0};
    clk_freq_t clk_freq;
    int fd, err;
    int found_hwi_device = -1;

    devinit.tty.port = 0;
    devinit.tty.port_shift = 0;
    devinit.tty.intr = 0;
    devinit.tty.baud = 115200;
    devinit.tty.isize = 2048;
    devinit.tty.osize = 2048;
    devinit.tty.csize = 256;
    devinit.tty.c_cflag = 0;
    devinit.tty.c_iflag = 0;
    devinit.tty.c_lflag = 0;
    devinit.tty.c_oflag = 0;
    devinit.tty.fifo = 0;
    devinit.tty.clk = 24000000;
    devinit.tty.div = 1;
    memcpy(&devinit.tty.name, "/dev/ser", sizeof("/dev/ser"));

    devinit.intr[0] = 0xFFFF;
    devinit.intr[1] = 0xFFFF;

    devinit.isr = 1;

    /*
     * Initialize the devinit to raw mode
     */
    ttc(TTC_INIT_RAW, &devinit, 0);

    unit = 0;

    /* Getting the UART Base address and irq from the Hwinfo Section if available */
    unsigned hwi_off = hwi_find_device("uart", 0);
    if(hwi_off != HWI_NULL_OFF) {
        hwi_tag *tag_inputclk = hwi_tag_find(hwi_off, HWI_TAG_NAME_inputclk, 0);
        if(tag_inputclk) {
            devinit.tty.clk = tag_inputclk->inputclk.clk;
        }
    }

    while (optind < argc) {
        /*
         * Process dash options.
         */
        while ((opt = getopt(argc, argv, IO_CHAR_SERIAL_OPTIONS "t:T:c:u:d:i:r:")) != -1) {
            switch (ttc(TTC_SET_OPTION, &devinit, opt)) {

            case 't':
                rx_fifo = strtoul(optarg, NULL, 0);
                if (rx_fifo > FIFO_SIZE) {
                    printf( "RX FIFO trigger level must be < %d.\n", FIFO_SIZE);
                    printf( "Using default RX FIFO trigger level of 32\n");
                    rx_fifo = 32;
                }
                break;

            case 'T':
                tx_fifo = strtoul(optarg, NULL, 0);
                if (tx_fifo > FIFO_SIZE) {
                    printf( "TX FIFO trigger level must be < %d.\n", FIFO_SIZE);
                    printf( "Using default TX FIFO trigger level of 32\n");
                    tx_fifo = 32;
                }

                break;

            case 'c':
                devinit.tty.clk = strtoul(optarg, &optarg, 0);
                break;

            case 'u':
                unit = strtoul(optarg, NULL, 0);
                break;

            case 'd':
                printf( "DMA Not supported\n");
                break;

            case 'i':
                devinit.isr = strtoul(optarg, NULL, 0);
                break;

            case 'r':
                real_clk_opt = strtoul(optarg, NULL, 0);
                break;
            }
        }

        devinit.tty.fifo = rx_fifo | (tx_fifo << 10);

        /*
         * Process ports and interrupts.
         */
        while (optind < argc && *(optarg = argv[optind]) != '-') {
            devinit.tty.port = strtoul(optarg, &optarg, 16);
            if (*optarg == ',') {
                devinit.intr[0] = strtoul(optarg + 1, &optarg, 0);
                if (*optarg == ',')
                    devinit.intr[1] = strtoul(optarg + 1, &optarg, 0);
            }

            if (devinit.tty.port != 0 && devinit.intr[0] != 0xFFFF) {
                create_device(&devinit, unit++);
                ++numports;
            }
            ++optind;
        }
    }

    if (devinit.tty.port == 0xE4000000) {
        devinit.uart_id = AMBA_UART_APB_CHANNEL0;
        devinit.uart_clk_id = AMBA_CLK_UARTAPB;
    } else if (devinit.tty.port == 0xE0017000) {
        devinit.uart_id = AMBA_UART_AHB_CHANNEL0;
        devinit.uart_clk_id = AMBA_CLK_UART0;
        real_clk_opt = 1;
    } else if (devinit.tty.port == 0xE0018000) {
        devinit.uart_id = AMBA_UART_AHB_CHANNEL1;
        devinit.uart_clk_id = AMBA_CLK_UART1;
        real_clk_opt = 1;
    } else if (devinit.tty.port == 0xE0019000) {
        devinit.uart_id = AMBA_UART_AHB_CHANNEL2;
        devinit.uart_clk_id = AMBA_CLK_UART2;
        real_clk_opt = 1;
    } else if (devinit.tty.port == 0xE001A000) {
        devinit.uart_id = AMBA_UART_AHB_CHANNEL3;
        devinit.uart_clk_id = AMBA_CLK_UART3;
        real_clk_opt = 1;
    }

    /*
     * Set the real clock of UART
     */
    if (real_clk_opt) {
        fd = open("/dev/clock", O_RDWR);
        if (fd == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager for UART %d. Use default clocks", __FUNCTION__, devinit.uart_clk_id);
        } else {
            clk_freq.freq = devinit.tty.clk;
            clk_freq.id = devinit.uart_clk_id;

            /* Set UART0 control clock */
            err = devctl(fd, DCMD_CLOCK_SET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: UART %d SET devctl failed: %s", __FUNCTION__, devinit.uart_clk_id, strerror(errno));
            }

            /* Get UART control clock */
            err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
            if (err) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: UART %d GET devctl failed: %s", __FUNCTION__, devinit.uart_clk_id, strerror(errno));
            }

            if (clk_freq.freq == -1) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: /dev/clock: Invalid frequency (-1) for UART %d", __FUNCTION__, devinit.uart_clk_id);
            } else {
                /* Set real frequency */
                devinit.tty.clk = clk_freq.freq;
            }

            close(fd);
        }
    }

    if (numports == 0) {
        unit = 0;
        link = NULL;
        devinit.tty.fifo = rx_fifo | (tx_fifo << 10);
        while (1) {
            found_hwi_device = query_hwi_device(&devinit,unit);
            if (!found_hwi_device)
                break;
            create_device(&devinit, unit++);
            ++numports;
        }
        while (1) {
            link = query_default_device(&devinit, link);
            if (link == NULL)
                break;
            create_device(&devinit, unit++);
            ++numports;
        }
    }

    return numports;
}

