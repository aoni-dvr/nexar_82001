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

#ifndef AMBA_ADC_H
#define AMBA_ADC_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <sys/mman.h>
#include <sys/procmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <hw/inout.h>
#include <hw/ambarella_adc.h>

#include <arm/ambarella.h>
#include "AmbaRTSL_ADC.h"
#include "AmbaReg_ADC.h"
#include "AmbaTypes.h"
#include "AmbaReg_RCT.h"

/* -------------------------------------------------------------------------
 * ADC
 * -------------------------------------------------------------------------
 */
#define AMBA_ADC_BASE               0xE4003000
#define AMBA_ADC_SIZE               0x1000

/* -------------------------------------------------------------------------
 * Clocks
 * -------------------------------------------------------------------------
 */
#define AMBA_RCT_BASE               0xed080000
#define AMBA_RCT_SIZE               0x1000

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_DUMP(s)          printf(s)
#else
#define DEBUG_DUMP(...)
#endif

int adc_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

int Amba_Adc_Get_SingleRead(UINT32 AdcCh, UINT32 *pData);
int Amba_Adc_Set_SampleRate(UINT32 SampleRate);
int Amba_Adc_Get_Info(UINT32 *pSampleRate, UINT32 *pActiveChanList);
int Amba_Adc_SeamlessRead(UINT32 AdcCh, UINT32 BufSize, UINT32 *pBuf);
int Amba_Adc_Stop(UINT32 *pActualSize);

int Amba_Adc_Init(void);
void Amba_Adc_Fini(void);

#endif
