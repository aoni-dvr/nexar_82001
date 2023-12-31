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

#include "startup.h"
#include <time.h>
#include "board.h"

extern struct callout_rtn reboot_ambarella;

const struct callout_slot callouts[] = {
    { CALLOUT_SLOT( reboot, _ambarella) },
};

#if !defined(CONFIG_XEN_SUPPORT_QNX)

const struct debug_device debug_devices[] = {
    {
        "cv2_uart",
        /* Debug on UART 0 */
        {"0xe4000000^0.115200.24000000.16",
        },
        Amba_init_Uart,
        Amba_put_char,
        {
            &display_char_ambarella,
            &poll_key_ambarella,
            &break_detect_ambarella,
        }
    }
};

#else

const struct debug_device debug_devices[] = {
    {
        "ambarella xen console",
        { "", "", },
        xen_ambarella_console_init,
        xen_ambarella_console_putc,
        {
            NULL,
            NULL,
            NULL,
        },
    },
};

#endif

/*
 * main()
 *    Startup program executing out of RAM
 *
 * 1. It gathers information about the system and places it in a structure
 *    called the system page. The kernel references this structure to
 *    determine everything it needs to know about the system. This structure
 *    is also available to user programs (read only if protection is on)
 *    via _syspage->.
 *
 * 2. It (optionally) turns on the MMU and starts the next program
 *    in the image file system.
 */

int
main(int argc, char **argv, char **envv)
{
    int    opt;

    add_callout_array(callouts, sizeof(callouts));

    /*
     * Initialize debugging output
     */
    select_debug(debug_devices, sizeof(debug_devices));

    kprintf("AMBA startup.\n");

    for (opt = 0; opt < argc; opt++) {
        kprintf("argc[%d] %s\n",opt, argv[opt]);
    }
    // common options that should be avoided are:
    // "AD:F:f:I:i:K:M:N:o:P:R:S:Tvr:j:Z"
    while ((opt = getopt(argc, argv, COMMON_OPTIONS_STRING "")) != -1) {
        switch (opt) {
            default:
                handle_common_option(opt);
                break;
        }
    }

    /*
     * Collect information on all free RAM in the system
     */
    amba_init_raminfo();

    /*
     * Remove RAM used by modules in the image
     */
    alloc_ram(shdr->ram_paddr, shdr->ram_size, 1);

    /*
     * Initialize SMP
     */
    init_smp();

    /*
     * Initialize MMU
     */
    if (shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL) {
        init_mmu();
        board_mmu_enable();
    }

    /* Initialize the Interrupts related Information */
    init_intrinfo();

    /* Initialize timestamp timer */

    /* Initialize the Timer related information */
    init_qtime();
    kprintf("timer_freq : %d\n", timer_freq);

    /* Init L2 Cache Controller */
    init_cacheattr();

    /* Initialize the CPU related information */
    init_cpuinfo();

    /* Initialize the Hwinfo section of the Syspage */
    init_hwinfo();

    add_typed_string(_CS_MACHINE, "AMBA CV2 bub board");

    /*
     * Load bootstrap executables in the image file system and Initialise
     * various syspage pointers. This must be the _last_ initialisation done
     * before transferring control to the next program.
     */
    init_system_private();

    board_mmu_disable();
    /*
     * This is handy for debugging a new version of the startup program.
     * Commenting this line out will save a great deal of code.
     */
    print_syspage();

    return 0;
}


/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
