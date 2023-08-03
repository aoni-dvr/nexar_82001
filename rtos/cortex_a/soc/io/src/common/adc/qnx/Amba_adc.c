/*
 * $QNXLicenseC:
 * Copyright 2010, 2018, QNX Software Systems.
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

#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <stdio.h>
#include "Amba_adc.h"

AMBA_ADC_REG_s * pAmbaADC_Reg;
AMBA_RCT_REG_s * pAmbaRCT_Reg;

int Amba_Adc_Init(void)
{
    uintptr_t virt_base;

    virt_base = mmap_device_io(AMBA_ADC_SIZE, AMBA_ADC_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaADC_Reg = (AMBA_ADC_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_RCT_SIZE, AMBA_RCT_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaRCT_Reg = (AMBA_RCT_REG_s *)virt_base;

    AmbaRTSL_AdcInit();

    return 0;
}

void Amba_Adc_Fini(void)
{
    munmap_device_io(AMBA_ADC_BASE, AMBA_ADC_SIZE);
    munmap_device_io(AMBA_RCT_BASE, AMBA_RCT_SIZE);
}

int Amba_Adc_Get_SingleRead(UINT32 AdcCh, UINT32 *pData)
{
    return (int)AmbaRTSL_AdcSingleRead(AdcCh, pData);
}

int Amba_Adc_Set_SampleRate(UINT32 SampleRate)
{
    return (int)AmbaRTSL_AdcConfig(SampleRate);
}

int Amba_Adc_Get_Info(UINT32 *pSampleRate, UINT32 *pActiveChanList)
{
    AmbaRTSL_AdcGetInfo(pSampleRate, pActiveChanList);
    return 0;
}

int Amba_Adc_SeamlessRead(UINT32 AdcCh, UINT32 BufSize, UINT32 *pBuf)
{
    return (int)AmbaRTSL_AdcSeamlessRead(AdcCh, BufSize, pBuf);
}

int Amba_Adc_Stop(UINT32 *pActualSize)
{
    AmbaRTSL_AdcStop(pActualSize);
    return 0;
}
