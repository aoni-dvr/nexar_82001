/*
 * $QNXLicenseC:
 * Copyright 2010, 2018, QNX Software Systems.
 * Copyright 2013, Adeneo Embedded.
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

#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <stdio.h>

#include "Amba_i2s.h"
#include "AmbaRTSL_I2S.h"
#include "AmbaCSL_I2S.h"

AMBA_I2S_REG_s * pAmbaI2S_Reg[2];

int Amba_I2s_Init(void)
{
    uintptr_t virt_base;
    AMBA_I2S_CTRL_s Ctrl;
    UINT32 RetVal;

    virt_base = mmap_device_io(AMBA_I2S_SIZE, AMBA_I2S0_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaI2S_Reg[0] = (AMBA_I2S_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_I2S_SIZE, AMBA_I2S1_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaI2S_Reg[1] = (AMBA_I2S_REG_s *)virt_base;

    Ctrl.ChannelNum = AMBA_I2S_AUDIO_CHANNELS_2;
    Ctrl.ClkDirection = AMBA_I2S_MASTER;
    Ctrl.ClkDivider = 1;
    Ctrl.DspModeSlots = 0;
    Ctrl.Echo = 0;
    Ctrl.Mode = AMBA_I2S_MODE_I2S;
    Ctrl.RxCtrl.Loopback = 0;
    Ctrl.RxCtrl.Order = AMBA_I2S_MSB_FIRST;
    Ctrl.RxCtrl.Rsp = AMBA_I2S_CLK_EDGE_RISING;
    Ctrl.RxCtrl.Shift = 0;
    Ctrl.TxCtrl.Loopback = 0;
    Ctrl.TxCtrl.Mono = 0;
    Ctrl.TxCtrl.Mute = 0;
    Ctrl.TxCtrl.Order = AMBA_I2S_MSB_FIRST;
    Ctrl.TxCtrl.Shift = 0;
    Ctrl.TxCtrl.Tsp = AMBA_I2S_CLK_EDGE_FALLING;
    Ctrl.TxCtrl.Unison = 0;
    Ctrl.WordPos = 0;
    Ctrl.WordPrecision = 32;
    RetVal = AmbaRTSL_I2sConfig(0, &Ctrl);
    if (RetVal == OK) {
        RetVal = AmbaRTSL_I2sConfig(1, &Ctrl);
    }

#if 0
    printf("I2S Mode: 0x%x\n", pAmbaI2S_Reg[0]->Mode);
    printf("I2S Wlen: 0x%x\n", pAmbaI2S_Reg[0]->Wlen);
    printf("I2S CLKDIV: 0x%x\n", pAmbaI2S_Reg[0]->ClkCtrl.ClkDiv);
    printf("I2S BitmuxMode24: 0x%x\n", pAmbaI2S_Reg[0]->BitmuxMode24.Multi24En);
    printf("I2S ChannelSelect: 0x%x\n", pAmbaI2S_Reg[0]->ChannelSelect.ChannelSelect);
#endif
    return RetVal;
}

void Amba_I2s_Fini(void)
{
    munmap_device_io(AMBA_I2S0_BASE, AMBA_I2S_SIZE);
    munmap_device_io(AMBA_I2S1_BASE, AMBA_I2S_SIZE);
}

