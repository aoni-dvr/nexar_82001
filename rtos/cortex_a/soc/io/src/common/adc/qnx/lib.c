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

#include "proto.h"

int get_adcfuncs(adc_functions_t *functable, int tabsize)
{
    ADC_ADD_FUNC(functable, init, Amba_Adc_Init, tabsize);
    ADC_ADD_FUNC(functable, fini, Amba_Adc_Fini, tabsize);

    ADC_ADD_FUNC(functable, adc_get_singleread, Amba_Adc_Get_SingleRead, tabsize);
    ADC_ADD_FUNC(functable, adc_set_samplerate, Amba_Adc_Set_SampleRate, tabsize);
    ADC_ADD_FUNC(functable, adc_get_info, Amba_Adc_Get_Info, tabsize);
    ADC_ADD_FUNC(functable, adc_get_seamlessread, Amba_Adc_SeamlessRead, tabsize);
    ADC_ADD_FUNC(functable, adc_set_stop, Amba_Adc_Stop, tabsize);

    return 0;
}
